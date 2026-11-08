# Cora implementation architecture

This document maps the compilation and cache design to a C++ repository
structure. It describes the proposed implementation shape, ownership, and
function flow from tokenization to object-file generation.

The central performance rule is simple:

```text
source file -> CIR cache -> in-memory semantic/codegen pipeline -> object cache
```

Only CIR and object files are persistent compiler artifacts. There are no
`TokenCache`, `ASTCache`, `AIRCache`, `MIRCache`, per-stage fingerprint files,
or metadata sidecars in the normal build path.

## Repository structure

The current repository already has `Core`, `Lexer`, `AST`, `Parse`, and
diagnostic code. The following structure keeps those concepts but groups the
new pipeline by ownership and cache boundary:

```text
cora/
|-- CMakeLists.txt
|-- cmake/
|-- cora.toml                         # project/package manifest
|-- arch/                             # design contracts
|   |-- PIPELINE.md
|   |-- CACHE.md
|   |-- DRIVER.md
|   `-- ARCHITECTURE.md
|-- docs/
|-- include/Cora/
|   |-- Core/
|   |   |-- Digest.h                   # fixed-size content digest
|   |   |-- SHA256.h                   # streaming SHA-256 implementation
|   |   `-- FileHash.h                 # incremental file hashing
|   |-- Driver/
|   |   |-- Driver.h
|   |   |-- BuildConfig.h
|   |   |-- CompilationSession.h
|   |   `-- BuildPlan.h
|   |-- Lexer/
|   |   |-- Token.h
|   |   `-- Lexer.h
|   |-- Parse/
|   |   `-- Parser.h
|   |-- CIR/
|   |   `-- CIR.h
|   |-- AIR/
|   |   `-- AIR.h
|   |-- MIR/
|   |   `-- MIR.h
|   |-- CodeGen/
|   |   |-- ObjectInput.h
|   |   `-- TargetSpec.h
|   |-- Backend/
|   |   `-- CBackend.h
|   |-- Cache/
|   |   |-- CacheKey.h
|   |   |-- ContentAddressedStorage.h
|   |   |-- CIRCache.h
|   |   `-- ObjectCache.h
|   `-- Link/
|       `-- Linker.h
|-- src/Cora/                       # mirrors include/Cora
|-- tools/cora/main.cpp             # CLI entry point only
|-- tests/
|   |-- Cora/Lexer/ and Cora/Parse/
|   |-- Cora/Semantic/
|   |-- Cora/Cache/
|   |-- Cora/CodeGen/
|   `-- Cora/Driver/
|-- examples/
|-- deps/                           # fetched/declared dependencies
|-- build/                          # generated user-facing outputs
`-- .cora-cache/                    # generated CIR/object cache
```

The lexer and parser are the source frontend. `Lexer::lexAll()` owns lexical
recovery and returns a `TokenBuffer`; `Parser::parse()` consumes that buffer
and returns the temporary AST. Public headers describe ownership and value
types; implementation details such as hash maps, arenas, and backend handles
belong in `src/`.

Generated data must not be committed:

```text
build/       # final executable/library/object requested by the user
.cora-cache/ # content-addressed CIR and object entries
```

## Ownership and dependency rules

The dependency direction is intentionally one-way:

```text
Core -> Lexer/Parse -> CIR -> AIR -> CodeGen -> Link
             \-> Cache --------------^
Driver depends on all layers and owns the session.
```

More precisely:

- `Core` has no compiler-stage dependency.
- Diagnostics are reported at the driver boundary in this skeleton; they do
  not participate in cache identities.
- `Lexer` and `Parse` use source/token value types; they do not know target ABI
  or registers.
- `Cache` depends on stable input/artifact types and filesystem primitives; it
  does not run compiler passes.
- `Semantic` consumes CIR and interfaces; it does not depend on target MIR.
- `CodeGen` consumes validated/optimized AIR and `TargetSpec`.
- `Link` consumes object paths and link configuration, not AST/AIR/MIR.
- `Driver` assembles all components, schedules them, and owns diagnostics.

No lower layer should call the driver. This keeps unit tests able to construct
a lexer, CIR builder, cache, or backend without starting a full build.

## Fundamental value types

Use small strongly typed IDs and digests rather than passing strings through
every stage:

```text
FileId       = dense session-local source identity
ModuleId     = stable package/module identity
UnitId       = code-generation unit identity
TokenId      = index into TokenBuffer
AstId        = index into AST storage
ValueId      = SSA/CIR/AIR value index
BlockId      = basic-block index
CacheKey     = fixed-size digest rendered as lowercase hex
Digest       = content digest with an algorithm/version tag
```

```text
SourceFile {
    FileId id
    ModuleId module
    string workspaceRelativePath
    ByteBuffer bytes
    Digest sourceDigest
    LineTable lines
}

SourceSpan {
    FileId file
    uint32 startByte
    uint32 endByte
}

TargetSpec {
    string triple
    string objectFormat
    string abi
    string cpu
    FeatureSet features
    DataLayout layout
    CallingConvention callingConvention
}
```

`SourceFile` is immutable after loading. It is owned by `SourceManager` and
borrowed by tokenizer, parser, lowerer, and diagnostics. Cache keys use a
workspace-relative identity and content digest, never an absolute path or
pointer address.

## Driver and session classes

### `BuildConfig`

`BuildConfig` is the normalized result of CLI and manifest parsing. Keep the
categories separate so key construction is explicit:

```text
BuildConfig {
    WorkspaceConfig workspace
    FrontendOptions frontend
    SemanticOptions semantic
    CodegenOptions codegen
    LinkOptions link
    TargetSpec target
    Profile profile
    EmitKind emit
    CachePolicy cachePolicy
    Path cacheRoot
    uint32 jobs
}
```

`frontend` feeds `CIRInput`; `semantic` and `codegen` feed `ObjectInput`;
`link` is passed only to `Linker` unless an option explicitly changes emitted
machine code. `BuildConfig::normalize()` resolves defaults, sorts unordered
sets, canonicalizes feature names, and records compiler/backend versions.

### `CompilationSession`

`CompilationSession` is the root owner for one build:

```text
CompilationSession {
    BuildConfig config
    SourceManager sources
    ModuleResolver resolver
    ModuleGraph modules
    DiagnosticEngine diagnostics
    CIRCache cirCache
    ObjectCache objectCache
    StringInterner strings
    TypeStore types
    PassRegistry passes
    TraceSink* trace
}
```

It owns one source buffer and one digest per `FileId`, an in-memory dependency
graph, and cache instances configured with the same root/policy. The session is
not serialized. A new process reconstructs it from the manifest and source
files.

### `Driver` and `BuildPlan`

`Driver::run()` parses commands and dispatches to `build`, `check`, `emit`, or
`cache`. `BuildPlan` is an in-memory DAG:

```text
BuildPlan {
    SourceUnit sourceUnits[]
    CodegenUnit codegenUnits[]
    DependencyEdges edges[]
    LinkInputs linkInputs[]
}
```

It is created after discovery and contains enough information to schedule
parallel work without reading a persisted dependency index.

## Lexing, parsing, and CIR data flow

The frontend produces exactly one persistent representation: `CIRArtifact`.

```text
SourceManager::loadAndHash
        |
        v
Lexer::lexAll(SourceFile.bytes)
        |
        v
Parser::parse(TokenBuffer)
        |
        v
CirLowerer::lower(AST, SourceFile)
        |
        v
CIRVerifier::verify(CIRModule)
        |
        v
CIRCache::publish(CIRInput, CIRArtifact)
```

### `SourceManager`

Responsibilities:

- canonicalize project paths and assign `FileId`;
- load bytes once and compute `sourceDigest` while reading;
- provide line/column lookup and source snippets;
- preserve workspace-relative source identity; and
- track generated/include/macro input digests.

Suggested functions:

```text
SourceFileId internPath(ProjectPath path)
const SourceFile& loadAndHash(SourceFileId id)
SourceSpan span(FileId, uint32 start, uint32 end)
LineAndColumn locate(SourceSpan)
```

### `ModuleResolver` and `ModuleGraph`

`ModuleResolver::discover()` reads imports from source/module declarations,
resolves packages and roots, reports missing modules/cycles, and returns a
stable graph. It should not parse full function bodies merely to discover an
edge; a lightweight declaration scan or the parser's module header can do that.

```text
ModuleGraph {
    ModuleNode nodes[]
    ImportEdge edges[]
    DependencyOrder cirOrder
    DependencyOrder objectOrder
}

ModuleNode {
    ModuleId id
    SourceFileId source
    ModuleId imports[]
    Digest packageInputDigest
    CIRHeader cirHeader?
    InterfaceSummary interface?
}
```

### `Lexer`, `Token`, and `TokenBuffer`

`Lexer::lexAll()` scans bytes once. A token should contain a kind, source
span, and either a small immediate value or an interned/sliced value:

```text
Token {
    TokenKind kind
    SourceSpan span
    TokenValue value
}

TokenBuffer {
    vector<Token> tokens
    vector<Trivia> retainedTrivia
    vector<InternedString> identifiers
}
```

Do not put a disk cache around `TokenBuffer`; it is cheaper to regenerate it
than to maintain another serializer and invalidation boundary. The tokenizer
reports through a `DiagnosticSink` and can recover from malformed literals or
unexpected characters.

### `Parser` and `AST`

`Parser::parse()` consumes `TokenBuffer` and builds source-oriented AST storage.
The current compact indexed representation can be retained:

```text
AST {
    SourceFileId source
    Node nodes[]
    TokenRange tokenRanges[]
    ChildIndex children[]
    ExtraData extra
    SourceSpan spans[]
}
```

Use packed child ranges or an arena instead of a heap allocation per child
list. Preserve the existing index-zero invalid sentinel if downstream code
depends on it. Explicit error nodes let lowering continue far enough to report
independent diagnostics. AST storage is temporary and should be released after
successful CIR publication unless an IDE request retains it.

### `CIRModule`, `CIRInput`, and `CirLowerer`

```text
CIRInput {
    SourceFileId sourceFile
    ModuleId module
    Digest sourceDigest
    Version frontendSchema
    FrontendOptions options
    Digest generatedInputs[]
}

CIRArtifact {
    CacheKey key
    CIRInput input
    Digest publicInterfaceDigest
    CIRModule module
    SourceMap sourceMap
}

CIRModule {
    Version schema
    ModuleId module
    StringTable strings
    ConstantTable constants
    SymbolTable unresolvedSymbols
    FunctionTable functions
    BasicBlockTable blocks
    InstructionTable instructions
    PublicDeclarationTable publicDeclarations
}
```

`CirLowerer::lower()` handles desugaring, explicit evaluation order, scopes,
branches, returns, unresolved names, and source provenance. It must not infer
types or lower to an ABI. `CIRVerifier` checks storage/index invariants,
operand arity, block terminators, value references, and deterministic output.

## Cache classes

The cache layer has two artifact classes and one writer utility:

```text
class CIRCache {
    CacheLookup<CIRHeader> lookup(const CIRInput&)
    CIRArtifact load(const CIRHeader&)
    CIRArtifact publish(const CIRInput&, CIRArtifact)
}

class ObjectCache {
    CacheLookup<ObjectPath> lookup(const ObjectInput&)
    ObjectPath publish(const ObjectInput&, ObjectBuffer)
}

class AtomicArtifactWriter {
    TempPath begin(CacheNamespace, CacheKey, UnitId)
    void commit(TempPath, FinalPath)
    void discard(TempPath)
}
```

`CacheLookup` carries hit/miss reason, key, expected path, and optional header;
it does not carry a stage index. `CIRCache` validates a self-describing CIR
header/payload. `ObjectCache` validates the key path, file existence, size, and
target object format; the object remains raw and linkable.

The path functions are deterministic:

```text
CIRPath(key, source)    = <root>/h/<key>/<safe-source-name>.cir
ObjectPath(key, unit)   = <root>/o/<key>/<safe-unit-name>.o
TempPath(build, worker) = <root>/tmp/<build>-<worker>/<nonce>.partial
```

No cache class knows about semantic passes or link order. Cache policies are
`Enabled`, `ReadOnly`, and `Disabled`; all three must return equivalent compiler
results.

## Semantic and AIR classes

Semantic work runs only after an object miss:

```text
CIRModule + InterfaceSet
        |
        v
SemanticAnalyzer
   |-- NameResolver
   |-- TypeChecker
   |-- ComptimeEvaluator
        |
        v
AIRModule + InterfaceSummary
        |
        v
SafetyAnalyzer -> ValidatedAIR + SafetyFacts
        |
        v
AIRPassManager -> OptimizedAIR
```

Suggested responsibilities:

| Class | Responsibility |
| --- | --- |
| `InterfaceProvider` | expose public declarations and digest for imported modules |
| `NameResolver` | scopes, imports, visibility, overloads, symbols |
| `TypeChecker` | inference, conversions, generic constraints, type errors |
| `ComptimeEvaluator` | deterministic bounded compile-time evaluation |
| `SemanticAnalyzer` | coordinate resolution/type/comptime and build AIR |
| `SafetyAnalyzer` | CFG, move, borrow, lifetime, effect, race facts |
| `AIRPassManager` | validate and optimize target-independent AIR |

`AIRModule` remains target independent and is not written to the cache. Its
public interface and implementation fingerprint are used while constructing
`ObjectInput`. A dependent module should normally consume an interface digest,
not the private body, unless LTO/cross-module optimization is enabled.

## Object/code-generation classes

### `ObjectInput`

`ObjectInput` is the complete identity of emitted machine code:

```text
ObjectInput {
    UnitId unit
    CacheKey cirInputs[]
    Digest interfaceInputs[]
    Digest implementationInputs[]
    Version semanticAndCodegenSchema
    TargetSpec target
    CodegenOptions options
    BackendId backend
    ToolchainIdentity toolchain
}
```

`ObjectKey::from(ObjectInput)` hashes a canonical serialization. The
serialization sorts feature/flag sets and excludes output paths, timestamps,
worker IDs, diagnostic formatting, and link-only arguments.

### `TargetLowerer` and MIR

```text
TargetLowerer::lower(OptimizedAIR, TargetSpec) -> MIRModule
MachinePassManager::run(MIRModule, CodegenOptions) -> AllocatedMIR
```

`MIRModule` contains target opcodes/pseudo-ops, virtual registers, register
classes, stack slots, blocks, calling conventions, frame facts, and debug
locations. `MachinePassManager` performs instruction selection, scheduling,
liveness, register allocation, spill/reload insertion, prologue/epilogue, and
final legality checks. Pre- and post-allocation MIR are separate C++ types or
tagged states, even though neither is serialized persistently.

### `Backend` and object emission

```text
Backend::emit(AllocatedMIR, TargetSpec, BackendOptions) -> ObjectBuffer
ObjectCache::publish(ObjectInput, ObjectBuffer) -> ObjectPath
```

LLVM, C, and native backends should implement one interface but have distinct
backend IDs/toolchain identities. The backend must preserve source/debug
locations where supported and fail clearly for unsupported MIR operations. The
object buffer is closed and validated before atomic publication.

## End-to-end function flow

The implemented skeleton uses the following call sequence for one
source/object unit:

```text
Driver::run
  -> Lexer::lexAll -> Parser::parse (CIR miss only)
  -> cir::build(CIRInput, ParsedUnit)
  -> CIRCache::lookup/publish
  -> SemanticAnalyzer::analyze (object miss or requested dump)
  -> AIRPassManager::optimize
  -> TargetLowerer::lower -> MachinePassManager::run
  -> CBackend::emitObject -> ObjectCache::lookup/publish
  -> Linker::link(ObjectBuffer[], linkArguments)
```

The hit branches are important: a CIR hit deserializes its AST/CIR payload and
does not lex or parse; an object hit reads one object and does not run AIR,
MIR, or backend generation. A build may have a mix of hits and misses across
modules.

## Scheduling and lifetimes

`BuildPlan` schedules source units in dependency order:

1. source loading/hashing is parallel for independent files;
2. CIR misses run lexer/parser/lowering in parallel;
3. interface summaries become available after CIR headers/artifacts are ready;
4. object misses run when their imported interfaces are ready; and
5. linking waits for all selected object paths.

Published CIR and object files are immutable and can be read by all workers.
Each worker owns temporary token/AST/AIR/MIR arenas. The session owns source
bytes, diagnostics, interners, and graph state. A worker result is transferred
to the driver as a verified artifact/path, then its temporary IR memory is
released.

The scheduler must preserve deterministic diagnostics by assigning each unit a
stable order and merging completed events at the driver boundary. Worker ID,
completion time, and temporary paths must not affect keys or normal output.

## Diagnostics and testing boundaries

Every class accepts or references a `DiagnosticSink`; lower layers report
structured diagnostics rather than printing. Suggested test layers are:

```text
tests/Cora/Lexer/Lexer.test.cpp
tests/Cora/Parse/Parser.test.cpp
tests/Cora/CIR/CIRLowerer.test.cpp
tests/Cora/Cache/CIRCache.test.cpp
tests/Cora/Cache/ObjectCache.test.cpp
tests/Cora/Semantic/SemanticAnalyzer.test.cpp
tests/Cora/CodeGen/TargetLowerer.test.cpp
tests/Cora/Driver/IncrementalBuild.test.cpp
```

Important cache tests include:

- identical source/input produces the same CIR key and one artifact;
- a source-byte or source-file change misses CIR;
- a target/CPU/feature/optimization/debug/sanitizer change misses objects;
- a link-only option reuses objects;
- corrupt/truncated artifacts are misses, never accepted;
- two workers publishing the same key leave one valid artifact; and
- `--no-cache` and `--cache-read-only` produce equivalent output.

Important pipeline tests verify that:

- CIR is target independent unless an explicit target condition is present;
- AIR and safety never contain target registers;
- MIR honors target ABI/data layout/features;
- source spans survive CIR/AIR/MIR/backend lowering; and
- an object cache hit bypasses semantic and backend counters.

## Migration from the current tree

The current repository can adopt this shape incrementally:

1. Keep existing `Cora/Core`, `Cora/Diag`, `Cora/Lexer`, `Cora/AST`, and
   `Cora/Parse` APIs while stabilizing token/span ownership.
2. Add CIR lowering and implement source-file CIR serialization.
3. Add `Cora/Cache/CIRCache` and `ObjectCache` with the two namespaces from
   `CACHE.md`; do not port the old stage/fingerprint tree.
4. Add `Driver/BuildConfig` and `CompilationSession` around existing components.
5. Add semantic/AIR and target/codegen directories as in-memory pipelines.
6. Add one backend and `Linker`, then add parallel scheduling and benchmarks.

This preserves useful existing code while making the repository boundaries
match the performance model instead of requiring every future stage to become
another serialized cache format.
