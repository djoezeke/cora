# Cora compilation pipeline

This document defines the fast, incremental compilation pipeline for Cora. It
is deliberately smaller than a traditional compiler pipeline on disk:

- the source file is the incremental frontend unit;
- the first persistent representation is normalized, untyped CIR;
- token buffers, AST, AIR, safety facts, and MIR live in memory for one build;
- the second persistent representation is a target-specific object file; and
- linking consumes object files and writes the user-facing output, but is not a
  third compiler cache boundary.

The repository currently has source management, diagnostics, a lexer, and an
AST storage type. Parser, CIR, AIR, MIR, cache, backend, and driver APIs are
implementation targets unless marked as existing infrastructure below.

## Performance model

There are only two reusable compiler artifacts:

```text
source file + frontend input
        |
        |  one lookup/write
        v
      CIR  ------------------------------+
        |                                |
        | semantic analysis, safety,     | object-input fingerprint
        | optimization, target lowering  | + target + codegen flags
        v                                v
   in-memory MIR/backend             object file
                                         |
                                         | linker input
                                         v
                              build/<target>/<profile>/...
```

A warm build therefore performs direct content-addressed lookups instead of
walking a stage directory, reading a fingerprint file, reading metadata, and
then reading a payload. A CIR cache hit skips tokenization, parsing, and
lowering. An object cache hit skips semantic analysis, safety analysis,
optimization, target lowering, MIR passes, and backend emission for that unit.

The linker still runs for a normal executable or library build. Link options
and output paths are not object inputs; changing only a link option must not
recompile or rewrite object files.

## Pipeline at a glance

```text
 CLI and manifest
        |
        v
 +----------------------+       target, profile, flags
 | Driver/session       | <-----------------------------+
 +----------+-----------+                              |
            v                                           |
 +----------------------+                              |
 | SourceManager and    |                              |
 | ModuleResolver       |                              |
 +----------+-----------+                              |
            | source bytes, file digest, module graph  |
            v                                           |
 +----------------------+                               |
 | CIR lookup           |---- hit ----------------------+---> CIR
 +----------+-----------+                               |
            | miss                                      |
            v                                           |
 +----------------------+                               |
 | Lexer::lexAll        |                               |
 +----------+-----------+                               |
            v                                           |
 +----------------------+                               |
 | Parser -> AST        |                               |
 +----------+-----------+                               |
            v                                           |
 +----------------------+                               |
 | AST -> CIR lowerer   |                               |
 +----------+-----------+                               |
            v                                           |
 +----------------------+                               |
 | CIR verifier +       |---- atomic CIR publish       |
 | CIR cache             |                               |
 +----------+-----------+                               |
            v                                           |
 +----------------------+       imported CIR interfaces|
 | Object lookup        | <------------------------------+
 +----------+-----------+
            | hit                              miss
            |                                  |
            |                                  v
            |                    +----------------------+
            |                    | Name/type analysis   |
            |                    | and comptime         |
            |                    +----------+-----------+
            |                               v
            |                    +----------------------+
            |                    | Typed AIR             |
            |                    +----------+-----------+
            |                               v
            |                    +----------------------+
            |                    | Safety + AIR passes   |
            |                    +----------+-----------+
            |                               v
            |                    +----------------------+
            |                    | Target lowering/MIR   |
            |                    +----------+-----------+
            |                               v
            |                    +----------------------+
            |                    | Backend -> object     |
            |                    +----------+-----------+
            |                               |
            |                               +-- atomic object publish
            +-------------------------------+
                                            v
                                assembler/linker/runtime
                                            |
                                            v
                              executable, library, or objects
```

The object lookup can be made before loading the CIR payload. The driver first
computes the CIR keys from source inputs and obtains the small interface
fingerprints required by the module graph. If the object exists, no frontend
or semantic payload needs to be deserialized for that unit.

## Cache boundaries and inputs

| Boundary | Unit | Input used for the key | Persistent output | A hit skips |
| --- | --- | --- | --- | --- |
| CIR | one source file/module input | `CIRInput`, including source-file identity and source digest | one self-describing `.cir` file | tokenization, parsing, AST lowering |
| object | one code-generation unit, normally one source file or module | `ObjectInput`, including CIR input keys, dependency fingerprints, target triple, and codegen flags | one target object file | semantic analysis through backend emission |
| link | build root | object paths/digests, runtime, libraries, and link flags | user-facing build output | nothing; not a compiler cache boundary |

The build keeps the module graph, source buffers, cache headers, interfaces,
and diagnostics in memory. It does not persist separate lex, AST, AIR, safety,
optimized-AIR, MIR, machine-MIR, or link-cache entries.

### CIR input and source-file identity

CIR is target-independent unless the language explicitly exposes a target
conditional. Its key is formed from a stable logical input, not from an
absolute path or a file timestamp:

```text
CIRInput {
    sourceFileId          // package + normalized workspace-relative path
    moduleId
    sourceDigest          // digest of the exact source bytes
    languageEdition
    lexerParserSchema
    cirSchema
    frontendOptions       // syntax features, macro mode, include policy
    generatedInputDigests // macros/includes/resources that affect lowering
    targetCondition       // only when cfg(target) changes the CIR
}

cir-key = H("cora-cir-v1", compilerFrontendId, CIRInput)
```

The source file is both a first-class input and part of the artifact identity.
The source digest detects content changes; `sourceFileId` preserves module and
diagnostic identity when two files contain identical bytes. Absolute paths,
timestamps, process IDs, and diagnostic formatting are not key inputs.

A CIR artifact contains the following logical data in one file:

```text
CIRArtifact {
    key
    sourceFileId
    sourceDigest
    moduleId
    cirSchema
    publicInterfaceDigest
    CIRModule
    sourceMap
}
```

`CIRModule` is a normalized, untyped representation. It makes evaluation
order, scopes, control-flow edges, literals, and unresolved symbols explicit,
but it does not resolve language types or choose an ABI. The public interface
digest is a compact summary used to build dependent object inputs; it is part
of the CIR artifact, not a separate interface cache.

### Object input, target, and flags

The object key must change whenever the emitted object can change:

```text
ObjectInput {
    codegenUnitId
    cirInputKeys[]          // own CIR and the relevant dependency inputs
    interfaceDigests[]      // imported public contracts
    implementationDigests[] // only for cross-module optimization/LTO
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

The following changes are intentionally visible in `ObjectInput`:

| Change | CIR | object |
| --- | --- | --- |
| source bytes or source-file identity | new key | new key through the CIR input |
| parser/lowering schema | new key | new key through the CIR input |
| imported public interface | unchanged unless it affects frontend lowering | new dependent object key |
| target triple, ABI, object format | normally unchanged | new key |
| CPU or enabled target features | unchanged unless target conditions are used | new key |
| optimization, debug, sanitizer, PIC, unwind, or codegen flags | unchanged | new key |
| link library/order/soname flags only | unchanged | unchanged; relink only |

For ordinary separate compilation, an implementation-only change in an
imported module does not invalidate a dependent object when its public
interface digest is unchanged. Cross-module optimization and LTO must add
implementation digests to `ObjectInput`; this makes the stronger dependency
visible rather than silently reusing stale code.

## Stage details

### 1. Driver and compilation session

The driver parses the command line and manifest once, normalizes all options,
and creates one `CompilationSession`:

```text
CompilationSession {
    BuildConfig
    TargetSpec
    SourceManager
    ModuleGraph
    DiagnosticEngine
    CIRCache
    ObjectCache
    NameInterner / TypeStore
    PassPipeline
}
```

`BuildConfig` separates options by their effect:

- frontend options affect CIR;
- semantic and optimization options affect object input;
- target and backend options affect object input; and
- linker options affect only the link invocation.

The session owns one source buffer per canonical `SourceFileId` and shares its
digest between discovery, CIR key construction, diagnostics, and tokenization.

### 2. Source loading and module discovery

`SourceManager` assigns a stable `SourceFileId`, loads UTF-8 bytes, computes a
content digest, and owns line tables and source spans. `ModuleResolver` turns
imports into a `ModuleGraph` before semantic analysis. The graph records:

- source files and their normalized module IDs;
- direct import edges and package/version identities;
- source/include/macro inputs;
- declaration and initialization cycle information; and
- a deterministic dependency order for diagnostics and code generation.

Discovery is intentionally in-memory. It is cheap to reconstruct and avoids a
mutable module-graph/fingerprint file on every build. A source or package
manifest change causes the affected `CIRInput` or dependency fingerprint to
change naturally.

### 3. Tokenization: source bytes to tokens

`Lexer::lexAll` scans the source buffer once and produces a `TokenBuffer` with
source spans. Whitespace and ordinary comments can be discarded; documentation
comments and exact trivia can be retained for tools that request them. Token
values should use source slices or interned IDs rather than allocating a new
string for every token.

For:

```cora
let total = 2 + 3;
```

the conceptual stream is:

```text
LET IDENT(total) EQUALS NUMBER(2) PLUS NUMBER(3) SEMICOLON EOF
```

The tokenizer reports invalid UTF-8, malformed literals, unterminated strings
and comments, and unexpected characters through `DiagnosticEngine`. Recovery
consumes a safe span and continues where possible. Tokens are discarded after
CIR construction unless a diagnostic or requested dump still needs them.

### 4. Parsing: tokens to AST

`Parser` builds a source-oriented AST with compact indices, explicit error
nodes, and source spans. The AST retains syntax needed for diagnostics,
formatting, refactoring, and lowering; it must not contain inferred types,
target sizes, ABI facts, or registers.

The current storage convention remains valid as an initial implementation:

```text
AST
 +-- source view
 +-- nodes[]
 +-- token table
 +-- extra data / packed child ranges
 +-- first/last token indices
 +-- source-span IDs
```

Index zero remains the reserved invalid sentinel if the existing `AST` type
continues to use `InvalidIndex == 0`. Parser recovery synchronizes at `;`, `}`
and declaration keywords. AST storage is released after CIR lowering on a
successful object build.

### 5. AST lowering: AST to CIR

`CirLowerer` is the only stage that turns syntax into the cacheable frontend
representation. It should:

- desugar syntax into a small deterministic operation set;
- make evaluation order, scopes, returns, and branches explicit;
- preserve unresolved names and generic references as symbolic IDs;
- retain source spans and macro/include provenance; and
- produce a deterministic artifact for the exact `CIRInput`.

A compact CIR shape is:

```text
CIRModule {
    schemaVersion
    moduleId
    strings[]
    constants[]
    symbols[]
    functions[]
    blocks[]
    instructions[]
    publicDeclarations[]
    sourceSpans[]
}

CIRInstruction {
    opcode
    resultValueId?
    operandValueIds[]
    immediate?
    sourceSpanId
}
```

The CIR verifier checks operand counts, value/block dominance, terminators,
symbol references, and deterministic IDs before publication. A failed verifier
never creates a reusable CIR entry.

### 6. CIR cache lookup and publication

The frontend computes `CIRInput` while the source is loaded. `CIRCache` then
performs one direct lookup:

```text
path = .cora-cache/h/<cir-key>/<safe-source-name>.cir
```

On a valid hit, the driver receives the CIR header and artifact. On a miss,
tokenization, parsing, and lowering run, the result is verified, and one
self-describing file is written through a temporary path followed by an atomic
rename. No AST, token, or metadata sidecar is written.

### 7. Object input and semantic analysis

Once CIR headers and imported public interfaces are available, the driver
constructs `ObjectInput` before starting the expensive target pipeline. This
allows a direct object lookup. On a miss, the following representations exist
only in memory:

```text
CIR + imported interfaces
        -> SemanticAnalyzer -> AIR
        -> SafetyAnalyzer   -> ValidatedAIR + SafetyFacts
        -> AirOptimizer     -> OptimizedAIR
```

`AIR` is typed, name-resolved, control-flow explicit, and target independent.
It contains language types, effects, ownership modes, generic substitutions,
and resolved symbols. It does not contain registers, ABI layout, or machine
instructions.

Semantic analysis performs name and visibility resolution, type inference,
generic constraints, comptime evaluation, exhaustiveness checks, definite
initialization, and language-level effect checking. Comptime must be
deterministic by default and bounded by recursion, memory, time, and output
limits. External file, network, clock, and process access requires an explicit
capability and its input digest must be included in `CIRInput` or `ObjectInput`.

Safety analysis validates control flow, moves, borrows, lifetimes, effects,
and races using target-independent AIR. It must not introduce target-specific
MIR. If a verifier or semantic/safety diagnostic fails, no object is
published, but earlier valid CIR entries remain reusable.

### 8. Target lowering and MIR

`TargetLowerer` consumes `OptimizedAIR` and `TargetSpec`:

```text
OptimizedAIR + TargetSpec -> MIR -> machine passes -> allocated MIR
```

`TargetSpec` contains the target triple, CPU, ABI, object format, data layout,
calling convention, atomics, unwind rules, and enabled features. MIR has
virtual registers before allocation and physical registers/stack slots after
allocation. These forms are distinct in memory even though neither is a
persistent cache point.

The MIR verifier checks register classes, legal operands, branch targets, stack
alignment, calling conventions, frame layout, and target feature constraints.
The pass pipeline should be deterministic for the same `ObjectInput`.

### 9. Backend and object publication

`Backend` lowers allocated MIR to the selected object format. LLVM, C, and a
native backend are separate implementations behind the same interface; their
identity and toolchain version are part of `ObjectInput`.

```text
AllocatedMIR -> Backend::emit -> ObjectBuffer
ObjectBuffer + ObjectInput -> ObjectCache::publish
```

The published object is a linkable object file. It is written once to a unique
temporary path, closed and checked, and atomically renamed into `o/<key>/`.
The object cache does not add a custom prefix to the object, so the linker can
consume it directly. The content-addressed path is its cache identity.

### 10. Linking

The driver supplies cache hits and newly emitted objects to `Linker` together
with runtime libraries, package libraries, exported symbols, and link flags.
The linker writes:

```text
build/<target-triple>/<profile>/bin/<program>
build/<target-triple>/<profile>/lib/<library>
```

The output directory is not disposable cache state. A link-only change reruns
the linker but does not touch CIR or object entries. A future link cache may be
added only if measurements show that linking is a dominant cost; it is not
part of this design.

## Representation contracts

The representations are a one-way refinement pipeline:

| Representation | Knows | Must not know | Lifetime |
| --- | --- | --- | --- |
| source/tokens | bytes, lexical values, spans | grammar meaning, types, ABI | source/session |
| AST | syntax, nesting, token ranges, spans | inferred types, target layout | frontend request |
| CIR | normalized operations, explicit order/CFG, symbolic names | resolved types, ABI, registers | cache + session |
| AIR | resolved symbols/types, language effects, ownership, CFG | CPU instructions, physical layout | object miss only |
| validated/optimized AIR | proven facts and language optimizations | target registers and ABI choices | object miss only |
| MIR | target instructions, register classes, ABI/frame facts | surface syntax and unresolved names | object miss only |
| object | target machine code, relocations, debug data | compiler semantic graph | cache + link |

Every serialized CIR and object identity must include a schema/compiler/backend
identity. Serialized data must contain stable IDs and no process-local
pointers, allocator addresses, absolute temporary paths, or nondeterministic
ordering. Source provenance must remain available from CIR instructions and
backend debug locations back to the original source span.

## Errors, dumps, and tracing

Diagnostics use stable phase codes:

```text
LEX-*      lexical errors
PARSE-*    grammar and recovery errors
LOWER-*    AST/CIR lowering errors
SEMA-*     name, type, and comptime errors
SAFE-*     move, borrow, lifetime, and effect errors
MIR-*      target lowering and machine legality errors
BACKEND-*  backend, assembler, and linker integration errors
CACHE-*    cache format, checksum, or publication errors
```

Lexing and parsing recover when safe. CIR and AIR may use error sentinels to
avoid cascades, but only verified CIR can be cached and only a successful
semantic/safety pipeline can emit an object. Internal errors include stage,
pass, compiler ID, target, and a reproducible input key without printing
source contents by default.

Requested dumps are produced from the in-memory representation or the single
CIR artifact:

```text
--dump-tokens       tokenizer output
--dump-ast          parser output
--dump-cir          CIR artifact
--dump-air          semantic output on an object miss
--dump-mir          target lowering output on an object miss
--explain-cache     key inputs, hit/miss reason, and artifact path
```

Dumps are diagnostic output, not additional cache points. Structured trace
events should contain stage, unit, input key, output key, elapsed time,
instruction counts, memory, cache hit/miss, and target. Traces are written only
when requested and never influence an artifact key.

## Parallelism and correctness

The module graph is the scheduler DAG. Independent sources can build CIR in
parallel; object units can start when their CIR and imported interface inputs
are ready. Workers:

- read immutable published entries;
- write only to unique temporary paths;
- publish with an atomic rename;
- never update a shared per-stage index; and
- preserve deterministic diagnostic ordering at the driver boundary.

If two workers produce the same key, the first complete artifact wins and the
other temporary file is discarded. A crash can leave an unreferenced
temporary file, but cannot make a partial entry appear valid. Cache cleanup
must ignore paths held by an active build lock.

## Invalidation and performance metrics

Expected invalidation is direct:

- a changed source file misses its CIR and its object;
- a changed frontend schema misses CIR and all objects consuming that CIR;
- a public interface change misses dependent objects;
- a target, ABI, CPU feature, or codegen flag change misses objects only;
- a link-only flag change relinks only; and
- a compiler/backend/toolchain schema change misses the affected artifact.

Measure cold builds, warm builds, CIR-only misses, object-only misses, and
link-only changes. Record wall time, CPU time, peak memory, source bytes/s,
token/AST/CIR/AIR/MIR counts, object bytes, cache lookup time, cache hit rate,
cache bytes read/written, and linker time. The key performance assertions are:

1. a warm unchanged build performs no cache writes;
2. a CIR hit does not tokenize, parse, or lower that source file;
3. an object hit does not run semantic analysis or backend code generation; and
4. changing link-only options does not read or rewrite compiler artifacts.

## Suggested implementation sequence

1. Stabilize source IDs, spans, token APIs, diagnostics, and AST invariants.
2. Complete parser recovery and deterministic AST dumps.
3. Implement `CIRInput`, `CIRModule`, the CIR verifier, serializer, and
   content-addressed CIR cache.
4. Implement module interfaces and AIR construction without adding a disk
   cache for AIR.
5. Add safety analysis, comptime limits, and target-independent AIR passes.
6. Implement `TargetSpec`, MIR, one backend, and `ObjectInput`.
7. Add atomic object publication, link integration, and cache explanations.
8. Add parallel scheduling and benchmark cold/warm/partial builds.

This order keeps every boundary testable while ensuring that performance work
does not require maintaining eleven independently serialized representations.
