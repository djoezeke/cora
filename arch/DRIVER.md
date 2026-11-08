# Cora driver

The driver is the boundary between the command line/project manifest and the
compilation session. It owns configuration normalization, source/module
discovery, cache lookups, scheduling, diagnostics, object selection, and
linking. It does not make token, AST, AIR, or MIR files on disk.

The driver follows the two cache boundaries in `PIPELINE.md` and `CACHE.md`:

```text
source file -> CIR lookup/build -> object-input lookup/build -> link
                  one `.cir`       one `.o` per codegen unit
```

The cache is an optimization. A cache miss, a disabled cache, a read-only
cache, and a corrupt entry must all produce the same compiler result.

## Command-line interface

The command shape is:

```text
cora <command> [options] [inputs]
```

### Build and check commands

```text
cora build [OPTIONS] [PROJECT|SOURCE...]
cora check [OPTIONS] [PROJECT|SOURCE...]
cora emit  [OPTIONS] [PROJECT|SOURCE...]
```

`build` performs the requested compilation and link. `check` runs through
semantic and safety analysis but does not emit objects or link. `emit` makes an
explicit representation or object request; it is useful for tooling and
debugging. With no input, the driver finds the project manifest in the current
directory. A source path is accepted for small single-file programs.

The default project layout is:

```text
cora.toml
src/main.cora
```

### Common options

| Option | Meaning | Key effect |
| --- | --- | --- |
| `--manifest-path <file>` | use a specific project manifest | selects project inputs |
| `--target <triple>` | target triple, such as `x86_64-unknown-linux-gnu` | object |
| `--profile <name>` | `debug`, `release`, `test`, or custom profile | object when flags differ |
| `-O <0..3>` / `--opt-level <0..3>` | optimization level | object |
| `--cpu <name>` | target CPU model | object |
| `--features <list>` | enabled target features | object |
| `--debug <none|line|full>` | debug information | object |
| `--sanitize <list>` | sanitizer modes | object |
| `--pic` / `--pie` | position-independent code/executable | object or link as applicable |
| `--lto <off|thin|full>` | link-time/cross-module optimization | object and link |
| `--backend <llvm|c|native>` | backend implementation | object |
| `--emit <exe|lib|obj|cir>` | final artifact to produce | selects stopping point |
| `-o <path>` / `--output <path>` | explicit final output path | output only |
| `--jobs <n>` | maximum parallel workers | not a cache key |
| `--cache-dir <path>` | cache root; default `.cora-cache` | cache location, not artifact identity |
| `--no-cache` | disable lookup and publication | no cache reads/writes |
| `--cache-read-only` | allow hits but do not publish misses | no cache writes |
| `--explain-cache` | print normalized keys and hit/miss reasons | diagnostics only |
| `-v` / `--verbose` | show phase and external-tool information | diagnostics only |
| `--diagnostic-format <text|json>` | diagnostic encoding | not a cache key |

The target triple, ABI, CPU, target features, backend, optimization/debug/
sanitizer/PIC/unwind/LTO settings, and backend toolchain identity are normalized
into `ObjectInput`. Link-only options such as library search order, soname,
entry point, and linker arguments are passed to the linker and do not invalidate
objects unless they change code generation.

### Cache commands

```text
cora cache list [OPTIONS]
cora cache show [OPTIONS]
cora cache clean [OPTIONS]
```

Useful options are:

```text
--kind <cir|object|all>
--source <path>
--unit <module-or-codegen-unit>
--target <triple>
--profile <name>
--older-than <duration>
--keep <n>
```

`list` and `show` may scan the cache because they are explicit inspection
commands. A normal build never scans the cache. `show` recomputes the expected
CIR or object key from current inputs and reports its direct path and reason for
a miss. `clean` takes the cache lock and removes only generated entries; it
must never remove `build/` output.

## Driver data model

The public driver types should make the cache boundary visible:

```text
BuildConfig {
    projectRoot
    manifestPath
    sourceRoots[]
    entrySources[]
    target: TargetSpec
    profile
    frontendOptions
    semanticOptions
    codegenOptions
    linkOptions
    emitKind
    outputPath?
    cachePolicy: enabled | readOnly | disabled
    cacheRoot
    jobs
}

SourceFile {
    sourceFileId
    moduleId
    relativePath
    bytes
    sourceDigest
}

CIRInput {
    sourceFileId
    moduleId
    sourceDigest
    frontendSchema
    frontendOptions
    generatedInputDigests[]
}

ObjectInput {
    codegenUnitId
    cirInputKeys[]
    interfaceDigests[]
    implementationDigests[]
    targetTriple
    targetAndBackendOptions
    backendIdAndVersion
}
```

`BuildConfig` is normalized before any key is computed. Paths are canonicalized
for discovery but only stable workspace-relative IDs are used in reproducible
keys. `linkOptions` are kept separate so a link-only edit does not create a new
object key.

## Build flow

The driver performs this flow for `cora build`:

1. Parse CLI options and the manifest into `BuildConfig`.
2. Resolve the target, profile, backend, runtime, package lock, and cache
   policy. Compute compiler/toolchain identities.
3. Create `CompilationSession` with `SourceManager`, `DiagnosticEngine`,
   `ModuleResolver`, `CIRCache`, and `ObjectCache`.
4. Discover source files/imports and load/hash source bytes once. Build the
   in-memory `ModuleGraph` and deterministic dependency order.
5. For every source unit, construct `CIRInput` and do a direct CIR lookup.
   On a miss, run tokenizer -> parser -> AST lowering -> CIR verifier and
   publish one `.cir` file unless caching is disabled/read-only.
6. Obtain each unit's public interface digest from its CIR artifact and create
   the code-generation dependency set.
7. Construct `ObjectInput` and do a direct object lookup. On a hit, retain the
   `.o` path and skip the target-independent and target-specific compiler.
8. On an object miss, run semantic analysis -> AIR -> safety -> optimization ->
   target lowering -> MIR passes -> backend, verify the result, and publish one
   `.o` file unless caching is disabled/read-only.
9. Link the selected object paths with runtime/libraries and `linkOptions`, or
   copy/write the requested object/CIR output.
10. Emit diagnostics, cache statistics, and optional trace information. The
    exit status is determined by diagnostics and external-tool results, not by
    whether a cache hit occurred.

The build can parallelize independent CIR units and ready object units. It
must order ordinary diagnostics deterministically by source/module order.

### Pseudocode

```text
Driver::build(BuildConfig config) {
    session = CompilationSession::create(normalize(config));
    graph = session.modules.discover(session.sources, config.entrySources);

    for (unit in graph.cirOrder()) {
        source = session.sources.loadAndHash(unit.sourceFileId);
        cirInput = makeCIRInput(source, session.config);

        cir = session.cirCache.lookup(cirInput);
        if (!cir) {
            tokens = Lexer::lexAll(source);
            ast = Parser::parse(tokens);
            module = CirLowerer::lower(ast, source, session.diagnostics);
            CIRVerifier::verify(module);
            cir = session.cirCache.publish(cirInput, makeCIRArtifact(module));
        }
        graph.recordCIR(unit, cir);
    }

    for (unit in graph.objectOrder()) {
        objectInput = makeObjectInput(unit, graph, session.config);
        object = session.objectCache.lookup(objectInput);
        if (!object) {
            cir = graph.loadCIR(unit);
            interfaces = graph.importedInterfaces(unit);
            air = SemanticAnalyzer::analyze(cir, interfaces, session);
            validated = SafetyAnalyzer::run(air, session);
            optimized = AirOptimizer::run(validated, session.config);
            mir = TargetLowerer::lower(optimized, session.config.target);
            allocated = MachinePipeline::run(mir, session.config.codegen);
            bytes = Backend::emit(allocated, session.config.backend);
            object = session.objectCache.publish(objectInput, bytes);
        }
        session.linkInputs.add(object);
    }

    return Linker::link(session.linkInputs, session.config.linkOptions,
                        session.config.outputPath);
}
```

The pseudocode shows logical ownership, not a requirement that every method be
static. Errors return a result carrying diagnostics; no failed or partially
verified artifact is published.

## Emit behavior

The `--emit` value selects where the driver stops:

| Emit | Work performed | Result |
| --- | --- | --- |
| `cir` | source discovery and CIR lookup/build | requested `.cir` dump or cache path |
| `obj` | CIR plus object lookup/build | linkable `.o` at `--output` or build output |
| `exe` | CIR, object, and link | executable |
| `lib` | CIR, object, and link/archive | library |
| `check` command | CIR plus semantic/safety in memory | diagnostics only |

`--dump-tokens`, `--dump-ast`, `--dump-air`, and `--dump-mir` are diagnostic
projections. They do not create cache entries. AIR and MIR dumps are available
only when the object pipeline is actually run; an object hit can be explained
without deserializing them.

Examples:

```text
# Cold debug executable; writes CIR under h/ and objects under o/.
cora build src/main.cora --target x86_64-unknown-linux-gnu \
    --profile debug -O0 --emit exe

# Warm build; direct CIR/object hits, then a link.
cora build src/main.cora --target x86_64-unknown-linux-gnu \
    --profile debug -O0 --emit exe

# Emit a target object without linking.
cora build src/math.cora --target x86_64-unknown-linux-gnu \
    --emit obj --output build/x86_64-unknown-linux-gnu/debug/math.o

# Changing optimization creates a new object key but can reuse CIR.
cora build src/main.cora --target x86_64-unknown-linux-gnu \
    --profile release -O3 --emit obj

# A link-only change reuses objects.
cora build src/main.cora --target x86_64-unknown-linux-gnu \
    --emit exe --link-arg=-Wl,--gc-sections

# Inspect the exact CIR key and object key without compiling.
cora cache show --kind cir --source src/main.cora
cora cache show --kind object --unit app.main \
    --target x86_64-unknown-linux-gnu --profile debug --explain-cache

# Reproduce a build without reading or writing cache entries.
cora build src/main.cora --no-cache --emit exe

# Allow reuse but keep the cache immutable, useful in a shared/CI cache.
cora build --cache-read-only --manifest-path cora.toml --emit exe
```

The shell continuation character in the examples is illustrative; the driver
accepts the same options on one line.

## Cache messages and exit behavior

With normal verbosity the driver may report one line per rebuilt unit:

```text
cache: CIR hit      src/math.cora
cache: CIR miss     src/main.cora (source changed)
cache: object hit   app.math
cache: object miss  app.main (optimization flags changed)
link:  x86_64-unknown-linux-gnu/debug/bin/app
```

`--explain-cache` adds the relevant input category, target triple, and changed
flag without dumping source or secret environment values. A corrupt cache file
is reported as a miss and rebuilt if the cache policy permits publication.

Exit statuses should distinguish at least:

```text
0  success
1  source, semantic, safety, or link diagnostic
2  command-line or manifest error
3  internal compiler error
4  cache error that prevents a requested read-only/no-rebuild operation
```

A normal cache miss is never an error.

## Driver invariants

- Source bytes are loaded and hashed once per `SourceFileId` per session.
- CIR is the first persistent compiler representation and is keyed by input and
  source file identity.
- Object files are keyed by their complete input, target triple, and
  object-affecting flags.
- There are no per-stage writes for tokens, AST, AIR, safety facts, or MIR.
- Published cache entries are immutable and atomically complete.
- Link-only flags do not invalidate objects.
- Cache policy changes I/O behavior, never language or code-generation results.
- Cache paths, temporary paths, worker IDs, and diagnostics do not affect keys.
- A failed build leaves all previous valid artifacts usable.
