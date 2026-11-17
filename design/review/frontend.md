# Cora Front-End Design Review

This document reviews the current lexer, parser, diagnostics, AST
structures, and their interaction. It is based on the implementation in
[`include/Cora/Lexer/`](C:/Users/sacke/Projects/cora/include/Cora/Lexer),
[`src/Cora/Lexer/Lexer.cpp`](C:/Users/sacke/Projects/cora/src/Cora/Lexer/Lexer.cpp),
[`include/Cora/Parser/`](C:/Users/sacke/Projects/cora/include/Cora/Parser),
[`src/Cora/Parser/Parser.cpp`](C:/Users/sacke/Projects/cora/src/Cora/Parser/Parser.cpp),
[`include/Cora/AST/`](C:/Users/sacke/Projects/cora/include/Cora/AST),
and [`include/Cora/Diag/`](C:/Users/sacke/Projects/cora/include/Cora/Diag).
The sample syntax is [`syntax.cora`](C:/Users/sacke/Projects/cora/syntax.cora).

The recommendations below are implementation-oriented. They preserve the
current compact/indexed design where it is useful and identify where stronger
invariants are needed before the language grows.

## 1. Current architecture

```text
source bytes
    |
    v
Lexer
  - UTF-8 validation
  - trivia/comment removal
  - keyword/operator recognition
  - literal decoding
  - TokenStream/TokenPool output
    |
    v
Parser
  - recursive descent for declarations/statements/types
  - Pratt parsing for expressions
  - recovery and malformed nodes
  - optional incremental top-level reuse
    |
    v
AST
  - compact node indexes
  - structure-of-arrays NodeList
  - extra_data arena for lists
  - token/source retention
    |
    +--> ParseError entries in AST
    +--> DiagnosticEngine and text/JSON/LSP emitters
```

The design already has several good properties:

* token and AST storage avoid per-item heap allocation;
* indexes make incremental copying and serialization practical;
* source spans are retained for AST nodes and diagnostics;
* parser recovery can continue after an error;
* diagnostics are value types and can be emitted in multiple formats;
* lexer and parser have focused unit tests plus incremental tests.

## 2. Lexer limitations and suggestions

### 2.1 Identifier rules are too permissive

`is_identifier_start` and `is_identifier_continue` accept almost any valid
non-ASCII, non-whitespace code point. This means punctuation and symbols that
are not language-approved identifier characters can become part of an
identifier. The implementation also has no normalization or policy for
confusable characters.

**Suggestion**

Define an explicit identifier policy:

1. keep ASCII letters and `_` as the fast path;
2. use Unicode XID_Start/XID_Continue tables for non-ASCII identifiers;
3. optionally normalize or reject mixed-script/confusable identifiers;
4. expose the policy as a language-version/configuration choice.

Add tests for combining marks, emoji, symbols, zero-width characters, scripts,
and invalid UTF-8 boundaries.

### 2.2 UTF-8 handling lacks complete lexical policy

UTF-8 is validated, but source positions remain byte offsets. The lexer also
does not make the distinction between an invalid byte sequence and a valid
Unicode character visible in the token model. Editors and diagnostics need a
single, documented byte-to-line/column policy.

**Suggestion**

Keep byte offsets as the canonical storage coordinate, but add a source-file
line index and a documented conversion API:

```text
byte offset -> line -> UTF-8 code-point column -> UTF-16 LSP column
```

Cache conversions per source revision. Make invalid UTF-8 replacement behavior
explicit instead of letting each emitter infer it.

### 2.3 Numeric literal grammar is underspecified

The lexer supports decimal and prefixed integer forms, floating-point forms,
and digit separators, but the token only stores a parsed `uint64_t` or
`double`. The source spelling, radix, suffix, overflow state, and exact
precision are not retained. Malformed forms can be split into several tokens
rather than represented as one invalid literal.

Likely missing or unclear cases include:

* integer overflow and floating-point overflow/underflow;
* exponent grammar and exponent diagnostics;
* invalid digits after `0x`, `0b`, or `0o`;
* suffixes for width, signedness, or user-defined types;
* `1..2` versus `1.2` versus `1.`;
* exact decimal values needed by compile-time evaluation.

**Suggestion**

Introduce a `NumericLiteral` payload containing source range, radix, kind,
separator validity, suffix range, and a checked parse result. Diagnose overflow
at lex time and preserve the original spelling for later semantic decisions.
Define the grammar in one place and generate focused lexer tests from that
grammar.

### 2.4 String and character literal validation is incomplete

Unterminated strings/chars still produce literal tokens, which helps recovery,
but character literals do not clearly enforce exactly one Unicode scalar value.
Escape decoding accepts values without a complete scalar-range check, and
unknown escapes are warnings even when they may change program meaning.

**Suggestion**

Represent literal status explicitly:

```text
valid | unterminated | invalid_escape | invalid_scalar | too_many_characters
```

Reject surrogate and out-of-range scalar values, distinguish warning-level
compatibility escapes from errors, and retain both decoded value and source
spelling. Add tests for empty chars, multi-character chars, escaped quotes,
newline escapes, Unicode escapes, surrogate values, and truncated escapes.

### 2.5 Trivia is discarded too early

Comments and whitespace are skipped, and only `TOKEN_AT_LINE_START` survives.
This makes formatting tools, documentation extraction, precise comment
attachment, and source-preserving rewrites harder. The sample contains both
ordinary and documentation comments, but the front end has no documented
comment attachment model.

**Suggestion**

Add an optional trivia channel containing comment/whitespace ranges. Keep it
disabled for normal compilation if memory is a concern. Attach documentation
comments to the next declaration using token ranges rather than reconstructing
them from source text later.

### 2.6 Token payload semantics are implicit

`Token` uses a union plus a tag, while `TokenPool::token()` reconstructs
payloads through special cases. Interned strings and error IDs share the same
storage shape, and callers must know which tags make each union member valid.
There is no invalid-token tag; invalid bytes use `T_EOF` internally as a
sentinel in `scan_operator`.

**Suggestion**

Add an explicit `T_INVALID`/`T_ERROR` tag and make internal invalid-token
handling separate from EOF. Document a payload invariant table or replace the
union with a tagged payload abstraction at API boundaries. Keep the compact
columnar representation internally.

### 2.7 Keyword and token definitions can drift

The token macros list `fn`, `var`, and related keywords, while the lexer also
accepts aliases such as `func` and `mut` in `keyword_tag`. This is useful for
compatibility but is not represented as a first-class language-version or
deprecation rule.

**Suggestion**

Generate keyword lookup and token names from one table with fields for
canonical spelling, aliases, version, and deprecation diagnostic. Test that
every keyword table entry has a token and that every accepted spelling is
documented.

### 2.8 Incremental lexing needs stronger state contracts

Incremental lexing uses common byte prefix/suffix and line-start fences. A
change inside a string, comment, escape, or numeric literal can affect tokens
far beyond the textual edit. The fallback behavior is safe in many cases, but
the state model is not exposed as a verifiable lexical checkpoint.

**Suggestion**

Store checkpoints at safe boundaries with lexical state:

```text
offset, token index, line-start, comment depth, literal mode, UTF-8 state
```

Resynchronize only when both old and new checkpoints have equal state and a
stable token window. Add adversarial tests for edits that open/close comments,
strings, escapes, and numeric exponents.

## 3. Parser limitations and suggestions

### 3.1 Syntax acceptance and language semantics are mixed

The parser accepts compatibility spellings and has options such as
`allowTopLevelExpressions`, while several constructs are represented by
generic nodes. Generic parameters and arguments are currently skipped by
`skipGenericParams`/`skipGenericArgs`, which means syntax can appear accepted
without preserving its structure.

**Suggestion**

Separate parser responsibilities:

* parse all syntactically valid constructs into explicit nodes;
* leave name lookup, type validity, mutability, and control-flow rules to
  semantic analysis;
* issue deprecation/compatibility diagnostics from a dedicated policy layer.

Do not silently skip syntax that later phases need. If generics are not ready,
produce a feature diagnostic and a `Malformed`/`Unsupported` node with the
complete source range.

### 3.2 Error recovery can obscure the original error

Recovery synchronizes at statement, top-level, and member boundaries and
creates `Malformed` nodes. This is appropriate for IDE use, but broad
synchronization can consume valid tokens and produce cascaded diagnostics.
There is no documented recovery invariant for which tokens each routine may
consume.

**Suggestion**

Define recovery contracts:

* every recovery routine must make progress;
* it must stop before a caller-owned closing delimiter;
* it must preserve the first unexpected token;
* it must record the skipped range;
* it must not emit duplicate diagnostics for the same cursor position.

Add a diagnostic deduplication key based on `(file, span, code)` and fuzz the
parser with truncated and randomly corrupted inputs.

### 3.3 Parser errors and engine diagnostics are duplicated

The AST always records `AST::errors`, and diagnostics may also be forwarded to
`DiagnosticEngine`. This is useful for tooling, but the two records can diverge
in filtering, error limits, text, or severity. Lexer errors are emitted directly
to the engine and are not represented in `AST::errors`.

**Suggestion**

Create one front-end diagnostic record stream with phase, code, severity, span,
and recovery metadata. The AST may retain a filtered index/view of parser
errors, but the engine should be the canonical reporting path. Define whether
lexer diagnostics belong to the parse result and expose a unified
`FrontendResult`.

### 3.4 Parser state is single-use and not obviously reentrant

`Parser` owns mutable lexer, token stream, AST, incremental state, and
diagnostic configuration. Copy/move are disabled. This is reasonable for a
worker-owned parser, but it prevents concurrent calls on one instance and
makes lifecycle misuse easy for clients.

**Suggestion**

Document the thread-affinity contract and enforce it in debug builds, or make
the parser an immutable parse operation over a separate `ParseContext`.
Prefer one parser/context per source file in parallel compilation.

### 3.5 Automatic semicolon behavior is unclear

The parser has `warnMissingSemicolon`, but the lexer discards newlines except
for a line-start flag. The exact conditions under which a semicolon is
required, optional, or synthesized are therefore encoded in parser control
flow rather than in a documented grammar.

**Suggestion**

Choose and document one model:

* explicit semicolons only;
* newline-sensitive semicolon insertion; or
* optional semicolons with unambiguous statement boundaries.

If newline sensitivity is intended, add a virtual `T_NEWLINE` or a parser
event derived from token line flags and test the ambiguous cases.

### 3.6 Grammar precedence and associativity need a single source of truth

Expression parsing uses a Pratt-style precedence function plus separate
assignment/unary helpers. As operators are added, precedence can drift between
the lexer token set, parser table, and semantic lowering.

**Suggestion**

Define an operator table containing token, precedence, associativity, fixity,
assignment status, and AST tag. Generate parser predicates and operator
documentation from that table. Add exhaustive tests for every operator pair and
associativity direction.

### 3.7 Limits are not consistently observable

`maxErrors` and the diagnostic engine's error limit intentionally stop work or
output, but callers need to distinguish a complete parse from a truncated
parse. The AST has errors and the parser has `m_hitLimit`, yet the public AST
does not expose a completeness/status enum.

**Suggestion**

Add a parse status:

```text
Success | Recovered | ErrorLimitReached | Fatal | Cancelled
```

Return it with the AST or in a `ParseResult`. Ensure CLI, IDE, JSON, and
incremental clients can tell "no errors found" from "diagnostics suppressed".

## 4. AST structure limitations and suggestions

### 4.1 The generic union is compact but weakly typed

`Node::Data` deliberately fits a compact payload, but a node's valid fields
depend on its tag. This is efficient and compatible with indexed storage, yet
unsafe access is easy and every new node requires updates to construction,
copy/rebase, dumping, and consumers.

**Suggestion**

Keep the compact physical layout but add typed accessors and validation:

```cpp
FnDeclView fnDecl(Node::NIndex);
CallView call(Node::NIndex);
bool validateNode(Node::NIndex, ASTInvariantError&);
```

Centralize the tag-to-payload schema so `AST.cpp`, dump code, incremental
rebasing, and visitors cannot silently disagree.

### 4.2 `Malformed` is overloaded

One `Malformed` tag represents missing tokens, unsupported syntax, and parser
recovery output. Downstream passes cannot distinguish these causes or decide
whether to continue safely.

**Suggestion**

Add an error-kind field or separate tags such as `Missing`, `Unsupported`, and
`Recovery`. Store the related diagnostic ID and skipped token range. Define
which later phases may inspect malformed subtrees.

### 4.3 AST spans are token-derived and can be lossy

Node spans are calculated through first/last token side tables. Trivia,
missing-token insertion points, and zero-width diagnostics do not naturally
belong to a token range. An inserted or recovered construct may therefore
have a misleading span.

**Suggestion**

Store an optional direct `SourceSpan` table for nodes that need it, especially
synthetic and malformed nodes. Keep token ranges for incremental reuse, but
make source span the public diagnostic coordinate.

### 4.4 Token ownership makes AST lifetime heavier

`AST` owns a copy of the token pool and the full source string. This makes
debugging and token text lookup convenient, but retaining multiple revisions
is expensive and can duplicate source memory during incremental updates.

**Suggestion**

Introduce a `SourceSnapshot`/revision object shared by lexer, AST, and
diagnostics. Use reference-counted immutable source storage or a caller-owned
snapshot. State clearly whether an AST remains valid after its parser/source
is destroyed.

### 4.5 Extra-data lists have fragile conventions

Lists are stored as untyped ranges in `extra_data`; function parameters also
store the return type as the final list entry, and struct initializers store
token/node pairs. These conventions are compact but easy to misuse.

**Suggestion**

Define typed list views for every list kind and encode list kind/arity in
debug validation. Avoid sentinel-plus-count ambiguity by using explicit empty
range semantics. Add an AST verifier that checks every node and every
extra-data range.

### 4.6 Index width and overflow are not addressed

Node, token, and extra-data indexes are 32-bit while vectors use `size_t`.
Large generated files or malicious input can exceed 32-bit ranges and cause
truncation during casts and incremental rebasing.

**Suggestion**

Add checked allocation/index conversion helpers. Return a fatal diagnostic
before an index overflows. Consider configurable 32/64-bit index types for
large-file tooling builds.

### 4.7 No stable visitor or serialization contract is visible

The AST is optimized for internal storage, but consumers need a stable way to
walk nodes without depending on union details. Incremental copying also
depends on manually maintained tag switches.

**Suggestion**

Provide:

* a read-only visitor with exhaustive tag handling;
* a verifier used after parsing and after incremental append;
* a versioned serialization format if AST persistence is planned;
* generated tag metadata for dumps, visitors, and schema validation.

## 5. Diagnostics limitations and suggestions

### 5.1 Source locations need one canonical coordinate model

`SourceSpan` stores byte offsets, while `LineCol` allows either byte or
code-point columns. Emitters and LSP output require different column units.
The ambiguity can cause caret and editor ranges to disagree for non-ASCII
source.

**Suggestion**

Make coordinate units explicit in names and APIs:

```text
ByteOffset, Utf8Column, Utf16Column
```

Centralize conversion in `SourceManager`; never let individual emitters
recompute columns.

### 5.2 Invalid and zero-width spans are under-specified

An invalid span is identified by file ID, but a valid zero-width insertion
point is also possible. Diagnostics at EOF, missing delimiters, and suggested
insertions need a documented distinction.

**Suggestion**

Add `SourceSpan::is_empty()` and an explicit `SpanKind` or insertion-point
helper. Ensure text, JSON, and LSP emitters render zero-width locations
consistently.

### 5.3 Suggestions are not yet a complete fix-it model

`Suggestion` supports one replacement range and an applicability enum. It does
not express a related edit set, edit ordering, conflict detection, or a
stable diagnostic identity. Multi-token fixes may require several edits.

**Suggestion**

Add a `FixIt` containing an ordered, non-overlapping edit list, applicability,
and a stable ID. Validate edits before marking them machine-applicable and
provide a conflict resolver for multiple diagnostics.

### 5.4 Diagnostic identity and deduplication are missing

Diagnostics have codes, messages, labels, and suggestions, but no stable
instance ID or revision. Incremental parsing can re-report unchanged
diagnostics, and multiple phases can report the same root cause.

**Suggestion**

Use a stable key such as `(file revision, primary span, code)` and allow
related diagnostics to reference a parent. IDE clients can then replace
diagnostics for one file revision instead of appending duplicates.

### 5.5 Severity accounting has policy complexity

The engine applies warning suppression, warning-as-error behavior, and error
limits while parser errors may also be retained independently. The public
counts need clear semantics for emitted, suppressed, and stored diagnostics.

**Suggestion**

Document and test a counter contract:

```text
found, emitted, suppressed, promoted, skipped-by-limit
```

Return the final compilation status from the engine rather than deriving it
only from `has_errors()`.

### 5.6 Diagnostic text is not fully localization- or tooling-ready

Codes are stable, but messages are formatted into strings early. Consumers
that need structured arguments, localization, or richer IDE actions must parse
text or duplicate formatting logic.

**Suggestion**

Store a diagnostic template ID plus typed arguments, and render text at the
emitter boundary. Keep the current formatted message as a convenience view.

### 5.7 JSON/LSP schemas need compatibility tests

Text, JSON, and LSP emitters are pluggable, which is good, but their output
contracts should be treated as public APIs. File URI encoding, line/column
conversion, empty spans, notes, and suggestions are easy to regress.

**Suggestion**

Add golden tests for each output format, including Windows paths, Unicode
source, multiple labels, zero-width fixes, suppressed diagnostics, and
incremental replacement of one file's diagnostics.

## 6. Cross-cutting design improvements

### 6.1 Introduce an explicit front-end result

Use a single result object for lexer/parser clients:

```cpp
struct FrontendResult {
    ast::AST ast;
    std::vector<Diagnostic> diagnostics;
    ParseStatus status;
    SourceRevision revision;
};
```

The exact ownership can be adjusted to avoid copying, but the contract should
make source revision, completeness, diagnostics, and AST lifetime explicit.

### 6.2 Add invariant validation in debug and test builds

Validate:

* token ranges are ordered and within source bounds;
* UTF-8 boundaries are not split by token ranges;
* node indexes and extra-data ranges are valid;
* every tag uses only its assigned payload fields;
* child ranges are inside the owning declaration;
* incremental copied declarations point into the new AST;
* diagnostics reference the correct file revision.

Make validation cheap enough for unit tests and optionally enabled in debug
builds, but keep it off in release builds unless requested.

### 6.3 Make grammar, token metadata, and AST metadata data-driven

A single metadata source should describe:

* token spelling and category;
* keyword aliases and language versions;
* operator precedence and associativity;
* AST tag payload schema;
* diagnostic code, default severity, and formatting arguments.

Generate names, lookup tables, visitors, and validation where practical.
This reduces the current risk that macros, lexer switches, parser switches,
AST rebasing, and emitters drift apart.

### 6.4 Strengthen incremental parsing around revisions

Use immutable source revisions and declaration fingerprints:

```text
file ID + revision + token range + declaration hash + lexical checkpoint
```

Reuse only when the token range, lexical state, grammar version, and relevant
configuration match. Invalidate reuse when keyword/operator tables or parser
configuration changes.

### 6.5 Improve fuzzing and property testing

Add fuzz targets for:

* arbitrary bytes to the lexer;
* balanced/unbalanced delimiters to the parser;
* incremental edits against full re-lex/full re-parse;
* AST copy/rebase followed by AST verification;
* diagnostic JSON/LSP output.

Important properties:

1. lexer always advances or returns EOF;
2. parser recovery always makes progress;
3. incremental output matches a fresh full parse for the same source;
4. AST verification succeeds for every non-crashing parse;
5. emitters never produce invalid JSON or out-of-range LSP positions.

## 7. Recommended implementation plan

### Phase 1: correctness and observability

1. Add `T_INVALID` and separate it from EOF.
2. Add checked numeric parsing and scalar-value validation.
3. Define byte/UTF-8/UTF-16 source-position APIs.
4. Add parse status and a unified diagnostic result.
5. Implement an AST verifier and run it in parser tests.
6. Add tests for recovery progress and incremental/full equivalence.

### Phase 2: structured language support

1. Replace skipped generic syntax with explicit nodes or explicit
   unsupported diagnostics.
2. Centralize operator and keyword metadata.
3. Define semicolon/newline behavior in the grammar.
4. Add typed AST views and typed extra-data list accessors.
5. Add documentation-comment retention/attachment.

### Phase 3: tooling and scalability

1. Add source revisions and lexical checkpoints.
2. Add stable diagnostic identities and multi-edit fix-its.
3. Add structured diagnostic templates and golden JSON/LSP tests.
4. Add optional trivia storage for formatting/refactoring tools.
5. Add fuzzing, large-file overflow tests, and performance benchmarks.

## 8. Suggested acceptance criteria

An enhancement should be considered complete only when it has:

* a documented grammar or invariant;
* focused unit tests for valid, invalid, and recovery cases;
* an incremental-versus-full equivalence test where applicable;
* AST verification coverage;
* text, JSON, and LSP diagnostic coverage where locations are involved;
* an explicit compatibility decision for existing syntax such as `func`/`mut`;
* no silent fallback that changes program meaning.

## 9. Current implementation status, limitations, and known issues

The frontend now exposes one `frontend::Compiler` pipeline:

```text
source -> TokenPool -> AST -> SoA CIR -> AIR
```

`AstGen` fingerprints each top-level declaration and reuses a matching CIR
body from the previous revision. CIR instruction columns are stored separately
from their materialized view, and AIR lowering no longer depends on a legacy
AST object model. `FrontendResult` carries the AST, CIR, AIR, diagnostics,
revision, and parse status together. AST verification, explicit empty spans,
and byte/UTF-8/UTF-16 source-location APIs are available.

The following limitations remain and are intentional follow-up work:

* **Lexer/token:** trivia is still discarded, identifier policy is permissive
  for non-ASCII text, numeric and escape payloads do not retain all source
  spelling/status information, and invalid input does not yet have a dedicated
  public token tag.
* **Parser:** generic parameters/arguments are not represented as full nodes,
  newline/semicolon insertion is not a formal grammar rule, and recovery
  diagnostics are still retained separately from engine buffering.
* **AST:** `Node::Data` remains a tag-dependent union; validation currently
  checks storage/range invariants but does not validate every tag-specific
  child field or typed extra-data list.
* **CIR:** body reuse is declaration-fingerprint based. A changed declaration
  is rebuilt, but dependency invalidation, lexical checkpoint matching,
  persistent binary SoA serialization, and stable cross-process body IDs are
  not implemented yet.
* **AIR:** AIR is a structural analysis IR. Name lookup, type checking,
  borrow checking, CFG construction, and target-independent optimization are
  still incomplete.
* **Diagnostics:** identity includes revision, file, span, and code, but the
  engine does not yet replace diagnostics by identity during incremental
  updates. Fix-its remain single-edit suggestions and diagnostic templates are
  formatted before emission.
* **Build/test integration:** the repository's CMake graph now includes the
  parser, CIR, AIR, and frontend sources, but the current Windows environment
  does not provide a configured CMake generator or Catch2 checkout, so full
  test execution must be completed in a configured developer environment.

The current compact AST and incremental architecture should be retained, but
future features should be added through explicit metadata, typed views, source
revision contracts, and verifiable error-recovery rules rather than additional
tag-specific conventions.
