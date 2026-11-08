# Cora compilation cache

The Cora cache is disposable, project-local build state. It is inspired by the
useful part of Zig's cache design: short content-addressed namespaces, direct
hash lookups, immutable published artifacts, and a temporary directory for
atomic publication.

The cache intentionally has only two compiler artifact kinds:

1. `CIR` (`h/`): one normalized frontend artifact for a source file/module
   input; and
2. `object` (`o/`): one target-specific object file for a code-generation unit.

Tokens, ASTs, AIR, safety facts, optimized AIR, MIR, machine MIR, and final
link outputs are not separate persistent cache points. They are created in
memory during a miss. This reduces disk reads, writes, directory traversal,
metadata synchronization, and cache invalidation bookkeeping.

Deleting `.cora-cache/` is always safe. It only turns later builds into cold
builds; it must never delete a user-facing artifact in `build/`.

## Design goals

The cache must:

- make an unchanged build perform direct lookups and no writes;
- use the source file and its exact input digest as the CIR identity;
- use the object input, target triple, and all object-affecting flags as the
  object identity;
- avoid per-stage indexes, fingerprint files, and metadata sidecars on the
  hot path;
- publish immutable artifacts atomically and tolerate interrupted builds;
- allow independent source files/code-generation units to build in parallel;
- keep target/profile/output directories separate from disposable cache state;
  and
- make cache hits, misses, invalidation reasons, and cleanup inspectable.

## Directory layout

The default cache root is `.cora-cache`. `--cache-dir` may select another
location. The `v1` format is represented by the key namespace and artifact
headers; an incompatible format gets a new root or namespace rather than a
best-effort read.

```text
.cora-cache/
|-- CACHEDIR.TAG
|-- h/                              # hashed frontend/CIR artifacts
|   `-- <cir-key>/
|       `-- <safe-source-name>.cir  # one self-describing file
|-- o/                              # hashed linkable object artifacts
|   `-- <object-key>/
|       `-- <safe-unit-name>.o      # raw target object, no sidecar
`-- tmp/                            # unreferenced build attempts
    `-- <build-id>-<worker-id>/
```

`h` and `o` are deliberately small namespaces, following the useful Zig-style
pattern of separating hashed frontend state from hashed object output. The
hash directory is the cache entry identity; the filename is only a readable,
sanitized label. An implementation may shard the first two key bytes if a
filesystem has a directory-entry limit, but sharding must not add an index or
change the key.

There is no `state/`, `stages/`, `packages/`, `dep-info/`, per-stage
`metadata.json`, or per-artifact fingerprint tree in this design. Package
download/source management belongs to the package manager; its resolved input
digests are passed into the build keys.

User-facing output is separate:

```text
build/
`-- <target-triple>/
    `-- <profile>/
        |-- bin/<program>
        `-- lib/<library>
```

The linker may consume an object directly from `.cora-cache/o/...` or copy it
to an explicitly requested `--output` path. The output path is never used as a
cache key.

## Artifact identity

Cache lookup is a pure function of an input record. The driver computes the
record in memory and opens one path; it never scans all entries during a
normal build.

### CIR key

CIR is keyed by both the logical source file and the input that produced it:

```text
CIRInput {
    sourceFileId          # package + normalized workspace-relative path
    moduleId
    sourceDigest          # hash of exact source bytes
    languageEdition
    lexerParserSchema
    cirSchema
    frontendOptions
    generatedInputDigests # includes, macros, generated source/resources
    targetCondition       # present only when target cfg affects CIR
}

cir-key = H("cora-cir-v1", compilerFrontendId, CIRInput)
```

`sourceFileId` prevents two identical files from sharing source provenance by
accident. `sourceDigest` detects content changes. A normalized relative path is
used instead of an absolute path so moving a workspace does not invalidate
everything; a source rename intentionally creates a new identity.

The `.cir` file contains a compact header and payload in one file:

```text
CORA-CIR
  formatVersion
  cirKey
  sourceFileId
  sourceDigest
  moduleId
  publicInterfaceDigest
  payloadLength / payloadDigest
  CIRModule + source map
```

The header is enough to validate the key and obtain the public interface
fingerprint. No `metadata.json` is needed. The source bytes are not duplicated
in the cache; the source manager reloads them from `sourceFileId` when a
diagnostic needs text that is not already in memory.

### Object key

An object is reusable only when its complete code-generation input is the same:

```text
ObjectInput {
    codegenUnitId
    cirInputKeys[]
    interfaceDigests[]
    implementationDigests[] # cross-module optimization/LTO only
    languageAndSemanticId
    targetTriple
    objectFormat
    abi
    cpu
    targetFeatures
    optimizationFlags
    debugFlags
    sanitizerAndAssertionFlags
    relocationAndCodeModel
    unwindAndExceptionMode
    ltoAndCodegenUnitPolicy
    backendIdAndToolchainVersion
}

object-key = H("cora-object-v1", ObjectInput)
```

The object path contains the key, while the object itself remains a normal
linkable file:

```text
.cora-cache/o/<object-key>/<safe-unit-name>.o
```

The cache does not prefix or wrap the object because assemblers and linkers
must be able to consume it directly. The key and the object format validation
are the cache metadata. The driver can explain the key from the normalized
`ObjectInput` without reading a sidecar.

### What invalidates what

| Input change | CIR entry | Object entry |
| --- | --- | --- |
| source bytes | miss | miss through `cirInputKeys` |
| source-file/module identity | miss | miss through `cirInputKeys` |
| parser/lowering/schema/compiler frontend | miss | miss through CIR input/compiler identity |
| imported public interface | normally unchanged | dependent object miss |
| target triple, ABI, object format | normally unchanged | miss |
| CPU or target feature set | unchanged unless target conditions are used | miss |
| optimization/debug/sanitizer/PIC/unwind/codegen flags | unchanged | miss |
| backend/toolchain version | unchanged | miss |
| link library/order/soname/output flags only | unchanged | object hit; relink |

Separate compilation may use a dependent module's public interface digest
without including its private implementation. LTO and any cross-module pass
that observes bodies must add implementation digests. A conservative build may
include dependency CIR keys for all imported modules; it gets fewer hits but
never reuses an object after a relevant input change.

## Lookup and publication

### CIR lookup

The driver hashes source bytes while loading them, constructs `CIRInput`, and
performs one direct lookup:

```text
path = cache/h/<cir-key>/<safe-source-name>.cir
```

On a hit, `CIRCache` checks the magic, format version, key, source digest,
payload bounds, payload digest, and CIR verifier version. A valid hit supplies
CIR and its public interface digest. A miss runs tokenizer, parser, and CIR
lowering in memory, verifies the result, and publishes exactly one file.

### Object lookup

After CIR input keys and imported interface fingerprints are available, the
driver constructs `ObjectInput` and performs one direct lookup:

```text
path = cache/o/<object-key>/<safe-unit-name>.o
```

The lookup checks existence, file size, target object magic/format, and the
expected unit label where the object format exposes one. It does not read a
metadata file. A valid hit goes directly to the linker. A miss runs semantic
analysis, safety, AIR optimization, target lowering, MIR passes, and backend
emission, then publishes one object file.

### Atomic publication

Workers write only to their own directory under `tmp/`:

```text
tmp/<build-id>-<worker-id>/<nonce>.partial
```

The publication sequence is:

1. create the destination directory if needed;
2. write the complete CIR or object artifact to the worker temporary file;
3. flush and close it;
4. validate the artifact and compute any required payload digest; and
5. atomically rename it to the final key path.

The destination is immutable. If another worker already published the same
key, the later worker discards its temporary file after confirming the existing
artifact is valid. A crash can leave a partial temporary file but cannot make a
partial cache entry look complete.

## Read/write behavior

The hot path has a deliberately small I/O budget:

| Build state | Reads | Writes |
| --- | --- | --- |
| unchanged warm build | source bytes, direct CIR/object entries as needed, linker inputs | none |
| source changed | changed source and affected CIR/object entries | one CIR and one object per rebuilt unit |
| target/flag changed | CIR headers or existing CIR, object lookup | one object per affected unit |
| link-only change | object files | final build output only |
| cold build | source files | one CIR and one object per successful unit |

The driver must not write a dirty marker before work starts or a fingerprint
record after every stage. A failed build reports its reason through diagnostics
and trace output; previously published artifacts remain valid.

The normal build does not scan `h/` or `o/`. Listing, explaining, and cleanup
are explicit commands and may scan the relevant namespace.

## Cache API shape

The implementation can expose a small interface:

```text
class CIRCache {
    optional<CIRHeader> lookup(const CIRInput& input);
    CIRArtifact load(const CIRHeader& header);
    void publish(const CIRInput& input, const CIRArtifact& artifact);
};

class ObjectCache {
    optional<ObjectPath> lookup(const ObjectInput& input);
    void publish(const ObjectInput& input, ObjectBuffer object);
};
```

`lookup` computes the key and path; it does not consult an index. `load` is
separate so an object hit never attempts to deserialize AIR/MIR and a CIR
header can be read without loading the full payload. The cache classes do not
own semantic analysis, scheduling, or linking.

## Profiles, targets, and output separation

Profile and target values are included in `ObjectInput`; they do not require
another directory level in the cache. This is the important difference from a
stage/profile tree:

```text
.cora-cache/o/<object-key>/app.o
build/x86_64-unknown-linux-gnu/debug/bin/app
build/aarch64-unknown-linux-gnu/release/bin/app
```

Two builds with the same profile name but different normalized flags produce
different keys. The target triple, ABI, CPU, object format, and target features
must be normalized before hashing. Link-only values can stay out of the object
key and are passed only to the linker.

## Cache commands and diagnostics

The driver should support commands equivalent to:

```text
cora cache list [--kind cir|object]
cora cache show --kind cir --source src/math.cora
cora cache show --kind object --unit app.math --target x86_64-unknown-linux-gnu
cora cache clean [--kind cir|object] [--older-than <duration>]
```

`cache show` recomputes the normalized input and displays the key, expected
path, artifact size, schema, and hit/miss reason. It must not print source
contents or environment secrets. `cache clean` takes the build lock, skips
currently published entries in use, and removes only generated cache paths.

Build diagnostics can be concise by default:

```text
cache: CIR hit      src/math.cora
cache: object miss  app.math (target feature changed: avx2)
cache: object hit   app.main
```

`--explain-cache` adds normalized key inputs and invalidation categories.
`--cache-read-only` permits hits and computes misses without publishing them;
`--no-cache` disables both lookup and publication. Cache errors are treated as
misses when safe; a corrupt entry is moved to `tmp`/quarantine or ignored and
never used as a valid artifact.

## Concurrency and recovery

The cache is safe for parallel compilation because entries are immutable and
keys are content addressed:

- workers read published files without modifying them;
- each worker has an isolated temporary path;
- the first verified publication for a key wins;
- no shared stage index needs a lock on the compilation hot path; and
- cleanup uses a short cache lock and never deletes an artifact held by a
  running build.

The driver must preserve deterministic diagnostics and trace ordering even if
workers finish in a different order. A temporary directory can be removed on
the next build. A missing or corrupt artifact is simply a cache miss and is
recreated from source or in-memory compiler output.

## Reproducibility and security

Cache keys and serialized CIR must not include timestamps, process IDs, worker
IDs, absolute paths, allocator addresses, or nondeterministic map ordering.
External inputs used by comptime or code generation must be content-addressed
and included in `CIRInput` or `ObjectInput`. Cache files must not contain
secrets; command-line/environment values that affect a build should be hashed
or redacted in diagnostics.

`CACHEDIR.TAG` and `.gitignore` should mark the whole `.cora-cache/` tree as
generated. Final installed artifacts remain outside the cache and follow the
project's normal backup and deployment policy.

## Migration from the stage cache

The previous design had `00-discovery` through `10-link`, a mutable `state/`
tree, and `metadata.json` beside every representation. The migration is:

| Previous entry | New behavior |
| --- | --- |
| lex + parse + AST entries | in-memory work feeding one CIR entry |
| CIR entry | retained as `h/<cir-key>/<source>.cir` |
| AIR/safety/optimized-AIR/MIR entries | in-memory work on an object miss |
| backend entry | object emission retained as `o/<object-key>/<unit>.o` |
| link entry | user-facing `build/` output; relink when needed |
| fingerprints/dep-info/metadata sidecars | removed from the hot path; keys are recomputed |

Old entries must not be interpreted as new entries. A fresh namespace or
explicit cache version makes the first build after migration a safe cold build.
