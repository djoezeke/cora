# Forge Language Specification

## 📚 Table of Contents

- [Intoduction]()
  - 1. [**About This Specification**]()
    - 1.1 [Purpose and Scope]()
    - 1.2 [Audience]()
    - 1.3 [Normative Language and Conventions]()
    - 1.4 [Versioning and Stability]()
    - 1.5 [Normative References]()
    - 1.6 [Informative References]()
    - 1.7 [Terms and Definitions]()
  - 2. [**Conformance**]()
    - 2.1 [Conforming Implementations]()
    - 2.2 [Conforming Programs]()
    - 2.3 [Required Diagnostics]()
    - 2.4 [Implementation-Defined Behavior]()
    - 2.5 [Unspecified Behavior]()
    - 2.6 [Undefined Behavior]()
- [Language Fundamentals]()
  - 3. [**Program Structure and Compilation Model**]()
    - 3.1 [Source Files and Translation Units]()
    - 3.2 [Crates, Packages, and Modules]()
    - 3.3 [Separate Compilation and Linking]()
    - 3.4 [Entry Points and Program Startup]()
    - 3.5 [Compilation Phases]()
    - 3.6 [Target Platforms and Target Triples]()
    - 3.7 [Freestanding and Hosted Environments]()
  - 4. [**Lexical Structure**]()
    - 4.1 [Source Encoding]()
    - 4.2 [Whitespace and Line Endings]()
    - 4.3 [Comments]()
    - 4.4 [Tokens]()
    - 4.5 [Identifiers and Keywords]()
    - 4.6 [Literals]()
    - 4.7 [Operators and Punctuation]()
    - 4.8 [Automatic Semicolon Insertion / Statement Termination]()
    - 4.9 [Lexical Ambiguities]()
  - 5. [**Names, Scopes, and Visibility**]()
    - 5.1 [Namespaces]()
    - 5.2 [Declarations and Definitions]()
    - 5.3 [Scopes and Shadowing]()
    - 5.4 [Visibility and Access Control]()
    - 5.5 [Imports and Re-exports]()
    - 5.6 [Name Resolution]()
    - 5.7 [Paths and Qualified Names]()
  - 6. [**Types**]()
    - 6.1 [Type System Overview]()
    - 6.2 [Primitive Types]()
    - 6.3 [Boolean and Integer Types]()
    - 6.4 [Floating-Point Types]()
    - 6.5 [Character and String Types]()
    - 6.6 [Unit and Never Types]()
    - 6.7 [Pointers and References]()
    - 6.8 [Arrays, Slices, and Tuples]()
    - 6.9 [Structs, Enums, and Unions]()
    - 6.10 [Function Types]()
    - 6.11 [Type Aliases and Opaque Types]()
    - 6.12 [Generic Types]()
    - 6.13 [Trait/Interface Types]()
    - 6.14 [Type Layout and Representation]()
    - 6.15 [Type Equivalence and Subtyping]()
    - 6.16 [Variance and Type Parameters]()
    - 6.17 [Dynamically Sized and Unsized Types]()
  - 7. [**Memory Model and Ownership**]()
    - 7.1 [Memory Model Overview]()
    - 7.2 [Values, Places, and References]()
    - 7.3 [Initialization and Drop]()
    - 7.4 [Ownership]()
    - 7.5 [Borrowing and Aliasing]()
    - 7.6 [Lifetimes and Regions]()
    - 7.7 [Mutability]()
    - 7.8 [Move Semantics and Copy Semantics]()
    - 7.9 [Allocation and Deallocation]()
    - 7.10 [Stack, Heap, and Static Storage]()
    - 7.11 [Validity and Undefined Behavior]()
- [Expressions and Statements]()
  - 8. [**Expressions**]()
    - 8.1 [Expression Categories]()
    - 8.2 [Place Expressions and Value Expressions]()
    - 8.3 [Literals and Constants]()
    - 8.4 [Paths and Names]()
    - 8.5 [Operators]()
    - 8.6 [Operator Precedence and Associativity]()
    - 8.7 [Arithmetic and Overflow Behavior]()
    - 8.8 [Comparison and Logical Operators]()
    - 8.9 [Bitwise Operators]()
    - 8.10 [Assignment and Compound Assignment]()
    - 8.11 [Calls and Method Calls]()
    - 8.12 [Field Access and Indexing]()
    - 8.13 [Casts and Conversions]()
    - 8.14 [Blocks and Control-Flow Expressions]()
    - 8.15 [Match Expressions]()
    - 8.16 [Closures and Function Expressions]()
    - 8.17 [Aggregate Expressions]()
    - 8.18 [Evaluation Order]()
    - 8.19 [Constant Expressions]()
  - 9. [**Statements and Control Flow**]()
    - 9.1 [Blocks]()
    - 9.2 [Item Declarations as Statements]()
    - 9.3 [Let Bindings]()
    - 9.4 [Assignment Statements]()
    - 9.5 [Expression Statements]()
    - 9.6 [Conditional Statements]()
    - 9.7 [Loops]()
    - 9.8 [Break, Continue, and Return]()
    - 9.9 [Match and Pattern Matching]()
    - 9.10 [Defer / Scope Exit]()
    - 9.11 [Unreachable Code and Divergence]()
  - 10. [**Patterns**]()
    - 10.1 [Pattern Syntax]()
    - 10.2 [Literal Patterns]()
    - 10.3 [Binding Patterns]()
    - 10.4 [Wildcard Patterns]()
    - 10.5 [Tuple and Struct Patterns]()
    - 10.6 [Enum and Variant Patterns]()
    - 10.7 [Reference Patterns]()
    - 10.8 [Range Patterns]()
    - 10.9 [Or-Patterns and Guards]()
    - 10.10 [Exhaustiveness and Reachability]()
- [Declarations and Abstraction]()
  - 11. [**Items and Declarations**]()
    - 11.1 [Item Categories]()
    - 11.2 [Visibility and Attributes]()
    - 11.3 [Constants and Statics]()
    - 11.4 [Type Declarations]()
    - 11.5 [Function Declarations]()
    - 11.6 [Module Declarations]()
    - 11.7 [Trait/Interface Declarations]()
    - 11.8 [Implementation Blocks]()
    - 11.9 [Foreign Declarations]()
    - 11.10 [Conditional Compilation]()
  - 12. [**Functions**]()
    - 12.1 [Function Items]()
    - 12.2 [Parameters and Return Types]()
    - 12.3 [Local Bindings]()
    - 12.4 [Function Bodies]()
    - 12.5 [Calling Conventions]()
    - 12.6 [Function Pointers and Closures]()
    - 12.7 [Inlining and Optimization Hints]()
    - 12.8 [Recursion and Tail Calls]()
    - 12.9 [Extern Functions]()
  - 13. [**Generics**]()
    - 13.1 [Type Parameters]()
    - 13.2 [Const Parameters]()
    - 13.3 [Generic Functions]()
    - 13.4 [Generic Types]()
    - 13.5 [Bounds and Constraints]()
    - 13.6 [Associated Types and Constants]()
    - 13.7 [Coherence and Overlap]()
    - 13.8 [Monomorphization]()
    - 13.9 [Generic ABI Considerations]()
  - 14. [**Traits / Interfaces**]()
    - 14.1 [Trait Declarations]()
    - 14.2 [Methods and Associated Items]()
    - 14.3 [Implementations]()
    - 14.4 [Default Methods]()
    - 14.5 [Trait Objects and Dynamic Dispatch]()
    - 14.6 [Supertraits and Trait Bounds]()
    - 14.7 [Blanket Implementations]()
    - 14.8 [Object Safety / Dyn Compatibility]()
    - 14.9 [Coherence Rules]()
  - 15. [**Modules and Packages**]()
    - 15.1 [Module System]()
    - 15.2 [Package Manifest Model]()
    - 15.3 [Dependencies and Versioning]()
    - 15.4 [Visibility Across Modules]()
    - 15.5 [Preludes and Standard Library Root]()
    - 15.6 [Namespace Organization]()
- [Systems Programming Facilities]()
  - 16. [**Unsafe Forge and Low-Level Operations**]()  
        16.1 [Unsafe Blocks and Functions]()  
        16.2 [Raw Pointers]()  
        16.3 [Pointer Arithmetic]()  
        16.4 [Dereferencing and Validity]()  
        16.5 [Transmutation and Reinterpretation]()  
        16.6 [Inline Assembly]()  
        16.7 [Volatile Operations]()  
        16.8 [Intrinsics]()  
        16.9 [Unchecked Operations]()  
        16.10 [Safety Invariants and Undefined Behavior]()
  - 17. [**Data Layout and ABI**]()
    - 17.1 [Type Layout Rules]()
    - 17.2 [Alignment and Size]()
    - 17.3 [Representation Attributes]()
    - 17.4 [Struct and Union Layout]()
    - 17.5 [Enum Layout]()
    - 17.6 [Calling Conventions]()
    - 17.7 [Name Mangling]()
    - 17.8 [Symbol Visibility and Linkage]()
    - 17.9 [Application Binary Interface]()
    - 17.10 [Platform-Specific ABI Notes]()
  - 18. [**Foreign Function Interface**]()
    - 18.1 [External Declarations]()
    - 18.2 [Calling Foreign Functions]()
    - 18.3 [Exporting Forge Functions]()
    - 18.4 [C ABI Compatibility]()
    - 18.5 [Variadic Functions]()
    - 18.6 [Foreign Types and Layout]()
    - 18.7 [Callbacks and Function Pointers]()
    - 18.8 [Linking and Symbols]()
  - 19. [**Concurrency and Atomics**]()
    - 19.1 [Threads and Execution Contexts]()
    - 19.2 [Memory Ordering]()
    - 19.3 [Atomic Types and Operations]()
    - 19.4 [Synchronization Primitives]()
    - 19.5 [Data Races and Undefined Behavior]()
    - 19.6 [Send/Sync or Thread-Safety Traits]()
    - 19.7 [Static Initialization and Destruction]()
  - 20. [**Compile-Time Evaluation and Metaprogramming**]()
    - 20.1 [Constant Evaluation]()
    - 20.2 [Compile-Time Functions]()
    - 20.3 [Compile-Time Execution Limits]()
    - 20.4 [Macros and Expansion]()
    - 20.5 [Procedural Macros / Plugins]()
    - 20.6 [Reflection and Introspection]()
    - 20.7 [Conditional Compilation and Configuration]()
    - 20.8 [Attributes and Pragmas]()
  - 21. [**Error Handling and Runtime Behavior**]()
    - 21.1 [Recoverable Errors]()
    - 21.2 [Unrecoverable Errors and Panics]()
    - 21.3 [Abort and Termination]()
    - 21.4 [Unwinding]()
    - 21.5 [Runtime Hooks]()
    - 21.6 [Allocator Interface]()
    - 21.7 [Program Exit]()
- [Standard Language Environment]()
  - 22. [**Standard Library Boundary**]()
    - 22.1 [Core Language vs Standard Library]()
    - 22.2 [Core Prelude]()
    - 22.3 [Platform Abstraction Layer]()
    - 22.4 [Allocator and Memory APIs]()
    - 22.5 [I/O and OS Interfaces]()
    - 22.6 [Concurrency APIs]()
    - 22.7 [Error Types]()
  - 23. [**Attributes and Directives**]()
    - 23.1 [Built-in Attributes]()
    - 23.2 [Representation Attributes]()
    - 23.3 [Optimization and Codegen Attributes]()
    - 23.4 [Conditional Compilation Attributes]()
    - 23.5 [Diagnostic Attributes]()
    - 23.6 [Deprecation and Stability Attributes]()
    - 23.7 [User-Defined Attributes]()
  - 24. [**Diagnostics and Tooling Requirements**]()
    - 24.1 [Required Errors]()
    - 24.2 [Required Warnings]()
    - 24.3 [Lints and Configurable Diagnostics]()
    - 24.4 [Diagnostic Severity and Suppression]()
    - 24.5 [Source Locations and Spans]()
    - 24.6 [Implementation Limits]()
- [Formal Appendices]()
  - 25. [**Formal Grammar**]()
    - 25.1 [Lexical Grammar]()
    - 25.2 [Syntactic Grammar]()
    - 25.3 [Grammar Notation]()
    - 25.4 [Reserved Words]()
    - 25.5 [Reserved Punctuation and Future Compatibility]()
  - 26. [**Built-in Definitions**]()
    - 26.1 [Built-in Types]()
    - 26.2 [Built-in Traits/Interfaces]()
    - 26.3 [Built-in Macros]()
    - 26.4 [Intrinsics]()
    - 26.5 [Compiler-Known Attributes]()
    - 26.6 [Target-Specific Definitions]()
  - 27. [**Platform and Target Model**]()
    - 27.1 [Target Triple Format]()
    - 27.2 [Data Models]()
    - 27.3 [Endianness]()
    - 27.4 [Pointer Width and Address Space]()
    - 27.5 [Supported Calling Conventions]()
    - 27.6 [Freestanding Target Requirements]()
  - 28. [**Compatibility and Evolution**]()
    - 28.1 [Editions and Language Versions]()
    - 28.2 [Stability Guarantees]()
    - 28.3 [Deprecation Policy]()
    - 28.4 [Semver for Language Features]()
    - 28.5 [Migration Rules]()

## Intoduction

**Version:** 0.1-draft
**Status:** Draft
**Editors:** _TBD_

## 1. About This Specification

### 1.1 Purpose and Scope

1.1.1 This document is the **normative specification** of the Forge programming language. It defines the syntax, static semantics, dynamic semantics, and standard environment of Forge, and it establishes the requirements that a conforming implementation and a conforming program must satisfy.

1.1.2 The **scope** of this specification includes:

- the lexical structure and syntactic grammar of Forge source text (§4, §25);
- the static semantics: name resolution, type checking, borrow checking, and coherence (§5–§7, §13–§14);
- the dynamic semantics: evaluation order, memory model, ownership, and undefined behavior (§7–§9, §16);
- the systems-programming facilities: unsafe code, data layout, ABI, FFI, concurrency, and compile-time evaluation (§16–§21);
- the standard environment: the standard library boundary, attributes, and diagnostic requirements (§22–§24);
- the formal appendices: grammar, built-in definitions, platform model, and evolution rules (§25–§28, Annexes A–L).

  1.1.3 The **scope** of this specification **does not** include:

- the implementation strategy of a compiler, linker, or runtime, except where the observable behavior of a program depends on it;
- the package manager's user interface, the build system's command-line syntax, or the documentation generator's output format, except where these affect the conformance of a program;
- the behavior of a program that executes undefined behavior (§7.11), which this specification deliberately leaves unconstrained;
- the performance characteristics of a compiled program, except where a program's correctness depends on a specified ordering or atomicity guarantee.

  1.1.4 This specification **SHALL** be the single authoritative reference for the meaning of a Forge program. Where this specification and any other document (including tutorials, reference manuals, and compiler documentation) disagree, this specification **SHALL** prevail, except that an implementation's documentation of its implementation-defined choices (§2.4) is authoritative for those choices.

  1.1.5 This specification **MAY** be supplemented by **informative** documents that explain, illustrate, or provide guidance. An informative document **MUST NOT** be interpreted as adding requirements beyond those stated in this specification.

  1.1.6 The **target audience** of the normative text is specified in §1.2. The **editors** are responsible for resolving ambiguities and for maintaining the change log (Annex I). The **governance process** for accepting changes is outside the scope of this document.

### 1.2 Audience

1.2.1 This specification is written for the following audiences:

- **Compiler and toolchain implementors**, who must produce a conforming implementation of Forge and therefore must understand every requirement of this document;
- **Language designers and reviewers**, who must evaluate proposed changes for consistency with the existing specification and for their impact on conformance;
- **Library authors**, who must understand the semantics of the language to write correct and portable libraries;
- **Advanced users and systems programmers**, who must understand the memory model, the unsafe rules, and the ABI requirements to write correct low-level code;
- **Verification and testing engineers**, who must derive test suites from the requirements of this document.

  1.2.2 This specification **ASSUMES** that the reader is familiar with the general concepts of statically typed, compiled programming languages, including types, scopes, functions, and memory. It **ASSUMES** familiarity with at least one systems programming language (such as C, C++, or Rust) but **MUST NOT** assume familiarity with Forge itself.

  1.2.3 Where this specification introduces a term that is specific to Forge, the term is defined at its first use and is listed in the Glossary (Annex A). A term that is used with its ordinary meaning in the programming-languages literature is not separately defined.

  1.2.4 The **non-normative** sections are marked as such. Informative examples, notes, and rationales are provided to aid understanding but **MUST NOT** be read as adding requirements.

### 1.3 Normative Language and Conventions

1.3.1 The key words **MUST**, **MUST NOT**, **REQUIRED**, **SHALL**, **SHALL NOT**, **SHOULD**, **SHOULD NOT**, **RECOMMENDED**, **MAY**, and **OPTIONAL** in this document are to be interpreted as described in RFC 2119.

1.3.2 In particular:

- **MUST** and **SHALL** indicate an absolute requirement. An implementation or program that does not satisfy a **MUST** requirement is non-conforming.
- **MUST NOT** and **SHALL NOT** indicate an absolute prohibition. An implementation or program that violates a **MUST NOT** requirement is non-conforming.
- **SHOULD** and **SHOULD NOT** indicate a recommendation. An implementation or program **MAY** deviate from a **SHOULD** requirement, but the deviation **MUST** be documented and **SHOULD** be justified.
- **MAY** and **OPTIONAL** indicate a permissible choice. An implementation or program that exercises a **MAY** is conforming, and an implementation or program that does not exercise a **MAY** is also conforming.
- **RECOMMENDED** is synonymous with **SHOULD**.

  1.3.3 The word **MUST** in a requirement on a **program** means that a program that does not satisfy the requirement is not a conforming program (§2.2). The word **MUST** in a requirement on an **implementation** means that an implementation that does not satisfy the requirement is not a conforming implementation (§2.1).

  1.3.4 Where this specification states that an implementation **MUST** reject a program, the implementation **MUST** produce a **required error** (§24.1) and **MUST NOT** produce an executable artifact for the program.

  1.3.5 Where this specification states that an implementation **MUST** produce a **required warning** (§24.2), the implementation **MUST** produce a diagnostic at the warning level, but the diagnostic **MUST NOT** prevent the production of an executable artifact unless the lint is configured to be an error (§24.3).

  1.3.6 Where this specification states that behavior is **implementation-defined** (§2.4), the implementation **MUST** choose a behavior and **MUST** document it, but the choice **MUST NOT** be constrained by this specification.

  1.3.7 Where this specification states that behavior is **unspecified** (§2.5), the implementation **MAY** choose any behavior, and the choice **NEED NOT** be documented.

  1.3.8 Where this specification states that behavior is **undefined** (§2.6), the program is not conforming if it exhibits that behavior, and the implementation **MAY** assume that the behavior does not occur.

  1.3.9 **Grammar notation** is specified in §25.3. A production that is written in this document is normative unless the surrounding text states otherwise. A production that appears only in an informative example is not normative.

  1.3.10 **Code examples** are written in Forge unless the surrounding text states otherwise. An example that is not labeled as **informative** or **non-normative** is normative and **MUST** behave as described.

  1.3.11 **Section references** are written in the form `§N.M` or `§N.M.K`. A reference to an annex is written `Annex X`. A reference to a table is written `Table N`. A reference to a production is by name (for example, `type`).

  1.3.12 The **modal verb "can"** is used to describe possibility or capability, not requirement. It is not a normative keyword.

  1.3.13 The **present tense** is used for statements that are always true of a conforming implementation or program. The **future tense** is avoided except in the discussion of compatibility and evolution (§28).

  1.3.14 The **singular "they"** and the **plural "implementations"** are used to avoid implying a single implementation. Where this specification refers to "the implementation", it means the implementation currently compiling or executing the program.

### 1.4 Versioning and Stability

1.4.1 This document specifies **Forge version 0.1-draft**. The version number is used to identify the specification and to track changes. A version is of the form `MAJOR.MINOR[-STATUS]`, where `MAJOR` and `MINOR` are non-negative integers and `STATUS` is an optional pre-release label (for example, `draft`, `alpha`, `beta`).

1.4.2 The **language version** is the version of the language that a program is written against. The language version is specified in the package manifest (§15.2) and **MUST** be supported by the toolchain that compiles the program.

1.4.3 The **specification version** is the version of this document. The specification version and the language version **MAY** differ, but a specification version **MUST** fully specify the language version it declares.

1.4.4 The **stability guarantee** of §28.2 applies to the language version, not to this document. A future specification version **MAY** change the language in ways that are governed by §28.

1.4.5 A **draft** specification (such as this one) **MUST NOT** be used as the basis for a stable implementation. A draft **MAY** change in incompatible ways between revisions. An implementation that targets a draft **MUST** document the draft revision it implements.

1.4.6 A **stable** specification **MUST** satisfy the stability guarantee of §28.2. A stable specification **MAY** be revised to fix defects, but the revision **MUST NOT** introduce incompatible changes without an edition boundary (§28.1).

1.4.7 The **standard library version** **MUST** match the toolchain version (§22.1.7). A program that uses the standard library **MUST** be compiled with a toolchain whose standard library version is compatible with the program's language version.

1.4.8 The **edition** of a crate (§28.1.2) **MUST** be specified in the package manifest. The default edition **MUST** be the latest stable edition supported by the toolchain.

### 1.5 Normative References

1.5.1 The following documents are **normatively referenced** by this specification. A conforming implementation **MUST** satisfy the requirements of these documents to the extent that this specification depends on them.

1.5.2 **RFC 2119**, "Key words for use in RFCs to Indicate Requirement Levels", S. Bradner, March 1997. This document defines the interpretation of the normative keywords used in §1.3.

1.5.3 **IEEE 754-2019**, "IEEE Standard for Floating-Point Arithmetic". This document defines the semantics of `f32` and `f64` (§6.4), including the representation of NaN, infinity, and subnormal values.

1.5.4 The **target platform's C ABI** for each supported target. The C ABI defines the calling convention, struct layout, and enum representation that Forge's `extern "C"` items must match (§17.6, §17.9, §18.4). The specific C ABI documents are listed in §27.10 of the platform model (Annex not yet published).

1.5.5 The **Unicode Standard**, version 15.0 or later. The Unicode Standard defines the character properties used by the lexical grammar (§4.5.3) and the `char` type's validity invariant (§6.5.1).

1.5.6 **ISO/IEC 9899:2018** ("C18") for the FFI's C type mapping (§18.4.2). Where this specification's C type mapping and C18 disagree, C18 **SHALL** prevail for the mapping of C types to Forge types.

1.5.7 The **target platform's loader and linker documentation** for the symbol visibility, linkage, and relocation rules that this specification references (§17.8). The specific documents are implementation-defined and are documented by each implementation.

1.5.8 A conforming implementation **MUST** document the exact revisions of the normative references that it conforms to. An implementation **MUST NOT** claim conformance to a revision of a normative reference that it does not implement.

### 1.6 Informative References

1.6.1 The following documents are **informatively referenced** by this specification. They provide background, rationale, or guidance but **MUST NOT** be read as adding requirements.

1.6.2 **ISO/IEC 14882:2020** ("C++20") for background on the memory model and on the comparison of Forge's concurrency facilities with C++'s. The memory model of §19.2 is modeled on C++'s, but Forge's rules are specified independently.

1.6.3 **The Rust Programming Language** (Klabnik and Nichols) for background on ownership and borrowing. Forge's ownership model is inspired by Rust's but is specified independently.

1.6.4 **The Rust Reference** and **The Rustonomicon** for background on unsafe code and data layout. These documents are informative only; Forge's unsafe rules are specified in §16 and its layout rules in §17.

1.6.5 **Boehm, H.-J., and Adve, S. V.**, "Foundations of the C++ Concurrency Memory Model" for background on the happens-before relation and on data-race freedom.

1.6.6 **Tofte, M., and Talpin, J.-P.**, "Region-Based Memory Management" and **Clarke, D., et al.**, "Ownership Types for Safe Programming" for background on the region and ownership analyses that inform Forge's borrow checker.

1.6.7 The **Forge Implementation Guide** (to be published) for guidance on implementing the specification. The guide is informative and is not part of this specification.

1.6.8 The **Forge Language Design Notes** (to be published) for rationale and design history. The notes are informative and are not part of this specification.

1.6.9 An **informative reference** **MUST NOT** be cited as the sole authority for a requirement. If this specification depends on a fact from an informative reference, the fact **MUST** also be stated in this specification.

### 1.7 Terms and Definitions

1.7.1 This section defines terms that are used throughout the specification with a specific meaning. Terms that are defined in a specific chapter are listed in the Glossary (Annex A) with a reference to their defining section.

1.7.2 **Program.** A program is a collection of crates that are linked into an executable or library, together with the standard library and any other libraries they depend on. A program is the unit of conformance for the dynamic semantics.

1.7.3 **Crate.** A crate is the unit of compilation and linking (§3.2.1). A crate consists of a crate root and the modules it declares.

1.7.4 **Package.** A package is a collection of one or more crates described by a manifest (§15.2.1). A package is the unit of distribution and dependency management.

1.7.5 **Module.** A module is a named scope that groups items (§11.6.1). Modules form a tree rooted at the crate root.

1.7.6 **Item.** An item is a named declaration at module scope (§11.1.1). Items include functions, constants, statics, types, traits, implementations, modules, and use declarations.

1.7.7 **Declaration.** A declaration introduces a name into a scope (§5.2.1). A declaration that also provides a value or a body is a definition.

1.7.8 **Definition.** A definition is a declaration that provides the value or body of the declared entity (§5.2.1). A definition is a special case of a declaration.

1.7.9 **Scope.** A scope is a region of source in which a name binding is visible (§5.3.1). Forge has crate, module, function, block, pattern, and generic scopes.

1.7.10 **Binding.** A binding is the association between a name and the entity it denotes (§5.2.1). A binding is introduced by a declaration.

1.7.11 **Value.** A value is a typed, self-contained datum (§7.2.1). A value may be moved, copied, or dropped.

1.7.12 **Place.** A place is a location in memory that can hold a value (§7.2.2). A place is denoted by a place expression.

1.7.13 **Expression.** An expression is a syntactic construct that denotes a value, a place, a computation, or a divergence (§8.1.1). Expressions have types and evaluation classes.

1.7.14 **Statement.** A statement is a syntactic construct that performs an action and does not itself produce a value (§9). Statements are used inside blocks.

1.7.15 **Type.** A type is a classification of values that determines their representation, the operations that are valid on them, and their interaction with the memory model (§6.1.1).

1.7.16 **Trait.** A trait is a set of associated items that a type may implement (§14.1.1). Traits provide a form of bounded polymorphism.

1.7.17 **Implementation (of a trait).** A trait implementation is a block that provides a trait's associated items for a specific type (§14.3.1). A type may implement a trait at most once.

1.7.18 **Generic parameter.** A generic parameter is a type, lifetime, or const parameter of an item (§13.1.1). A generic item is instantiated by providing arguments for its parameters.

1.7.19 **Instantiation.** An instantiation is the result of substituting concrete arguments for a generic item's parameters (§13.4.3). Two instantiations with the same arguments are the same item.

1.7.20 **Monomorphization.** Monomorphization is the process of compiling a separate copy of a generic item for each distinct instantiation (§13.8.1).

1.7.21 **Unsafe operation.** An unsafe operation is an operation whose correctness the compiler cannot verify (§16.1.4). An unsafe operation **MUST** occur inside an unsafe block or an unsafe function body.

1.7.22 **Safety invariant.** A safety invariant is a property that a value, reference, or function must satisfy for unsafe code to be correct (§16.10.1). Safety invariants are documented with each unsafe item.

1.7.23 **Undefined behavior.** Undefined behavior is behavior that this specification does not define and that a conforming program **MUST NOT** exhibit (§2.6, §7.11.2). An implementation may assume that undefined behavior does not occur.

1.7.24 **Implementation-defined behavior.** Implementation-defined behavior is behavior that an implementation **MUST** choose and **MUST** document, but whose choice is not constrained by this specification (§2.4).

1.7.25 **Unspecified behavior.** Unspecified behavior is behavior that an implementation **MAY** choose without documenting it (§2.5). A program **MUST NOT** rely on unspecified behavior.

1.7.26 **Diagnostic.** A diagnostic is a message that the compiler emits about the program (§24.4.1). Diagnostics have severities and spans.

1.7.27 **Required error.** A required error is a diagnostic that an implementation **MUST** produce for a non-conforming program and that **MUST** prevent code generation (§24.1.1).

1.7.28 **Required warning.** A required warning is a diagnostic that an implementation **MUST** produce for a program that is likely incorrect, but that **MUST NOT** prevent code generation unless promoted to an error (§24.2.1).

1.7.29 **Conforming implementation.** A conforming implementation is a compiler or interpreter that satisfies all the requirements of this specification on implementations (§2.1).

1.7.30 **Conforming program.** A conforming program is a program that satisfies all the requirements of this specification on programs and that does not exhibit undefined behavior (§2.2).

1.7.31 The **Glossary** (Annex A) provides additional terms and their defining sections. A term that is used in this Intoduction with a specific meaning is listed in the Glossary.

## 2. Conformance

### 2.1 Conforming Implementations

2.1.1 A **conforming implementation** is a compiler, interpreter, or other tool that processes Forge source text and satisfies all the requirements of this specification on implementations.

2.1.2 A conforming implementation **MUST**:

- accept every conforming program that does not exceed the implementation's documented limits (§24.6);
- reject every program that contains a required error (§24.1);
- produce a required warning for every program that contains a construct listed in §24.2.2;
- implement the semantics of every construct specified in this document;
- document its choices for every implementation-defined behavior (Annex G);
- document its limits for the quantities listed in §24.6.1;
- implement the platform model of §27 for every target it supports;
- implement the standard environment of Part V to the extent that the target supports it.

  2.1.3 A conforming implementation **MUST NOT**:

- accept a program that contains a required error;
- silently accept a program that uses an unstable feature without the enabling feature (§23.6.4);
- silently ignore an unrecognized attribute (§11.2.5);
- produce an executable artifact for a program that contains a required error;
- introduce undefined behavior into a conforming program through optimization (§24.6.8);
- change the observable behavior of a program through optimization, except where the behavior is unspecified or implementation-defined.

  2.1.4 A conforming implementation **MAY**:

- reject a program that is not conforming for a reason that this specification does not require to be diagnosed, provided the rejection is justified by the specification;
- accept a program that is not conforming if the program would be conforming under a different interpretation of an ambiguous construct, provided the interpretation is documented;
- provide additional diagnostics, lints, or warnings that this specification does not require;
- provide additional language features that are enabled only with `#![feature(...)]` (§23.1.7).

  2.1.5 A conforming implementation **SHOULD** provide a **debug runtime** (§24.6.6) that detects a subset of undefined behavior. The absence of a debug runtime **MUST** be documented, but it does not make the implementation non-conforming.

  2.1.6 A conforming implementation **MUST** be **deterministic**: compiling the same source with the same options and the same toolchain version **MUST** produce the same diagnostics and the same executable artifact (subject to the implementation's use of addresses and timestamps in the artifact, which **MAY** vary).

  2.1.7 A conforming implementation **MUST** document the exact version of the specification that it conforms to. An implementation **MUST NOT** claim conformance to a draft specification without documenting the draft revision.

  2.1.8 An implementation that conforms to a **draft** specification **MUST** be labeled as a draft implementation. A draft implementation **MAY** deviate from the draft in ways that are documented, but the deviations **MUST NOT** be presented as conformance to a stable specification.

### 2.2 Conforming Programs

2.2.1 A **conforming program** is a program that satisfies all the requirements of this specification on programs and that does not exhibit undefined behavior when executed.

2.2.2 A conforming program **MUST**:

- be lexically valid (§4.1);
- have a valid syntax tree according to the grammar (§25.2);
- pass name resolution (§5.6) and type checking (§6);
- pass the borrow checker (§7.5.4);
- satisfy every bound on every generic item it instantiates (§13.5);
- be coherent with respect to every trait implementation it contains or depends on (§14.9);
- contain no non-exhaustive match (§8.15.4);
- contain no use of an unsafe operation outside an unsafe context (§16.1.8);
- contain no use of an unstable feature without the enabling feature (§23.6.4);
- avoid every form of undefined behavior listed in Annex F;
- not rely on unspecified behavior in a way that would change its meaning.

  2.2.3 A conforming program **MAY** rely on implementation-defined behavior, but the program's meaning is determined by the implementation's documented choice. A program that depends on an implementation-defined choice **SHOULD** document the choice it depends on.

  2.2.4 A conforming program **MUST NOT** rely on:

- the layout of a type without a `#[repr(...)]` attribute (§17.1.4);
- the calling convention `forge` (§12.5.2);
- the order of static initialization (§11.3.10);
- the order of static destruction (§19.7.4);
- the contents of uninitialized memory;
- the behavior of a data race (§19.5.1);
- the behavior of a program that exceeds an implementation limit (§24.6.4).

  2.2.5 A program that is otherwise conforming but that exceeds an implementation's documented limit **MUST** be rejected by the implementation with a diagnostic. A program that is rejected for exceeding a limit is not a conforming program for that implementation, but it **MAY** be a conforming program for a different implementation with a larger limit.

  2.2.6 A program that is rejected by an implementation for a reason that this specification does not require is **not** a conforming program for that implementation, but the implementation's rejection **MUST** be justified by the specification. An implementation **MUST NOT** reject a conforming program for a reason that is not justified by the specification.

  2.2.7 The **conformance** of a program is determined by its behavior on a conforming implementation, not by the implementation's acceptance of the program. An implementation **MAY** accept a program that is not conforming, but the program is not conforming for that implementation.

### 2.3 Required Diagnostics

2.3.1 A **required diagnostic** is a diagnostic that a conforming implementation **MUST** produce for a specific class of program. Required diagnostics are divided into required errors (§24.1) and required warnings (§24.2).

2.3.2 A required error **MUST** be produced for every program that contains a construct listed in §24.1.2. A program that contains a required error **MUST NOT** be compiled to an executable artifact.

2.3.3 A required warning **MUST** be produced for every program that contains a construct listed in §24.2.2. A program that contains a required warning **MAY** be compiled to an executable artifact unless the lint is configured to be an error.

2.3.4 A required diagnostic **MUST** include:

- a **message** that describes the problem;
- a **span** that identifies the source location of the problem (§24.5);
- a **code** that identifies the class of the diagnostic (Annex H).

  2.3.5 A required diagnostic **SHOULD** include:

- a **suggestion** that describes a possible fix;
- a **note** that explains why the construct is problematic;
- a **reference** to the relevant section of this specification.

  2.3.6 An implementation **MAY** suppress a required warning with a lint configuration (§24.3), but the suppression **MUST NOT** apply to a required error. A required error **MUST** be produced regardless of any `allow` attribute.

  2.3.7 An implementation **MUST NOT** produce a required diagnostic for a conforming program. A false positive is a defect in the implementation, not in the program.

  2.3.8 An implementation **MUST NOT** omit a required diagnostic for a non-conforming program. A false negative is a defect in the implementation, and the program's non-conformance is not cured by the omission.

### 2.4 Implementation-Defined Behavior

2.4.1 **Implementation-defined behavior** is behavior that a conforming implementation **MUST** choose and **MUST** document, but whose choice is not constrained by this specification.

2.4.2 The implementation-defined behaviors of Forge are listed in Annex G. For each, a conforming implementation **MUST** document its choice.

2.4.3 An implementation **MUST NOT** change an implementation-defined choice without a deprecation cycle (§28.3), except where the change is required to fix a defect.

2.4.4 An implementation **MAY** make an implementation-defined choice that is **not** the same as another implementation's choice. Programs that depend on an implementation-defined choice are portable only to implementations that make the same choice.

2.4.5 An implementation **MUST** document its implementation-defined choices in a form that is accessible to program authors. The documentation **SHOULD** include the choice, its rationale, and any known portability implications.

2.4.6 An implementation **MUST NOT** make an implementation-defined choice that violates a requirement of this specification. For example, an implementation **MUST NOT** choose a layout for a `#[repr(C)]` type that does not match the target's C ABI.

2.4.7 The choice of a default overflow mode (§8.7.3) and the choice of a default calling convention (§12.5.2) are implementation-defined. A program that depends on these choices **MUST** specify them explicitly (for example, by using checked operations or `extern "C"`).

2.4.8 The layout of a type without a `#[repr(...)]` attribute (§17.1.4) is implementation-defined, but it **MUST** be stable for a given implementation and target. A program **MUST NOT** rely on the layout across implementations or across versions of the same implementation.

### 2.5 Unspecified Behavior

2.5.1 **Unspecified behavior** is behavior for which this specification provides no requirements. An implementation **MAY** choose any behavior for an unspecified construct, and the choice **NEED NOT** be documented.

2.5.2 The unspecified behaviors of Forge include:

- the order of evaluation of subexpressions that this specification does not order (§8.18);
- the exact layout of a type without a `#[repr(...)]` attribute, within the constraints of §17.1;
- the exact set of registers used by a function with the `forge` calling convention;
- the exact size of an enum with the niche optimization, when no `#[repr(...)]` attribute is present;
- the exact behavior of a program that exceeds an implementation limit, subject to §24.6.4.

  2.5.3 A conforming program **MUST NOT** rely on unspecified behavior. A program that relies on unspecified behavior is not guaranteed to behave correctly on any implementation, including the implementation that produced the behavior.

  2.5.4 An implementation **MAY** make an unspecified behavior deterministic (for example, always evaluate left to right) or nondeterministic (for example, evaluate in an order determined by the optimizer). An implementation **MUST NOT** make an unspecified behavior that is required to be deterministic by another section of this specification nondeterministic.

  2.5.5 An implementation **MUST NOT** use an unspecified behavior to introduce undefined behavior into a conforming program. For example, an implementation **MUST NOT** choose an evaluation order that causes a data race in a program that would not have a data race under any other order.

  2.5.6 An implementation **SHOULD** document its choices for unspecified behaviors that are observable in ordinary programs, such as the evaluation order of function arguments. The documentation is informative and **MUST NOT** be read as adding a requirement.

### 2.6 Undefined Behavior

2.6.1 **Undefined behavior** is behavior for which this specification provides no requirements and which a conforming program **MUST NOT** exhibit. A program that exhibits undefined behavior is non-conforming, regardless of whether the implementation detects the behavior.

2.6.2 The undefined behaviors of Forge are listed in Annex F. The list is exhaustive: a behavior that is not listed in Annex F is not undefined behavior, even if it is surprising or implementation-dependent.

2.6.3 A conforming implementation **MAY** assume that a program does not exhibit undefined behavior. In particular, the optimizer **MAY** assume that:

- a reference is non-null and aligned (§7.11.1);
- a value of a type is valid for its type (§7.11.1);
- a data race does not occur (§19.5.1);
- an unsafe operation's safety invariants are satisfied (§16.10.1);
- a pointer's provenance is not violated (§16.4.5).

  2.6.4 A conforming implementation **MUST NOT** introduce undefined behavior into a conforming program. An optimization that turns a conforming program into a program with undefined behavior is a defect in the implementation, not a consequence of the specification.

  2.6.5 A conforming implementation **MUST NOT** be required to detect undefined behavior. The absence of detection does not make the program conforming, and the presence of detection does not make the program non-conforming.

  2.6.6 A conforming implementation **MAY** provide a **debug runtime** (§24.6.6) that detects a subset of undefined behavior and reports it as a diagnostic at runtime. The runtime detection **MUST NOT** change the behavior of a conforming program, except by adding the diagnostic.

  2.6.7 A conforming implementation **MUST NOT** define behavior that Annex F lists as undefined. An implementation **MAY** extend the language with a well-defined behavior for a construct that is undefined in this specification, but the extension **MUST** be opt-in and **MUST NOT** be enabled by default.

  2.6.8 A program that exhibits undefined behavior **MUST** be considered non-conforming even if the implementation produces an executable artifact and the artifact appears to work. The artifact's behavior is not evidence of conformance.

  2.6.9 The distinction between **undefined behavior** and **unspecified behavior** is that undefined behavior makes the program non-conforming, while unspecified behavior does not. A program that relies on unspecified behavior **MAY** behave differently on different implementations or on different executions of the same implementation, but it is still a conforming program.

  2.6.10 The distinction between **undefined behavior** and **implementation-defined behavior** is that implementation-defined behavior is documented and stable, while undefined behavior is neither. A program that relies on implementation-defined behavior is conforming for implementations that make the same choice; a program that relies on undefined behavior is non-conforming for every implementation.

## 3. Program Structure and Compilation Model

### 3.1 Source Files and Translation Units

3.1.1 A **source file** is a sequence of octets interpreted as UTF-8 (§4.1). A source file has a name provided by the implementation, typically a filesystem path.

3.1.2 A **translation unit** is the smallest independently compilable unit of Forge source. A translation unit is a single source file together with the transitive closure of the modules it declares and imports (§15.1).

3.1.3 A source file **MUST** be valid UTF-8. An implementation **MUST** reject a source file that is not valid UTF-8 with a diagnostic.

3.1.4 A source file **MAY** begin with an optional byte-order mark (`U+FEFF`). If present, it **MUST** be ignored and is not part of any token.

3.1.5 Every source file belongs to exactly one **package** (§15.2). The mapping from source files to packages is defined by the package manifest, not by the language grammar.

### 3.2 Crates, Packages, and Modules

3.2.1 A **crate** is the unit of compilation and linking. A crate consists of a **crate root** source file and the modules it declares. Every crate has a unique **crate name**, which is a valid identifier (§4.5).

3.2.2 A **package** is a collection of one or more crates described by a manifest file. A package **MUST** contain at least one library crate or one binary crate.

3.2.3 A **library crate** exposes a public interface to other crates. A **binary crate** defines a program entry point (§3.4).

3.2.4 A **module** is a named scope that groups items (§15.1). Modules may be nested to arbitrary depth. Modules are declared with the `mod` item (§11.6).

3.2.5 The **crate root module** is the module corresponding to the crate root file. Its name is the crate name.

### 3.3 Separate Compilation and Linking

3.3.1 An implementation **MAY** compile crates separately, provided that the observable behavior of the resulting program is consistent with the semantics defined by this specification.

3.3.2 The interface of a crate that is visible to other crates **SHALL** be called its **public interface**. The public interface consists of the items in the crate root module and in re-exported modules (§5.5) that are visible outside the crate (§5.4).

3.3.3 Changes to the public interface of a crate **MAY** require recompilation of dependent crates. Such changes are governed by the compatibility rules of §28.

3.3.4 Two crates **MUST NOT** define the same symbol in a way that causes a link-time conflict unless the symbol has internal linkage (§17.8).

### 3.4 Entry Points and Program Startup

3.4.1 A binary crate **MUST** define exactly one **entry point** function with the following signature:

```
fn main() { ... }
fn main() -> Result<(), E> { ... }
fn main() -> ! { ... }
```

where `E` is any type implementing the core error trait (§22.7). The entry point **MUST** be a free function at the crate root or be declared with the `#[entry]` attribute.

3.4.2 A program whose entry point returns normally terminates with exit status `0`. If the entry point returns `Result::Err(e)`, the implementation **SHALL** report the error to the standard error stream and terminate with a nonzero exit status (§21.7).

3.4.3 A program with an entry point returning `!` never returns; termination occurs by abort, panic with abort, or process exit (§21.3).

3.4.4 The **startup sequence** of a hosted program is:

1. The runtime initializes the environment.
2. Static initializers run (§19.7).
3. Command-line arguments and environment are made available (§22.5).
4. The entry point is invoked.
5. The program terminates (§21.7).

3.4.5 In a **freestanding** environment (§3.7), the startup sequence is implementation-defined. A freestanding entry point **MUST** be declared with the `#[start]` attribute and receives the target's native entry convention.

### 3.5 Compilation Phases

3.5.1 An implementation **SHALL** process source according to the following conceptual phases, in order:

1. **Lexical analysis** — source octets are converted to a token sequence (§4).
2. **Parsing** — tokens are converted to a syntax tree (§25.2).
3. **Name resolution** — identifiers are bound to declarations (§5.6).
4. **Type checking** — expressions and items are checked against the type rules (§6).
5. **Borrow checking** — ownership and lifetime constraints are verified (§7).
6. **Constant evaluation** — constant expressions are evaluated (§8.19, §20.1).
7. **Monomorphization / lowering** — generic items are instantiated (§13.8).
8. **Code generation and linking** — machine code is produced and linked (§3.3).

3.5.2 An implementation **MAY** reorder or combine phases as long as the observable behavior is preserved. In particular, an implementation **MAY** perform name resolution and type checking together.

3.5.3 An implementation **MUST** emit a diagnostic and refuse to produce an executable artifact if any phase reports a **required error** (§24.1). Warnings do not prevent the production of an artifact unless promoted (§24.4).

### 3.6 Target Platforms and Target Triples

3.6.1 A **target** describes the environment for which code is generated. A target is identified by a **target triple** of the form:

```
<arch>-<vendor>-<os>[-<env>]
```

3.6.2 Examples of well-formed target triples include `x86_64-unknown-linux-gnu`, `aarch64-apple-darwin`, and `thumbv7em-none-eabi`.

3.6.3 The target determines at least the following properties: pointer width and alignment (§27.4), endianness (§27.3), data model (§27.2), calling conventions (§17.6), and supported atomics (§19.3).

3.6.4 An implementation **MUST** reject a program that uses a target-dependent feature not supported by the selected target, unless the feature is guarded by conditional compilation (§20.7).

### 3.7 Freestanding and Hosted Environments

3.7.1 A **hosted environment** provides the full Forge standard library (§22) and an operating system interface.

3.7.2 A **freestanding environment** provides only the `core` subset of the standard library (§22.1). In a freestanding environment, the entry point, allocator, panic handler (§21.2), and any platform abstractions **MUST** be provided by the program or its dependencies.

3.7.3 A program intended for a freestanding environment **MUST** declare `#![no_std]` at its crate root. The absence of this attribute **SHALL** be interpreted as a hosted program.

---

## 4. Lexical Structure

### 4.1 Source Encoding

4.1.1 Source files are encoded in UTF-8 (§3.1.3). The **source character set** is the set of Unicode scalar values.

4.1.2 All characters in the source **MUST** be Unicode scalar values. Surrogate code points (`U+D800`–`U+DFFF`) are not scalar values and **MUST** be rejected.

4.1.3 Line endings may be `LF` (`U+000A`), `CRLF` (`U+000D U+000A`), or a lone `CR` (`U+000D`). Implementations **MUST** treat each as a single line terminator.

### 4.2 Whitespace and Line Endings

4.2.1 **Whitespace** consists of the following characters: space (`U+0020`), horizontal tab (`U+0009`), and line terminators (§4.1.3). Whitespace separates tokens and is otherwise insignificant except as required for token boundaries.

4.2.2 A **line terminator** delimits comments terminated by end-of-line (§4.3) and participates in source spans (§24.5).

### 4.3 Comments

4.3.1 A **line comment** begins with `//` and extends to the next line terminator or end of file. Line comments do not nest.

4.3.2 A **block comment** begins with `/*` and ends with the matching `*/`. Block comments **MAY** nest; an implementation **MUST** count nested `/*` and `*/` pairs. An unterminated block comment is a lexical error.

4.3.3 Comments are treated as whitespace. Comments do **not** appear in the token stream, but source spans and doc-comment attributes (§23.1) are preserved for tooling.

4.3.4 **Documentation comments** `///` (outer) and `//!` (inner) are a distinct token category and are lowered to documentation attributes (§23.1).

### 4.4 Tokens

4.4.1 A **token** is the smallest lexical unit. The token categories are: identifiers and keywords, literals, punctuation, delimiters, and documentation comments.

4.4.2 The token grammar is:

```
token          ::= ident
| keyword
| literal
| punct
| delimiter
| doc-comment

ident          ::= XID_Start XID_Continue*
| "_" XID_Continue+

keyword        ::= "as" | "async" | "await" | "break" | "const" | "continue"
| "crate" | "defer" | "dyn" | "else" | "enum" | "extern"
| "false" | "fn" | "for" | "if" | "impl" | "in" | "let"
| "loop" | "match" | "mod" | "move" | "mut" | "pub"
| "ref" | "return" | "self" | "Self" | "static" | "struct"
| "super" | "trait" | "true" | "type" | "unsafe" | "use"
| "where" | "while" | "unsized" | "yield"
```

4.4.3 Identifiers use the Unicode `XID_Start` and `XID_Continue` properties, with the exception that `_` alone is a **wildcard token** rather than an identifier.

4.4.4 An implementation **MUST** produce the same token sequence for a source that differs only in the form of line terminators.

### 4.5 Identifiers and Keywords

4.5.1 The **reserved keywords** are listed in §4.4.2. A reserved keyword **MUST NOT** be used as an identifier except when escaped (§4.5.5).

4.5.2 The following identifiers are **reserved for future use** and **MUST** be rejected as identifiers unless escaped: `abstract`, `become`, `box`, `do`, `final`, `gen`, `macro`, `override`, `priv`, `try`, `typeof`, `unsized`, `virtual`, `yield`.

4.5.3 An identifier **MUST NOT** be a single underscore `_`, which is reserved as the wildcard pattern (§10.4).

4.5.4 Identifiers are case-sensitive. Two identifiers are identical if and only if they consist of the same sequence of Unicode scalar values, except that identifiers are **NFC-normalized** before comparison. An implementation **SHOULD** warn when an identifier is not already in NFC form.

4.5.5 A **raw identifier** is written `r#name` and denotes the identifier `name` even if `name` is a reserved keyword. Raw identifiers exist to allow interoperability with foreign code and future keywords.

4.5.6 The identifiers `self`, `Self`, `super`, and `crate` are **contextual keywords** with special meaning in paths (§5.7) and method receivers (§12.1).

### 4.6 Literals

4.6.1 **Integer literals** have the following form:

```
int-literal    ::= dec-literal | hex-literal | oct-literal | bin-literal
dec-literal    ::= "0" | [1-9] { digit-or-underscore }
hex-literal    ::= "0x" hex-digit { hex-digit-or-underscore }
oct-literal    ::= "0o" oct-digit { oct-digit-or-underscore }
bin-literal    ::= "0b" bin-digit { bin-digit-or-underscore }
int-suffix     ::= "i8" | "i16" | "i32" | "i64" | "i128" | "isize"
| "u8" | "u16" | "u32" | "u64" | "u128" | "usize"
```

4.6.2 An integer literal **MAY** carry an **integer suffix** naming its type. If no suffix is present, the literal is **unsuffixed** and its type is inferred (§6.2.3). A suffix **MUST** name an integer primitive type.

4.6.3 Underscores **MAY** appear between digits and are ignored. An underscore **MUST NOT** appear as the first or last character, nor adjacent to the radix prefix.

4.6.4 The **value** of an integer literal is the mathematical integer denoted by its digits in the given radix. If the literal carries a suffix, the value **MUST** be representable in the named type, or a diagnostic is required (§24.1). An unsuffixed literal that does not fit the inferred type is an error.

4.6.5 **Floating-point literals** have the form:

```
float-literal  ::= [0-9] { digit-or-underscore } "." [0-9] { digit-or-underscore } [exp] [float-suffix]
| [0-9] { digit-or-underscore } exp [float-suffix]
exp            ::= ("e" | "E") ["+" | "-"] [0-9] { digit-or-underscore }
float-suffix   ::= "f32" | "f64"
```

4.6.6 A floating-point literal without a suffix has type `f64` unless inference (§6.2.3) selects `f32`. A floating-point literal that is not representable as a finite value of its type evaluates to infinity; a literal that overflows the format **MUST** produce a diagnostic.

4.6.7 **Boolean literals** are `true` and `false`, of type `bool`.

4.6.8 **Character literals** are written `'c'` where `c` is a single Unicode scalar value or an escape sequence (§4.6.11). A character literal has type `char`.

4.6.9 **String literals** are written `"..."` containing any characters except an unescaped `"`, `\`, or line terminator, or escape sequences. A string literal has type `&'static str` whose contents are the denoted sequence of Unicode scalar values encoded as UTF-8.

4.6.10 **Raw string literals** are written `r"..."`, `r#"..."#`, `r##"..."##`, and so on, where the number of `#` on each side **MUST** match. The contents are taken verbatim, with no escape processing, and **MUST NOT** contain a `"` followed by the closing `#` sequence. Raw string literals have type `&'static str`.

4.6.11 **Escape sequences** are:

| Escape       | Meaning                                              |
| ------------ | ---------------------------------------------------- |
| `\n`         | line feed (`U+000A`)                                 |
| `\r`         | carriage return (`U+000D`)                           |
| `\t`         | horizontal tab (`U+0009`)                            |
| `\\`         | backslash                                            |
| `\'`         | single quote (in char literals)                      |
| `\"`         | double quote (in string literals)                    |
| `\0`         | null (`U+0000`)                                      |
| `\xNN`       | byte value `NN` (hex), only where a byte is expected |
| `\u{NNNNNN}` | Unicode scalar value                                 |
| `\u{...}`    | Unicode scalar value, 1–6 hex digits                 |

4.6.12 A `\u{...}` escape **MUST** denote a Unicode scalar value; surrogate code points are rejected.

4.6.13 **Byte string literals** `b"..."` and **byte literals** `b'c'` denote values of type `&'static [u8; N]` and `u8` respectively. Their contents are ASCII or `\xNN` escapes, and they **MUST NOT** contain non-ASCII characters.

4.6.14 **Lifetime tokens** `'a` are lexed as a distinct category and are only valid where a lifetime is expected (§7.6).

### 4.7 Operators and Punctuation

4.7.1 The following punctuation tokens are recognized. Longer tokens are matched first (maximal munch).

```
{  }  (  )  [  ]
,  ;  :  ::  ->  =>  .  ..  ..=  ...
#  #!  @  $  ?  !  ~
+  -  *  /  %  ^  &  |  &&  ||  <<  >>  <<=  >>=
=  ==  !=  <  >  <=  >=  +=  -=  *=  /=  %=  &=  |=  ^=
_  '  "
```

4.7.2 The tokens `<` and `>` are ambiguous with generic argument delimiters. Disambiguation **MUST** follow the rules of §4.9.3.

4.7.3 An implementation **MUST** reject any character that is not whitespace, part of a token, or part of a comment, with a diagnostic giving the offending scalar value.

### 4.8 Statement Termination

4.8.1 Forge **does not** perform automatic semicolon insertion. Except where the grammar explicitly provides for an omitted terminator, a **statement** other than a block-like expression statement **MUST** be terminated by `;`.

4.8.2 The following statements omit the semicolon: `if`, `match`, `loop`, `while`, `for`, block expressions, and item declarations that end in `}`.

4.8.3 A missing required `;` **MUST** produce a diagnostic. The diagnostic **SHOULD** suggest insertion when the next token is on a different line and could begin a new statement.

### 4.9 Lexical Ambiguities

4.9.1 A `//` begins a line comment even if preceded by another `/` and even inside generic argument lists.

4.9.2 A `<<` is tokenized as a single token even when it could be two `<`. In generic contexts the parser **MUST** split a `<<` into two `<` when required by the grammar (for example, `Vec<Vec<T>>`).

4.9.3 The parser **SHALL** treat `<` and `>` as generic delimiters only when the surrounding syntax is a type, path, or expression where generics are expected (§13). Otherwise they are comparison operators.

4.9.4 A `>` that closes a generic argument list is **not** re-tokenized; the parser consumes generic arguments by counting delimiter balance (§4.9.2).

4.9.5 The characters `..` and `...` are distinct tokens. `...` is reserved for variadic parameter lists and range patterns (§18.5, §10.8) and is not a range operator.

---

## 5. Names, Scopes, and Visibility

### 5.1 Namespaces

5.1.1 Forge has three **namespaces**:

- the **type namespace**, containing types, traits, and type aliases;
- the **value namespace**, containing functions, constants, statics, and local bindings;
- the **macro namespace**, containing macros and procedural macros.

  5.1.2 The same identifier **MAY** denote different entities in different namespaces. For example, a struct `Point` introduces a type `Point` in the type namespace and a constructor `Point` in the value namespace.

  5.1.3 Name resolution (§5.6) is performed separately per namespace. A reference to a name is resolved in the namespace determined by the syntactic context.

### 5.2 Declarations and Definitions

5.2.1 A **declaration** introduces a name into a scope. A **definition** is a declaration that also provides a value or body.

5.2.2 An item declaration **MAY** be a **forward declaration** (for example, `extern` blocks and trait method signatures) that provides no body.

5.2.3 Two definitions of the same name in the same namespace and the same scope is a **duplicate definition** error, except where the grammar permits overloading (§5.6.4).

5.2.4 A declaration **MUST** appear before any use that requires its type, except for items that are resolved in an earlier phase (module items, type items, and functions at item scope), which are visible throughout their enclosing module (see §5.3.4).

### 5.3 Scopes and Shadowing

5.3.1 A **scope** is a region of source in which a name binding is visible. The scopes of Forge are:

- the **crate scope** (crate root and `use` declarations at crate root);
- **module scopes**, one per module;
- **function scopes**, including parameter scope and body scope;
- **block scopes**, one per `{}` block;
- **pattern scopes**, introduced by `let` bindings and pattern bindings;
- **generic scopes**, introduced by type and const parameters (§13).

  5.3.2 Inner scopes **shadow** outer scopes. A binding in an inner scope with the same name and namespace as a binding in an outer scope hides the outer binding for the remainder of the inner scope.

  5.3.3 Shadowing of an item by a local binding is permitted and is the mechanism by which `let x = ...; let x = ...;` rebinds.

  5.3.4 Item declarations at module scope (functions, types, modules, traits) are visible throughout the entire module, including before their textual position. This is **item hoisting**. Local declarations are **not** hoisted; a local binding is visible only after its `let` statement, and a use before the binding is an error.

  5.3.5 A use of a name that is shadowed and used in the shadowing declaration's initializer refers to the outer binding, not the new one. For example:

```
let x = 1;
let x = x + 1;   // the initializer's `x` refers to the outer binding
```

### 5.4 Visibility and Access Control

5.4.1 Every item **MAY** carry a **visibility** modifier. The possible visibilities are:

| Modifier       | Meaning                                             |
| -------------- | --------------------------------------------------- |
| (none)         | private to the enclosing module and its descendants |
| `pub`          | visible to the entire crate and to dependents       |
| `pub(crate)`   | visible to the entire crate                         |
| `pub(super)`   | visible to the parent module                        |
| `pub(in path)` | visible within the module denoted by `path`         |
| `pub(self)`    | same as private                                     |

5.4.2 The **default visibility** of an item is private. Private items are visible only to the enclosing module and to descendant modules.

5.4.3 An item **MUST NOT** be more visible than the module that contains it. Declaring a `pub` item inside a private module does not make that item reachable from outside the crate, because the module itself is not reachable.

5.4.4 Fields of a struct, variants of an enum, and methods of an implementation **MAY** carry visibility modifiers (§6.9). The visibility of a field or method **MUST NOT** exceed the visibility of its containing item.

5.4.5 A **use declaration** **MUST** be able to see the item it imports. Importing a private item from outside its visibility is an error.

5.4.6 Visibility is checked after name resolution. A name that resolves to an item that is not visible at the use site is a **visibility error**.

### 5.5 Imports and Re-exports

5.5.1 A **use declaration** binds names from another module into the current module. Its grammar is:

```
use-item   ::= ["pub"] "use" use-tree ";"
use-tree   ::= path ["as" ident]
| path "::" "{" use-tree {"," use-tree} [","] "}"
| "*"
| "{" use-tree {"," use-tree} [","] "}"
```

5.5.2 A `use` declaration with `pub` **re-exports** the imported name, making it part of the current module's public interface (§3.3.2). The original item **MUST** be visible at the `use` site.

5.5.3 A glob import `path::*` imports all items visible at `path` that are visible to the current module. Glob imports have **lower priority** than explicit imports; an explicit import shadows a glob import of the same name.

5.5.4 An import **MUST NOT** create a cycle of `use` declarations. A cycle is detected after resolution and is a required error (§24.1).

5.5.5 The names introduced by `use` occupy the namespaces of the imported items. A `use` that imports a name already bound in the same namespace and scope is an error unless the original is a glob import.

### 5.6 Name Resolution

5.6.1 **Name resolution** is the process of binding each identifier reference to its declaration. Resolution proceeds by searching scopes from innermost to outermost (§5.3.1) until a matching declaration is found in the required namespace.

5.6.2 Resolution of a path `a::b::c` (§5.7) first resolves `a`, then, based on the kind of entity `a` denotes, resolves `b`, and so on. If any segment fails to resolve, the path is unresolved.

5.6.3 Resolution is performed **after** macro expansion (§20.4) and **before** type checking, with the caveat that method-call resolution (§5.6.4) requires type information and occurs during type checking.

5.6.4 **Method resolution** for a call `receiver.method(args)` selects a method by inspecting the type of `receiver` and the set of methods visible in the current scope and implemented for that type (§14.2). If multiple applicable methods exist, the most specific is selected according to the rules of §14.9; if no unique most-specific method exists, the call is ambiguous and is an error.

5.6.5 **Item paths** and **expression paths** are resolved in different namespaces. The same syntax `Foo::bar` may denote an associated function in the value namespace or an associated type in the type namespace depending on context.

5.6.6 An implementation **MUST** report an unresolved name as a required error, including the textual name and the scope search that failed (§24.5).

### 5.7 Paths and Qualified Names

5.7.1 A **path** is a sequence of identifiers separated by `::`. Paths are used to name items, types, and modules.

5.7.2 Path forms:

```
path          ::= "::"? path-segment { "::" path-segment }
path-segment  ::= ident ["::" "<" generic-args ">"]
| "self"
| "Self"
| "super"
| "crate"
| "<" type ">" "::" ident ["::" "<" generic-args ">"]
```

5.7.3 A **leading `::`** denotes a path that begins at the crate root or an extern prelude root (see §15.5). A path without a leading `::` is **relative**: it begins at the current module.

5.7.4 The segment `self` denotes the current module. The segment `super` denotes the parent module. The segment `crate` denotes the current crate's root module. These segments **MUST** appear only as the first segment of a relative path, except `Self`, which may appear as the first segment and denotes the implementing type in an implementation (§11.8).

5.7.5 **Qualified paths** `<T as Trait>::item` disambiguate associated items (§13.6). The type `T` **MUST** implement `Trait`, and `item` **MUST** be an associated item of `Trait`.

5.7.6 The **prelude** (§22.2) is an implicit set of names in scope in every module unless shadowed. Explicit imports shadow the prelude.

---

## 6. Types

### 6.1 Type System Overview

6.1.1 Forge is **statically typed**. Every expression has a type determined at compile time (§8). The type system is **nominal** for user-defined types and **structural** for references, tuples, arrays, and function types.

6.1.2 The type system is **sound** with respect to the memory model (§7): a well-typed, safe program cannot exhibit undefined behavior. `unsafe` code (§16) may relax this guarantee.

6.1.3 Types are divided into **sized** types, which have a statically known size, and **unsized** types, which do not (§6.17). A value of an unsized type cannot be held directly; it is accessed through a reference or pointer.

### 6.2 Primitive Types

6.2.1 Forge has the following **primitive types**:

| Type    | Description                    | Size         |
| ------- | ------------------------------ | ------------ |
| `bool`  | boolean                        | 1 byte       |
| `i8`    | signed 8-bit integer           | 1 byte       |
| `i16`   | signed 16-bit integer          | 2 bytes      |
| `i32`   | signed 32-bit integer          | 4 bytes      |
| `i64`   | signed 64-bit integer          | 8 bytes      |
| `i128`  | signed 128-bit integer         | 16 bytes     |
| `isize` | signed pointer-sized integer   | pointer size |
| `u8`    | unsigned 8-bit integer         | 1 byte       |
| `u16`   | unsigned 16-bit integer        | 2 bytes      |
| `u32`   | unsigned 32-bit integer        | 4 bytes      |
| `u64`   | unsigned 64-bit integer        | 8 bytes      |
| `u128`  | unsigned 128-bit integer       | 16 bytes     |
| `usize` | unsigned pointer-sized integer | pointer size |
| `f32`   | 32-bit IEEE 754 binary32       | 4 bytes      |
| `f64`   | 64-bit IEEE 754 binary64       | 8 bytes      |
| `char`  | Unicode scalar value           | 4 bytes      |
| `()`    | unit type                      | 0 bytes      |
| `!`     | never type                     | 0 bytes      |
| `str`   | unsized string slice           | unsized      |

6.2.2 `isize` and `usize` have a width equal to the target's pointer width (§27.4) and are the type of array and slice indexing (§8.12).

6.2.3 **Type inference.** An unsuffixed integer literal has a type determined by inference. Inference **MUST** choose a type that can represent the literal and satisfies all constraints from the surrounding context. If no unique type can be chosen, the **default** is `i32` for unsuffixed integers and `f64` for unsuffixed floats.

### 6.3 Boolean and Integer Types

6.3.1 `bool` has exactly two values: `true` and `false`. No other value is valid for `bool`. A `bool` value that is neither `true` nor `false` is undefined behavior (§7.11).

6.3.2 Integer types are two's-complement with the ranges:

- `iN`: `-2^(N-1)` to `2^(N-1) - 1`
- `uN`: `0` to `2^N - 1`

  6.3.3 Integer operations are defined by §8.7. Overflow behavior is a property of the operation, not the type: checked operations produce an error, wrapping operations wrap, and saturating operations saturate.

  6.3.4 An implementation **MUST NOT** assume that an integer operation cannot overflow unless the operation is a checked or saturating operation. Optimization in the presence of overflow is specified by §8.7.

### 6.4 Floating-Point Types

6.4.1 `f32` and `f64` are IEEE 754 binary32 and binary64 respectively. They support infinities, negative zero, and NaN as defined by IEEE 754.

6.4.2 NaN comparison semantics follow IEEE 754: any comparison involving NaN is `false`, except `!=` which is `true`. The `total_cmp` operation (§22.4) provides a total order for use in sorting.

6.4.3 The set of operations on floating-point values and their results **SHALL** be those of the target's IEEE 754 implementation. An implementation **MAY** use fused multiply-add or reassociate expressions only if the program requests it via an attribute (§23.3).

### 6.5 Character and String Types

6.5.1 `char` denotes a Unicode scalar value (§4.1.2) and **MUST NOT** contain a surrogate code point. The valid range of `char` is `0..=0x10FFFF` excluding `0xD800..=0xDFFF`.

6.5.2 `str` is an **unsized** type denoting a sequence of UTF-8 encoded bytes that form a valid UTF-8 string. The length is encoded in the reference to it, not in the type.

6.5.3 The type `&str` is a reference to a string slice. It is valid UTF-8 by construction. The type `String` (§22.3) is an owned, growable UTF-8 string.

6.5.4 Indexing a `str` by an integer is not permitted, because a byte index might not fall on a UTF-8 boundary. Byte-level access is available through `str::as_bytes` (§22.3). Character-level access is through iteration.

### 6.6 Unit and Never Types

6.6.1 The **unit type** `()` has exactly one value, also written `()`. It is the type of expressions and functions that produce no meaningful value.

6.6.2 The **never type** `!` has no values. An expression of type `!` diverges (§9.11). The never type is a subtype of every type (§6.15.3).

### 6.7 Pointers and References

6.7.1 A **shared reference** `&T` is a non-owning pointer to a value of type `T`. A **mutable reference** `&mut T` is a non-owning pointer through which the referent may be mutated.

6.7.2 A reference **MUST** be non-null and **MUST** point to a valid value of type `T` for the duration of its use (§7.11). A reference that violates these invariants is undefined behavior.

6.7.3 A **raw pointer** `*const T` or `*mut T` is a pointer that carries no validity guarantee beyond alignment. Raw pointers may be null and may dangle. Dereferencing a raw pointer is an `unsafe` operation (§16.2).

6.7.4 References carry a **lifetime** (§7.6). A reference with lifetime `'a` is written `&'a T` or `&'a mut T`. Lifetimes are erased after borrow checking and do not affect layout.

6.7.5 The **size and alignment** of `&T`, `&mut T`, `*const T`, and `*mut T` are the target's pointer size and alignment (§27.4).

### 6.8 Arrays, Slices, and Tuples

6.8.1 An **array** `[T; N]` is a fixed-size sequence of `N` values of type `T`. `N` is a **const expression** (§8.19) of type `usize`. The size of the array is `N * size_of::<T>()`, and its alignment is that of `T`.

6.8.2 A **slice** `[T]` is an **unsized** type denoting a contiguous sequence of values of type `T` of runtime-determined length. A reference to a slice `&[T]` carries the length. Slices are the standard way to accept a contiguous sequence of unknown length.

6.8.3 A **tuple** `(T1, T2, ..., Tn)` is a heterogeneous product type. The empty tuple is `()`. Tuple fields are accessed by index (§8.12). The layout of a tuple is the same as a struct with the same fields and the `#[repr(C)]`-like field ordering, unless otherwise specified (§17.4).

6.8.4 A **tuple struct** is a named struct with unnamed fields (§6.9.3). Its syntax and layout are equivalent to a struct with positional fields.

### 6.9 Structs, Enums, and Unions

6.9.1 A **struct** is a nominal product type:

```
struct-item  ::= ["pub"] "struct" ident [generic-params] "where"-clause?
( "{" struct-fields "}" | "(" struct-fields ")" ";" | ";" )
struct-field ::= ["pub"] ident ":" type
```

6.9.2 A struct with `{ ... }` fields is a **named-field struct**. A struct with `( ... )` fields is a **tuple struct**. A struct with `;` and no fields is a **unit struct**.

6.9.3 The **constructor** of a struct is a value in the value namespace with the same name as the struct. It takes the struct's fields as arguments and returns the struct. For unit structs, the constructor is the value `StructName` itself.

6.9.4 An **enum** is a nominal sum type:

```
enum-item   ::= ["pub"] "enum" ident [generic-params] "where"-clause? "{" variants "}"
variant     ::= ident ( "(" field-list ")" | "{" struct-fields "}" )?
```

6.9.5 An enum value is exactly one of its **variants**. A variant with no payload is a **unit variant**; a variant with tuple-style fields is a **tuple variant**; a variant with named fields is a **struct variant**.

6.9.6 A **union** is a nominal type whose fields overlap in storage:

```
union-item  ::= ["pub"] "union" ident [generic-params] "{" struct-fields "}"
```

6.9.7 Reading a union field is an `unsafe` operation (§16). The fields of a union **MUST** be `Copy` (§7.8) or otherwise have no drop glue (§7.3), or the union **MUST** be declared with an explicit drop implementation.

6.9.8 The **layout** of structs, enums, and unions is specified in §17.4 and §17.5. Unless a `#[repr(...)]` attribute is present, the layout is implementation-defined but **MUST** be stable for a given implementation and target.

### 6.10 Function Types

6.10.1 A **function type** is written `fn(A1, A2, ..., An) -> R`, where `A1..An` are parameter types and `R` is the return type. The return type `-> ()` may be omitted.

6.10.2 A function type has no size and no values of its own; it describes the type of **function pointers** and **closures** (§12.6). A function pointer has the same size and alignment as a data pointer.

6.10.3 The **calling convention** of a function type may be specified with `extern "abi"` (§17.6). The default convention is `extern "forge"`, which is implementation-defined and stable within a crate's compiled output.

6.10.4 Function pointers **MUST** be non-null. The null pointer is not a valid value of a function pointer type, except when `Option<fn(...)>` is used (§17.5.4).

### 6.11 Type Aliases and Opaque Types

6.11.1 A **type alias** introduces a new name for an existing type:

```
type-alias ::= ["pub"] "type" ident [generic-params] "=" type ";"
```

6.11.2 A type alias is **transparent**: it is exactly equal to the aliased type for all purposes of type checking, layout, and ABI. Aliases are erased during compilation.

6.11.3 An **opaque type** is declared with `type Name = impl Trait;` at item scope (§14.8). An opaque type hides its underlying type outside its defining scope; the only operations available on a value of the opaque type are those provided by the declared trait bounds.

6.11.4 The `impl Trait` return-position syntax (§14.8) introduces an anonymous opaque type. Return-position opaque types are distinct types for each function that defines them and **MUST NOT** be named.

### 6.12 Generic Types

6.12.1 A generic item is declared with one or more **generic parameters** (§13):

```
generic-params ::= "<" generic-param {"," generic-param} ">"
generic-param  ::= lifetime-param | type-param | const-param
lifetime-param ::= "'" ident [":" lifetime-bounds]
type-param     ::= ident [":" trait-bounds]
const-param    ::= "const" ident ":" type
```

6.12.2 A **generic type** is a type constructor that is instantiated with concrete arguments. The instantiation `Vec<u8>` denotes the type obtained by substituting `u8` for the type parameter of `Vec`.

6.12.3 A generic parameter **MUST** be used in the definition of the generic item, except for lifetime parameters, which **MAY** be unused in specific definitions (subject to the elision rules of §7.6.5).

6.12.4 The set of types that may be used as a generic argument is constrained by the parameter's bounds (§13.5). Substitution with a type that does not satisfy the bounds is a type error.

6.12.5 The **layout** of a generic type depends on its concrete arguments (§17.1). Two instantiations with different arguments are distinct types, even if their layout is identical.

### 6.13 Trait/Interface Types

6.13.1 A **trait** declares a set of methods and associated items that a type may implement (§14.1).

6.13.2 The type `dyn Trait` is a **trait object type** and is **unsized**. A reference to a trait object `&dyn Trait` is a **fat pointer** consisting of a data pointer and a vtable pointer (§14.5).

6.13.3 The type `impl Trait` in argument position is **sugar** for a generic type parameter with the given bound (§14.8). The type `impl Trait` in return position is an opaque type (§6.11.3).

6.13.4 The set of traits a type implements is fixed at the type's definition. External types **MAY** implement local traits (subject to coherence, §14.9). A type **MUST NOT** implement the same trait twice.

### 6.14 Type Layout and Representation

6.14.1 Every **sized** type has a **size** and an **alignment**, both non-negative integers, with the alignment a nonzero power of two. The size of a type is always a multiple of its alignment.

6.14.2 The **layout** of a type is the arrangement of its fields in memory, including padding. Layout is fully specified for types with an explicit `#[repr(...)]` attribute (§17.3). For types without such an attribute, the layout is **implementation-defined** and **MUST NOT** be relied upon across crate boundaries or compiler versions.

6.14.3 The `size_of::<T>()` and `align_of::<T>()` operations return the size and alignment of `T` as `usize` (§22.4).

6.14.4 The **niche optimization** (§17.5.4) may make an enum whose variants would otherwise require a discriminant fit in a smaller size. The size of an enum is **not** guaranteed to be the size of its largest variant plus discriminant, and programs **MUST NOT** rely on a particular enum size unless the enum has a `#[repr(...)]` attribute.

### 6.15 Type Equivalence and Subtyping

6.15.1 Two types are **equal** if they are identical after alias expansion and lifetime erasure. Type equality is the basis for assignment, function calls, and generic argument matching.

6.15.2 **Nominal equality**: two structs, enums, unions, or traits with the same fields but different names are **not** equal.

6.15.3 **Subtyping** in Forge is limited to the following rules:

- `!` is a subtype of every type.
- `&'long T` is a subtype of `&'short T` if `'long: 'short` (lifetime subtyping, §7.6.3).
- `&'long mut T` is a subtype of `&'short mut T` and of `&'short T` under the same condition.
- `fn(A) -> B` is a subtype of `fn(C) -> D` if `C` is a subtype of `A` and `B` is a subtype of `D` (contravariant in arguments, covariant in return).

  6.15.4 Generic types are **invariant** in their type parameters by default. Variance annotations (§13.9) allow a type parameter to be declared covariant or contravariant.

  6.15.5 There is no implicit subtyping between user-defined types. Conversions between related types are explicit (§8.13).

### 6.16 Variance and Type Parameters

6.16.1 **Variance** describes how the subtyping relationship of a generic type's arguments affects the subtyping of the generic type itself.

6.16.2 The variance of a type parameter is inferred from its usage:

- a parameter used only in **output** position (for example, `fn() -> T`) is covariant;
- a parameter used only in **input** position (for example, `fn(T)`) is contravariant;
- a parameter used in both positions, or in a position that is invariant (such as `&mut T` or a field of a `Cell<T>`), is invariant.

  6.16.3 An implementation **MUST** infer variance according to the rules of §6.16.2 or reject the declaration as an error if the inference is ambiguous. Programs **SHOULD NOT** rely on variance inference for soundness; the compiler enforces it.

### 6.17 Dynamically Sized and Unsized Types

6.17.1 An **unsized type** is a type whose size is not known at compile time. The unsized types of Forge are:

- `str`;
- `[T]`;
- `dyn Trait` (§6.13.2);
- any struct or tuple whose last field is unsized.

  6.17.2 An unsized type **MUST NOT** be used as the type of a local binding, a function parameter passed by value, a return type, a struct field other than the last field, or a generic argument without a `?Sized` bound (§13.5.3).

  6.17.3 The **last-field rule**: a struct or tuple may contain at most one unsized field, and if present it **MUST** be the last field. Such a type is itself unsized.

  6.17.4 A reference to an unsized type is a **fat pointer** consisting of a data pointer and a **metadata** word. The metadata is the length for slices and `str`, and the vtable pointer for `dyn Trait`.

---

## 7. Memory Model and Ownership

### 7.1 Memory Model Overview

7.1.1 Every Forge program executes in an abstract **memory model** consisting of a set of **allocations**. An allocation is a contiguous region of bytes with a unique identity, a base address, and a size.

7.1.2 Every access to memory **MUST** be to a byte within the allocation that contains the pointer, or to the one-past-the-end address of the allocation (which may be computed but not dereferenced).

7.1.3 Each allocation is **mutable** or **immutable** for the duration of a borrow (§7.5). The interaction between allocations and references is the subject of the borrow checker (§7.5.4).

7.1.4 The **validity** of a value is a property of its type and its representation. A value whose bytes do not satisfy the validity invariant of its type is undefined behavior to observe (§7.11).

### 7.2 Values, Places, and References

7.2.1 A **value** is a typed, self-contained datum. Values are moved, copied, or dropped. A value may be temporary (an rvalue) or may live in a **place**.

7.2.2 A **place** is a location in memory that can hold a value. The places of Forge are: local variables, struct fields, tuple elements, array and slice elements, and the referents of references and raw pointers.

7.2.3 The distinction between value expressions and place expressions is specified in §8.2. Most operations consume values; a subset (borrowing, field access, assignment) operate on places.

7.2.4 A **reference** is a value that refers to a place. A reference is valid for a specified **lifetime** (§7.6), and the borrow checker guarantees that the referent is not mutated or deallocated in a way that invalidates the reference during its lifetime.

### 7.3 Initialization and Drop

7.3.1 A local binding **MUST** be initialized before it is read. Reading an uninitialized local is a compile-time error (the **definite initialization** rule).

7.3.2 A value that is no longer used is **dropped**. Dropping a value runs its **drop glue**, which consists of calling the destructor (the `Drop::drop` method, §22.7) if the type implements it, and then recursively dropping its fields.

7.3.3 Drop glue runs at the end of the scope of a binding, in reverse declaration order for multiple bindings. Drop glue also runs when a value is moved out of its place and the place is subsequently not reinitialized.

7.3.4 A type with a `Drop` implementation **MUST NOT** be `Copy` (§7.8.2). A type that contains a field with a destructor **MUST NOT** be `Copy`.

7.3.5 A value **SHOULD NOT** be dropped while a reference to it is live. The borrow checker enforces this for safe references (§7.5.4).

### 7.4 Ownership

7.4.1 Every value in a Forge program has a single **owner**, which is responsible for dropping it. The owner is a binding, a temporary, or a field of an owning value.

7.4.2 **Ownership** is transferred by **moves**. Moving a value out of a place invalidates the place for future reads, unless the place is reinitialized before its next read.

7.4.3 A value **MUST NOT** be moved while a reference to it that is live in the current borrow region exists. The borrow checker enforces this constraint.

7.4.4 Ownership of a value ends when the value is dropped (§7.3), moved out of its owner, or when the owner goes out of scope.

7.4.5 A **cycle of ownership** is permitted only through `unsafe` code or through reference counting that intentionally leaks (§22.4). Safe Forge cannot create an ownership cycle without leaking, because the borrow checker prevents `Rc<T>` cycles from dropping.

### 7.5 Borrowing and Aliasing

7.5.1 A **borrow** is a temporary reference to a place. A borrow is created by `&place` (a **shared borrow**) or `&mut place` (a **mutable borrow**).

7.5.2 **Aliasing rules.** At any point in execution, a place that is borrowed may be accessed only as follows:

- a place that is the target of a shared borrow may be read, but not mutated, and may be shared-borrowed again;
- a place that is the target of a mutable borrow may be read or mutated only through that mutable borrow, and may not be borrowed at all (shared or mutable) while the mutable borrow is live.

  7.5.3 The set of places that a borrow overlaps is determined by structural reachability: a borrow of a struct field overlaps only that field, not sibling fields, and a borrow of a whole struct overlaps all its fields.

  7.5.4 **The borrow checker** is the static analysis that enforces §7.5.2. The borrow checker **MUST** reject any program in which a borrow region (§7.6.4) contains an access that violates §7.5.2. A program rejected by the borrow checker is not a conforming Forge program.

  7.5.5 A **reborrow** is a borrow that is derived from an existing borrow. The reborrow's lifetime **MUST** be contained in the lifetime of the original borrow. Reborrowing a mutable reference through a shared reference is not permitted.

  7.5.6 **Interior mutability** (types such as `Cell<T>` and `Mutex<T>`, §22.4) permits mutation through a shared reference. Such types carry their own synchronization invariants and are subject to their own safety rules; the aliasing rules of §7.5.2 do not apply to the interior of such types while the program is in a valid state for that type.

### 7.6 Lifetimes and Regions

7.6.1 A **lifetime** is a name for a region of execution during which a reference is valid. Lifetimes are denoted `'a`, `'b`, and so on.

7.6.2 Lifetimes are introduced by lifetime parameters on items (§13), by elision (§7.6.5), and by the `'static` lifetime.

7.6.3 **Lifetime subtyping**: `'a: 'b` means that `'a` outlives `'b`. A reference with lifetime `'a` may be used wherever a reference with lifetime `'b` is expected if `'a: 'b`.

7.6.4 A **borrow region** is the region of program execution during which a borrow is live. The borrow checker infers borrow regions based on the uses of the borrowed reference. A borrow region ends at the **last use** of the reference, not at the end of its syntactic scope, under **non-lexical lifetimes**.

7.6.5 **Lifetime elision.** In function signatures, lifetimes may be elided. The elision rules are:

1. each elided lifetime in argument position becomes a distinct lifetime parameter;
2. if there is exactly one input lifetime, it is assigned to all elided output lifetimes;
3. if there are multiple input lifetimes but one of them is `&self` or `&mut self`, the `self` lifetime is assigned to all elided output lifetimes;
4. otherwise, eliding an output lifetime is an error.

7.6.6 The `'static` lifetime is the lifetime of the entire program. A reference with lifetime `'static` is valid for the entire execution. Constants and statics have `'static` lifetime.

7.6.7 A **lifetime bound** `'a: 'b + 'c` requires `'a` to outlive both `'b` and `'c`. Lifetime bounds appear on type parameters and associated types (§13).

7.6.8 Lifetimes are **erased** after borrow checking. They do not appear in the runtime representation of any value and do not affect ABI (§17).

### 7.7 Mutability

7.7.1 A binding is **mutable** if declared with `mut`. Only mutable bindings, mutable references, and fields reached through a mutable place may be assigned to.

7.7.2 The type `&mut T` is a mutable reference; `&T` is a shared reference. Mutation through a shared reference is permitted only through interior mutability (§7.5.6).

7.7.3 A place is mutable if and only if every step in the path from the root binding to the place is through a mutable binding or a mutable reference. The root binding's mutability is established at declaration and does not change.

7.7.4 Mutation of a value that is borrowed in violation of §7.5.2 is rejected by the borrow checker. Mutation of a value that is not borrowed and whose place is mutable is permitted.

### 7.8 Move Semantics and Copy Semantics

7.8.1 **Move semantics** apply to all types by default. Assigning, passing, or returning a value of a non-`Copy` type transfers ownership (§7.4.2).

7.8.2 **Copy semantics** apply to types that implement the `Copy` trait. A `Copy` type is duplicated bitwise on assignment, and the source place remains valid. A type **MAY** implement `Copy` only if all of its fields implement `Copy` and it does not implement `Drop`.

7.8.3 The primitive scalar types, references (`&T`, `&mut T`), raw pointers, and tuples and arrays of `Copy` types are `Copy`. Types containing a non-`Copy` field are not `Copy`.

7.8.4 A **partial move** occurs when a field of a struct is moved out, leaving the struct partially initialized. A partially moved value **MUST NOT** be used as a whole; individual remaining fields may be used. Reinitializing the moved field restores the value to a fully initialized state.

7.8.5 Moving a value out of a place that is borrowed in a way that would invalidate the borrow is rejected by the borrow checker.

### 7.9 Allocation and Deallocation

7.9.1 An **allocation** is created by the allocator (§22.4). The standard allocator is provided by the runtime in a hosted environment; a freestanding environment **MUST** provide its own global allocator (§22.4).

7.9.2 Every allocation has a **layout** consisting of a size and an alignment. The alignment **MUST** be a nonzero power of two, and the size **MUST** be a multiple of the alignment or zero.

7.9.3 An allocation **MUST** be deallocated with the same layout with which it was allocated. Deallocating with a different layout is undefined behavior.

7.9.4 Memory **MAY** be reused after deallocation. A pointer to deallocated memory is a **dangling pointer** and **MUST NOT** be dereferenced.

7.9.5 The **stack** is managed automatically by the runtime. Stack allocations are not individually deallocated; they are released when the owning frame returns.

7.9.6 **Statics** are allocated at program startup and deallocated at program termination, if at all. Their addresses are stable for the entire program.

### 7.10 Stack, Heap, and Static Storage

7.10.1 **Stack storage** holds local bindings and temporaries. Stack storage is allocated on function entry and released on function exit. The address of a stack value is valid only for the lifetime of the enclosing frame.

7.10.2 **Heap storage** holds values created through the allocator. Heap storage is released explicitly by the owning smart pointer (§22.4) or by the program's exit.

7.10.3 **Static storage** holds `static` items and string literals. Static storage is initialized before `main` and lives for the entire program. Mutating a `static mut` item is an `unsafe` operation (§16).

7.10.4 The **thread stack** is per-thread. A reference to a value on one thread's stack **MUST NOT** be sent to another thread unless the value is `Send` (§19.6) and the reference has `'static` lifetime, or the reference is bounded by a scoped-thread mechanism (§22.6).

### 7.11 Validity and Undefined Behavior

7.11.1 A value is **valid** for its type if its bits satisfy the type's validity invariant. The validity invariants are:

- `bool`: the byte is `0` or `1`;
- `char`: the value is a Unicode scalar value (§6.5.1);
- `&T`, `&mut T`: non-null and aligned, pointing to a valid `T` for the duration of the borrow;
- function pointers: non-null and pointing to a valid function of the given signature;
- enums with explicit `#[repr(...)]`: the discriminant is within the declared range;
- structs, tuples, and arrays: every field or element is valid.

  7.11.2 The following operations are **undefined behavior**:

- dereferencing a null or dangling pointer;
- reading uninitialized memory as a value;
- creating a reference that does not satisfy §7.11.1;
- violating the aliasing rules of §7.5.2;
- accessing memory outside the allocation that contains the pointer;
- calling a function through a function pointer with the wrong signature;
- data races (§19.5);
- deallocating an allocation with a layout different from its allocation layout;
- transmuting a value to a type for which its bits are not valid (§16.5).

  7.11.3 An implementation **MAY** assume that a program does not execute undefined behavior. In particular, the optimizer **MAY** assume that an alias introduced by a reference is not violated and that a value of a valid type is valid.

  7.11.4 The `unsafe` keyword (§16.1) does not disable these rules. It permits the programmer to write code that the compiler cannot verify, but the code **MUST** still uphold the rules to be correct. A program that executes undefined behavior is not conforming, even if it is accepted by the compiler.

  7.11.5 An implementation **SHOULD** provide tools (such as a debug-mode runtime and a sanitizer, §24.6) that detect a subset of undefined behavior and report it as a diagnostic at runtime.

## Part II — Expressions and Statements

## 8. Expressions

### 8.1 Expression Categories

8.1.1 An **expression** is a syntactic construct that denotes a value, a place, a computation, or a divergence. Every expression has a type (§6), determined by type inference (§8.4.6) and the rules of this chapter.

8.1.2 Expressions are categorized by the **kind** of thing they denote:

- **Value expressions** produce a value, which may be moved or copied (§7.8).
- **Place expressions** denote a location in memory that can be read, written, or borrowed (§7.2.2).
- **Type expressions** denote a type; the only such expression in Forge is the path in a qualified path (§5.7.5), and it is only legal in type contexts.
- **Diverging expressions** have type `!` and never produce a value (§9.11).

  8.1.3 Every expression also has an **evaluation class**, which is one of **lvalue** (a place), **rvalue** (a value that is a temporary), or **constant** (a value known at compile time). The evaluation class determines how the expression interacts with assignment, borrowing, and pattern matching.

  8.1.4 The following table summarizes the principal expression forms and their evaluation class. The remainder of this chapter specifies each form.

| Form                             | Evaluation Class | Section  |
| -------------------------------- | ---------------- | -------- |
| Literal                          | rvalue, constant | §8.3     |
| Path to local                    | lvalue           | §8.4     |
| Path to constant / static        | lvalue, constant | §8.4     |
| Unary op                         | rvalue           | §8.5–8.9 |
| Binary op                        | rvalue           | §8.5–8.9 |
| Assignment                       | rvalue (unit)    | §8.10    |
| Call / method call               | rvalue           | §8.11    |
| Field access                     | lvalue           | §8.12    |
| Indexing                         | lvalue           | §8.12    |
| Cast                             | rvalue           | §8.13    |
| Block                            | rvalue           | §8.14    |
| Match                            | rvalue           | §8.15    |
| Closure                          | rvalue           | §8.16    |
| Aggregate (struct, tuple, array) | rvalue           | §8.17    |

### 8.2 Place Expressions and Value Expressions

8.2.1 A **place expression** (also called an **lvalue**) denotes a location. The place expressions of Forge are:

- a local binding path `x` (§8.4.2);
- a field access `e.field` (§8.12.1);
- a tuple or struct field access `e.0`, `e.1`, ... (§8.12.2);
- an indexing expression `e[i]` (§8.12.3);
- a dereference `*e` of a reference or raw pointer (§8.5.6);
- a parenthesized place `(e)` if `e` is a place.

  8.2.2 A **value expression** (also called an **rvalue**) produces a value. Value expressions include all literals, all operator expressions other than the place-producing ones listed in §8.2.1, all calls, and all control-flow expressions.

  8.2.3 When a place expression appears in a value context (for example, as the right-hand side of an assignment), it is **read**, producing a copy or a move of the value at that place, according to the `Copy`-ness of the type (§7.8.2).

  8.2.4 When a place expression appears in a **borrow context** (`&place` or `&mut place`), it is not read; instead, a reference to the place is created (§7.5.1).

  8.2.5 The distinction between place and value contexts is determined by the surrounding construct:

- the left operand of an assignment is a place context;
- the operand of `&` or `&mut` is a place context;
- the operand of `*` is a place context (it produces a place);
- the scrutinee of a `match` is a place context if the pattern matches by reference, otherwise a value context (§9.9);
- every other operand position is a value context.

### 8.3 Literals and Constants

8.3.1 A **literal expression** is an integer, float, boolean, character, string, byte, or byte-string literal (§4.6). A literal expression has the type determined by its suffix, or by inference if unsuffixed (§6.2.3).

8.3.2 A literal expression is a **constant expression** if it denotes a value that can be computed at compile time. All literals of scalar types are constant expressions. String literals are constant expressions of type `&'static str`.

8.3.3 The **value** of a literal expression is the value denoted by its token, subject to the representability rules of §4.6.4 and §4.6.6.

### 8.4 Paths and Names

8.4.1 A **path expression** is an expression that consists of a path (§5.7). A path expression denotes an item, a local binding, an associated item, or a variant.

8.4.2 A path that resolves to a **local binding** denotes the place bound to that local. Its evaluation class is lvalue. Reading it produces a copy or move of the local's value.

8.4.3 A path that resolves to a **constant** or a **static** denotes the value or place of that item. A path to a `const` is a constant expression and its evaluation class is constant. A path to a `static` is a place expression whose evaluation class is lvalue, but reading a mutable static requires `unsafe` (§16).

8.4.4 A path that resolves to a **function item** denotes the function. In a value context, the function item is coerced to a function pointer (§8.4.7). In a call context, the function item is called directly.

8.4.5 A path that resolves to a **unit variant** or a **unit struct** denotes the constructor value. For a unit struct `S`, the path `S` in the value namespace denotes the unit value `S`. For a unit variant `E::V`, the path `E::V` denotes the value of that variant.

8.4.6 **Type inference.** When the type of an expression is not fully determined by its syntax, the compiler performs **inference**:

- unsuffixed integer literals default to `i32`, unsuffixed floats to `f64` (§6.2.3);
- an empty array `[]` has type `[T; 0]` where `T` is inferred;
- a `let` binding without a type annotation infers the type of its initializer;
- a generic function call's type arguments are inferred from argument and expected types (§13.3);
- a closure's parameter types are inferred from expected types (§8.16).

If inference cannot produce a unique solution, the program is ill-typed and the compiler **MUST** report an inference error (§24.1).

8.4.7 **Function item coercion.** A function item may be used in any context where a function pointer of the corresponding signature is expected. The coercion is implicit and does not change the function's address. A function item **MUST NOT** be used where a closure type is expected without an explicit coercion through `as` (§8.13).

### 8.5 Operators

8.5.1 **Unary operators** are `-`, `!`, `*`, `&`, and `&mut`.

8.5.2 **Binary operators** are grouped by precedence in §8.6. The groups are: multiplicative, additive, shift, bitwise, comparison, logical, assignment, and range.

8.5.3 The **operand types** for each operator are constrained by the operator's trait requirements (§14). An operator `a + b` is legal if and only if the type of `a` implements the corresponding operator trait (§22.7) for an operand of type `b`, or a built-in rule applies.

8.5.4 Built-in operator rules apply to primitive scalar types (§6.3, §6.4), to references to scalar types, and to `bool` for logical operators. For these types, the operator has the semantics specified in §8.7–§8.9.

8.5.5 For non-primitive types, the operator is a call to the corresponding trait method. The trait methods for each operator are:

| Operator    | Trait method                                    |
| ----------- | ----------------------------------------------- |
| `+`         | `Add::add`, `AddAssign::add_assign`             |
| `-`         | `Sub::sub`, `SubAssign::sub_assign`             |
| `*`         | `Mul::mul`, `MulAssign::mul_assign`             |
| `/`         | `Div::div`, `DivAssign::div_assign`             |
| `%`         | `Rem::rem`, `RemAssign::rem_assign`             |
| `&`         | `BitAnd::bitand`, `BitAndAssign::bitand_assign` |
| `\|`        | `BitOr::bitor`, `BitOrAssign::bitor_assign`     |
| `^`         | `BitXor::bitxor`, `BitXorAssign::bitxor_assign` |
| `<<`        | `Shl::shl`, `ShlAssign::shl_assign`             |
| `>>`        | `Shr::shr`, `ShrAssign::shr_assign`             |
| `==`        | `PartialEq::eq`                                 |
| `!=`        | `PartialEq::ne`                                 |
| `<`         | `PartialOrd::lt`                                |
| `<=`        | `PartialOrd::le`                                |
| `>`         | `PartialOrd::gt`                                |
| `>=`        | `PartialOrd::ge`                                |
| `-` (unary) | `Neg::neg`                                      |
| `!` (unary) | `Not::not`                                      |
| `..`        | `RangeBounds` (§22.4)                           |
| `..=`       | `RangeBounds` (§22.4)                           |

8.5.6 The **dereference operator** `*e`:

- if `e` has type `&T` or `&mut T`, `*e` is a place of type `T`;
- if `e` has type `*const T` or `*mut T`, `*e` is a place of type `T`, and evaluating it is an `unsafe` operation (§16.2);
- `*e` is not defined for other types.

  8.5.7 The **borrow operators** `&e` and `&mut e` produce a reference to the place denoted by `e`. `&e` requires that `e` be a place expression; `&mut e` additionally requires that `e` be a mutable place (§7.7.3).

  8.5.8 The **logical operators** `&&` and `||` are **short-circuiting**: the right operand is evaluated only if the result is not determined by the left operand. They are not trait-dispatched; their operands **MUST** have type `bool`.

### 8.6 Operator Precedence and Associativity

8.6.1 Operator precedence, from **tightest binding** to **loosest**, is:

| Level | Operators                                                          | Associativity                  |
| ----- | ------------------------------------------------------------------ | ------------------------------ |
| 1     | Paths, method calls, field access, indexing, call, `?`             | left                           |
| 2     | Unary `-`, `!`, `*`, `&`, `&mut`                                   | prefix                         |
| 3     | `as`                                                               | left                           |
| 4     | `*`, `/`, `%`                                                      | left                           |
| 5     | `+`, `-`                                                           | left                           |
| 6     | `<<`, `>>`                                                         | left                           |
| 7     | `&`                                                                | left                           |
| 8     | `^`                                                                | left                           |
| 9     | `\|`                                                               | left                           |
| 10    | `==`, `!=`, `<`, `<=`, `>`, `>=`                                   | require parentheses for chains |
| 11    | `&&`                                                               | left                           |
| 12    | `\|\|`                                                             | left                           |
| 13    | `..`, `..=`                                                        | non-associative                |
| 14    | `=`, `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `\|=`, `^=`, `<<=`, `>>=` | right                          |
| 15    | `return`, `break`, `continue`, `yield`                             | prefix                         |

8.6.2 **Comparison operators are non-associative.** An expression such as `a < b < c` is a syntax error and **MUST** be parenthesized as `(a < b) < c` if that is intended. Implementations **MUST** reject chained comparison operators without parentheses with a diagnostic explaining the requirement.

8.6.3 **Range operators are non-associative.** An expression such as `a..b..c` is a syntax error. Ranges may appear as the operand of another operator only if parenthesized.

8.6.4 The **assignment operators are right-associative**: `a = b = c` parses as `a = (b = c)`. However, the left operand of an assignment **MUST** be a place expression, and the type of the right operand **MUST** be assignable to the left (§8.10).

8.6.5 Parentheses may be used to override precedence and associativity. Parenthesized expressions do not change the evaluation order of their subexpressions (§8.18).

### 8.7 Arithmetic and Overflow Behavior

8.7.1 For built-in integer types, the arithmetic operators `+`, `-`, `*`, `/`, `%` are defined as follows:

- **Addition, subtraction, multiplication**: if the mathematical result is representable in the operand type, the result is that value. If the result is not representable, the behavior depends on the **overflow mode** of the operation.
- **Division**: `a / b` for integers is **truncating** division toward zero. If `b` is zero, the operation **MUST** panic (§21.2). If `a` is the minimum value of the type and `b` is `-1`, the mathematical result is not representable; this is **overflow**.
- **Remainder**: `a % b` has the sign of the dividend `a` and satisfies `a == (a / b) * b + (a % b)`. Division by zero **MUST** panic. Remainder of the minimum value by `-1` produces `0` and is not overflow.

  8.7.2 The **overflow mode** of an operation is one of:

- **Checked**: if the result is not representable, the operation **MUST** panic (§21.2).
- **Wrapping**: the result is the two's-complement wrap of the mathematical result modulo `2^N`.
- **Saturating**: the result is the maximum or minimum value of the type, whichever is closer to the mathematical result.

  8.7.3 The default overflow mode for the operators `+`, `-`, `*`, `/`, `%` is **implementation-defined**, but it **MUST** be consistent across an entire program compiled in one invocation. An implementation **MUST** document whether its default is checked or wrapping. In debug builds, an implementation **SHOULD** use checked mode; in release builds, an implementation **SHOULD** use wrapping mode.

  8.7.4 The checked and wrapping operations are also available as explicit methods (§22.3): `checked_add`, `wrapping_add`, `saturating_add`, and their counterparts for other operators. These methods **MUST** be available on all integer primitive types and **MUST** have the semantics of §8.7.2 regardless of the default mode.

  8.7.5 **Shift operators** `<<` and `>>`:

- For `uN` and `iN`, the shift amount **MUST** be less than `N`. A shift amount greater than or equal to `N` is **undefined behavior** in wrapping mode and **MUST** panic in checked mode.
- `>>` on a signed integer is an **arithmetic shift**: it replicates the sign bit. `>>` on an unsigned integer is a **logical shift**: it shifts in zeros.
- The result type of a shift is the type of the left operand. The right operand **MAY** have a different integer type; it is converted to the left operand's type before the shift.

  8.7.6 **Unary negation** `-a` on an integer has the same overflow rules as subtraction from zero. Negating the minimum value of a signed type is overflow.

  8.7.7 For floating-point types (§6.4), arithmetic follows IEEE 754 with the following clarifications:

- overflow produces an infinity of the appropriate sign;
- underflow produces a subnormal or zero;
- `0.0 / 0.0` and `inf - inf` produce NaN;
- division by zero produces infinity of the appropriate sign, not a panic.

  8.7.8 An implementation **MUST NOT** transform an expression with potential overflow into a form that has different overflow behavior, unless the program has explicitly opted into such transformations via an attribute (§23.3).

### 8.8 Comparison and Logical Operators

8.8.1 The equality operators `==` and `!=` on built-in types are defined as follows:

- integer types: mathematical equality of the integer values;
- floating-point types: IEEE 754 equality (`NaN != NaN` is `true`, all other comparisons with NaN are `false`);
- `bool`: `true == true` and `false == false`;
- `char`: equality of the scalar values;
- references: equality of the referred values (not the addresses), if the referent implements `PartialEq`; otherwise the reference types are not comparable;
- raw pointers: equality of the addresses;
- function pointers: equality of the function addresses;
- tuples, arrays, and structs: **lexicographic** equality over their fields or elements, if every component type implements `PartialEq`;
- enums: two values are equal if they have the same variant and their payloads are equal.

  8.8.2 The ordering operators `<`, `<=`, `>`, `>=` are defined only for types that implement `PartialOrd`. For floating-point types, the NaN comparison rules of IEEE 754 apply: any comparison involving NaN is `false`.

  8.8.3 The **logical operators** `&&` and `||` operate only on `bool` operands. `&&` returns `true` if both operands are `true`; `||` returns `true` if either operand is `true`. They short-circuit as specified in §8.5.8.

  8.8.4 The **logical not** operator `!` operates on `bool` and on integer types (as bitwise complement). For `bool`, `!true == false` and `!false == true`. For integers, `!a` is the bitwise complement of `a`.

### 8.9 Bitwise Operators

8.9.1 The bitwise operators `&`, `|`, `^` operate on integer types and on `bool`:

- for integers, they are the bitwise AND, OR, and XOR of the operand bits;
- for `bool`, `&` is logical AND without short-circuiting, `|` is logical OR without short-circuiting, and `^` is logical XOR.

  8.9.2 The **bitwise not** `!` for integers is the complement of the operand's bits. For `bool`, it is logical negation (§8.8.4).

  8.9.3 The shift operators `<<` and `>>` are specified in §8.7.5.

  8.9.4 Bitwise operators on integers of different types are not permitted; the operands **MUST** have the same type. An implementation **MUST NOT** implicitly promote the smaller operand.

### 8.10 Assignment and Compound Assignment

8.10.1 The **assignment expression** `lhs = rhs`:

- requires `lhs` to be a mutable place expression (§7.7.3);
- evaluates `rhs` to a value;
- drops the current value of `lhs` (§7.3);
- stores the value of `rhs` in `lhs`;
- evaluates to `()`.

  8.10.2 The **drop-before-write** order is significant for types with destructors. An implementation **MUST** drop the old value of `lhs` before storing the new value, except when doing so would be observable differently due to `unsafe` code, in which case the order is unspecified.

  8.10.3 The **compound assignment** operators `+=`, `-=`, `*=`, `/=`, `%=`, `&=`, `|=`, `^=`, `<<=`, `>>=` are equivalent to `lhs = lhs op rhs`, except that the place expression `lhs` is evaluated only once:

```
lhs += rhs   // equivalent to  { let place = &mut lhs; *place = *place + rhs; }
```

8.10.4 For compound assignment on integers, the operation follows the overflow rules of §8.7. For `Copy` types, the left operand's old value is read by copy; for non-`Copy` types, the operation **MUST** be implemented by a trait method (§8.5.5) that consumes the left value.

8.10.5 Assignment to a place that is borrowed in a way that would invalidate the borrow is rejected by the borrow checker (§7.5.4).

8.10.6 The **destructuring assignment** `(a, b) = (b, a)` is a syntactic sugar for a series of assignments. Each component is evaluated in left-to-right order, and the entire expression evaluates to `()`.

### 8.11 Calls and Method Calls

8.11.1 A **call expression** `callee(args...)`:

- evaluates `callee` to a callable value: a function item, a function pointer, a closure, or a type implementing the `Fn` traits (§14.2);
- evaluates each argument in left-to-right order (§8.18);
- invokes the callable with the arguments;
- produces the callable's return value.

  8.11.2 The **callable** may be a path to a function, a field of a struct, or any expression of a callable type. The callee is evaluated before the arguments.

  8.11.3 The number and types of the arguments **MUST** match the callable's parameter types, subject to inference (§8.4.6). Passing a value to a function transfers ownership unless the value is `Copy` (§7.8).

  8.11.4 A **method call** `receiver.method(args...)`:

- resolves the method `method` on the type of `receiver` using the method resolution rules of §5.6.4;
- evaluates `receiver` to a place, and performs an auto-reference and auto-dereference to match the method's `self` parameter;
- evaluates the arguments in left-to-right order;
- invokes the method and produces its return value.

  8.11.5 **Auto-reference and auto-dereference** for method calls: if `receiver` has type `T` and the method is defined on `U` with receiver type `&U`, `&mut U`, `U`, or `Box<U>`, the compiler inserts the minimal sequence of dereferences and borrows to match. The sequence tried, in order, is: `T`, `&T`, `&mut T`, then dereferences of `T`. If multiple methods match, the most specific is selected (§5.6.4); if none matches, the call is an error.

  8.11.6 A **static method call** `Type::method(args...)` is a call whose callee is an associated function (§14.2) that does not take `self`. It is resolved in the type's associated item scope.

  8.11.7 The **turbofish** syntax `function::<TypeArgs>(args...)` explicitly provides the type arguments of a generic function, bypassing inference for those arguments (§13.3).

  8.11.8 The argument list **MUST** be evaluated in left-to-right order. An implementation **MUST NOT** evaluate arguments in a different order, because side effects in argument expressions are observable.

### 8.12 Field Access and Indexing

8.12.1 A **field access** expression `e.field`:

- if `e` is a place expression, `e.field` is a place expression denoting the field of the struct at `e`;
- if `e` is a value expression, `e.field` reads the field out of the value, copying or moving it according to the field type's `Copy`-ness (§7.8);
- the field **MUST** be accessible at the current visibility (§5.4).

  8.12.2 A **tuple field access** `e.N` is a field access where `N` is a decimal integer literal. It selects the `N`-th element of a tuple, tuple struct, or tuple-like struct. The index is zero-based and **MUST** be in range.

  8.12.3 An **indexing expression** `e[i]`:

- if `e` is an array `[T; N]` or a slice `[T]`, `i` **MUST** have type `usize`, and `e[i]` is a place expression denoting the `i`-th element;
- if `e` is a tuple, indexing with a constant integer selects the element (equivalent to field access);
- if `e` is `&[T]` or `&mut [T]`, auto-dereference applies: the indexing is performed on the referent;
- for any other type implementing `Index` and `IndexMut` (§22.4), the indexing is a call to those trait methods.

  8.12.4 **Bounds checking.** Indexing an array or slice with an out-of-bounds index **MUST** panic (§21.2) in safe code. In `unsafe` code using `get_unchecked`, the caller **MUST** ensure the index is in bounds; an out-of-bounds `get_unchecked` is undefined behavior.

  8.12.5 The **address-of** operation on a field `&e.field` borrows only the field, not the whole struct, so sibling fields remain available for borrowing (§7.5.3).

### 8.13 Casts and Conversions

8.13.1 The **cast expression** `e as T` converts the value of `e` to type `T`, subject to the rules of this section. Casts are checked at compile time for compatibility, but some casts are value-dependent at runtime (§8.13.6).

8.13.2 **Numeric casts** between integer and floating-point types:

- integer to integer: truncates or extends according to the destination width; signed to unsigned reinterprets the two's-complement bits; unsigned to signed reinterprets the bits;
- integer to float: rounds to nearest, with ties to even; may lose precision for large integers;
- float to integer: truncates toward zero; a float that is NaN, infinite, or out of range is **undefined behavior** in `unsafe`-free contexts; the safe conversion functions (§22.3) return an `Option` or saturate instead;
- float to float: rounds to the destination format, following IEEE 754 rounding rules.

  8.13.3 **Pointer casts**: `*const T` to `*mut T`, `*T` to `*U`, and function pointer to function pointer are permitted. Casting a reference to a raw pointer is permitted. Casting a raw pointer back to a reference is an `unsafe` operation (§16.2).

  8.13.4 **Boolean and character casts**: `bool as u8` produces `0` or `1`; `u8 as char` is **not** permitted, because not every byte is a valid scalar value; `char as u32` produces the scalar value.

  8.13.5 **Enum casts**: a fieldless enum `E` may be cast to its underlying integer type with `e as uN`; the integer type **MUST** be large enough to hold all discriminants. The reverse conversion `int as E` is an `unsafe` operation that is undefined behavior if the integer does not correspond to a discriminant.

  8.13.6 **Trait object casts**: `&T as &dyn Trait` creates a trait object if `T: Trait` and `Trait` is object-safe (§14.8). The vtable is selected at compile time based on the static type of `T`.

  8.13.7 The following casts are **not permitted**:

- casting to or from a type without a defined representation (§6.14.2);
- casting to `bool` from any type other than `bool`;
- casting to `char` from any type other than `u8` or `u32`, and from `u8` only if it is a valid ASCII scalar value (the cast `u8 as char` is not provided; use `char::from_u32` instead);
- casting between a pointer and an integer without an explicit `unsafe` block (§16.2).

  8.13.8 Casts between references and raw pointers, and between function pointers and integers, are `unsafe` and are specified by §16.2 and §16.3.

### 8.14 Blocks and Control-Flow Expressions

8.14.1 A **block expression** `{ stmts }`:

- has a value equal to the value of its **tail expression**, if present, or `()` if the block ends with a semicolon or a non-tail statement;
- has the type of its tail expression, or `()` if there is no tail expression.

  8.14.2 The **tail expression** is the final expression of the block, written without a trailing semicolon. A block whose last statement is an expression statement (with semicolon) evaluates to `()`.

  8.14.3 The statements of a block are evaluated in order. Each statement is a **statement** as defined in Chapter 9.

  8.14.4 **If expressions** `if cond { ... } else { ... }`:

- `cond` **MUST** have type `bool`;
- if `cond` is `true`, the first block is evaluated; otherwise the `else` block, if present, is evaluated;
- the two blocks **MUST** have the same type if the `if` is used as a value;
- the `else` branch **MUST** be present if the `if` is used as a value;
- an `else if` chain is `else` followed by another `if` expression.

  8.14.5 **Match expressions** `match e { pat => expr, ... }` are specified in §8.15.

  8.14.6 **Loop expressions** are expressions of type `!` in their body and may produce a value via `break value` (§9.7.4).

  8.14.7 **Return expressions** `return e` have type `!`. They transfer control to the enclosing function with the value of `e` as the return value.

  8.14.8 **Break and continue expressions** have type `!` in their body and control loop execution (§9.8).

### 8.15 Match Expressions

8.15.1 A **match expression** has the form:

```
match-expr ::= "match" scrutinee "{" match-arm {"," match-arm} [","] "}"
match-arm  ::= pattern ["if" guard] "=>" expr
```

8.15.2 The `scrutinee` is a place or value expression. Each `match-arm` is tried in order. The first arm whose `pattern` matches the scrutinee and whose `guard` evaluates to `true` is selected, and its expression is evaluated and becomes the value of the match.

8.15.3 The **scrutinee's ownership** is determined by the bindings in the patterns. If a pattern binds a variable by value, the scrutinee is moved into the arm. If all patterns bind by reference, the scrutinee is borrowed. The compiler **MUST** choose the least restrictive borrowing consistent with all patterns; if different arms require different ownership modes, the program is ill-typed.

8.15.4 **Exhaustiveness**: a match expression **MUST** be exhaustive, meaning that every possible value of the scrutinee's type is matched by at least one arm. A non-exhaustive match is a required error (§24.1). The `_` wildcard pattern (§10.4) matches any value and makes a match exhaustive.

8.15.5 **Reachability**: an arm that can never be selected because all of its values are already matched by earlier arms is **unreachable**. An unreachable arm **MUST** produce a warning. An implementation **MAY** promote this warning to an error under a lint configuration (§24.3).

8.15.6 **Guards**: an arm's `guard` is an expression of type `bool` that may reference bindings introduced by the pattern. If the pattern matches but the guard is `false`, the next arm is tried. The guard is evaluated in the scope of the pattern's bindings, but the bindings are not moved from the scrutinee until the arm is selected.

8.15.7 The **scrutinee is evaluated once**, before any arm is tried. Side effects in the scrutinee occur exactly once per match.

8.15.8 The types of all arm expressions **MUST** unify to a single type. If any arm has type `!`, it is coerced to the common type (§6.15.3).

8.15.9 The `match` on an enum **MUST** cover all variants, either explicitly or through a `_` arm. A match on a `bool` **MUST** cover both `true` and `false`, or have a `_` arm.

### 8.16 Closures and Function Expressions

8.16.1 A **closure** is an anonymous function with captured environment. Its syntax is:

```
closure ::= ["move"] "|" params "|" ["->" type] expr
```

or, with a block body:

```
closure ::= ["move"] "|" params "|" ["->" type] block
```

8.16.2 A closure has a unique, compiler-generated type that implements one or more of the `Fn`, `FnMut`, and `FnOnce` traits (§14.2). The trait implemented depends on how the closure captures and uses its environment:

- `Fn`: the closure borrows its captures immutably and may be called multiple times through a shared reference;
- `FnMut`: the closure mutates its captures and may be called multiple times through a mutable reference;
- `FnOnce`: the closure moves out of its captures and may be called only once.

  8.16.3 The **capture mode** of a closure is inferred. By default, the closure captures the minimal set of variables by the least restrictive mode (by reference when possible). The `move` keyword forces all captures to be moved into the closure.

  8.16.4 A closure's **parameter types** and **return type** may be inferred from context. If the closure is passed to a function expecting a specific `Fn` trait, the parameters are inferred from the expected type. If the types cannot be inferred, the program is ill-typed.

  8.16.5 Calling a closure `f(args...)` is a call expression (§8.11). The closure is `Fn`, `FnMut`, or `FnOnce` depending on its captures; the call dispatches to the appropriate trait method.

  8.16.6 A closure **MUST NOT** outlive its captured environment unless every capture is `move`d or borrowed with a sufficiently long lifetime. The borrow checker enforces this.

  8.16.7 A **function expression** `fn(params) -> type { body }` may appear in expression position and denotes a function item (which coerces to a function pointer, §8.4.7). Function expressions do not capture the environment.

### 8.17 Aggregate Expressions

8.17.1 An **aggregate expression** creates a value of a composite type. The forms are:

- **struct expression** `StructName { field: expr, ... }` for named-field structs;
- **tuple struct expression** `StructName(expr, ...)` for tuple structs;
- **tuple expression** `(expr, ...)` for tuples;
- **array expression** `[expr, ...]` or `[expr; n]` for arrays;
- **enum variant construction** `EnumName::Variant { ... }` or `EnumName::Variant(expr, ...)`.

  8.17.2 **Field shorthand** `StructName { field }` is sugar for `StructName { field: field }`, where `field` is a local binding of the same name.

  8.17.3 **Struct update syntax** `StructName { field: expr, ..base }` constructs a new struct by taking the named fields from the initializers and the remaining fields from `base`. The `base` is evaluated once and its moved fields are moved out of it; `base` **MUST** be a place expression, and it is partially moved.

  8.17.4 An **array expression** `[expr; n]` is **array repeat** and requires `expr` to be `Copy` (§7.8.2), or `n` to be `0`, or `expr` to be a `const` expression (§8.19). Non-`Copy` values cannot be repeated because each repetition would require a distinct value.

  8.17.5 The **evaluation order** of aggregate expressions is left to right for tuple and array elements, and the order written in source for struct fields (not the declaration order of the fields). An implementation **MUST NOT** reorder field initializers.

  8.17.6 A **composite literal** used in a `const` context (§8.19) is a constant expression if every component is a constant expression.

### 8.18 Evaluation Order

8.18.1 Unless otherwise specified, the operands of an expression are evaluated **left to right**. This applies to:

- binary operators (left then right);
- function call arguments (in source order);
- tuple and array elements (in source order);
- aggregate fields (in source order);
- assignment (right-hand side before left-hand side's drop, and left-hand side's place is evaluated before the right-hand side's value in a simple assignment).

  8.18.2 The **left operand of an assignment** is a place, and evaluating it may have side effects (for example, `a[i()] = value()`). The order is: evaluate the place `a[i()]` (including `i()`), evaluate `value()`, then drop the old value and store the new one.

  8.18.3 The **scrutinee of a match** is evaluated once, before any arm. The **guard** of an arm is evaluated after the pattern matches but before the arm's expression is evaluated.

  8.18.4 The **condition of an `if` or loop** is evaluated before the body. In a `while` loop, the condition is evaluated before each iteration, including the first.

  8.18.5 The **short-circuit operators** `&&` and `||` evaluate the left operand, then, only if the result is not determined, the right operand (§8.5.8).

  8.18.6 The **evaluation order of closures** and function bodies is determined by the function body, not by the call site. The call site evaluates the callee and arguments as specified in §8.11.

  8.18.7 An implementation **MUST NOT** reorder expressions in a way that changes the observable side effects. In particular, an implementation **MUST NOT** move a side-effecting expression past another side-effecting expression. Side-effect-free expressions may be reordered freely.

### 8.19 Constant Expressions

8.19.1 A **constant expression** is an expression that can be evaluated at compile time. Constant expressions appear in the following contexts:

- the initializer of a `const` item (§11.3);
- the initializer of a `static` item (§11.3);
- the length `N` in an array type `[T; N]` and in the `[expr; N]` array repeat;
- the argument to an attribute macro that requires a constant (§23);
- the value of an enum discriminant;
- the value of a `match` pattern that is a range (§10.8).

  8.19.2 The forms of constant expression are a subset of expression forms. The following are **constant expressions**:

- literals of scalar types (§8.3);
- paths to `const` items and to `static` items (for `static`, only the address is constant);
- arithmetic, bitwise, comparison, and logical operators on constant expressions of scalar types, provided no operation panics;
- casts between integer types (§8.13.2);
- tuple, array, and struct construction from constant expressions;
- field access on a constant struct or tuple;
- function calls to `const fn` functions (§20.2) with constant arguments;
- blocks that contain only constant statements and whose tail expression is constant;
- `if` and `match` expressions whose condition and arms are constant;
- references to `static` items and string literals.

  8.19.3 The following are **not** constant expressions:

- allocation (calling the allocator);
- any operation that has a runtime side effect;
- calls to functions that are not `const fn`;
- dereferencing a raw pointer;
- any expression involving mutable state that is not `static mut` accessed in `const` context (which is restricted further by §20.1).

  8.19.4 The **value** of a constant expression is computed by the compiler using the constant evaluation rules of §20.1. A constant expression that would panic, overflow (in checked mode), or produce undefined behavior is a **compile-time error**.

  8.19.5 A `const` item's initializer is evaluated at compile time and its value is substituted at each use site. Constants have no address and are not allocated at runtime, unless they are `static` items.

  8.19.6 `static` items have a runtime address and are initialized at program startup (§19.7). Their initializers **MUST** be constant expressions, but a `static mut` may be modified at runtime under `unsafe` (§16).

---

## 9. Statements and Control Flow

### 9.1 Blocks

9.1.1 A **block** `{ stmts }` is a sequence of statements enclosed in braces. A block introduces a new **block scope** (§5.3.1); bindings declared inside the block are visible only within it.

9.1.2 The **result** of a block is its tail expression's value, or `()` if the block has no tail expression or ends with a semicolon (§8.14.1).

9.1.3 A block **MAY** contain item declarations (§11) mixed with statements. Items declared in a block are visible throughout the block (§5.3.4) but not outside it.

9.1.4 The **drop order** of a block's bindings is reverse declaration order (§7.3.3). The tail expression is evaluated before the block's bindings are dropped.

### 9.2 Item Declarations as Statements

9.2.1 An **item declaration** at statement position declares a module-scoped or block-scoped item. Items declared in a block are **block-scoped** and visible only within that block.

9.2.2 An item declaration statement **MAY** omit its terminating semicolon if the item ends with `}`. Item declarations that end with `;` (such as type aliases) **MUST** include the semicolon.

9.2.3 Item declarations in a block are **hoisted** (§5.3.4) and are visible throughout the block, including before their textual position. Local `let` bindings are not hoisted.

### 9.3 Let Bindings

9.3.1 A **let statement** introduces one or more local bindings:

```
let-stmt ::= "let" pattern [":" type] ["=" expr] ";"
```

9.3.2 The `pattern` **MUST** be **irrefutable** (§10.10.2), except in a `let-else` statement (§9.3.7). A refutable pattern in a plain `let` is an error.

9.3.3 The **scope** of a `let` binding is from the end of the `let` statement to the end of the enclosing block. The binding is not visible in its own initializer.

9.3.4 A `let` statement **MAY** omit the initializer, in which case the binding is **uninitialized**. Reading an uninitialized binding is an error (§7.3.1). Uninitialized bindings are useful in combination with later assignment:

```
let x: i32;
if cond {
    x = 1;
} else {
    x = 2;
}
println!("{}", x); // definitely initialized
```

9.3.5 The **type annotation** `: T` is optional; if present, the initializer's value is coerced to `T` and the binding's type is `T`. If absent, the type is inferred from the initializer.

9.3.6 A **mutable binding** `let mut x = ...` permits assignment to `x`. Mutability does not transfer to fields reached through a shared reference.

9.3.7 A **let-else statement** `let pat = expr else { diverging-block };`:

- `pat` **MAY** be refutable;
- `expr` is evaluated;
- if `pat` matches, its bindings are introduced into the enclosing scope;
- otherwise, the `else` block is evaluated, and it **MUST** diverge (have type `!`, §9.11).

  9.3.8 The **drop order** of `let` bindings is reverse declaration order (§7.3.3). A `let` that shadows a previous binding does not drop the previous binding until the end of the scope; the previous binding remains alive but inaccessible.

### 9.4 Assignment Statements

9.4.1 An **assignment statement** is an expression statement (§9.5) whose expression is an assignment or compound assignment (§8.10).

9.4.2 The **left-hand side** of an assignment **MUST** be a mutable place (§7.7.3). The **right-hand side** **MUST** be assignable to the left's type, subject to coercion (§6.15).

9.4.3 The value of an assignment expression is `()`, so it is legal as a statement and produces no value.

9.4.4 The **drop of the old value** occurs before the store of the new value (§8.10.2).

### 9.5 Expression Statements

9.5.1 An **expression statement** evaluates an expression for its side effects and discards its value. Its grammar is:

```
expr-stmt ::= expr ";" | block-expr-without-semicolon
```

9.5.2 An expression statement that evaluates to a value other than `()` **MUST** be terminated by `;`, or the value is silently discarded. An implementation **SHOULD** warn about discarding a value whose type is not `()` and is not `#[must_use]`, under the unused-value lint (§24.3).

9.5.3 The following expressions may be used as statements **without** a trailing semicolon: `if`, `match`, `loop`, `while`, `for`, block expressions, and `unsafe` blocks. This is because their bodies already include braces and the semicolon would be redundant.

9.5.4 A **block expression statement** without a semicolon has its value discarded, same as with a semicolon. The absence of the semicolon is purely stylistic in statement position.

### 9.6 Conditional Statements

9.6.1 An **if statement** is an `if` expression used in statement position (§8.14.4). Its `then` block is evaluated if the condition is `true`.

9.6.2 The **condition** of an `if` **MUST** have type `bool`. No implicit conversion from integers or pointers is performed.

9.6.3 An `if` **MUST NOT** have a tail value if used as a statement without `else`. If used as an expression in value position, the `else` branch is required (§8.14.4).

9.6.4 An `else if` chain is evaluated top to bottom. The first condition that is `true` selects its block; subsequent conditions are not evaluated.

9.6.5 The **bodies** of an `if` are **block expressions**. A non-block body would be a syntax error, ensuring consistent scoping and drop behavior.

### 9.7 Loops

9.7.1 A **loop expression** is one of the following:

```
loop-expr  ::= "loop" block
             | "while" condition block
             | "for" pattern "in" iterable block
             | "while" "let" pattern "=" expr block
```

9.7.2 A **`loop`** expression repeats its block indefinitely until a `break` or `return` exits. Its type is the type of its `break` values (§9.7.4), or `!` if it never breaks with a value.

9.7.3 A **`while`** loop evaluates its condition before each iteration. If the condition is `false`, the loop exits. If `true`, the block is evaluated and the condition is re-checked. The condition **MUST** have type `bool`.

9.7.4 A **`for`** loop iterates over an `iterable`:

- if `iterable` implements `IntoIterator` (§22.4), the loop calls `into_iter()` once to obtain an iterator and then repeatedly calls `next()`;
- each yielded item is matched against the `pattern` (which **MUST** be irrefutable) and the block is evaluated;
- the loop ends when `next()` returns `None`.

  9.7.5 A **`while let`** loop is a `while` loop whose condition is a pattern match:

```
while let Some(x) = iter.next() { ... }
```

is equivalent to:

```
loop {
    match iter.next() {
        Some(x) => { ... }
        _ => break,
    }
}
```

The pattern **MAY** be refutable; the loop exits when the pattern does not match.

9.7.6 The **loop body** is a block. The loop's value is the value of the `break` expression that exits it (§9.7.4), or `()` if no value is broken out.

9.7.7 A loop **MUST** be well-formed with respect to the borrow checker. Specifically, a borrow that is live across an iteration boundary is subject to §7.5.2; the borrow checker enforces this statically.

### 9.8 Break, Continue, and Return

9.8.1 A **`break` expression**:

- with no operand: exits the innermost enclosing loop with value `()`;
- with an operand `break value`: exits the innermost enclosing loop, and the loop's value is `value`;
- with a label `break 'label value`: exits the loop labeled `'label` and produces `value`.

  9.8.2 A **`continue` expression**:

- with no operand: skips to the next iteration of the innermost enclosing loop;
- with a label `continue 'label`: skips to the next iteration of the loop labeled `'label`.

  9.8.3 A **`return` expression** `return value` exits the enclosing function with `value`. `return` without an operand is equivalent to `return ()`.

  9.8.4 The **type** of `break`, `continue`, and `return` is `!` (§6.6). They may appear wherever an expression of any type is expected, because `!` is a subtype of every type (§6.15.3).

  9.8.5 A `break` **MUST** be inside a loop. A `continue` **MUST** be inside a loop. A `return` **MUST** be inside a function. An implementation **MUST** report these as errors.

  9.8.6 The **labels** of loops and blocks are written `'label: loop { ... }` and `'label: { ... }`. A `break 'label value` exits the labeled block or loop. A labeled block may be exited with a `break`, and its value becomes the block's value.

  9.8.7 A label **MUST** be unique within its enclosing function. A `break 'label` **MUST** refer to a label in scope.

### 9.9 Match and Pattern Matching

9.9.1 The `match` expression is specified in §8.15. This section specifies its use as a control-flow construct and its interaction with patterns.

9.9.2 The **scrutinee** of a `match` is evaluated once. The arms are tried in order; the first matching arm's expression is evaluated and its value becomes the match's value.

9.9.3 **Pattern matching order**: patterns are matched structurally. A pattern matches a value if the value's shape corresponds to the pattern's structure and every sub-pattern matches the corresponding sub-value.

9.9.4 **Binding modes**: a pattern binding may bind by value, by reference (`ref`), or by mutable reference (`ref mut`). The default binding mode is determined by the scrutinee's ownership and the match ergonomics rules:

- if the scrutinee is a place and the pattern is a reference pattern (`&pat`), the binding mode is inherited from the reference;
- if the scrutinee is a reference and the pattern is a non-reference pattern, **default binding mode** is set to `ref` or `ref mut` according to the reference's mutability;
- explicit `ref`/`ref mut` override the default.

  9.9.5 The **exhaustiveness** and **reachability** rules of §8.15.4–8.15.5 apply to every `match`, including those in statement position.

  9.9.6 A `match` arm whose pattern is a **binding** (a plain identifier) matches any value and is equivalent to the `_` wildcard, except that it introduces a binding. A `match` with a single binding arm is equivalent to a `let` binding followed by the arm's expression.

### 9.10 Defer / Scope Exit

9.10.1 A **defer statement** `defer block;` registers a block to be evaluated when the enclosing scope exits, in **reverse order** of registration, after the scope's locals are dropped.

9.10.2 The **deferred block** is evaluated on every exit path from the scope: normal completion, `return`, `break`, `continue`, or an unwinding panic (§21.4). A deferred block **MUST NOT** use `return`, `break`, or `continue` to alter the enclosing scope's control flow; doing so is an error.

9.10.3 The **deferred block** captures the environment at the point of registration, not at the point of execution. It may reference variables declared before the `defer` statement. The borrow checker treats the deferred block as a use of those variables at the end of the scope.

9.10.4 `defer` is **not** a replacement for destructors (§7.3). Destructors run when a value is dropped, which may be before the enclosing scope exits if the value is moved or explicitly dropped. `defer` runs only at scope exit.

9.10.5 A `defer` statement **MUST** appear in a block, not at item scope. A `defer` inside a loop registers a new block on each iteration.

### 9.11 Unreachable Code and Divergence

9.11.1 A **diverging expression** is an expression of type `!` that does not produce a value: `return`, `break`, `continue`, a call to a function returning `!`, an `if` whose both branches diverge, a `loop` with no `break`, and a `panic!` macro.

9.11.2 An expression of type `!` is **coercible** to any type (§6.15.3). This allows diverging expressions in any value position, including as the tail of a block whose type is inferred from context.

9.11.3 **Unreachable code** is code that follows a diverging expression in the same block, with no intervening `break`, `continue`, or label. Unreachable code **MUST** produce a warning under the `unreachable_code` lint (§24.3). An implementation **MAY** promote this warning to an error.

9.11.4 The **`unreachable!()`** macro evaluates to `!` and may be used to indicate a code path the programmer believes is unreachable. Executing `unreachable!()` **MUST** panic with a message indicating an unreachable branch (§21.2).

9.11.5 A **function whose return type is `!`** never returns normally. Its body **MUST** diverge on every path; a function with a `!` return type that falls through its tail is an error. The only way for such a function to return is to diverge (for example, by calling another `!`-returning function or by panicking).

9.11.6 The **`loop` expression without any `break`** has type `!` because control never leaves it normally. The `while` and `for` loops always have type `()` because they may terminate normally.

---

## 10. Patterns

### 10.1 Pattern Syntax

10.1.1 A **pattern** describes a structure that a value may have, and binds names to parts of the value. Patterns appear in `let` statements, function parameters, `match` arms, `for` loops, and closure parameters.

10.1.2 The pattern grammar is:

```
pattern        ::= or-pattern
or-pattern     ::= range-pattern { "|" range-pattern }
range-pattern  ::= literal-pattern
                 | path-pattern
                 | binding-pattern
                 | wildcard-pattern
                 | reference-pattern
                 | tuple-pattern
                 | struct-pattern
                 | enum-pattern
                 | array-pattern
                 | slice-pattern
                 | range-pattern-syntax
                 | "(" pattern ")"
```

10.1.3 Every pattern has a **type** that must match the type of the value being matched (the **scrutinee**). A pattern that cannot match the scrutinee's type is an error.

### 10.2 Literal Patterns

10.2.1 A **literal pattern** is a literal expression (§8.3) and matches any value equal to that literal. The literal's type is constrained by the scrutinee's type.

10.2.2 For floating-point literals, a literal pattern matches if the value is bitwise equal to the pattern's value. `-0.0` and `0.0` are distinct bit patterns and match distinct literals.

10.2.3 For string literals, a pattern matches a `&str` value that is equal to the string. String patterns are `const`-comparable and are not supported for `String` (which is not a primitive pattern type).

10.2.4 A **negative literal** `-N` is a literal pattern for signed integer types. It matches the value `-N`.

### 10.3 Binding Patterns

10.3.1 A **binding pattern** is an identifier (or `mut ident`, `ref ident`, or `ref mut ident`) that matches any value and binds it to the name.

10.3.2 The **binding modes** are:

- `x` (by value): the value is moved or copied into the new binding;
- `ref x`: a reference to the matched value is bound;
- `ref mut x`: a mutable reference to the matched value is bound;
- `mut x` (by value, mutable): the value is moved or copied into a mutable binding.

  10.3.3 The **default binding mode** in a pattern is determined by the match ergonomics rules of §9.9.4. In a `let` pattern at the top level, the default is by value.

  10.3.4 A binding pattern's name **MUST** be a valid identifier. The name `_` is not a binding; see §10.4.

  10.3.5 A binding pattern is **irrefutable** (always matches) in isolation. Combining a binding with other patterns in a `let` **MUST** keep the whole pattern irrefutable.

### 10.4 Wildcard Patterns

10.4.1 The **wildcard pattern** `_` matches any value and binds no name. It may appear anywhere a pattern is expected.

10.4.2 The wildcard pattern is **irrefutable**. A match arm with the wildcard pattern matches any remaining value and makes the match exhaustive.

10.4.3 The wildcard pattern **does not** copy or move the matched value. When the wildcard matches a non-`Copy` value, the value is not consumed; this is relevant in match arms where the value is used after the match.

10.4.4 The identifier `_` in an expression position is not a value; using `_` as an expression is a syntax error.

### 10.5 Tuple and Struct Patterns

10.5.1 A **tuple pattern** `(p1, p2, ..., pn)` matches a tuple of `n` elements, matching each sub-pattern against the corresponding element.

10.5.2 A **tuple struct pattern** `StructName(p1, ..., pn)` matches a tuple struct or tuple enum variant, matching each sub-pattern against the corresponding field.

10.5.3 A **struct pattern** `StructName { field: pat, ... }` matches a named-field struct or struct variant:

- each named field is matched against its sub-pattern;
- omitted fields are ignored, unless the struct pattern uses `..` to explicitly ignore the rest;
- without `..`, all fields **MUST** be named;
- with `..`, the remaining fields are ignored (not bound, not matched).

  10.5.4 The **`..` rest pattern** `StructName { field, .. }` matches the named fields and ignores the rest. It may appear at most once per struct pattern.

  10.5.5 The **field shorthand** `StructName { field }` is sugar for `StructName { field: field }`.

  10.5.6 A tuple, tuple struct, or struct pattern **MUST** have the correct number of sub-patterns (for tuple patterns) or the correct set of named fields (for struct patterns). A mismatch is an error.

### 10.6 Enum and Variant Patterns

10.6.1 An **enum pattern** `EnumName::Variant` matches an enum value whose variant is `Variant`. With no payload, the pattern matches only the unit variant; with a payload, the sub-patterns are matched against the variant's fields:

- `EnumName::Variant(p1, ..., pn)` for tuple variants;
- `EnumName::Variant { field: pat, ... }` for struct variants.

  10.6.2 The **path** of an enum pattern **MUST** resolve to an enum variant. A path to a struct or a constant is not a variant pattern; structs use struct patterns (§10.5), and constants use literal patterns (§10.2) or path patterns (§10.9).

  10.6.3 A **fully qualified variant** `EnumName::<TypeArgs>::Variant` may be used to disambiguate generic enum variants (§13).

  10.6.4 **Exhaustiveness** for enum patterns requires either matching all variants or a wildcard arm (§8.15.4). The compiler **MUST** check exhaustiveness for enums and report non-exhaustive matches.

### 10.7 Reference Patterns

10.7.1 A **reference pattern** `&pat` matches a reference `&T` by dereferencing it and matching `pat` against the referent. A **mutable reference pattern** `&mut pat` matches a `&mut T` similarly.

10.7.2 A reference pattern **MUST** match a reference type. Matching a reference pattern against a non-reference type is an error.

10.7.3 A **double reference pattern** `&&pat` matches a `&&T`, dereferencing twice.

10.7.4 When a reference pattern binds a value, the **default binding mode** of the sub-pattern is set to `ref` (or `ref mut` for `&mut`), so that the binding does not move out of the reference. Explicit `ref`/`ref mut` override this.

### 10.8 Range Patterns

10.8.1 A **range pattern** matches an integer, `char`, or byte value within a half-open or inclusive range:

```
low..high    // half-open: low <= x < high
low..=high   // inclusive: low <= x <= high
```

10.8.2 Range patterns are **only** permitted for integer types, `char`, and `u8` (for bytes). They are not permitted for floating-point types, because floating-point ranges are not well-defined for NaN.

10.8.3 The bounds `low` and `high` **MUST** be **const expressions** (§8.19) and **MUST** be of the same type as the scrutinee. `low` **MUST** be less than or equal to `high`; a reversed range is an error.

10.8.4 Range patterns are **exhaustive** only when they cover the entire value range of the scrutinee's type. A range pattern that covers only part of the range does not make a match exhaustive on its own.

10.8.5 The `..` wildcard range pattern, written `_..=x` or `low.._`, is **not** supported. Use a range pattern with explicit bounds.

### 10.9 Or-Patterns and Guards

10.9.1 An **or-pattern** `p1 | p2 | ... | pn` matches if any of its sub-patterns matches. The bindings of all sub-patterns **MUST** be identical: each sub-pattern **MUST** bind the same set of names with the same types and binding modes.

10.9.2 An or-pattern's **irrefutability** is the disjunction of its sub-patterns' irrefutability: it is irrefutable if any sub-pattern is irrefutable.

10.9.3 The **`@` binding pattern** `name @ pat` binds the matched value to `name` and additionally matches `pat` against it. The `@` pattern is useful for binding a value while also constraining it:

```
match x {
    n @ 1..=5 => println!("small: {}", n),
    n => println!("large: {}", n),
}
```

10.9.4 A **guard** `if cond` on a `match` arm is evaluated after the pattern matches, in the scope of the pattern's bindings. If the guard evaluates to `false`, the match proceeds to the next arm. Guards **SHOULD NOT** have side effects, but are not required to be pure.

10.9.5 **Guards and exhaustiveness**: an arm with a guard does not count toward exhaustiveness, because the guard may be `false` for any value. A match whose only arm has a guard is not exhaustive unless a wildcard arm is present.

### 10.10 Exhaustiveness and Reachability

10.10.1 **Exhaustiveness** is the property that every possible value of the scrutinee's type is matched by at least one arm. The compiler **MUST** check exhaustiveness for every `match` and report a non-exhaustive match as a required error (§24.1).

10.10.2 **Irrefutability** is the property that a pattern matches every value of its type. `let` bindings, function parameters, and `for` loop patterns **MUST** be irrefutable. A refutable pattern in these positions is an error, except in `let-else` and `while let`, which explicitly accept refutable patterns.

10.10.3 The **reachability** of an arm is the property that some value of the scrutinee's type is matched by that arm and by no earlier arm. An unreachable arm **MUST** produce a warning (§8.15.5). An implementation **MAY** promote this warning to an error.

10.10.4 The **usefulness** algorithm used to check reachability and exhaustiveness **MUST** be sound: it **MUST NOT** classify a reachable arm as unreachable, and it **MUST NOT** classify a non-exhaustive match as exhaustive. The algorithm **MAY** be conservative: it **MAY** report an exhaustive match as non-exhaustive if it cannot prove exhaustiveness, but this is a quality-of-implementation concern, not a correctness one.

10.10.5 For matches on **integers** and **ranges**, exhaustiveness requires covering the full value range with non-overlapping ranges or a wildcard. The compiler **MUST** check this.

10.10.6 For matches on **structs and tuples**, exhaustiveness is determined by the exhaustiveness of the field patterns. A struct pattern with all fields irrefutably matched is irrefutable. A struct pattern with `..` is irrefutable if every named field's sub-pattern is irrefutable.

10.10.7 For matches on **references**, a reference pattern `&pat` is irrefutable if `pat` is irrefutable, because the reference is always non-null (for `&T`) and matches the reference pattern. Raw pointer patterns are not supported; raw pointers must be converted to references in `unsafe` code before pattern matching.

## Part III — Declarations and Abstraction

## 11. Items and Declarations

### 11.1 Item Categories

11.1.1 An **item** is a named declaration that introduces an entity into a module scope (§5.3.1). Items are the primary unit of organization in Forge and are the only declarations permitted at module scope.

11.1.2 The item categories of Forge are:

| Category            | Keyword(s) | Namespace    | Section |
| ------------------- | ---------- | ------------ | ------- |
| Function            | `fn`       | value        | §12     |
| Constant            | `const`    | value        | §11.3   |
| Static              | `static`   | value        | §11.3   |
| Struct              | `struct`   | type + value | §6.9    |
| Enum                | `enum`     | type + value | §6.9    |
| Union               | `union`    | type         | §6.9    |
| Type alias          | `type`     | type         | §6.11   |
| Trait               | `trait`    | type         | §14.1   |
| Implementation      | `impl`     | (none)       | §11.8   |
| Module              | `mod`      | type         | §11.6   |
| Use declaration     | `use`      | all imported | §5.5    |
| Foreign declaration | `extern`   | value / type | §11.9   |
| Macro               | `macro`    | macro        | §20.4   |
| Attribute macro     | `#[...]`   | macro        | §23.7   |

11.1.3 An item **MAY** carry an outer attribute `#[...]` before its declaration and inner attributes `#![...]` inside a block-like item's braces. Attributes are specified in Chapter 23.

11.1.4 An item's **visibility** (§5.4) is established by an optional `pub` modifier placed before the item keyword. The visibility applies to the item's name in its namespace; fields and variants carry their own visibility.

11.1.5 Items **MUST** be declared at module scope or block scope (§9.2). Items **MUST NOT** appear in expression position except as block-scoped items inside a block expression.

11.1.6 An item's **definition order** does not affect its visibility within its module: items are hoisted (§5.3.4). A module may declare items in any order, and cyclic references between items within a module are permitted.

11.1.7 An item **MUST NOT** be declared twice with the same name and namespace in the same module (§5.2.3), except for items that participate in overloading (§5.6.4) or in trait implementation merging (§14.3.6).

### 11.2 Visibility and Attributes

11.2.1 The visibility modifiers accepted on an item are those of §5.4.1. An item without a modifier is private to its enclosing module and its descendants (§5.4.2).

11.2.2 A visibility modifier **MUST** precede the item keyword and **MUST** precede any attributes that apply to the item as a whole, unless the attribute is explicitly documented to appear before visibility. An implementation **MUST** reject the malformed order with a diagnostic.

11.2.3 The **effective visibility** of an item is the minimum of its declared visibility and the visibility of every module on the path from the crate root to the item. An item with `pub` visibility inside a private module is not reachable from outside the crate (§5.4.3).

11.2.4 Attributes on an item **MAY** affect its visibility, representation, stability, and code generation. The attributes that are relevant to item declarations include:

- `#[repr(...)]` — representation of structs, enums, and unions (§17.3);
- `#[inline]`, `#[cold]`, `#[no_mangle]` — code generation (§12.7, §17.7);
- `#[deprecated]` — stability (§23.6);
- `#[cfg(...)]` — conditional compilation (§11.10);
- `#[derive(...)]` — automatic trait implementation (§14.7.5);
- `#[must_use]` — use-site diagnostics (§23.5).

  11.2.5 An unknown attribute **MUST** produce an error under the `unknown_attribute` lint (§24.3), unless the attribute is namespaced by a registered procedural macro (§20.5). An implementation **SHOULD NOT** silently ignore an unrecognized attribute.

### 11.3 Constants and Statics

11.3.1 A **constant** is declared with:

```
const-item ::= ["pub"] "const" ident ":" type "=" const-expr ";"
```

11.3.2 The initializer of a constant **MUST** be a **constant expression** (§8.19). It is evaluated at compile time, and its value is substituted at each use site.

11.3.3 A constant has **no address**. Two uses of the same constant may refer to different memory locations, and an implementation **MAY** duplicate the constant's value at each use site.

11.3.4 A constant **MUST** have a type annotation. The annotation is mandatory because the constant's value may be used in contexts where the constant's type must be known without reference to its initializer.

11.3.5 A **static** is declared with:

```
static-item ::= ["pub"] "static" ["mut"] ident ":" type "=" const-expr ";"
```

11.3.6 A static has a **fixed address** for the entire program. Its initializer **MUST** be a constant expression, and it is evaluated once at program startup or load time (§19.7).

11.3.7 A `static mut` may be modified at runtime. Reading or writing a `static mut` is an `unsafe` operation (§16), because the compiler cannot verify that accesses are synchronized. A `static` without `mut` is immutable and may be read from safe code.

11.3.8 The **address** of a static is a constant expression and may be used in other constant expressions (for example, as an array length or a raw pointer initializer). The **value** of a static is not a constant expression in general, because it may be modified.

11.3.9 A static's type **MUST** be `Sync` (§19.6) unless the static is declared with `unsafe` (§16.9). This requirement prevents a `static` from being a source of data races.

11.3.10 The **initialization order** of statics is unspecified, except that each static is initialized before the first access to it and before `main` begins (§3.4.4). A static initializer **MUST NOT** depend on the value of another static, because the initialization order is unspecified; an implementation **MUST** reject such a dependency if it can detect it statically.

11.3.11 A `const` item **MAY** be generic (§13). A generic constant is instantiated with type arguments, and each instantiation produces a distinct constant value. A `static` **MUST NOT** be generic.

### 11.4 Type Declarations

11.4.1 The type declarations of Forge are the **struct** (§6.9.1), **enum** (§6.9.4), **union** (§6.9.6), and **type alias** (§6.11.1) items. Their grammars are specified in Chapter 6.

11.4.2 A type declaration **MUST** be uniquely named in its namespace. A type and a value item of the same name may coexist if they are in different namespaces; for example, a struct `Point` introduces a type `Point` and a constructor `Point` in the value namespace.

11.4.3 The **fields** of a struct or union and the **variants** of an enum carry their own visibility (§5.4.4). A field's or variant's visibility **MUST NOT** exceed the visibility of the type that contains it.

11.4.4 A struct or enum **MUST** be fully defined at its declaration. Recursive types are permitted only through indirection: a struct field of the same type **MUST** be behind a reference, a raw pointer, or a boxed indirection (`Box<T>`, §22.4). A directly recursive struct would have infinite size and is an error.

11.4.5 An enum's **discriminant** is the integer that identifies its variant. Discriminants are specified explicitly with `= const-expr` after a variant name, or assigned automatically. Automatic assignment starts at `0` and increments by `1` for each variant without an explicit discriminant; a variant with an explicit discriminant resets the next automatic value to that discriminant plus one.

11.4.6 An enum's discriminant **MUST** be representable in the enum's discriminant type. The discriminant type is `isize` by default, or the integer type named by a `#[repr(int_type)]` attribute (§17.5).

11.4.7 A type alias **MUST** refer to a type that is defined or will be defined in the same crate, or to a type imported from another crate. Recursive type aliases are permitted only through indirection, as with structs.

### 11.5 Function Declarations

11.5.1 A **function** is declared with:

```
fn-item ::= ["pub"] ["const"] ["async"] ["unsafe"] ["extern" abi-string?]
            "fn" ident [generic-params] "(" params ")" ["->" type]
            ["where"-clause] ( block | ";" )
```

11.5.2 The `const` modifier declares a **compile-time function** (§20.2). The `async` modifier declares an **asynchronous function** whose call returns a future (§22.6). The `unsafe` modifier declares a function whose call is an `unsafe` operation (§16.1). The `extern` modifier declares a function with a foreign calling convention (§12.9).

11.5.3 A function declaration **MUST** have a body unless it is declared in an `extern` block (§11.9) or is a trait method without a default body (§14.2). A function with a body is a **definition**; one without is a **signature**.

11.5.4 The **name** of a function is bound in the value namespace (§5.1.1). A function **MAY** share its name with a type, a constant, or a variant, because those occupy different namespaces.

11.5.5 A function's **signature** consists of its parameter types, return type, generics, where-clause, calling convention, and safety/const/async modifiers. The signature is part of the function's type for purposes of function pointers (§12.6) and trait implementations (§14.3).

11.5.6 The **visibility** of a function is governed by §5.4. A `pub` function is part of its module's public interface. A private function is callable only within its module and descendant modules.

### 11.6 Module Declarations

11.6.1 A **module** is declared with:

```
mod-item ::= ["pub"] "mod" ident ( "{" items "}" | ";" )
```

11.6.2 A module declared with `{ ... }` is an **inline module**. Its items are declared textually within the braces. An inline module introduces a new module scope (§5.3.1).

11.6.3 A module declared with `;` is a **file module**. The module's contents are read from a source file whose path is determined by the module's name and the enclosing module's path, according to the module file resolution rules of §15.1.4.

11.6.4 The **visibility** of a module controls whether its items are reachable from outside. A `pub` module is reachable from its parent; a private module is reachable only within its parent and its descendants. An item inside a module is reachable from outside only if the module and every enclosing module are `pub` (or otherwise visible).

11.6.5 A module **MUST NOT** be declared with both a body and a file path. The two forms are mutually exclusive.

11.6.6 The **crate root module** is the module corresponding to the crate root file (§3.2.5). Its name is the crate name, and its items are the crate's public interface unless the crate is compiled as a binary.

11.6.7 A module **MAY** contain a `mod` declaration for a submodule, a `use` declaration (§5.5), and any item category of §11.1. Modules **MAY** nest to arbitrary depth, subject to implementation limits (§24.6).

11.6.8 An implementation **MUST** reject a module file that is not reachable from the crate root, unless the module is declared with `#[path]` (§23.4) or is part of a module declared via the package manifest.

### 11.7 Trait/Interface Declarations

11.7.1 A **trait** is declared with:

```
trait-item ::= ["pub"] ["unsafe"] "trait" ident [generic-params] [":" supertraits]
               ["where"-clause] "{" trait-items "}"
```

11.7.2 The **supertraits** are a set of trait bounds that any implementing type **MUST** also satisfy (§14.6). The `unsafe` modifier declares an **unsafe trait**, whose implementations carry additional safety invariants (§16.9).

11.7.3 The **items** of a trait are associated types, associated constants, and methods. Each is declared in the trait's body and is implemented by the trait's implementors (§14.2).

11.7.4 A trait **MUST** be uniquely named in the type namespace. A trait and a type of the same name **MUST NOT** coexist in the same module.

11.7.5 A trait's **visibility** controls whether the trait is reachable for implementing and for use as a bound. A private trait may be used as a bound within the module but not as a public interface (§5.4).

11.7.6 A trait **MAY** be sealed by declaring its supertraits in a private module and not re-exporting them. A sealed trait cannot be implemented by external crates, because the external crate cannot name the private supertrait (§14.9.6).

### 11.8 Implementation Blocks

11.8.1 An **implementation block** provides the implementation of a trait for a type, or the inherent methods of a type:

```
impl-item ::= ["unsafe"] "impl" [generic-params] ( trait-path "for" )? type
              ["where"-clause] "{" impl-items "}"
```

11.8.2 An implementation with a trait path is a **trait implementation**. It provides the trait's associated items for the specified type and **MUST** implement every item without a default (§14.4).

11.8.3 An implementation without a trait path is an **inherent implementation**. It provides methods and associated items that are called with the type's name, without trait dispatch (§12.1).

11.8.4 The **self type** of an implementation is the type after `for` (for trait implementations) or the type being implemented (for inherent implementations). The keyword `Self` within the implementation's body denotes this type (§5.7.4).

11.8.5 An implementation **MUST** satisfy the **orphan rules** of §14.9.3: either the trait or the self type **MUST** be defined in the current crate. This prevents external crates from adding implementations that conflict with the crate's own.

11.8.6 A type **MAY** have multiple inherent implementations, but a type **MUST NOT** have two trait implementations for the same trait. A trait implementation **MUST** be **coherent** with all other implementations of the same trait (§14.9).

11.8.7 The `unsafe` modifier on an implementation is required when implementing an `unsafe trait`. The modifier indicates that the implementor has verified the trait's safety invariants.

11.8.8 An implementation's **generic parameters** are in scope in its items. A generic implementation `impl<T> Trait for Wrapper<T>` applies to all instantiations of `Wrapper`; a specific implementation `impl Trait for Wrapper<u8>` applies only to that instantiation. A type **MUST NOT** have both a generic and a specific implementation of the same trait if they would overlap (§14.9.2).

### 11.9 Foreign Declarations

11.9.1 An **extern block** declares items that are defined outside the current crate, typically in a foreign language:

```
extern-block ::= "extern" abi-string? "{" foreign-items "}"
foreign-item ::= ["pub"] "fn" ident "(" params ")" ["->" type] ";"
               | ["pub"] "static" ["mut"] ident ":" type ";"
               | ["pub"] "type" ident ";"
```

11.9.2 The **ABI string** specifies the calling convention of the foreign items (§17.6). The default ABI is `"C"`. An implementation **MUST** reject an unknown ABI string with a diagnostic.

11.9.3 A **foreign function** declaration has no body. Its definition is provided by the linked foreign object. Calling a foreign function is an `unsafe` operation unless the function is marked `safe` by an implementation-specific attribute (§23.3), because the compiler cannot verify the function's safety invariants.

11.9.4 A **foreign static** declaration declares a static that is defined outside the crate. Accessing a foreign static is an `unsafe` operation, because the compiler cannot verify its type or initialization.

11.9.5 A **foreign type** declaration declares an opaque type whose layout is determined by the foreign definition. The type is **unsized** and **MUST** be used behind a pointer. It **MUST NOT** be used in a context that requires its size or layout, unless a `#[repr(...)]` attribute is present (§17.3).

11.9.6 The **linkage** of foreign items is specified by the ABI and by linkage attributes such as `#[link_name]` and `#[link(...)]` (§17.8). An implementation **MUST** produce the symbol references specified by these attributes.

### 11.10 Conditional Compilation

11.10.1 An item, statement, or expression **MAY** be annotated with a `#[cfg(predicate)]` attribute. If the predicate evaluates to `false`, the annotated construct is removed from the program before name resolution and type checking.

11.10.2 The **predicate** grammar is:

```
cfg-predicate ::= cfg-atom { ("all" | "any") "(" cfg-predicate-list ")" }
                | "not" "(" cfg-predicate ")"
cfg-atom      ::= ident [ "=" string-literal ]
cfg-predicate-list ::= cfg-predicate {"," cfg-predicate}
```

11.10.3 The **built-in configuration atoms** include:

| Atom                           | Meaning                                    |
| ------------------------------ | ------------------------------------------ |
| `target_os = "..."`            | the target operating system                |
| `target_arch = "..."`          | the target architecture                    |
| `target_family = "..."`        | the target family (unix, windows, wasm)    |
| `target_env = "..."`           | the target ABI environment                 |
| `target_endian = "..."`        | `"little"` or `"big"`                      |
| `target_pointer_width = "..."` | `32`, `64`, etc.                           |
| `debug_assertions`             | whether debug assertions are enabled       |
| `test`                         | whether the crate is compiled in test mode |
| `feature = "..."`              | a feature enabled in the package manifest  |

11.10.4 A custom configuration atom **MUST** be declared with the `--cfg` command-line flag or the `#[cfg_attr]` mechanism. An undeclared custom atom **MUST** produce an error under the `unexpected_cfgs` lint (§24.3), because it is likely a typo.

11.10.5 A `#[cfg(...)]` attribute **MUST** be evaluated before the annotated construct is parsed for semantic purposes. The construct's tokens **MUST** still be lexically valid, but its names need not resolve. This permits conditional compilation to select between mutually exclusive definitions.

11.10.6 The `cfg_attr(predicate, attr)` form applies `attr` only if `predicate` is true. It is used to conditionally apply representation, linkage, and stability attributes.

11.10.7 An implementation **MUST** produce the same program semantics regardless of whether a `cfg` predicate is evaluated at the item, statement, or expression level; the removal of the annotated construct **MUST** leave a syntactically valid surrounding construct.

---

## 12. Functions

### 12.1 Function Items

12.1.1 A **function item** is the entity introduced by a `fn` declaration (§11.5). A function item is a first-class value in the value namespace that may be called, referenced, or coerced to a function pointer (§8.4.7).

12.1.2 The **name** of a function item is its identifier. When the function is generic, the name denotes the generic function; instantiations are obtained by providing type arguments (§13.3).

12.1.3 A function item **MAY** be **inherent** (declared in an inherent implementation, §11.8.3), **trait-associated** (declared in a trait or a trait implementation, §14.2), or **free** (declared at module scope). All three are called the same way when fully qualified.

12.1.4 The **`self` parameter** of a method (§14.2.2) is the first parameter and is written `self`, `&self`, `&mut self`, or `self: Type`. It denotes the receiver of a method call (§8.11.4).

12.1.5 A function item **MUST NOT** be declared with a name that conflicts with another item of the same name and namespace in the same scope, except for overloading of trait methods (§14.2.6) and inherent methods (§12.1.6).

12.1.6 **Inherent method overloading**: a type **MAY** have multiple inherent methods with the same name if they are distinguished by a trait-bound or where-clause that makes them mutually exclusive, or if they have different receiver types. An implementation **MUST** select the most specific applicable method at the call site (§5.6.4).

### 12.2 Parameters and Return Types

12.2.1 A function's **parameter list** is a sequence of `pattern: type` pairs separated by commas. Each parameter's pattern **MUST** be irrefutable (§10.10.2).

12.2.2 The **type** of a parameter is mandatory. The type is checked against the argument at every call site (§8.11.3).

12.2.3 A parameter **MUST NOT** be declared with the name `self` unless it is a method's receiver (§12.1.4). A parameter named `self` in a free function is an error.

12.2.4 The **return type** follows `->` and is a type expression. The return type **MAY** be omitted, in which case it defaults to `()`.

12.2.5 A function whose return type is `!` never returns normally (§9.11.5). A function whose return type is `()` returns the unit value.

12.2.6 The **last statement** of a function body may be an expression without a semicolon; its value is the function's return value. A function body whose last statement is not an expression (or ends with a semicolon) returns `()`.

12.2.7 A function's **arity** (number of parameters) is fixed. Variadic functions are supported only in `extern` declarations with the `"C"` ABI (§18.5).

12.2.8 The **order of evaluation** of function parameters is determined by the call site, not the declaration: arguments are evaluated left to right before the call (§8.11.8). The function body then receives the evaluated values.

### 12.3 Local Bindings

12.3.1 A function body is a block (§9.1). Local bindings are introduced by `let` statements (§9.3) and by the parameters themselves.

12.3.2 A **parameter** is bound as a local at the start of the function body. The parameter's binding is mutable if the pattern is `mut name`, immutable otherwise. A parameter's pattern **MAY** destructure a struct or tuple, subject to irrefutability.

12.3.3 Local bindings **MUST** be initialized before use (§7.3.1). The compiler performs **definite initialization analysis** to prove that every read of a local follows an assignment on every path.

12.3.4 The **scope** of a local binding is the remainder of the block after its declaration. A `let` binding may shadow an outer binding, including a parameter, for the remainder of the scope (§5.3.2).

12.3.5 Local bindings are **dropped** at the end of their scope in reverse declaration order (§7.3.3). The drop order is observable through the destructor calls and is part of the program's semantics.

### 12.4 Function Bodies

12.4.1 A function body is a **block expression** (§8.14.1) whose type is the function's return type. The block's tail expression, if any, is the function's return value.

12.4.2 The body **MAY** contain `return` expressions (§9.8.3) that exit the function early. A `return` expression's operand **MUST** be assignable to the function's return type.

12.4.3 The body's **local items** (§9.2) are visible throughout the body. They are hoisted and may reference each other in any order, subject to name resolution.

12.4.4 A **`const fn` body** is restricted to the forms that can be evaluated at compile time (§20.2). A `const fn` **MUST** have a body that is valid in both const and runtime contexts; an implementation **MUST** reject a `const fn` that uses runtime-only operations.

12.4.5 An **`async fn` body** is transformed into a state machine that implements the `Future` trait (§22.6). The transformation is semantic-preserving: the body's expressions are evaluated in the same order and with the same side effects as a synchronous body, except that `.await` points yield control.

12.4.6 An **`unsafe fn` body** is not automatically wrapped in `unsafe`; the body's operations retain their individual safety requirements (§16.1). The `unsafe` modifier on the function declaration means that the **caller** must uphold the function's safety invariants, not that the body is unchecked.

12.4.7 A function body **MUST** be **well-formed** under all the analyses of this specification: name resolution, type checking, borrow checking, const evaluation (for `const fn`), and exhaustiveness (for `match` expressions within the body).

### 12.5 Calling Conventions

12.5.1 The **calling convention** of a function specifies how arguments are passed, how the return value is returned, and what registers and stack are used. Forge supports the following calling conventions:

| Convention                                              | Meaning                                                                      |
| ------------------------------------------------------- | ---------------------------------------------------------------------------- |
| `forge`                                                 | the default Forge convention (implementation-defined, stable within a crate) |
| `"C"`                                                   | the platform's C ABI                                                         |
| `"rust"`                                                | (reserved; identical to `forge`)                                             |
| `"system"`                                              | the platform's system ABI (usually `"C"`)                                    |
| `"fastcall"`, `"stdcall"`, `"thiscall"`, `"vectorcall"` | platform-specific conventions                                                |
| `"win64"`, `"sysv64"`                                   | explicit x86-64 ABI selection                                                |

12.5.2 The default calling convention **`forge`** is implementation-defined but **MUST** be consistent within a single compilation and **MUST** be stable across recompilations of the same source. It is **not** the platform's C ABI and **MUST NOT** be assumed to match it.

12.5.3 A function declared with `extern "C"` **MUST** use the platform's C ABI for its parameters and return value. The C ABI's rules for integer promotion, struct passing, and register assignment are specified by the target platform (§17.6, §17.10).

12.5.4 The **`"system"`** convention selects `"C"` on most platforms and the platform-specific system ABI where it differs (for example, `"stdcall"` on 32-bit Windows). Programs that need a specific ABI **SHOULD** use the explicit string rather than `"system"`.

12.5.5 A function pointer's **type** includes its calling convention. A pointer to a `"C"` function **MUST NOT** be coerced to a pointer to a `forge` function or vice versa; the coercion is an error.

12.5.6 The **`"vectorcall"`** and other SIMD-oriented conventions pass arguments in vector registers. Their availability is target-dependent; an implementation **MUST** reject an unsupported convention with a diagnostic.

### 12.6 Function Pointers and Closures

12.6.1 A **function pointer** is a value of type `fn(A1, ..., An) -> R` (§6.10). It may point to any function item or closure that has the same signature and calling convention.

12.6.2 A function item **MAY** be coerced to a function pointer implicitly (§8.4.7). The coercion is a no-op at runtime; the function pointer contains the function's address.

12.6.3 A function pointer **MUST** be non-null. The null pointer is not a valid value of a function pointer type. The type `Option<fn(...)>` uses the null-pointer niche to represent `None` in the same size as `fn(...)` (§17.5.4).

12.6.4 A **closure** (§8.16) implements one or more of the `Fn`, `FnMut`, and `FnOnce` traits. A closure **MAY** be coerced to a function pointer only if it captures no environment (a **non-capturing closure**), in which case the coercion is the same as for a function item.

12.6.5 A closure's **size and layout** are implementation-defined. A closure **MAY** be stored in a local, passed by value, or boxed. A closure's captured environment is stored inline in the closure value, not behind an indirection, unless the closure is boxed.

12.6.6 A **function pointer** to a generic function is obtained by providing the type arguments: `let f: fn(i32) -> i32 = generic::<i32>;`. The instantiation is monomorphized (§13.8) and its address is stable for the program's lifetime.

12.6.7 The **calling convention** of a closure is the `forge` convention. A closure **MUST NOT** be coerced to a function pointer with a different calling convention.

### 12.7 Inlining and Optimization Hints

12.7.1 The `#[inline]` attribute on a function is a **hint** to the optimizer that the function's body should be inlined at call sites. The attribute **MUST NOT** change the function's semantics; an implementation **MAY** ignore it.

12.7.2 The `#[inline(always)]` attribute is a stronger hint. An implementation **SHOULD** honor it when possible, but **MAY** ignore it if inlining would violate a required property (for example, if the function is recursive or its body is too large).

12.7.3 The `#[inline(never)]` attribute requests that the function not be inlined. An implementation **SHOULD** honor it but **MAY** inline if doing so is necessary to satisfy another requirement.

12.7.4 Inlining is **not** observable in safe Forge: a function's observable behavior is the same whether it is inlined or not, except for the addresses of function items (which are not observable in safe code) and the behavior of `unsafe` code that depends on stack frames (which is undefined behavior if it relies on implementation details).

12.7.5 An implementation **MUST NOT** inline a function across an ABI boundary if doing so would change the function's calling convention or its exported symbol. A function marked `#[no_mangle]` is **not** inlined at external call sites.

12.7.6 The `#[cold]` attribute marks a function as unlikely to be called, allowing the optimizer to place it in a cold section. The `#[track_caller]` attribute makes the function's caller information available through the `Location` API (§24.5).

### 12.8 Recursion and Tail Calls

12.8.1 **Recursion** is permitted for all functions, including `const fn` (§20.2.7) and `async fn` (§22.6). The compiler **MUST NOT** reject a recursive function solely because it is recursive; recursion depth is limited by the runtime stack, not by the language.

12.8.2 **Tail-call optimization** is **not** guaranteed by Forge. An implementation **MAY** perform tail-call optimization when it is safe to do so, but a program **MUST NOT** rely on it for correctness. A deeply recursive function **SHOULD** be written iteratively or with an explicit heap-allocated stack to avoid stack overflow.

12.8.3 A function marked `#[tailcall]` requests that the implementation perform tail-call optimization. The request is a hint; an implementation **MAY** ignore it. If the implementation cannot perform the optimization, the function's behavior is unchanged.

12.8.4 **Mutual recursion** between functions is permitted without forward declarations, because items are hoisted (§5.3.4). A function **MAY** call a function declared later in the same module.

12.8.5 **Stack overflow** is not a defined behavior of the language. An implementation **SHOULD** abort the program with a diagnostic on stack overflow (§21.2), but an implementation **MAY** allow the program to continue with corrupted state if it cannot detect the overflow. Programs **MUST NOT** rely on stack overflow behavior.

### 12.9 Extern Functions

12.9.1 An **extern function** is a function declared with an `extern` ABI string and either a body or a foreign declaration. An extern function with a body is an **exported function**; one without a body is an **imported function** (§11.9.3).

12.9.2 The **parameters and return type** of an extern function **MUST** be **FFI-safe** (§18.6). An implementation **MUST** reject an extern function whose signature contains a type that is not FFI-safe, unless the function is annotated with `#[allow(improper_ctypes)]`.

12.9.3 An extern function **MUST** use the specified ABI for its calling convention, parameter passing, and return value. The ABI's rules are specified in §17.6 and §17.10.

12.9.4 An **exported extern function** is visible to the linker under a symbol name determined by §17.7. An implementation **MUST** emit the symbol with the specified visibility and linkage. By default, the symbol name is the function's name with a crate-specific mangling; the `#[no_mangle]` attribute (§17.7.2) exports the function under its unmangled name.

12.9.5 An extern function **MUST NOT** be generic, because a generic function's instantiations are not known at the ABI boundary. An implementation **MUST** reject a generic extern function with a diagnostic.

12.9.6 An extern function's **body** is compiled according to the function's calling convention, not the default Forge convention. The body **MAY** use any Forge construct, but values that cross the ABI boundary are subject to the FFI-safe rules.

12.9.7 The **unwinding behavior** of an extern function is specified by §21.4. By default, an extern function **MUST NOT** unwind across its ABI boundary; a panic that would unwind out of an extern function **MUST** abort the program. The `"C-unwind"` ABI (§21.4.4) permits unwinding across the boundary.

---

## 13. Generics

### 13.1 Type Parameters

13.1.1 A **type parameter** is a placeholder for a type that is supplied at the item's use site. Type parameters are declared in the item's **generic parameter list**:

```
generic-params ::= "<" generic-param {"," generic-param} ">"
generic-param  ::= lifetime-param | type-param | const-param
type-param     ::= ident [":" bound-list]
lifetime-param ::= "'" ident [":" lifetime-bounds]
const-param    ::= "const" ident ":" type
```

13.1.2 A type parameter's **name** is in scope in the item's signature and body. The name **MUST** be unique within the generic parameter list. The name **MUST NOT** shadow an item name that is in scope; an implementation **MUST** reject such a shadowing.

13.1.3 A type parameter **MUST** be used in the item's definition (signature or body), except that it **MAY** be unused if the item is an implementation and the parameter is only used in the trait's associated items (§14.2.4).

13.1.4 The **order** of generic parameters is significant: callers provide arguments in the order of declaration, unless they use named arguments (Forge does not support named generic arguments; the turbofish syntax `::<...>` provides positional arguments, §13.3.3).

13.1.5 A type parameter **MAY** have a **default** written `T = DefaultType`. The default is used when the caller does not provide an argument for that parameter. A parameter with a default **MUST** come after all parameters without defaults in the list.

13.1.6 A type parameter **MUST NOT** be named with a reserved keyword or with the name `Self`. The name `Self` is reserved for the implementing type in implementations (§5.7.4).

### 13.2 Const Parameters

13.2.1 A **const parameter** is a compile-time value parameter. It is declared `const N: Type` and **MUST** have an integer, `bool`, or `char` type (§6.2).

13.2.2 A const parameter is used in the item's signature or body as a constant expression. In an array type `[T; N]`, the length `N` is a const expression that may be a const parameter.

13.2.3 The **value** of a const parameter is supplied by the caller as a constant expression. The value **MUST** be a constant expression of the parameter's type and **MUST** satisfy the parameter's bounds, if any.

13.2.4 A const parameter **MUST** be used in the item's definition, unless it is used only in a where-clause bound. An unused const parameter is a warning under the `unused_parameter` lint.

13.2.5 The **equality** of const parameter values is determined by their type's equality. Two instantiations `Foo<3>` and `Foo<3>` are the same type; `Foo<3>` and `Foo<4>` are different types. A const parameter value **MUST** be a constant expression that the compiler can evaluate at monomorphization time (§13.8).

13.2.6 A const parameter **MUST NOT** depend on a type parameter's value in a way that makes its value unknown at monomorphization time. An implementation **MUST** reject such a dependency with a diagnostic.

### 13.3 Generic Functions

13.3.1 A **generic function** is a function with one or more type, lifetime, or const parameters. Its declaration is the same as a non-generic function with a generic parameter list (§11.5.1).

13.3.2 A generic function's **type arguments** are inferred at each call site from the argument types and the expected return type (§8.4.6). The inference algorithm **MUST** find the most specific solution consistent with all constraints, or report an inference error.

13.3.3 The **turbofish** syntax `function::<TypeArgs>(args)` explicitly provides type arguments. Type arguments provided by turbofish take precedence over inference. An implementation **MUST** reject a turbofish that provides more arguments than the function has parameters, or that provides an argument of the wrong kind (a type where a const is expected, or vice versa).

13.3.4 A generic function **MUST** be **monomorphized** for each distinct set of type arguments (§13.8). The monomorphized instances are compiled as if they were separate non-generic functions.

13.3.5 A generic function **MAY** be recursive. The recursion's termination is not required by the type system; a non-terminating recursive generic function is a runtime divergence, not a type error.

13.3.6 A generic function **MAY** be a method of a trait or an inherent method (§14.2). The method's generic parameters are in addition to the trait's and the type's generic parameters; the order is: type's parameters, trait's parameters, method's parameters.

13.3.7 A generic function **MUST NOT** be exported with a `#[no_mangle]` attribute, because the symbol would clash across instantiations. An implementation **MUST** reject this combination.

### 13.4 Generic Types

13.4.1 A **generic type** is a struct, enum, union, or type alias with generic parameters. The parameters are in scope in the type's definition (§6.12).

13.4.2 A generic type **MUST** be instantiated with type arguments when used in a type position. The arguments **MUST** satisfy the type's bounds (§13.5). An implementation **MUST** reject an instantiation that does not satisfy the bounds.

13.4.3 The **instantiation** `Foo<A1, ..., An>` is a distinct type for each distinct tuple of arguments, subject to the **normalization** rules of §13.4.4. Two instantiations that normalize to the same tuple are the same type.

13.4.4 **Type normalization** reduces a type to a canonical form: type aliases are expanded, associated types are projected (§13.6), and const expressions are evaluated. Two types are **equal** if their normal forms are syntactically identical after this reduction.

13.4.5 A generic type **MAY** be used recursively, as in `struct List<T> { head: T, tail: Option<Box<List<T>>> }`. The recursion **MUST** be through indirection (§11.4.4), or the type would have infinite size.

13.4.6 The **layout** of a generic type depends on its arguments. Two instantiations with different arguments **MAY** have different sizes, alignments, and field offsets. An implementation **MUST NOT** assume a uniform layout across instantiations unless the type is annotated with `#[repr(...)]` (§17.3).

13.4.7 A generic type's **drop glue** is monomorphized for each instantiation. The order of field drops is the declaration order, reversed (§7.3.3).

### 13.5 Bounds and Constraints

13.5.1 A **bound** constrains the types that may be used as a generic argument. Bounds appear after a colon in the generic parameter list, or in a `where` clause:

```
type-param ::= ident [":" bound-list]
where-clause ::= "where" where-predicate {"," where-predicate}
where-predicate ::= ( type ":" bound-list ) | ( lifetime ":" lifetime-list )
```

13.5.2 A **trait bound** `T: Trait` requires that the type argument implement `Trait`. A **lifetime bound** `'a: 'b` requires that `'a` outlive `'b` (§7.6.3). A **const bound** constrains the value of a const parameter.

13.5.3 The **`?Sized`** bound relaxes the default `Sized` bound on a type parameter. By default, every type parameter is implicitly `Sized` (§6.17.2); `?Sized` permits unsized types as arguments. A type parameter with `?Sized` may only be used behind a pointer, because unsized values cannot be held directly.

13.5.4 Multiple bounds are written `T: A + B + C` and require the type to implement all of them. Lifetime bounds and trait bounds may be mixed: `T: 'a + Trait`.

13.5.5 A **where clause** is equivalent to bounds in the parameter list, but may be used to express bounds that involve associated types, generic types, or complex relationships. The where clause **MUST** be consistent with the parameter list's bounds: a type that satisfies the parameter list bounds **MUST** also satisfy the where clause, or the item is ill-formed.

13.5.6 **Implied bounds**: a type parameter's bounds are in scope in the item's body. A function with `T: Trait` may call `Trait`'s methods on a value of type `T` without additional bounds.

13.5.7 The **`Sized`** bound is implicit on all type parameters and on all associated types, unless the associated type is declared with `?Sized`. An implementation **MUST** enforce this default.

13.5.8 A bound that cannot be satisfied by any type is a **vacuous** bound. An implementation **MAY** accept a vacuous bound, but the item that declares it is **uninstantiable**. An implementation **SHOULD** warn about a vacuous bound under the `unused_bounds` lint.

### 13.6 Associated Types and Constants

13.6.1 An **associated type** is a type declared inside a trait that each implementor provides:

```
assoc-type ::= "type" ident [":" bound-list] [where-clause] ";"
```

13.6.2 An associated type **MUST** be implemented for every trait implementation, unless the trait provides a default (§14.4). The implementor specifies the type with `type Name = ConcreteType;` in the implementation block.

13.6.3 An associated type is **projected** with the syntax `<T as Trait>::Assoc` (§5.7.5) or, in a context where `T` is a type parameter bounded by `Trait`, as `T::Assoc`. The projection is normalized during type checking (§13.4.4).

13.6.4 An associated type **MAY** have bounds. The bounds constrain the associated type in every implementation and in every use of the projection.

13.6.5 An **associated constant** is a constant declared in a trait, provided by each implementor:

```
assoc-const ::= "const" ident ":" type ["=" const-expr] ";"
```

13.6.6 An associated constant is referenced as `<T as Trait>::CONST` or `T::CONST`. Its value is determined by the implementor's definition and is a constant expression.

13.6.7 An associated type or constant **MUST NOT** be declared in an inherent implementation. Only traits may have associated items (beyond the methods of an inherent implementation).

### 13.7 Coherence and Overlap

13.7.1 **Coherence** is the property that a trait has at most one implementation for any given type. Coherence is checked at the trait implementation level and is part of the orphan rules (§14.9).

13.7.2 **Overlap** occurs when two implementations of the same trait could apply to the same type. Overlap is an error unless the implementations are provably disjoint by the compiler's coherence analysis.

13.7.3 The **coherence rules** (§14.9) constrain which implementations are permitted in a crate. The principal rule is that an implementation **MUST** satisfy the orphan rules: at least one of the trait or the self type **MUST** be defined in the current crate.

13.7.4 A generic implementation `impl<T> Trait for Wrapper<T>` overlaps with a specific implementation `impl Trait for Wrapper<u8>` if the specific type is an instantiation of the generic one. An implementation **MUST** reject this overlap with a diagnostic.

13.7.5 **Negative reasoning** is not permitted: an implementation **MUST NOT** assume that a type does not implement a trait, because another crate might add the implementation later. The only exception is the `Sized` bound, which is part of the type system rather than a trait.

13.7.6 The coherence check **MUST** be deterministic and **MUST** be consistent across recompilations of the same source. An implementation **MUST NOT** accept a program on one compilation and reject it on another due to coherence analysis.

### 13.8 Monomorphization

13.8.1 **Monomorphization** is the process of instantiating a generic item for each distinct set of type arguments used in the program. Each instantiation is compiled as a separate non-generic item with the type arguments substituted.

13.8.2 The **set of instantiations** of a generic item is determined by the program's uses: every call to a generic function, every use of a generic type, and every implementation of a generic trait contributes instantiations.

13.8.3 Monomorphization **MUST** be semantically transparent: the monomorphized item **MUST** have the same observable behavior as the generic item would have under the type substitution. An implementation **MUST NOT** introduce behavior that depends on the instantiation's type in a way not specified by the generic definition, except through trait dispatch.

13.8.4 Monomorphization **MAY** produce duplicate instances if the same type arguments are used in multiple crates. The linker merges duplicate instances of the same function if they are marked as **linkonce** (§17.8.4).

13.8.5 The **code size** of a program grows with the number of instantiations. An implementation **SHOULD** provide a mechanism to avoid unbounded monomorphization, such as **dynamic dispatch** through trait objects (§14.5) or **boxed** generic values (§22.4).

13.8.6 Monomorphization **MUST** terminate. An implementation **MUST** reject a program whose monomorphization would not terminate, such as a generic function that instantiates itself with a growing type. The detection of non-terminating monomorphization is required for correctness; an implementation **MAY** limit the depth of instantiation and report a diagnostic when the limit is exceeded.

13.8.7 The **const evaluation** of a generic item's const parameters occurs at monomorphization time. A const parameter whose value depends on a type parameter's associated constant is evaluated once the associated constant is known.

### 13.9 Generic ABI Considerations

13.9.1 A generic function or type **MUST NOT** be directly exported across an ABI boundary, because its instantiations are not known to the foreign code. An implementation **MUST** reject an `extern` generic function or a `#[no_mangle]` generic function.

13.9.2 An instantiated generic function **MAY** be coerced to a function pointer of the appropriate signature (§12.6.6). The function pointer's ABI is the function's declared ABI (defaulting to `forge`), not a generic ABI.

13.9.3 The **name mangling** of a monomorphized generic function includes its type arguments (§17.7.3), so that different instantiations produce different symbols. An implementation **MUST** use a mangling scheme that distinguishes all distinct instantiations.

13.9.4 A generic type's **layout** is determined by its instantiation. Passing a generic type across an ABI boundary is permitted only if the type has a `#[repr(...)]` attribute that specifies the layout independently of the instantiation's type arguments, or if the type is a newtype over an FFI-safe type.

13.9.5 The **vtable** of a trait object `dyn Trait` is monomorphized for each concrete type that is coerced to `dyn Trait`. The vtable's layout is specified in §14.5.4.

13.9.6 An implementation **MUST NOT** rely on the layout of a generic type across crate boundaries unless the type has a `#[repr(...)]` attribute. The layout of a generic type is implementation-defined (§6.14.2).

---

## 14. Traits / Interfaces

### 14.1 Trait Declarations

14.1.1 A **trait** declares a set of associated types, associated constants, and methods that a type may implement. Its grammar is specified in §11.7.1.

14.1.2 A trait's **name** is bound in the type namespace. A trait **MUST NOT** be a value; it cannot be instantiated or used where a value is expected.

14.1.3 A trait's **body** contains the declarations of its associated items. An associated item **MAY** have a default implementation (§14.4) or be a required declaration without a body.

14.1.4 A trait **MUST** be object-safe (§14.8) to be used as `dyn Trait`. A non-object-safe trait **MAY** still be used as a bound on generic parameters, but it cannot be used as a trait object.

14.1.5 A trait's **supertraits** are a set of trait bounds that every implementor must satisfy. A supertrait is declared after a colon in the trait's declaration: `trait Sub: Super { ... }`. An implementor of `Sub` **MUST** also implement `Super`.

14.1.6 The **`unsafe`** modifier declares an unsafe trait (§16.9). Implementing an unsafe trait requires an `unsafe impl` block and an assertion by the implementor that the trait's safety invariants are satisfied.

14.1.7 A trait **MAY** be **generic** (§13). A generic trait's parameters are in scope in its associated items and in its implementors.

14.1.8 A trait **MAY** have a **default implementation** of a method by providing a body in the trait declaration. An implementor that does not provide a body uses the default (§14.4).

### 14.2 Methods and Associated Items

14.2.1 The **associated items** of a trait are:

- **methods**, declared with `fn`, that may take a `self` receiver;
- **associated types**, declared with `type` (§13.6.1);
- **associated constants**, declared with `const` (§13.6.5).

  14.2.2 A method's **receiver** is one of:

- `self` — the method takes ownership of the receiver;
- `&self` — the method borrows the receiver immutably;
- `&mut self` — the method borrows the receiver mutably;
- `self: Box<Self>` — the method takes a boxed receiver;
- `self: Rc<Self>`, `self: Arc<Self>`, etc. — an arbitrary pointer type.

  14.2.3 A method **MAY** omit the receiver, in which case it is an **associated function** (also called a **static method**). An associated function is called with the type's name, not with an instance.

  14.2.4 A method's **generics** are declared after its name and before its parameters. The method's generic parameters are in addition to the trait's and the self type's parameters (§13.3.6).

  14.2.5 The **visibility** of a trait's methods is always public within the trait's visibility. An implementation **MUST NOT** change the visibility of a trait method; it inherits the trait's visibility (§5.4.4).

  14.2.6 A trait **MUST NOT** declare two methods with the same name and the same receiver signature. A trait **MAY** declare methods that differ only in their generic parameters, but the call site **MUST** disambiguate with turbofish (§13.3.3).

  14.2.7 The **default body** of a method is evaluated in the context of the implementing type. The body may use `Self` to refer to the implementing type and may call other methods of the trait, which are resolved to the implementor's versions.

### 14.3 Implementations

14.3.1 A **trait implementation** provides the associated items of a trait for a specific type. Its grammar is specified in §11.8.1.

14.3.2 An implementation **MUST** provide a definition for every associated item of the trait that does not have a default. An implementation **MAY** override a default item by providing its own definition.

14.3.3 An implementation's **self type** is the type after `for`. The self type **MUST** be a local type or a type parameter of the current crate (§14.9.3). A trait implementation for a foreign type is permitted only if the trait is local.

14.3.4 An implementation's **generic parameters** are in scope in its associated items. The implementation **MUST** satisfy the trait's bounds for its self type and for its generic parameters.

14.3.5 An implementation **MUST** be coherent with all other implementations of the same trait (§14.9). An implementation that overlaps another is an error.

14.3.6 An implementation **MAY** be **specialized** for a more specific type, provided the specialization is permitted by the coherence rules and the trait is marked with `#[specialization]`. Specialization is an unstable feature in Forge 0.1; an implementation **MAY** reject it with a diagnostic indicating it is not yet stable.

14.3.7 The **`unsafe`** modifier on an implementation is required for implementing an unsafe trait (§14.1.6). The modifier **MUST** be present even if the implementation's body is empty.

14.3.8 An implementation **MUST NOT** be declared for a type that is not visible at the implementation's location. An implementation **MUST NOT** be declared for a type parameter's instantiation in a way that conflicts with the parameter's bounds.

### 14.4 Default Methods

14.4.1 A trait **MAY** provide a **default method** by including a body in the trait declaration. An implementor that does not override the method inherits the default.

14.4.2 A default method's body is compiled once per implementing type, because it may refer to `Self` and to other associated items that differ across implementations. An implementation **MUST** monomorphize the default method for each implementor that uses it.

14.4.3 A default method **MAY** call other methods of the same trait. The calls are resolved to the implementor's overrides, if any, not to the default. This is **dynamic dispatch within the default**, and it is resolved at monomorphization time for generic code and at runtime for trait objects.

14.4.4 A default method **MUST** be object-safe if the trait is used as `dyn Trait` (§14.8). A default method that is not object-safe makes the whole trait non-object-safe, unless the method is marked `where Self: Sized`, which excludes it from the trait object's vtable.

14.4.5 An implementation **MAY** override a default method by providing its own body. The override **MUST** have the same signature as the default, including receiver type, generics, and where-clause.

### 14.5 Trait Objects and Dynamic Dispatch

14.5.1 A **trait object** is a value of type `dyn Trait` (§6.13.2). A trait object is a fat pointer consisting of a data pointer and a vtable pointer (§6.17.4).

14.5.2 The type `&dyn Trait` is a reference to a trait object. The `&` borrows the concrete value, and the vtable pointer is stored alongside the data pointer in the reference itself.

14.5.3 A value of a concrete type `T` that implements `Trait` **MAY** be coerced to `&dyn Trait` or `Box<dyn Trait>` (§8.13.6). The coercion selects the vtable for `T` and stores it with the data pointer.

14.5.4 The **vtable** of `dyn Trait` is an implementation-defined data structure containing pointers to the trait's methods for the concrete type. The vtable's layout is not specified by the language; programs **MUST NOT** rely on it. The vtable **MUST** be stable for a given concrete type within a program's execution.

14.5.5 A call through a trait object is **dynamically dispatched**: the method is selected at runtime based on the vtable. The selection is determined by the vtable's entries, which are fixed at the coercion site.

14.5.6 The **`dyn Trait` type** is unsized and **MUST** be used behind a pointer (`&dyn Trait`, `Box<dyn Trait>`, `*mut dyn Trait`). It **MUST NOT** be used as a local binding type, a function parameter passed by value, or a struct field without indirection.

14.5.7 A trait object **MUST** have an explicit lifetime bound unless it is inside a reference. `Box<dyn Trait>` defaults to `Box<dyn Trait + 'static>`; `&'a dyn Trait` has lifetime `'a`. An implementation **MUST** apply the default lifetime bound according to §7.6.5's elision rules.

14.5.8 The **size** of `&dyn Trait` and `Box<dyn Trait>` is two pointers (§6.17.4). The size of a trait object's metadata is one pointer; an implementation **MUST NOT** use a larger metadata representation unless it preserves the two-pointer size.

### 14.6 Supertraits and Trait Bounds

14.6.1 A **supertrait** is a trait bound on a trait's declaration that requires every implementor to also implement the supertrait. The syntax is `trait Sub: Super + OtherSuper { ... }`.

14.6.2 An implementation of `Sub` **MUST** also implement every supertrait of `Sub`. An implementation **MUST** reject a `Sub` implementation whose self type does not implement `Super`.

14.6.3 The **methods of a supertrait** are in scope in a generic function bounded by the subtrait. A function `fn f<T: Sub>(x: T)` may call `Super`'s methods on `x`, because `T: Sub` implies `T: Super`.

14.6.4 A **trait bound** may be a conjunction of traits `T: A + B`, a disjunction (not supported), or a lifetime bound `T: 'a`. The bound's **kind** determines how it is satisfied: trait bounds require implementations, lifetime bounds require subtyping relationships.

14.6.5 A **higher-ranked trait bound** (HRTB) is written `for<'a> Trait<'a>` and requires the trait to hold for every lifetime `'a`. An HRTB is satisfied if the implementor implements the trait for all lifetimes, not just a specific one.

14.6.6 The **`?Sized`** bound is a special bound that relaxes the implicit `Sized` bound (§13.5.3). It **MUST** appear alone; it **MUST NOT** be combined with other bounds in the same `+` list, except with a lifetime bound.

14.6.7 A trait's **supertrait bounds** are transitive: if `Sub: Super` and `Super: Base`, then `Sub`'s implementors must implement `Base`. An implementation **MUST** enforce transitivity.

### 14.7 Blanket Implementations

14.7.1 A **blanket implementation** is an implementation of a trait for all types that satisfy a bound: `impl<T: Bound> Trait for T { ... }`. The implementation applies to every type that implements `Bound`.

14.7.2 A blanket implementation **MUST** satisfy the orphan rules (§14.9.3). A blanket implementation in a crate is permitted only if the trait is local to the crate or the bound constrains the self type to be local.

14.7.3 A blanket implementation **MUST NOT** overlap with a specific implementation for a type that also satisfies the blanket's bound. An implementation **MUST** reject overlapping blanket implementations.

14.7.4 Blanket implementations are commonly used to provide **derived** trait implementations, such as `impl<T: Display> ToString for T`. The blanket's body **MUST** be generic over the bound and **MUST NOT** rely on the concrete type.

14.7.5 The `#[derive(...)]` attribute (§23.1) generates a **derived implementation** for a specific type. A derived implementation is a specific implementation, not a blanket one; it **MUST NOT** overlap with a blanket implementation in the same crate.

14.7.6 A blanket implementation's **method resolution** is subject to the coherence rules: a call to a blanket method on a concrete type resolves to the blanket implementation if no more specific implementation exists. An implementation **MUST** select the most specific method (§5.6.4).

14.7.7 A blanket implementation **MUST NOT** be used to implement a trait for a type that is not local to the crate, unless the trait is local. This restriction follows from the orphan rules and **MUST** be enforced by the implementation.

### 14.8 Object Safety / Dyn Compatibility

14.8.1 A trait is **object-safe** (also called **dyn-compatible**) if it can be used as `dyn Trait`. A trait **MUST** be object-safe to be used as a trait object type.

14.8.2 A trait is object-safe if and only if:

- every method has a receiver of a supported form (`self`, `&self`, `&mut self`, or a supported pointer type);
- no method has generic parameters other than lifetimes;
- no method has a `where Self: Sized` bound **and** is required (methods with this bound are excluded from the trait object and may be non-object-safe);
- no associated type is used in a way that requires its concrete value at the trait object's type (associated types are permitted if they have bounds and are not used in method signatures that would require them to be known);
- the trait's supertraits are all object-safe.

  14.8.3 An **associated type** in an object-safe trait **MUST NOT** be used in a method signature in a position that requires its concrete value, unless the method has a `where Self: Sized` bound. The trait object `dyn Trait<Assoc = T>` may specify the associated type for a particular instantiation.

  14.8.4 The **`Self: Sized`** bound excludes a method from the trait object's vtable. A method with this bound **MAY** be non-object-safe, because it is not called through a trait object.

  14.8.5 An implementation **MUST** reject a coercion to `dyn Trait` if `Trait` is not object-safe. The diagnostic **MUST** identify the method or associated item that makes the trait non-object-safe.

  14.8.6 A **trait alias** (unstable) may combine multiple traits into a single object-safe trait. Trait aliases are not part of Forge 0.1; an implementation **MAY** reject them with a diagnostic.

### 14.9 Coherence Rules

14.9.1 **Coherence** is the property that a trait's implementations are consistent: no two implementations apply to the same type, and no implementation can be added in a way that conflicts with an existing one.

14.9.2 **No overlap**: a trait **MUST NOT** have two implementations that could apply to the same type. The compiler **MUST** check for overlap at every implementation and **MUST** report overlapping implementations as an error.

14.9.3 **Orphan rules**: an implementation is permitted only if at least one of the following holds:

- the trait is defined in the current crate;
- the self type is a local type (a type defined in the current crate) or a type parameter of the current crate;
- the self type is a fundamental type (`&T`, `&mut T`, `Box<T>`) whose parameter is a local type or a local type parameter.

This ensures that a crate cannot add implementations that would conflict with implementations in another crate, because either the trait or the type is local to the crate that owns it.

14.9.4 **Transitive coherence**: an implementation for a type parameter `T` is permitted only if the bounds on `T` are sufficient to guarantee that the implementation does not conflict with future implementations. The compiler **MUST** check that every implementation for a generic type is coherent with all possible instantiations of the type parameter.

14.9.5 **Future-proofing**: the coherence rules are designed to allow future additions of implementations without breaking existing code. A program that is accepted under the coherence rules **MUST** remain accepted if a new crate adds an implementation that is coherent with all existing ones.

14.9.6 **Sealed traits**: a trait **MAY** be sealed by requiring a private supertrait. A sealed trait can be implemented only within the crate that defines it, because external crates cannot name the private supertrait. Sealed traits are permitted and are not an error.

14.9.7 The coherence analysis **MUST** be deterministic and **MUST** be performed before monomorphization, so that the set of accepted programs does not depend on the set of instantiations used by a particular program.

14.9.8 An implementation **MUST** reject an implementation that violates the orphan rules, even if the implementation would be coherent in the current program. The rejection is required to preserve the coherence guarantee for future crates.

---

## 15. Modules and Packages

### 15.1 Module System

15.1.1 The **module system** organizes items into a tree of modules rooted at the crate root (§3.2.5). Each module has a name and a parent module, except the crate root, which has no parent.

15.1.2 A **module path** is the sequence of names from the crate root to a module, separated by `::` (§5.7). The path of the crate root is `crate`; the path of a submodule `a` of the crate root is `crate::a`; the path of `b` inside `a` is `crate::a::b`.

15.1.3 A module **MAY** be declared inline (§11.6.2) or in a separate file (§11.6.3). The choice is semantically transparent: an inline module and a file module with the same items have the same visibility and resolution behavior.

15.1.4 **Module file resolution**: a file module `mod name;` declared in the module at path `p` is resolved to a file named `name.forge` in the directory corresponding to `p`, or to `p/name/mod.forge` if `name` is a directory. The exact file naming and directory layout is specified by the package manifest (§15.2). An implementation **MUST** use a consistent resolution rule and **MUST** report an error if the file cannot be found.

15.1.5 A module **MUST NOT** be declared twice under the same name in the same parent. A module **MUST NOT** create a cycle of `mod` declarations (a module cannot be its own ancestor).

15.1.6 The **visibility** of a module controls the reachability of its items from outside (§11.6.4). An item in a `pub` module is reachable if the item itself is `pub`; an item in a private module is not reachable from outside the crate regardless of its own visibility.

15.1.7 A module **MAY** contain `use` declarations (§5.5) that re-export items from other modules. A re-exported item appears in the module's namespace under the imported name, and its visibility is the visibility of the `use` declaration.

15.1.8 The **crate root** is the root module of the crate. Its items are the crate's public interface if the crate is compiled as a library (§3.3.2). The crate root **MUST** contain a `fn main` for a binary crate (§3.4.1).

### 15.2 Package Manifest Model

15.2.1 A **package** is a collection of crates described by a **manifest file**. The manifest is a declarative description of the package's name, version, dependencies, targets, and features.

15.2.2 The manifest **MUST** contain at least:

- the package name, a valid identifier (§4.5);
- the package version, a semantic version (§15.3.2);
- at least one target (library or binary);
- the source directory layout, if it differs from the default.

  15.2.3 A **target** is a crate produced by the package. A package **MAY** have one library target and multiple binary targets. Each binary target has a name and a source file; the library target has the package's name.

  15.2.4 The manifest's **dependencies** are the other packages that the package uses (§15.3). Each dependency is specified by a name, a version requirement, and optionally a source (registry, git, or path).

  15.2.5 The manifest's **features** are named sets of optional dependencies and configuration flags. A feature is enabled by the consumer of the package, and enabling a feature enables its dependencies and `cfg` atoms.

  15.2.6 The manifest **MUST** be validated by the implementation before compilation begins. An implementation **MUST** reject a manifest that is malformed, has a missing target, or has an unresolvable dependency.

  15.2.7 The manifest **MAY** specify **profile** settings such as optimization level, debug assertions, and panic strategy. These settings are implementation-defined but **MUST** be applied consistently to the crate's compilation.

### 15.3 Dependencies and Versioning

15.3.1 A **dependency** is another package that the current package uses. A dependency is identified by its name and version requirement, and resolved to a specific version by the package manager.

15.3.2 **Semantic versioning**: package versions are of the form `MAJOR.MINOR.PATCH`. A change in `MAJOR` indicates a breaking change; `MINOR` adds backward-compatible features; `PATCH` fixes bugs without changing the API. A version requirement such as `^1.2.3` accepts `1.x.y` with `x >= 2` and `y` any.

15.3.3 **Version resolution**: the package manager **MUST** select a single version of each package that satisfies all version requirements from the dependency graph. If no such version exists, the resolution fails and the package cannot be built.

15.3.4 **Lockfile**: the resolved versions of all dependencies are recorded in a lockfile, which **MUST** be used for reproducible builds. An implementation **MUST** produce a lockfile after a successful resolution and **MUST** use it for subsequent builds unless the manifest changes.

15.3.5 **Source kinds**: a dependency may be resolved from a **registry** (a versioned package repository), a **git** repository at a specific revision, or a **path** (a local directory). The source kind determines how the package manager fetches and verifies the dependency.

15.3.6 **Checksums**: a registry dependency **MUST** have a checksum, and the package manager **MUST** verify the checksum of the downloaded package. A mismatch **MUST** abort the build with a diagnostic.

15.3.7 A dependency **MAY** be declared **optional**, in which case it is enabled only when the corresponding feature is enabled. An optional dependency **MUST NOT** be used unless its feature is enabled; an implementation **MUST** reject a use of a disabled optional dependency.

### 15.4 Visibility Across Modules

15.4.1 An item's visibility (§5.4) is checked with respect to the module in which the item is used. An item is **reachable** from a module `m` if there is a path from `m` to the item's module such that every module on the path is visible from `m`.

15.4.2 The **crate root** is visible to all modules in the crate. An item in the crate root with `pub` visibility is reachable from every module in the crate, subject to the visibility of intermediate modules (there are none for the crate root).

15.4.3 A **child module** **MUST** be able to access its parent module's items only if the items are visible to the child. By default, a private item in the parent is **not** visible to the child; the child must use `super::item` and the item must be `pub(super)` or more visible.

15.4.4 The **`pub(in path)`** visibility restricts an item to a specific ancestor module. The path **MUST** denote an ancestor of the item's module. An implementation **MUST** reject a `pub(in path)` whose path is not an ancestor.

15.4.5 A **use declaration** (§5.5) imports an item into a module. The imported name is subject to the same visibility rules as the original item: an imported item **MUST** be visible at the use site, and a re-export (`pub use`) makes the item visible under the re-export's visibility.

15.4.6 The **prelude** (§22.2) is visible in every module without an explicit `use`. A prelude item **MUST NOT** be shadowed by a local item of the same name without an explicit import; the shadowing is a **deprecation warning**, not an error, unless the shadowing is ambiguous.

15.4.7 A module **MUST NOT** re-export an item that it cannot name. A `pub use` of a private item is an error, because the item is not visible at the use site.

### 15.5 Preludes and Standard Library Root

15.5.1 A **prelude** is a set of names that are implicitly in scope in every module. The names in the prelude **MUST NOT** require an explicit `use` to be used.

15.5.2 The **core prelude** contains the types and traits that are used by the language itself: `bool`, the integer and float types, `char`, `str`, `Option`, `Result`, `Clone`, `Copy`, `Drop`, `Sized`, `Fn`, `FnMut`, `FnOnce`, and the operator traits of §8.5.5.

15.5.3 The **standard prelude** extends the core prelude with the types and traits of the standard library: `String`, `Vec`, `Box`, `Rc`, `Arc`, `Iterator`, `IntoIterator`, `Extend`, `ToString`, and the other traits that are commonly used in ordinary programs.

15.5.4 A **`#![no_std]`** crate disables the standard prelude and provides only the core prelude (§3.7.3). A `#![no_std]` crate **MUST** provide its own platform abstractions and entry point.

15.5.5 The **standard library root** is the `std` crate, which re-exports the core library and the allocator library. A crate **MUST NOT** depend on `std` explicitly; the standard library is provided by the toolchain and its version is the toolchain's version.

15.5.6 An implementation **MUST** reject a program that relies on a name that is only in the prelude when the program is compiled with `#![no_std]` and has not explicitly imported the name. The diagnostic **SHOULD** suggest the explicit import.

15.5.7 The prelude **MUST NOT** be modified by a program without an explicit `#![feature(prelude_import)]` attribute. An implementation **MAY** provide a mechanism to customize the prelude, but the customization **MUST** be explicit and **MUST NOT** change the behavior of a program that does not opt in.

### 15.6 Namespace Organization

15.6.1 The **namespaces** of Forge are the type namespace, the value namespace, and the macro namespace (§5.1.1). A module's contents are organized by namespace: each name in each namespace is unique within the module.

15.6.2 An item **MAY** introduce names in multiple namespaces. A struct `S` introduces `S` in the type namespace and a constructor `S` in the value namespace. An enum `E` introduces `E` in the type namespace and its variants `E::V` in the value namespace.

15.6.3 The **import resolution** (§5.5) imports names from all namespaces that the imported item occupies. A `use` of a struct imports both the type and the constructor, unless the import is restricted with `use path::Type` and the item is used only in a type position.

15.6.4 The **method resolution** (§5.6.4) uses the type namespace to find the type and the value namespace to find the method. A method call `x.method()` resolves `method` in the type's inherent and trait method sets, not in the value namespace directly.

15.6.5 A **macro** is resolved in the macro namespace. A macro and a type or value of the same name **MAY** coexist in a module, because they are in different namespaces. An implementation **MUST NOT** report a conflict between names in different namespaces.

15.6.6 The **glob import** `use path::*` imports names from all namespaces into the current module. A glob import **MUST NOT** shadow an explicit import; an explicit import takes precedence over a glob import of the same name (§5.5.3).

15.6.7 The **namespace of a module** is the type namespace: a module is a type-like entity in the module tree. A module **MUST NOT** be imported as a value; a module **MUST** be imported in the type namespace and used in paths.

15.6.8 An implementation **MUST** reject a program that uses a name in a namespace in which it is not defined, with a diagnostic identifying the available namespaces. The diagnostic **SHOULD** suggest the correct namespace when the name exists in a different namespace.

## Part IV — Systems Programming Facilities

## 16. Unsafe Forge and Low-Level Operations

### 16.1 Unsafe Blocks and Functions

16.1.1 The `unsafe` keyword has two roles in Forge: it marks operations whose correctness the compiler cannot verify (**unsafe operations**, §16.1.4), and it marks items that carry safety invariants beyond the type system (**unsafe items**, §16.1.6). Both roles are specified in this chapter.

16.1.2 An **unsafe block** is written:

```
unsafe-block ::= "unsafe" block
```

Within an unsafe block, the operations listed in §16.1.4 are permitted. Outside an unsafe block, they are rejected at compile time.

16.1.3 An **unsafe block** does not disable any rule of the language. It is an assertion by the programmer that the operations inside satisfy the safety invariants of §7.11. A program that executes undefined behavior inside an unsafe block is non-conforming, even if it compiles.

16.1.4 The following operations are **unsafe operations** and **MUST** occur inside an unsafe block or an unsafe function body:

- dereferencing a raw pointer (§16.2.4);
- reading or writing a `static mut` item (§11.3.7);
- calling an `unsafe fn` (§16.1.7);
- accessing a field of a `union` (§6.9.7);
- casting a raw pointer to a reference or a reference to a raw pointer for the purpose of dereference (§16.3.2);
- casting an integer to an enum with a discriminant that is not checked (§8.13.5);
- calling an intrinsic marked `unsafe` (§16.8.3);
- accessing the value of a `static` declared `unsafe` (§16.9.3);
- calling an extern function declared without a `safe` guarantee (§18.2.4);
- executing inline assembly (§16.6);
- accessing a `#[no_mangle]` item whose ABI does not match (§17.7.2).

  16.1.5 An **unsafe block** **MAY** appear in any expression position. Its value is the value of its block expression, and its type is the type of its block expression.

  16.1.6 The following items are **unsafe items** and carry additional invariants that the programmer **MUST** uphold:

- `unsafe fn` declarations (§16.1.7);
- `unsafe trait` declarations (§14.1.6);
- `unsafe impl` blocks (§14.3.7);
- `unsafe static` items (§16.9.3).

  16.1.7 An **`unsafe fn`** is a function whose call is an unsafe operation. The function's **caller** **MUST** uphold the safety invariants that the function's implementation assumes. The function's **body** is not implicitly unsafe; the body's unsafe operations must still be inside an `unsafe` block or another `unsafe fn`.

  16.1.8 An implementation **MUST** report an unsafe operation outside an unsafe block as a required error (§24.1). The diagnostic **SHOULD** identify the operation and the enclosing safe context.

  16.1.9 An implementation **MUST NOT** report an unsafe operation inside an unsafe block as an error, regardless of whether the operation is provably safe. The unsafe block is the programmer's assertion; checking its correctness is outside the compiler's responsibility.

### 16.2 Raw Pointers

16.2.1 A **raw pointer** is a value of type `*const T` or `*mut T` (§6.7.3). A raw pointer may be null, may dangle, and may be misaligned. Its only guarantee is that its value is a scalar of the target's pointer width.

16.2.2 A raw pointer is created by:

- casting a reference with `&x as *const T` or `&mut x as *mut T` (§8.13.3);
- casting an integer with `n as *const T` or `n as *mut T` (§16.3.2);
- the `addr_of!` and `addr_of_mut!` macros (§16.9.4), which avoid intermediate references;
- arithmetic on an existing raw pointer (§16.3.1).

  16.2.3 A raw pointer is **valid** for a type `T` if it is non-null, aligned to `align_of::<T>()`, and points to a live value of type `T` for the duration of its use. Validity is not checked at creation; it is checked (by the programmer's reasoning) at dereference.

  16.2.4 **Dereferencing a raw pointer** `*p` is an unsafe operation. The programmer **MUST** ensure that `p` is valid for `T` and that the resulting reference would satisfy §7.11.1. A dereference of a null, dangling, or misaligned pointer is undefined behavior.

  16.2.5 A raw pointer **MAY** be compared for equality with another raw pointer. The comparison is by address, and two pointers of different types may be compared after casting to `*const ()`.

  16.2.6 A raw pointer **MUST NOT** be sent across threads unless it is `Send` (§19.6). By default, `*const T` and `*mut T` are **not** `Send` or `Sync`, because the compiler cannot verify the safety invariants of the pointee.

  16.2.7 A raw pointer is **`Copy`**. Copying a raw pointer duplicates the address; both pointers remain valid (or invalid) independently.

### 16.3 Pointer Arithmetic

16.3.1 **Pointer arithmetic** on a raw pointer `p: *const T` or `*mut T` is performed with the `offset`, `add`, `sub`, and `wrapping_offset` methods (§22.4). The result is a new raw pointer; no dereference occurs.

16.3.2 The `add(n)` and `sub(n)` methods require that `p` be valid for `n` elements of type `T` in the direction of the arithmetic, or that `n` be zero. A pointer arithmetic that would produce a pointer outside its allocation (or one-past-the-end) is undefined behavior.

16.3.3 The **one-past-the-end** pointer of an allocation is a valid pointer for arithmetic but is not valid for dereference. It is the result of `p.add(len)` where `p` points to the start of an allocation of `len` elements.

16.3.4 The `wrapping_offset` method performs arithmetic modulo `2^N` and never invokes undefined behavior. The result may be a pointer to any address; dereferencing it is safe only if it is valid.

16.3.5 Casting a raw pointer to an integer with `p as usize` produces the address. Casting an integer back to a raw pointer with `n as *const T` produces a raw pointer at that address. Both casts are safe; the dereference of the result is unsafe.

16.3.6 The **alignment** of a raw pointer `p` is `p as usize % align_of::<T>()`. A raw pointer that is not a multiple of its type's alignment is misaligned, and dereferencing it is undefined behavior.

16.3.7 The `align_offset` method (§22.4) computes the offset needed to align a raw pointer to a given alignment. It is a safe operation and **MUST** return a value that, when added to the pointer, produces an aligned pointer, or a value greater than or equal to the maximum possible offset if the alignment cannot be achieved.

### 16.4 Dereferencing and Validity

16.4.1 To **dereference** a raw pointer `p`, the programmer **MUST** have established that `p` is valid for the type it points to. The dereference expression `*p` produces a place, and the place is a value of type `T`.

16.4.2 The validity of `p` includes:

- `p` is non-null;
- `p` is aligned to `align_of::<T>()`;
- `p` points to a live allocation that is at least `size_of::<T>()` bytes;
- the bytes at `p` form a valid value of `T` according to §7.11.1;
- `p` is not aliased in a way that violates §7.5.2, if a reference is created from `p`.

  16.4.3 A raw pointer that is valid for a type `T` may be converted to a reference with `&*p` or `&mut *p`. The resulting reference is subject to all the rules of §6.7.2 and §7.5. The conversion is an unsafe operation, and the programmer **MUST** ensure the reference's validity.

  16.4.4 A reference that is converted to a raw pointer and back to a reference is valid only if the intermediate raw pointer operations did not invalidate the reference. In particular, if the referent was deallocated or moved between the conversions, the second reference is invalid.

  16.4.5 The **provenance** of a raw pointer is the allocation it points into. A raw pointer derived from a reference retains the reference's provenance. A raw pointer derived from an integer has **no provenance**; dereferencing it is defined only if the integer is the address of a live allocation, and the validity rules of §16.4.2 apply.

  16.4.6 An implementation **MAY** use provenance information to optimize pointer operations. In particular, an implementation **MAY** assume that a pointer with provenance for allocation `A` is not used to access allocation `B`, even if the addresses coincide. Programs that violate this assumption by casting integers to pointers are undefined behavior.

  16.4.7 The `read_volatile` and `write_volatile` methods (§16.7) are the only ways to access memory that may be modified by external agents. A non-volatile read of such memory is undefined behavior if the value changes between reads.

### 16.5 Transmutation and Reinterpretation

16.5.1 **Transmutation** is the reinterpretation of a value's bits as a value of a different type. The `transmute::<Src, Dst>(value)` function performs a transmutation from `Src` to `Dst`.

16.5.2 Transmutation is an unsafe operation. The programmer **MUST** ensure that:

- `size_of::<Src>() == size_of::<Dst>()`; a mismatch is a compile-time error;
- `align_of::<Dst>() <= align_of::<Src>()`; otherwise the result may be misaligned;
- the bits of `Src` are a valid value of `Dst` according to §7.11.1; otherwise the transmutation is undefined behavior.

  16.5.3 Transmutation **MUST NOT** be used to duplicate a non-`Copy` value. Transmuting a value that owns an allocation produces two values that believe they own the same allocation, leading to a double free. The programmer **MUST** ensure that the source value is `Copy` or is forgotten after the transmutation.

  16.5.4 The `transmute_copy::<Src, Dst>(&value)` function reads the bits of `value` and reinterprets them as `Dst` without moving `value`. The programmer **MUST** ensure that `value` remains valid after the call; the source is not consumed.

  16.5.5 The `from_bytes` and `to_bytes` methods (§22.4) provide safe conversions between a value and its byte representation for types that have a defined byte layout. They are **not** transmutations and are safe.

  16.5.6 Transmutation between `&T` and `&U` is undefined behavior unless `T` and `U` have the same alignment and size, **and** the resulting reference is valid for `U`. In particular, transmuting `&u8` to `&bool` is undefined behavior if the byte is not `0` or `1`.

  16.5.7 An implementation **MUST** reject a `transmute` whose source and destination sizes differ. The rejection **MUST** include the sizes in the diagnostic.

### 16.6 Inline Assembly

16.6.1 The `asm!` macro allows the programmer to embed target-specific assembly instructions in a function body. Its grammar is:

```
asm-macro ::= "asm!" "(" asm-template {"," asm-operand} ","? ")"
asm-template ::= string-literal
asm-operand  ::= ident "(" operand-expression ")"
               | "in" "(" reg ")" operand-expression
               | "out" "(" reg ")" place-expression
               | "inout" "(" reg ")" place-expression
               | "clobber_abi" "(" string-literal ")"
```

16.6.2 The `asm!` macro is an unsafe operation. The programmer **MUST** ensure that the assembly is valid, that the operands' constraints are satisfied, and that the assembly does not violate the safety invariants of the surrounding code.

16.6.3 The **template** string contains literal assembly text with `{}` placeholders for operands. The placeholders are substituted with the operands' register names or memory locations. A placeholder **MUST** be well-formed and **MUST** correspond to an operand.

16.6.4 The **operand constraints** (`in`, `out`, `inout`) specify how the operand is passed. An `out` operand **MUST** be a place expression; the assembly writes its result there. An `in` operand is a value expression; its value is read by the assembly. An `inout` operand is a place that is both read and written.

16.6.5 The `clobber_abi("...")` directive declares the ABI whose caller-saved registers the assembly may clobber. The compiler inserts the necessary save/restore code around the assembly block.

16.6.6 Inline assembly **MUST** be target-specific. An implementation **MUST** reject `asm!` in a context where the target does not support inline assembly, with a diagnostic identifying the target.

16.6.7 The `global_asm!` macro emits assembly at module scope, outside any function. It is used to define global symbols and assembly functions. Its operands are similar to `asm!` but restricted to template text and symbol definitions.

16.6.8 An implementation **MUST NOT** reorder, duplicate, or eliminate inline assembly. Each `asm!` invocation corresponds to exactly one assembly block in the generated code. The assembly block's memory effects are opaque to the compiler unless declared with `clobber_abi` or `options(nostack)`.

### 16.7 Volatile Operations

16.7.1 A **volatile** operation is a read or write that **MUST NOT** be reordered, eliminated, or duplicated by the compiler. Volatile operations are used to access memory-mapped I/O, hardware registers, and other memory whose value may change outside the program's control.

16.7.2 The `read_volatile::<T>(p: *const T) -> T` function reads the value at `p` with volatile semantics. The `write_volatile::<T>(p: *mut T, value: T)` function writes `value` to `p` with volatile semantics.

16.7.3 Volatile operations are **not** atomic. They do not provide synchronization with other threads. A volatile read of a location that is concurrently modified by another thread is a data race unless the location is also atomic (§19.3).

16.7.4 Volatile operations **MUST NOT** be used for ordinary memory. Their cost is higher than ordinary operations, and their ordering guarantees are weaker than atomic operations. They are intended for memory that is accessed only by the current thread or by an external agent.

16.7.5 The **order** of volatile operations relative to non-volatile operations is unspecified. An implementation **MAY** reorder a non-volatile operation across a volatile operation, unless the volatile operation is also an atomic operation with a specified ordering.

16.7.6 A volatile operation **MUST NOT** be elided even if the result is unused. An implementation **MUST** preserve every volatile operation in the generated code.

### 16.8 Intrinsics

16.8.1 An **intrinsic** is a function that is implemented directly by the compiler rather than by Forge code. Intrinsics are used for operations that cannot be expressed in the language, such as SIMD instructions, CPU feature detection, and platform-specific primitives.

16.8.2 Intrinsics are declared in the `core::intrinsics` module and are **not** part of the stable language. An implementation **MUST** reject a call to an intrinsic that is not declared in the current language version, unless the crate uses `#![feature(...)]` to enable the intrinsic.

16.8.3 An intrinsic **MAY** be marked `unsafe` if its operation has safety invariants that the compiler cannot verify. Calling an unsafe intrinsic is an unsafe operation (§16.1.4).

16.8.4 The set of available intrinsics is implementation-defined. An implementation **MUST** document its intrinsics, and **MUST NOT** remove an intrinsic without a deprecation cycle (§28.3).

16.8.5 An intrinsic **MUST** be semantically specified in this document or in an implementation's documentation. An intrinsic that is not documented **MUST NOT** be used by a program that does not opt into unstable features.

16.8.6 An intrinsic **MUST NOT** be used where a safe operation would suffice. The compiler **MAY** replace a call to an intrinsic with a safe operation if it can prove that the intrinsic's semantics match the safe operation's semantics.

### 16.9 Unchecked Operations

16.9.1 An **unchecked operation** is a function or method that performs a checkable operation without checking. Unchecked operations are named with an `_unchecked` suffix (for example, `get_unchecked`) and are unsafe.

16.9.2 The `get_unchecked` method on slices and arrays returns a reference to an element without a bounds check. The programmer **MUST** ensure that the index is in bounds; an out-of-bounds index is undefined behavior.

16.9.3 An **`unsafe static`** is a static whose type does not implement `Sync` but which is declared `unsafe` to permit access to it. Accessing the static is an unsafe operation. The programmer **MUST** ensure that all accesses are synchronized.

16.9.4 The `addr_of!` and `addr_of_mut!` macros take a place expression and return a raw pointer to that place, without creating an intermediate reference. This is important for `unsafe` code that manipulates partially-initialized or `repr(packed)` values, where creating a reference would be undefined behavior.

16.9.5 An unchecked operation **MUST NOT** be used where the corresponding checked operation would be as fast. The optimizer **MAY** eliminate the bounds check of a checked operation if it can prove the index is in bounds; the programmer **SHOULD** prefer the checked operation unless the check is measurably expensive and the programmer has proven the index is in bounds.

16.9.6 An implementation **MUST** provide the checked operation as the default and **MUST** provide the unchecked operation only as an explicit opt-in. The unchecked operation **MUST** have the same semantics as the checked operation on all inputs for which the checked operation does not panic.

### 16.10 Safety Invariants and Undefined Behavior

16.10.1 A **safety invariant** is a property that a value, a reference, or a function **MUST** satisfy for `unsafe` code to be correct. Safety invariants are documented in each `unsafe` item's documentation and **MUST** be upheld by the code that creates or calls the item.

16.10.2 The safety invariants of Forge's primitive types are specified in §7.11.1. The safety invariants of library types are specified in their documentation (§22).

16.10.3 A **soundness hole** is a way for safe code to trigger undefined behavior. The language and its libraries **MUST NOT** contain soundness holes; a soundness hole is a defect in the specification or the implementation, not in the user's program.

16.10.4 An `unsafe` operation **MUST** be documented with the safety invariants that its caller **MUST** uphold. The documentation **MUST** state the invariants precisely enough that a programmer can determine whether a call is safe.

16.10.5 An implementation **MUST** reject a program that uses an unsafe item without an `unsafe` block, even if the program is otherwise correct. The rejection is required to make the programmer's assertion explicit.

16.10.6 An implementation **SHOULD** provide a mechanism to detect a subset of undefined behavior at runtime (§24.6). The mechanism **MUST NOT** be required for conformance, but its absence **MUST** be documented.

16.10.7 A program that executes undefined behavior **MUST** be considered non-conforming, even if the implementation does not detect the behavior. The implementation's failure to detect undefined behavior does not make the program conforming.

---

## 17. Data Layout and ABI

### 17.1 Type Layout Rules

17.1.1 The **layout** of a type is the arrangement of its fields in memory, including their offsets, the size of the type, and the alignment of the type. Layout is determined by the type's declaration and its `#[repr(...)]` attribute (§17.3).

17.1.2 The **size** of a type is the number of bytes that a value of the type occupies. The **alignment** is the largest power of two that divides the addresses at which a value of the type may be placed. The size **MUST** be a multiple of the alignment.

17.1.3 For a **scalar type** `T`, `size_of::<T>()` is the size given in §6.2.1, and `align_of::<T>()` equals `size_of::<T>()` for all primitive scalars except the `str` type (which is unsized).

17.1.4 For a **tuple** `(A, B, ..., Z)` without an explicit representation, the layout is **implementation-defined**. An implementation **MUST** lay out the fields in an order that minimizes padding, but **MAY** reorder the fields. The size of a tuple **MUST** be at least the sum of its fields' sizes.

17.1.5 For an **array** `[T; N]`, the layout is `N` consecutive `T` values with no padding between them. The size is `N * size_of::<T>()`, and the alignment is `align_of::<T>()`.

17.1.6 For a **reference** `&T`, `&mut T`, `*const T`, or `*mut T`, the size and alignment are the target's pointer size and alignment (§27.4).

17.1.7 For an **enum** without an explicit representation, the layout is **implementation-defined**. An implementation **MAY** use the **niche optimization** (§17.5.4) to represent some variants without a discriminant. The size of an enum is **not** guaranteed to be the sum of its variants' sizes plus a discriminant.

17.1.8 For a **struct** with a `#[repr(...)]` attribute, the layout is specified by §17.4. For a struct without an attribute, the layout is implementation-defined but **MUST** be stable for a given implementation and target.

### 17.2 Alignment and Size

17.2.1 The `size_of::<T>()` function returns the size of `T` as a `usize`. The `align_of::<T>()` function returns the alignment of `T` as a `usize`. Both are constant expressions when `T` is known at compile time (§8.19).

17.2.2 The `size_of_val(&value)` and `align_of_val(&value)` functions return the size and alignment of a value, including unsized types. For a `&dyn Trait`, the size and alignment are those of the concrete type, obtained from the vtable.

17.2.3 The **alignment** of a type **MUST** be a power of two. An implementation **MUST** reject a type whose alignment is not a power of two, unless the type is declared with a `#[repr(align(N))]` attribute that specifies a power-of-two alignment.

17.2.4 The `align(N)` attribute (§17.3.4) increases a type's alignment to at least `N`. The `packed` attribute (§17.3.5) reduces a struct's alignment to `1` and **MUST NOT** be used with `align`.

17.2.5 The **size** of an unsized type is not known at compile time. A reference to an unsized type is a fat pointer (§6.17.4), and the size is obtained from the pointer's metadata at runtime.

### 17.3 Representation Attributes

17.3.1 The `#[repr(...)]` attribute specifies the layout of a struct, enum, or union. Its forms are:

```
repr-attr ::= "#" "[" "repr" "(" repr-args ")" "]"
repr-args ::= repr-arg {"," repr-arg}
repr-arg  ::= "C" | "transparent" | "packed" | "align" "(" int-literal ")"
            | "u8" | "u16" | "u32" | "u64" | "u128" | "usize"
            | "i8" | "i16" | "i32" | "i64" | "i128" | "isize"
            | "simd"
```

17.3.2 The `#[repr(C)]` attribute lays out a type according to the C ABI for the target platform. For a struct, the fields are laid out in declaration order with C-style padding between them. For a union, the fields overlap at offset zero. For an enum, the representation is implementation-defined but **MUST** match the C ABI's enum representation for the target.

17.3.3 The `#[repr(transparent)]` attribute may be applied to a struct with exactly one non-zero-sized field. The struct's layout is identical to that field's layout. A transparent struct **MUST** have no other fields except zero-sized fields, and **MUST NOT** have a `Drop` implementation if the field does not.

17.3.4 The `#[repr(align(N))]` attribute increases the type's alignment to at least `N`. `N` **MUST** be a power of two. The attribute **MUST NOT** reduce the alignment; it **MAY** increase it. The attribute may be combined with `#[repr(C)]` to specify both layout and alignment.

17.3.5 The `#[repr(packed)]` attribute reduces the type's alignment to `1` and removes padding between fields. Accessing a field of a packed struct through a reference is undefined behavior if the field's natural alignment is greater than `1`; the programmer **MUST** use `addr_of!` and `read_unaligned` instead.

17.3.6 The `#[repr(simd)]` attribute requests a SIMD vector representation for a struct. The struct's fields **MUST** all have the same type, and the number of fields **MUST** be a power of two. The layout is target-specific and **MUST** match the target's SIMD ABI.

17.3.7 The integer representation attributes (`#[repr(u8)]`, `#[repr(i32)]`, etc.) specify the discriminant type of an enum (§17.5.2). They **MAY** be combined with `#[repr(C)]` to specify both the discriminant type and the field layout.

### 17.4 Struct and Union Layout

17.4.1 A **C-layout struct** has its fields laid out in declaration order, each at the smallest offset that is a multiple of the field's alignment and at least the end of the previous field. The struct's size is the smallest multiple of its alignment that is at least the end of its last field.

17.4.2 A **C-layout union** has all fields at offset zero. The union's size is the maximum of its fields' sizes rounded up to its alignment. The union's alignment is the maximum of its fields' alignments.

17.4.3 A **packed struct** has its fields at consecutive offsets with no padding. The struct's size is the sum of its fields' sizes, and its alignment is `1`. The struct's fields may be misaligned; accessing them requires unaligned loads or `addr_of!`.

17.4.4 An **aligned struct** has its alignment increased to at least `N` and its size rounded up to a multiple of `N`. The fields' offsets are unchanged unless the natural layout requires padding, which is preserved.

17.4.5 A **transparent struct** has the same layout as its single non-zero-sized field, including size, alignment, and field offsets (which are all zero, since the struct has one field).

17.4.6 A **generic struct** with a `#[repr(C)]` attribute has its layout determined by its concrete instantiation. The `#[repr(C)]` attribute **MUST** appear on the struct declaration; the layout is not uniform across instantiations unless the struct's generic parameters are used in a way that preserves the layout.

17.4.7 The **field offset** of a field `f` in a struct `S` is obtained with `offset_of!(S, f)` (§22.4). The offset is a constant expression for a struct with a specified layout; for an implementation-defined layout, the offset is a compile-time constant within a compilation but **MUST NOT** be relied upon across recompilations.

### 17.5 Enum Layout

17.5.1 An **enum** without a `#[repr(...)]` attribute has an **implementation-defined** layout. An implementation **MAY** choose any layout that satisfies the following:

- every variant's payload is stored at a valid location for its type;
- the discriminant is stored at a valid location for the discriminant type;
- the enum's size and alignment are at least the size and alignment of any variant.

  17.5.2 An enum with a **`#[repr(int_type)]`** attribute has its discriminant stored as `int_type` (§17.3.7). The enum's size is the size of the smallest type that can hold both the discriminant and the largest variant's payload. The enum's alignment is the maximum of the discriminant type's alignment and the variants' alignments.

  17.5.3 An enum with **`#[repr(C)]`** has its layout specified by the C ABI's enum representation. The discriminant is stored as the C ABI's enum type (usually `int`), and the variants' payloads are stored at offsets determined by the C ABI's rules. The enum's size and alignment **MUST** match what a C compiler would produce for the equivalent declaration.

  17.5.4 The **niche optimization** allows an enum to use an invalid bit pattern of one of its variants' types as the discriminant for another variant. For example, `Option<&T>` has the same size as `&T` because the null pointer is an invalid value for `&T` and can represent `None`. The niche optimization is **implementation-defined**; a program **MUST NOT** rely on a particular niche being used unless the enum has a `#[repr(...)]` attribute that guarantees it.

  17.5.5 An enum with a **single variant** with a payload and no other variants has the same layout as a struct with that payload. An enum with no variants (`enum Void {}`) has size `0` and alignment `1`.

  17.5.6 The **discriminant** of an enum value is obtained with `std::mem::discriminant(&value)`. The discriminant's value is **implementation-defined** unless the enum has an explicit `#[repr(int_type)]`; in that case, the discriminant is the `int_type` value of the variant.

  17.5.7 The `#[repr(u8)]` and related attributes **MUST** specify a discriminant type large enough to hold the maximum discriminant. An implementation **MUST** reject a `#[repr(u8)]` enum whose discriminant exceeds `255`.

### 17.6 Calling Conventions

17.6.1 The **calling convention** of a function specifies how arguments are passed to it, how the return value is returned, and what registers are preserved across the call. The calling conventions supported by Forge are specified in §12.5.1.

17.6.2 The **default convention** `forge` is implementation-defined but **MUST** be consistent within a compilation. An implementation **MAY** use the platform's C ABI for `forge`, or a different convention optimized for Forge's type system.

17.6.3 The **`"C"` convention** **MUST** match the platform's C ABI. The rules for parameter passing, return values, and register preservation are specified by the platform's ABI document, not by this specification.

17.6.4 The **`"system"` convention** selects the platform's system ABI. On most platforms, this is the C ABI; on 32-bit Windows, it is `"stdcall"`.

17.6.5 The **`"fastcall"`** convention passes the first two arguments in registers and the rest on the stack, with the callee cleaning up the stack. The exact register assignment is target-specific.

17.6.6 The **`"stdcall"`** convention passes all arguments on the stack, with the callee cleaning up the stack. It is the default for Win32 API functions.

17.6.7 The **`"thiscall"`** convention passes the first argument (the `this` pointer) in a register and the rest on the stack. It is used by C++ member functions on 32-bit Windows.

17.6.8 The **`"vectorcall"`** convention passes SIMD arguments in vector registers. Its availability and behavior are target-specific.

17.6.9 A function pointer's **type** includes its calling convention. A pointer to a function with one convention **MUST NOT** be used to call a function with a different convention; the call is undefined behavior.

17.6.10 An implementation **MUST** document the mapping from Forge types to ABI types for each supported calling convention (§17.10). The mapping **MUST** be consistent with the platform's ABI for the C convention and **MAY** be implementation-defined for the `forge` convention.

### 17.7 Name Mangling

17.7.1 **Name mangling** is the process of encoding a Forge item's name into a linker symbol. Mangling is necessary because Forge's namespaces, generics, and modules may produce symbols that conflict with C or with each other.

17.7.2 The **`#[no_mangle]`** attribute disables mangling for an item. The item's name is used as the linker symbol directly. An implementation **MUST** reject a `#[no_mangle]` item whose name conflicts with another `#[no_mangle]` item or with a foreign symbol, unless the conflict is resolved by the linker.

17.7.3 The **default mangling scheme** is implementation-defined but **MUST** satisfy:

- distinct items have distinct mangled names;
- the mangled name is a valid symbol for the target (usually a sequence of alphanumeric characters and underscores);
- the mangled name encodes the item's module path and its generic arguments.

  17.7.4 A **`#[link_name("symbol")]`** attribute overrides the symbol name of a foreign declaration. The declared symbol is used to resolve the reference, regardless of the Forge name.

  17.7.5 A **`#[export_name("symbol")]`** attribute overrides the symbol name of a Forge item that is exported to foreign code. The symbol is the name under which the linker and foreign code see the item.

  17.7.6 An implementation **MUST NOT** change the mangling scheme without a deprecation cycle (§28.3). A change in mangling is a breaking change for code that depends on symbols being stable across recompilations.

  17.7.7 An implementation **SHOULD** use a mangling scheme that is stable across recompilations of the same source with the same compiler version. The scheme **MAY** change across compiler versions, subject to §28.3.

### 17.8 Symbol Visibility and Linkage

17.8.1 The **visibility** of a linker symbol controls which symbols are visible to other objects at link time. The visibility of a Forge item **MAY** be specified with a `#[link_section]` or `#[visibility(...)]` attribute.

17.8.2 A symbol with **external** visibility is visible to all objects in the link. A symbol with **internal** visibility is visible only to the object that defines it. A symbol with **hidden** visibility is visible within the shared library but not exported.

17.8.3 The **linkage** of a symbol is its behavior when the same symbol is defined in multiple objects. Forge supports the following linkages:

- **external** — the symbol **MUST** be defined exactly once across the link;
- **internal** — the symbol is local to the object and does not participate in the link;
- **linkonce** — the linker **MUST** merge multiple definitions and keep one (§17.8.4);
- **weak** — the linker **MUST** prefer a strong definition if present; otherwise it uses the weak one.

  17.8.4 A **linkonce** symbol is used for monomorphized generic functions and other symbols that **MAY** be duplicated across objects. The linker **MUST** merge duplicate definitions and keep exactly one. If the duplicates are not identical, the behavior is undefined; an implementation **MUST** ensure that all duplicates are identical.

  17.8.5 A **weak** symbol allows a program to override a default implementation without modifying the library that provides it. The strong definition **MUST** be present in the link; the weak definition is used only if no strong definition exists.

  17.8.6 The **`#[no_mangle]`** attribute gives an item external linkage by default. The `#[used]` attribute prevents the linker from discarding an item that is not referenced; it is used for exported symbols that foreign code may reference.

  17.8.7 The **`#[link_section("...")]`** attribute places an item in a specific linker section. It is used for interrupt vectors, initialization tables, and other sections whose layout is determined by the linker script.

### 17.9 Application Binary Interface

17.9.1 An **Application Binary Interface** (ABI) is the contract between compiled code units that allows them to call each other. The ABI of a Forge program is determined by the target platform, the calling conventions of its functions, and the layout of its types.

17.9.2 Two object files **MUST** agree on their ABI to be linked together. A mismatch in calling convention, struct layout, or enum representation **MUST** be detected by the linker or by runtime checks, or **MUST** be prevented by the language's type system.

17.9.3 The **Forge ABI** is the ABI used by default for functions and types within a single crate. It is implementation-defined but **MUST** be stable across recompilations of the same source with the same compiler version.

17.9.4 The **C ABI** is the ABI used for functions and types that are exposed to foreign code. It is specified by the target platform's C ABI, and Forge's C ABI conformance is specified in §18.4.

17.9.5 A **`#[repr(C)]`** type **MUST** have the same layout as the corresponding C type on the target. A **`extern "C"`** function **MUST** have the same calling convention as a C function with the same signature.

17.9.6 A **cross-language call** is a call between Forge code and foreign code. The call **MUST** respect the C ABI (or the specified foreign ABI) for both sides. The Forge side **MUST** use `extern "C"` and a `#[repr(C)]` type for the parameters and return value.

### 17.10 Platform-Specific ABI Notes

17.10.1 On **x86-64 System V** (Linux, macOS, BSD), the first six integer or pointer arguments are passed in `rdi`, `rsi`, `rdx`, `rcx`, `r8`, and `r9`. Floating-point arguments are passed in `xmm0` through `xmm7`. The return value is in `rax` (integer) or `xmm0` (floating-point). Structs that fit in two registers are passed in registers; larger structs are passed by reference.

17.10.2 On **x86-64 Windows**, the first four arguments are passed in `rcx`, `rdx`, `r8`, and `r9`. Floating-point arguments are passed in `xmm0` through `xmm3`. The return value is in `rax` or `xmm0`. Structs of size 1, 2, 4, or 8 are passed in registers; larger structs are passed by reference. The caller allocates shadow space for the callee's use.

17.10.3 On **AArch64** (ARM64), the first eight arguments are passed in `x0` through `x7`. Floating-point arguments are passed in `v0` through `v7`. The return value is in `x0` (integer) or `v0` (floating-point). Structs up to 16 bytes are passed in registers; larger structs are passed by reference.

17.10.4 On **32-bit x86**, all arguments are passed on the stack. The return value is in `eax` or `st0`. Structs are returned via a hidden pointer passed as the first argument.

17.10.5 On **RISC-V 64**, the first eight arguments are passed in `a0` through `a7`. Floating-point arguments are passed in `fa0` through `fa7`. Structs up to 16 bytes are passed in registers; larger structs are passed by reference.

17.10.6 An implementation **MUST** document the ABI details for every supported target. The details **MUST** match the platform's ABI specification for `extern "C"` items and **MAY** differ for `forge` items.

17.10.7 A program that depends on a specific ABI behavior **MUST** use `extern "C"` and `#[repr(C)]` types, not the default `forge` convention and layout. The `forge` convention is implementation-defined and **MUST NOT** be relied upon across compiler versions or crates.

---

## 18. Foreign Function Interface

### 18.1 External Declarations

18.1.1 An **external declaration** declares an item that is defined outside the current crate. Its grammar is specified in §11.9.1.

18.1.2 The **`extern` block** groups external declarations and specifies their ABI:

```
extern-block ::= "extern" abi-string? "{" foreign-items "}"
```

The ABI string **MUST** be a valid calling convention (§12.5.1) or an implementation-specific ABI. The default ABI is `"C"`.

18.1.3 An **external function** declaration has no body and is resolved at link time to a symbol defined in a foreign object. The symbol name **MAY** be overridden with `#[link_name("...")]` (§17.7.4).

18.1.4 An **external static** declaration has no initializer and is resolved at link time. Its type **MUST** be FFI-safe (§18.6).

18.1.5 An **external type** declaration declares an opaque type whose layout is determined by the foreign definition. The type is unsized and **MUST** be used behind a pointer, unless a `#[repr(...)]` attribute specifies its layout.

18.1.6 The **linkage** of an external declaration **MUST** match the linkage of the foreign symbol. A mismatch **MUST** be reported by the linker as an undefined symbol or a duplicate symbol.

18.1.7 An implementation **MUST** reject an external declaration whose ABI string is not supported for the target. The rejection **MUST** include the ABI string and the target.

### 18.2 Calling Foreign Functions

18.2.1 A **foreign function call** is a call to a function declared in an `extern` block. The call **MUST** use the declared ABI and **MUST** pass arguments according to the ABI's calling convention.

18.2.2 The **arguments** of a foreign function call **MUST** be FFI-safe (§18.6). An argument whose type is not FFI-safe is an error unless the function is annotated with `#[allow(improper_ctypes)]`.

18.2.3 The **return value** of a foreign function **MUST** be FFI-safe. A return value whose type is not FFI-safe is an error unless the function is annotated with `#[allow(improper_ctypes)]`.

18.2.4 Calling a foreign function is an **unsafe operation** unless the function is marked `safe` by an implementation-specific attribute. The programmer **MUST** ensure that the arguments satisfy the foreign function's preconditions and that the return value satisfies the Forge type's validity invariants.

18.2.5 A foreign function **MUST NOT** unwind across the FFI boundary. If the foreign function panics or throws an exception, the behavior is undefined unless the ABI is `"C-unwind"` (§21.4.4) and the foreign function's language supports unwinding.

18.2.6 A foreign function **MUST NOT** retain a reference to its arguments after it returns, unless the arguments are `'static` or the foreign function is documented to take ownership. The Forge side **MUST** ensure that the arguments outlive the call.

18.2.7 A foreign function's **calling convention** **MUST** match the Forge declaration's ABI. A mismatch is undefined behavior; the Forge side **MUST** use the correct ABI string.

### 18.3 Exporting Forge Functions

18.3.1 A **exported function** is a Forge function that foreign code may call. An exported function **MUST** be declared with `extern "C"` (or another foreign ABI) and **MUST** be marked `#[no_mangle]` or `#[export_name("...")]` to give it a stable symbol.

18.3.2 An exported function's **parameters and return value** **MUST** be FFI-safe (§18.6). An exported function whose signature is not FFI-safe **MUST** be rejected with a diagnostic.

18.3.3 An exported function's **body** **MUST NOT** unwind across the FFI boundary unless the ABI is `"C-unwind"`. A panic in an exported function **MUST** be caught and converted to an abort or an error code before returning to the foreign caller.

18.3.4 An exported function **MUST NOT** receive a Forge reference to a value it does not own, unless the reference is guaranteed to outlive the foreign call. The foreign caller is responsible for ensuring that the reference is valid for the duration of the call.

18.3.5 An exported function **MAY** be called from multiple threads concurrently. The function's body **MUST** be thread-safe (§19.6), or the function **MUST** be documented as requiring external synchronization.

18.3.6 The **symbol** of an exported function **MUST** be unique across the link. An implementation **MUST** reject an exported function whose symbol conflicts with another symbol in the same link.

### 18.4 C ABI Compatibility

18.4.1 Forge's `extern "C"` ABI **MUST** match the target platform's C ABI for functions and types. The rules for parameter passing, return values, and struct layout are specified by the platform's C ABI, not by this specification.

18.4.2 The **C types** that have Forge equivalents are:

| C type                      | Forge type                          |
| --------------------------- | ----------------------------------- |
| `void`                      | `()` (as return type only)          |
| `bool` (`_Bool`)            | `bool`                              |
| `char` (signed or unsigned) | `i8` or `u8` (platform-dependent)   |
| `short`                     | `i16`                               |
| `int`                       | `i32`                               |
| `long`                      | `i32` or `i64` (platform-dependent) |
| `long long`                 | `i64`                               |
| `float`                     | `f32`                               |
| `double`                    | `f64`                               |
| `void *`                    | `*mut c_void` (§22.5)               |
| `T *`                       | `*mut T` or `*const T`              |
| `size_t`                    | `usize`                             |
| `ssize_t`                   | `isize`                             |
| `struct S { ... }`          | `#[repr(C)] struct S { ... }`       |
| `union U { ... }`           | `#[repr(C)] union U { ... }`        |
| `enum E { ... }`            | `#[repr(C)] enum E { ... }`         |

18.4.3 A **C struct** is represented by a Forge struct with `#[repr(C)]` and fields whose types match the C struct's fields. The struct's size and alignment **MUST** match the C compiler's layout.

18.4.4 A **C union** is represented by a Forge union with `#[repr(C)]` and fields whose types match the C union's fields.

18.4.5 A **C enum** is represented by a Forge enum with `#[repr(C)]` or `#[repr(int_type)]` and variants whose discriminants match the C enum's values. The discriminant type **MUST** be large enough to hold all values.

18.4.6 A **C bit-field** is not supported by Forge's FFI. A Forge struct **MUST NOT** be used to represent a C struct with bit-fields; the programmer **MUST** represent the struct as an array of bytes and access the bits manually.

18.4.7 A **C function pointer** is represented by a Forge function pointer with the `"C"` convention. The function pointer **MUST** be non-null; a null C function pointer **MUST** be represented by `Option<extern "C" fn(...)>` (§17.5.4).

### 18.5 Variadic Functions

18.5.1 A **variadic function** is a C function that accepts a variable number of arguments. Forge supports calling variadic functions but does not support defining them.

18.5.2 A variadic function **MUST** be declared with a trailing `...` in its parameter list:

```
extern "C" {
    fn printf(format: *const c_char, ...) -> c_int;
}
```

The `...` **MUST** appear only in an `extern "C"` function declaration. A Forge function **MUST NOT** be variadic.

18.5.3 The **fixed arguments** of a variadic call are those before the `...`. Their types are specified in the declaration. The **variadic arguments** are the remaining arguments; they are passed according to the C ABI's variadic calling convention, which **MAY** differ from the fixed calling convention (for example, floating-point variadic arguments are passed differently on x86-64 System V).

18.5.4 The **`va_list`** type and the `va_start`, `va_arg`, and `va_end` macros are available through the C library's headers. Forge does not define these macros; they are accessed through the foreign interface (§22.5).

18.5.5 A variadic argument whose type is not FFI-safe **MUST** be rejected. An implementation **MUST** check the variadic arguments' types against the C ABI's requirements.

18.5.6 A variadic call **MUST** pass the correct number and types of arguments for the callee's format or expectations. Passing the wrong number or types is undefined behavior; the Forge side **MUST** ensure the call is well-formed.

### 18.6 Foreign Types and Layout

18.6.1 A type is **FFI-safe** if it can be passed across an FFI boundary without violating the ABI's requirements. The FFI-safe types are:

- primitive scalar types (`bool`, integers, floats, `char`);
- `()` as a return type only;
- raw pointers `*const T` and `*mut T` for any `T`;
- references `&T` and `&mut T` when `T` is FFI-safe and the reference's lifetime is not restricted by a `'static` bound;
- function pointers `extern "C" fn(...)` and `Option<extern "C" fn(...)>`;
- `#[repr(C)]` structs, unions, and enums whose fields are FFI-safe;
- `#[repr(transparent)]` structs whose single field is FFI-safe;
- `#[repr(C)]` or `#[repr(int_type)]` enums with no payload or with FFI-safe payloads;
- `ManuallyDrop<T>` and `MaybeUninit<T>` when `T` is FFI-safe (§22.4);
- arrays of FFI-safe types;
- tuples of FFI-safe types **only** if the tuple is `#[repr(C)]`, which is not the default.

  18.6.2 A **generic type** is FFI-safe if its instantiation is FFI-safe. However, a generic type **MUST NOT** be exposed across an FFI boundary unless the specific instantiation is known to be FFI-safe; the C side must agree on the instantiation.

  18.6.3 A **`#[repr(transparent)]`** struct **MUST** have exactly one non-zero-sized field. The field's type determines the struct's FFI representation. A transparent struct is FFI-safe if its field is FFI-safe.

  18.6.4 A **`#[repr(packed)]`** struct is **not** FFI-safe, because its fields may be misaligned and C does not support packed structs directly. A packed struct **MUST NOT** be passed across an FFI boundary; the programmer **MUST** serialize it to bytes.

  18.6.5 A **`#[repr(align(N))]`** struct is FFI-safe if its fields are FFI-safe, but the C side **MUST** use the same alignment. A mismatch in alignment is undefined behavior.

  18.6.6 An **unsized type** is not FFI-safe and **MUST NOT** appear in an FFI signature. A reference to an unsized type is a fat pointer, which is not FFI-safe.

  18.6.7 An implementation **MUST** reject an FFI declaration whose signature contains a non-FFI-safe type, unless the declaration is annotated with `#[allow(improper_ctypes)]`. The rejection **MUST** identify the non-FFI-safe type.

### 18.7 Callbacks and Function Pointers

18.7.1 A **callback** is a foreign function pointer that Forge code calls, or a Forge function that foreign code calls. Callbacks are represented as function pointers with the `"C"` convention.

18.7.2 A **Forge callback** passed to foreign code **MUST** be an `extern "C" fn(...)` with an FFI-safe signature. The callback **MUST NOT** unwind across the FFI boundary unless the ABI is `"C-unwind"`.

18.7.3 A **foreign callback** received by Forge code **MUST** be an `extern "C" fn(...)` with an FFI-safe signature. The foreign callback **MUST NOT** unwind into Forge code; the Forge side **MUST** catch any foreign exceptions at the boundary.

18.7.4 A callback's **lifetime** is the duration of the foreign call that received it. A Forge callback **MUST NOT** retain any references to Forge stack values after it returns, unless the foreign code guarantees to call it only during the enclosing call.

18.7.5 A callback **MAY** be invoked from a foreign thread. The Forge callback **MUST** be thread-safe (§19.6), or the foreign code **MUST** guarantee to call it only from the thread that registered it.

18.7.6 The **function pointer type** of a callback **MUST** include its calling convention. A callback declared with `"C"` **MUST** be called with the C ABI; a callback declared with `"system"` **MUST** be called with the system ABI.

### 18.8 Linking and Symbols

18.8.1 **Linking** is the process of combining object files into an executable or library. Forge programs **MUST** be linked with the foreign libraries they depend on, either statically or dynamically.

18.8.2 The **`#[link(name = "...")]`** attribute on an `extern` block specifies the foreign library to link against. The library name **MUST** match the foreign library's name, without the platform-specific prefix or suffix.

18.8.3 The **`#[link(kind = "...")]`** attribute specifies the kind of library to link: `"static"` for a static library, `"dylib"` for a dynamic library, or `"framework"` for a macOS framework.

18.8.4 A **dynamic library** symbol is resolved at load time. The loader **MUST** find the library and resolve the symbol, or the program **MUST** fail to start with a diagnostic.

18.8.5 A **static library** symbol is resolved at link time. The linker **MUST** find the symbol in the library, or the link **MUST** fail with a diagnostic.

18.8.6 The **`#[used]`** attribute prevents the linker from discarding an item that is not referenced. It is used for exported symbols that foreign code may reference by name.

18.8.7 The **`#[no_mangle]`** attribute on an exported item **MUST** produce a symbol with the item's name. The symbol's visibility **MUST** match the item's visibility; a private item **MUST NOT** be exported with `#[no_mangle]`.

18.8.8 The **`#[export_name("...")]`** attribute on an exported item **MUST** produce a symbol with the specified name. The name **MUST** be a valid symbol for the target, and **MUST NOT** conflict with another symbol in the same link.

---

## 19. Concurrency and Atomics

### 19.1 Threads and Execution Contexts

19.1.1 A **thread** is an independent execution context with its own stack and its own copy of thread-local storage. Threads share the program's heap and static storage.

19.1.2 The Forge standard library provides thread creation through `std::thread::spawn` (§22.6), which returns a `JoinHandle` that can be used to wait for the thread's completion.

19.1.3 A **thread-local** value is a value that is distinct for each thread. Thread-locals are declared with the `thread_local!` macro (§22.6) and are accessed through a `LocalKey` handle.

19.1.4 A **scoped thread** is a thread whose lifetime is bounded by a scope. Scoped threads may borrow values from the parent thread's stack, because the scope ensures the borrowed values outlive the thread (§22.6).

19.1.5 The **main thread** is the thread that starts the program and calls `main`. A program **MUST** terminate when all threads have joined or been detached.

19.1.6 A **detached thread** is a thread whose handle has been dropped. The thread continues to run independently; the program **MUST NOT** terminate until the detached thread has finished, unless the process exits explicitly.

19.1.7 The **number of threads** available to a program is implementation-defined. An implementation **MUST** provide at least the main thread and **MUST** support thread creation through the standard library.

### 19.2 Memory Ordering

19.2.1 A **memory ordering** specifies the constraints on the relative order of memory operations observed by different threads. Forge supports the following orderings, modeled on the C++ memory model:

| Ordering  | Meaning                                                                                                 |
| --------- | ------------------------------------------------------------------------------------------------------- |
| `Relaxed` | no synchronization; only the atomicity of the operation is guaranteed                                   |
| `Acquire` | a load that synchronizes with a `Release` store, ensuring subsequent reads see the store's prior writes |
| `Release` | a store that synchronizes with an `Acquire` load, ensuring the store's prior writes are visible         |
| `AcqRel`  | both `Acquire` and `Release`; used for read-modify-write operations                                     |
| `SeqCst`  | sequentially consistent; all `SeqCst` operations have a total order consistent with the program order   |

19.2.2 A **synchronizes-with** relationship is established between a `Release` store and a subsequent `Acquire` load of the same atomic location. The `Acquire` load observes the `Release` store's prior writes.

19.2.3 A **happens-before** relationship is the transitive closure of the program order and the synchronizes-with relationship. An operation that happens-before another operation is guaranteed to be visible to it.

19.2.4 A **data race** occurs when two threads access the same non-atomic location, at least one of the accesses is a write, and the accesses are not ordered by happens-before. A data race is undefined behavior (§19.5).

19.2.5 The **`SeqCst`** ordering is the strongest and is the default for atomic operations. It provides a total order across all `SeqCst` operations, which is often easier to reason about than weaker orderings.

19.2.6 The **`Relaxed`** ordering provides only atomicity. It is suitable for counters and flags where the relative order of operations is not important. A `Relaxed` operation **MUST NOT** be used to establish synchronization.

19.2.7 An implementation **MUST** implement the memory orderings on the target platform's atomic instructions. On platforms with weaker memory models, the implementation **MUST** insert the necessary fences.

### 19.3 Atomic Types and Operations

19.3.1 An **atomic type** is a type whose operations are atomic: `AtomicBool`, `AtomicI8`, `AtomicI16`, `AtomicI32`, `AtomicI64`, `AtomicIsize`, `AtomicU8`, `AtomicU16`, `AtomicU32`, `AtomicU64`, `AtomicUsize`, and `AtomicPtr<T>` (§22.6).

19.3.2 Atomic types are **not** `Copy`. Their operations take `&self` and modify the value in place, ensuring that the modification is atomic.

19.3.3 The **`load`** method reads the atomic value with the specified ordering. The **`store`** method writes the atomic value with the specified ordering. Both are atomic: they **MUST NOT** be torn or otherwise observed partially.

19.3.4 The **`swap`** method atomically replaces the value and returns the previous value. The **`compare_exchange`** method atomically compares the value with an expected value and, if equal, replaces it with a new value. The comparison and replacement are a single atomic operation.

19.3.5 The **`fetch_add`**, **`fetch_sub`**, **`fetch_and`**, **`fetch_or`**, and **`fetch_xor`** methods perform the corresponding operation atomically and return the previous value.

19.3.6 An atomic type's **size and alignment** are the same as the corresponding non-atomic type. An `AtomicU64` is 8 bytes with 8-byte alignment on platforms that support 64-bit atomics.

19.3.7 A **`AtomicPtr<T>`** holds a raw pointer and provides atomic operations on it. The pointer **MUST** be valid when dereferenced; the atomicity of the pointer does not make the pointee's access atomic.

19.3.8 An implementation **MUST** support atomic operations for all atomic types on all targets. On targets that do not have native support for a given width, the implementation **MUST** emulate the atomic operation with a lock or a sequence of narrower atomics.

19.3.9 The **`AtomicBool`** type is 1 byte. Its operations are atomic on all platforms that support byte-granularity atomics. On platforms that do not, the implementation **MUST** use a lock.

### 19.4 Synchronization Primitives

19.4.1 A **mutex** is a mutual-exclusion lock that allows one thread at a time to access the protected data. The standard library provides `Mutex<T>` (§22.6), which wraps a value of type `T` and guards its access.

19.4.2 A **read-write lock** allows multiple readers or one writer. The standard library provides `RwLock<T>` (§22.6), which wraps a value of type `T` and provides `read` and `write` guards.

19.4.3 A **condition variable** allows a thread to wait for a condition to become true. The standard library provides `Condvar` (§22.6), which is used with a `Mutex<T>` to wait for a change to the protected data.

19.4.4 A **semaphore** is a counter that allows up to a specified number of threads to access a resource. The standard library provides `Semaphore` (§22.6) for this purpose.

19.4.5 A **barrier** allows a set of threads to wait until all of them have reached the barrier. The standard library provides `Barrier` (§22.6) for this purpose.

19.4.6 A **channel** allows threads to send messages to each other. The standard library provides `mpsc::channel` (§22.6) for multiple-producer, single-consumer channels.

19.4.7 An implementation **MUST** ensure that the standard synchronization primitives are correct with respect to the memory model of §19.2. A `Mutex::lock` **MUST** provide `Acquire` semantics; a `Mutex::unlock` **MUST** provide `Release` semantics.

### 19.5 Data Races and Undefined Behavior

19.5.1 A **data race** is a pair of accesses to the same non-atomic memory location, by different threads, at least one of which is a write, with no happens-before relationship between them. A data race is undefined behavior (§7.11.2).

19.5.2 A **data race** may result in torn reads, stale reads, or arbitrary values. An implementation **MUST NOT** be required to detect data races; a program with a data race is incorrect regardless of whether the race is detected.

19.5.3 **Atomic operations** on the same location are never a data race. Two atomic operations are ordered by the memory ordering they specify; if they are both `Relaxed`, they are still atomic, but no ordering is guaranteed.

19.5.4 A **mixed access** — an atomic access and a non-atomic access to the same location — is a data race. Atomic and non-atomic accesses **MUST NOT** be mixed on the same location; the programmer **MUST** use one or the other consistently.

19.5.5 The **`UnsafeCell<T>`** type (§22.4) provides interior mutability for types that need to be mutated through a shared reference. Accessing an `UnsafeCell` from multiple threads without synchronization is a data race unless the accesses are atomic.

19.5.6 The **`Mutex<T>`** and **`RwLock<T>`** types ensure that access to their inner `T` is synchronized. Accessing the inner `T` without holding the lock is a data race; the lock's guard is the only way to access the data.

19.5.7 An implementation **MAY** provide a **data race detector** (§24.6) that detects a subset of data races at runtime. The detector **MUST NOT** be required for conformance.

### 19.6 Send/Sync or Thread-Safety Traits

19.6.1 The **`Send`** trait marks a type that can be transferred across threads. A value of a `Send` type **MAY** be moved to another thread and used there.

19.6.2 The **`Sync`** trait marks a type that can be shared across threads. A reference `&T` of a `Sync` type **MAY** be sent to another thread and used there.

19.6.3 The **`Send`** and **`Sync`** traits are **auto traits**: they are implemented automatically for a type if all of its fields are `Send` or `Sync`, respectively. A type **MAY** opt out of an auto trait with a negative implementation (§14.7.6).

19.6.4 A type **MUST** be `Send` to be passed to `thread::spawn`. A type **MUST** be `Sync` for a shared reference to it to be passed to `thread::spawn`.

19.6.5 A raw pointer is **not** `Send` or `Sync`, because the compiler cannot verify the pointee's safety invariants. A program that needs to send a raw pointer across threads **MUST** wrap it in a type that asserts the necessary invariants with `unsafe impl Send for Wrapper`.

19.6.6 A **`!Send`** type is a type that cannot be transferred across threads. Examples include `Rc<T>` (which has a non-atomic reference count) and types that contain a raw pointer to thread-local data.

19.6.7 A **`!Sync`** type is a type that cannot be shared across threads. Examples include `Cell<T>` (which allows mutation through a shared reference) and `RefCell<T>` (whose borrow state is not atomic).

19.6.8 The `Mutex<T>` and `RwLock<T>` types are `Send` and `Sync` if `T` is `Send`. The lock ensures that access to the inner `T` is synchronized, so sharing the lock across threads is safe.

19.6.9 An implementation **MUST** provide the `Send` and `Sync` traits and **MUST** enforce their bounds on the standard library's thread and synchronization APIs.

### 19.7 Static Initialization and Destruction

19.7.1 **Static initialization** is the process of initializing the program's `static` items and thread-locals before `main` begins. The order of initialization across statics is unspecified (§11.3.10).

19.7.2 A **static initializer** **MUST** be a constant expression (§8.19). A static whose initialization requires runtime computation **MUST** be initialized lazily through a `OnceCell` or `Lazy` wrapper (§22.6).

19.7.3 A **thread-local initializer** **MAY** be an arbitrary expression. The initializer runs on first access in each thread, and the thread-local's value is distinct for each thread.

19.7.4 **Static destruction** is the process of dropping `static` items at program exit. Statics with destructors **MUST** be dropped in reverse initialization order, but the initialization order is unspecified, so the destruction order is also unspecified.

19.7.5 A static's destructor **MUST NOT** access another static that has already been destructed. The order is unspecified, so the programmer **MUST NOT** rely on a particular order.

19.7.6 A **`static mut`** has no destructor unless the type implements `Drop`. Accessing a `static mut` in a destructor is an unsafe operation and **MUST** be synchronized.

19.7.7 An implementation **MUST** run static destructors on normal program exit and **MUST NOT** run them on `abort` or on a signal-triggered termination.

---

## 20. Compile-Time Evaluation and Metaprogramming

### 20.1 Constant Evaluation

20.1.1 **Constant evaluation** is the process of computing the value of a constant expression (§8.19) at compile time. The result is a value of the expression's type, computed by the compiler.

20.1.2 The **evaluator** **MUST** implement the semantics of §8 for the operations that are permitted in constant expressions. The evaluator **MUST** be deterministic: the same constant expression **MUST** produce the same value on every compilation.

20.1.3 The evaluator **MUST** reject a constant expression that would panic, overflow (in checked mode), or produce undefined behavior. The rejection **MUST** include the location of the offending operation.

20.1.4 The evaluator **MAY** have a limited **step budget** to prevent non-terminating constant evaluation. An implementation **MUST** reject a constant expression that exceeds the budget with a diagnostic identifying the budget.

20.1.5 The evaluator **MUST NOT** access memory that is not part of a constant expression. In particular, it **MUST NOT** read `static mut` items (except in restricted `const` contexts), allocate memory, or perform I/O.

20.1.6 The evaluator **MAY** use the host's arithmetic for integer operations if the host's behavior matches the target's behavior for the operation. If it does not, the evaluator **MUST** emulate the target's behavior.

20.1.7 The **result** of a constant expression is a value that is substituted at each use site. The value is not allocated at runtime unless the constant is a `static` (§11.3.6).

### 20.2 Compile-Time Functions

20.2.1 A **`const fn`** is a function that can be evaluated at compile time. Its body is restricted to the operations that are permitted in constant expressions (§20.2.3).

20.2.2 A `const fn` **MAY** be called at runtime or at compile time. The compiler **MUST** evaluate it at compile time when its arguments are constant and the context requires a constant; otherwise it **MAY** evaluate it at runtime.

20.2.3 The **permitted operations** in a `const fn` body include:

- literal construction;
- arithmetic, bitwise, comparison, and logical operations on scalar types;
- `if`, `match`, `loop`, and `while` expressions;
- `let` bindings and `return`;
- calls to other `const fn` functions;
- tuple, array, and struct construction;
- field access on constants;
- casts between integer types and between integer and `char`;
- references to `static` items (with restrictions);
- `unsafe` blocks with a restricted set of operations (§20.2.6).

  20.2.4 The **prohibited operations** in a `const fn` body include:

- allocation;
- I/O;
- floating-point operations that are not bit-exact (in the current version);
- calls to non-`const` functions;
- `panic!` with a non-constant message;
- `static mut` access (except in restricted contexts);
- inline assembly;
- `transmute` between types of different sizes.

  20.2.5 A `const fn` **MAY** be generic. Its generic parameters are instantiated at each call, and the const evaluation is performed for each instantiation.

  20.2.6 A `const fn` **MAY** contain `unsafe` blocks, but the operations inside are restricted to the subset that the evaluator can perform. Dereferencing a raw pointer in a `const fn` is permitted if the pointer is derived from a `const` reference or from another constant.

  20.2.7 A `const fn` **MAY** be recursive, subject to the evaluator's step budget (§20.1.4). A recursion that exceeds the budget **MUST** be rejected with a diagnostic.

  20.2.8 A `const fn` **MUST NOT** be declared `async`, because asynchronous execution is not permitted in constant evaluation.

### 20.3 Compile-Time Execution Limits

20.3.1 An implementation **MUST** limit the resources consumed by constant evaluation. The limits **MUST** be documented and **MUST** be sufficient for ordinary constant expressions.

20.3.2 The **step budget** limits the number of evaluation steps. An implementation **MUST** provide a budget that is sufficient for typical compile-time computations; a budget of less than 10^6 steps **SHOULD** be rejected as too small.

20.3.3 The **memory budget** limits the amount of memory that constant evaluation may consume. An implementation **MUST** provide a budget that is sufficient for typical constant expressions.

20.3.4 The **recursion depth** limits the depth of `const fn` recursion. An implementation **MUST** provide a limit that is sufficient for typical recursive constant expressions.

20.3.5 An implementation **MUST** reject a constant expression that exceeds any of the limits with a diagnostic identifying the limit and the location of the expression. The diagnostic **SHOULD** suggest how the expression can be rewritten to fit within the limit.

20.3.6 An implementation **MAY** provide a mechanism for the program to increase the limits, subject to the implementation's maximum. The mechanism **MUST** be explicit and **MUST NOT** be enabled by default.

### 20.4 Macros and Expansion

20.4.1 A **macro** is a compile-time function that takes a token sequence and produces a token sequence. Macros are expanded before name resolution and type checking (§3.5.1).

20.4.2 The **`macro_rules!`** form declares a declarative macro with pattern-based expansion:

```
macro-rules ::= "macro_rules!" ident "{" macro-rule {macro-rule} "}"
macro-rule  ::= "(" matcher ")" "=>" "{" transcriber "}" ";"?
```

The `matcher` is a pattern that matches a token sequence; the `transcriber` is the replacement token sequence. The matcher may bind fragments (identifiers, expressions, types, etc.) that are substituted into the transcriber.

20.4.3 A **macro invocation** is written `name!(tokens)` or `name![tokens]` or `name!{tokens}`. The delimiters are interchangeable; an implementation **SHOULD** preserve the delimiters in the expanded output.

20.4.4 **Hygiene**: a macro's expansion **MUST NOT** accidentally capture names from the call site. A name introduced by the macro's transcriber is distinct from a name with the same spelling at the call site, unless the name is explicitly passed as a fragment. An implementation **MUST** implement hygiene for `macro_rules!` macros.

20.4.5 A **fragment specifier** in a matcher binds a sub-sequence of tokens to a fragment type: `$x:expr`, `$t:ty`, `$i:ident`, `$p:pat`, `$s:stmt`, `$b:block`, `$l:lifetime`, `$v:vis`, `$m:meta`, `$tt:tt`. The fragment type determines how the bound tokens are parsed and how they may be used in the transcriber.

20.4.6 A **repetition** in a matcher `$( ... )*`, `$( ... )+`, or `$( ... )?` matches zero or more, one or more, or zero or one occurrence of the inner pattern. The transcriber uses the corresponding repetition to emit the matched tokens, with `$( ... )*` or `$( ... )+` for each repetition.

20.4.7 A **macro's expansion** is inserted at the invocation site and then processed by the normal compilation phases. A macro that produces an invalid token sequence **MUST** be rejected with a diagnostic at the expansion site.

20.4.8 A **macro's name** is bound in the macro namespace (§5.1.1). A macro **MAY** share its name with a type or value, because they are in different namespaces.

20.4.9 A **macro recursion** **MUST** terminate. An implementation **MUST** limit the depth of macro expansion and **MUST** reject an expansion that exceeds the limit.

### 20.5 Procedural Macros / Plugins

20.5.1 A **procedural macro** is a Rust-like function that runs at compile time and produces token sequences. Procedural macros are more powerful than declarative macros because they can perform arbitrary computation.

20.5.2 A **function-like procedural macro** is invoked as `name!(tokens)`. It receives the tokens as input and returns a token sequence as output.

20.5.3 An **attribute macro** is invoked as `#[name(tokens)]` on an item. It receives the item's tokens and returns a replacement token sequence. The attribute macro **MAY** modify or replace the item.

20.5.4 A **derive macro** is invoked as `#[derive(Name)]` on a struct or enum. It receives the item's tokens and returns a token sequence that is appended to the item's module. Derive macros are commonly used to implement traits automatically.

20.5.5 A procedural macro **MUST** be defined in a `proc-macro` crate. The crate's `Cargo.toml` **MUST** specify `[lib] proc-macro = true`.

20.5.6 A procedural macro **MUST** be deterministic: the same input **MUST** produce the same output on every invocation. A procedural macro that produces different output on different runs is a defect.

20.5.7 A procedural macro **MUST NOT** access the filesystem, network, or other external state during expansion, except through explicit input tokens. An implementation **MAY** sandbox procedural macros to enforce this restriction.

20.5.8 A procedural macro **MUST** be registered in the crate's `proc-macro` section and **MUST** be exported with `#[proc_macro]`, `#[proc_macro_attribute]`, or `#[proc_macro_derive]`.

20.5.9 A procedural macro's **expansion** is subject to the same hygiene rules as declarative macros, but procedural macros **MAY** opt out of hygiene for specific identifiers if they document why.

### 20.6 Reflection and Introspection

20.6.1 Forge does **not** provide runtime reflection. A program **MUST NOT** inspect its own types, fields, or methods at runtime.

20.6.2 Compile-time introspection is available through **procedural macros** (§20.5), which receive the item's tokens and can inspect its structure. A procedural macro **MAY** generate code that depends on the item's fields, variants, or methods.

20.6.3 The **`TypeId`** type (§22.4) provides a runtime identifier for a type. Two types are the same if and only if they have the same `TypeId`. `TypeId` **MUST NOT** be used to inspect a type's structure, only to compare types for identity.

20.6.4 The **`Any`** trait (§22.4) provides a mechanism for downcasting a trait object to a concrete type. `Any::downcast_ref::<T>()` returns `Some(&T)` if the trait object's concrete type is `T`, and `None` otherwise.

20.6.5 `TypeId` and `Any` **MUST** be limited to `'static` types. A type that borrows non-`'static` data **MUST NOT** be used with `TypeId` or `Any`, because the type's identity may not be stable across the borrow's lifetime.

20.6.6 An implementation **MUST** provide `TypeId` and `Any` with the semantics specified in §22.4. An implementation **MUST NOT** use reflection to expose a type's private fields or methods.

### 20.7 Conditional Compilation and Configuration

20.7.1 **Conditional compilation** is specified in §11.10. This section specifies its interaction with macros and procedural macros.

20.7.2 A **`#[cfg(...)]`** attribute on a macro invocation **MUST** be evaluated before the macro is expanded. If the predicate is `false`, the invocation is removed and the macro is not called.

20.7.3 A **`cfg!`** macro expands to `true` or `false` depending on the configuration. It is used in expression position, and its expansion is a boolean literal.

20.7.4 A **`#[cfg_attr(predicate, attr)]`** applies `attr` only if `predicate` is true. It is used to conditionally apply attributes to items and macros.

20.7.5 The **`--cfg`** command-line flag sets a custom configuration atom. A program **MUST NOT** rely on a custom atom being set unless the manifest requires it.

20.7.6 The **`#[cfg_attr]`** on a procedural macro **MUST** be evaluated before the macro is expanded. The macro sees the item with the conditional attribute applied or removed.

20.7.7 An implementation **MUST** evaluate `cfg` predicates consistently: the same predicate **MUST** produce the same value throughout a compilation.

### 20.8 Attributes and Pragmas

20.8.1 An **attribute** is a compile-time annotation written `#[...]` on an item or `#![...]` at module scope. Attributes are processed by the compiler and by procedural macros.

20.8.2 The **built-in attributes** are specified in Chapter 23. A user-defined attribute **MUST** be a procedural macro (§20.5) or **MUST** be registered with the compiler.

20.8.3 An **attribute's arguments** are a token sequence inside the parentheses. The arguments' syntax is determined by the attribute; a built-in attribute **MUST** have the syntax specified in Chapter 23.

20.8.4 The **`#[doc = "..."]`** attribute documents an item. The documentation is available to tools (such as documentation generators) and **MUST** be preserved by the compiler.

20.8.5 The **`#[allow(...)]`** and **`#[warn(...)]`** attributes control lints (§24.3). They **MAY** be applied at module scope, item scope, or statement scope.

20.8.6 The **`#[deprecated]`** attribute marks an item as deprecated. Uses of the item **MUST** produce a warning under the `deprecated` lint (§24.3).

20.8.7 The **`#[must_use]`** attribute marks a value that must be used. A discarded value of a `#[must_use]` type **MUST** produce a warning under the `unused_must_use` lint (§24.3).

20.8.8 A **pragma** is a compiler-specific directive that is not part of the language's grammar. Pragmas are accessed through attributes and **MUST** be documented by the implementation.

---

## 21. Error Handling and Runtime Behavior

### 21.1 Recoverable Errors

21.1.1 A **recoverable error** is an error that the program can handle and continue from. Recoverable errors are represented by the `Result<T, E>` type (§22.7), which is an enum with variants `Ok(T)` and `Err(E)`.

21.1.2 A function that can fail **SHOULD** return `Result<T, E>` rather than aborting or panicking. The error type `E` **SHOULD** implement the `Error` trait (§22.7).

21.1.3 The **`?` operator** (§8.5.3) propagates an error from a function call: if the call returns `Err(e)`, the `?` operator returns `Err(e.into())` from the enclosing function; if `Ok(v)`, it evaluates to `v`.

21.1.4 The `?` operator **MUST** be used only in a function that returns `Result<T, E>` or `Option<T>`. A `?` in a function with a different return type **MUST** produce a compile-time error.

21.1.5 The **`From<E1> for E2`** conversion is applied by the `?` operator when the error types differ. The conversion **MUST** be infallible; a failing conversion **MUST NOT** be performed by `?`.

21.1.6 An **error's source** is the error that caused it. The `Error::source` method (§22.7) returns the source as an `Option<&dyn Error>`. The source chain **MUST NOT** contain cycles.

21.1.7 An error **MUST NOT** be silently ignored. A `Result` whose value is discarded **MUST** produce a warning under the `unused_must_use` lint (§24.3), unless the `Result` is explicitly discarded with `let _ = ...`.

### 21.2 Unrecoverable Errors and Panics

21.2.1 A **panic** is an unrecoverable error that aborts the current thread or the program. A panic is triggered by `panic!`, by an operation that the language requires to panic (such as out-of-bounds indexing), or by `unwrap` on a `None` or `Err`.

21.2.2 The **`panic!`** macro has the forms:

```
panic!()
panic!("message")
panic!("format", args...)
```

The message and arguments are formatted into a string that is reported to the panic handler.

21.2.3 The **panic strategy** is either `unwind` or `abort`. The strategy is selected at compile time and **MUST** be consistent across the whole program. A program that mixes strategies **MUST** be rejected at link time.

21.2.4 With the **`unwind`** strategy, a panic unwinds the current thread's stack, running destructors, and is caught by `catch_unwind` (§21.4) or terminates the thread. The program continues if other threads are running.

21.2.5 With the **`abort`** strategy, a panic calls `abort`, which terminates the program without unwinding. Destructors do **NOT** run. The abort is reported to the operating system.

21.2.6 A panic **MUST NOT** cross an FFI boundary unless the ABI is `"C-unwind"` (§21.4.4). A panic in an `extern "C"` function **MUST** abort the program.

21.2.7 The **panic handler** is a function that is called when a panic occurs. The default handler prints a message and aborts. A custom handler **MAY** be installed with `#[panic_handler]` in a `#![no_std]` program (§3.7.2).

21.2.8 The **`panic::catch_unwind`** function (§22.7) catches a panic and returns a `Result` indicating success or failure. The closure passed to `catch_unwind` **MUST** be `UnwindSafe`; the programmer **MUST** ensure that the closure does not leave shared state inconsistent.

### 21.3 Abort and Termination

21.3.1 **`abort`** terminates the program immediately, without unwinding or running destructors. `abort` is implemented by the standard library as `std::process::abort`.

21.3.2 **`exit`** terminates the program with a specified exit code, running static destructors but not unwinding. `exit` is implemented by the standard library as `std::process::exit`.

21.3.3 A **signal** may terminate the program externally. The default behavior for most signals is to terminate the program without running destructors. A program **MAY** install a signal handler to catch specific signals, but the handler **MUST** be async-signal-safe.

21.3.4 An **unhandled panic** with the `unwind` strategy terminates the thread, not the program, unless the panicking thread is the main thread. When the main thread panics, the program terminates with exit code `101`.

21.3.5 An **unhandled panic** with the `abort` strategy terminates the program immediately. The exit code is implementation-defined but **MUST** be nonzero.

21.3.6 The **`std::process::Termination`** trait (§22.7) allows a function returning a custom type to be used as an entry point. The trait's `report` method returns an `ExitCode`.

21.3.7 A program **SHOULD** return exit code `0` for success and a nonzero code for failure. The convention for specific nonzero codes is implementation-defined.

### 21.4 Unwinding

21.4.1 **Unwinding** is the process of propagating a panic up the call stack, running destructors for each frame. Unwinding is enabled by the `unwind` panic strategy (§21.2.4).

21.4.2 A **drop during unwinding** **MUST** run the destructor. If the destructor itself panics, the program **MUST** abort, because a double panic is not recoverable.

21.4.3 An **unwinding across a frame** **MUST** run the destructors of the frame's locals in reverse declaration order. The unwinding **MUST NOT** run the frame's `return` expression or its tail expression.

21.4.4 The **`"C-unwind"`** ABI permits unwinding across the FFI boundary. A foreign function with this ABI **MUST** support unwinding in its language; a foreign function with `"C"` that unwinds into Forge is undefined behavior.

21.4.5 A **catch** of a panic is performed by `catch_unwind`, which catches the panic and returns it as a `Box<dyn Any + Send>`. The programmer **MAY** inspect the panic value or re-raise it.

21.4.6 An **abort during unwinding** occurs when the unwinding would cross a frame that cannot be unwound (such as a `#[no_mangle]` function without `"C-unwind"`). The program **MUST** abort with a diagnostic.

21.4.7 The **unwinding table** is the data structure that describes how to unwind each frame. It is generated by the compiler and **MUST** be preserved in the object file.

### 21.5 Runtime Hooks

21.5.1 The **panic hook** is a function that is called when a panic occurs, before the panic handler. The default hook prints a backtrace to stderr. A custom hook **MAY** be installed with `panic::set_hook` (§22.7).

21.5.2 The **allocator hook** is the function that the standard library calls to allocate and deallocate memory. The default hook uses the system allocator; a program **MAY** install a custom allocator with `#[global_allocator]` (§22.4).

21.5.3 The **oom hook** is called when the allocator fails to allocate memory. The default oom hook aborts the program. A custom hook **MUST** not return; it **MUST** abort or terminate the program.

21.5.4 The **thread hook** is called when a thread starts or exits. It **MAY** be used for thread-local bookkeeping. The hook **MUST** be thread-safe.

21.5.5 An implementation **MUST** provide the standard hooks and **MUST** allow a program to install custom hooks. The hooks **MUST NOT** be called during the initialization of the runtime that provides them.

### 21.6 Allocator Interface

21.6.1 The **`Allocator`** trait (§22.4) is the interface for memory allocation. Its `allocate` method allocates a block of memory with a given layout; its `deallocate` method deallocates a previously allocated block.

21.6.2 The **global allocator** is the allocator used by `Box`, `Vec`, `String`, and other standard library types. It is specified by the `#[global_allocator]` attribute on a `static` that implements `Allocator`.

21.6.3 The **`GlobalAlloc`** trait (§22.4) is a lower-level allocator interface that does not require an allocator instance. It is implemented by the global allocator and is used by the standard library's default allocator.

21.6.4 An allocator **MUST** return a pointer that is aligned to the layout's alignment and non-null. A failed allocation **MUST** return a null pointer (for `GlobalAlloc`) or `Err(AllocError)` (for `Allocator`).

21.6.5 An allocator's `deallocate` method **MUST** accept a pointer and layout that were returned by the allocator's `allocate` method. A deallocation with a different layout is undefined behavior.

21.6.6 A custom allocator **MUST** be thread-safe. It **MAY** be implemented with a global lock, a lock-free data structure, or per-thread arenas.

21.6.7 A `#![no_std]` program **MUST** provide a global allocator if it uses any allocating type. A `#![no_std]` program that does not use allocation **MAY** omit the allocator.

### 21.7 Program Exit

21.7.1 A program **MUST** exit normally when its entry point returns. The exit code is `0` for a normal return, or the error code from `Result::Err` (§3.4.2).

21.7.2 A program **MAY** exit explicitly with `std::process::exit(code)`. The exit code is the specified code; static destructors run before the process terminates.

21.7.3 A program **MAY** exit by `abort`, which terminates the process without running destructors. The exit code is implementation-defined but **MUST** be nonzero.

21.7.4 A program **MUST** exit when all threads have terminated (joined or detached and finished). A program with a detached thread that never terminates **MAY** hang indefinitely; the programmer **SHOULD** ensure that detached threads terminate.

21.7.5 **Static destructors** **MUST** run on normal exit and on `exit`. They **MUST NOT** run on `abort`, on a panic during unwinding, or on a signal-triggered termination.

21.7.6 The **order** of static destructors is the reverse of the order of static constructors, but the order of static constructors is unspecified (§19.7.4). A static destructor **MUST NOT** rely on a specific destructor order.

21.7.7 An implementation **MUST** flush the standard output and standard error streams on normal exit. On `abort` or signal-triggered termination, the streams **MAY** be lost.

## Part V — Standard Language Environment

## 22. Standard Library Boundary

### 22.1 Core Language vs Standard Library

22.1.1 The **core language** consists of the constructs specified in Parts I through IV: the type system, the ownership model, the expression and statement grammar, the item and trait system, and the unsafe facilities. A conforming implementation **MUST** support the core language without any library.

22.1.2 The **standard library** is a set of crates distributed with the toolchain that provides types and functions built on the core language. The standard library is **not** part of the language grammar, but its types and traits are normative for programs that use them.

22.1.3 The standard library is organized into three layers:

| Layer    | Crate   | Contents                                                             |
| -------- | ------- | -------------------------------------------------------------------- |
| Core     | `core`  | Types and traits that require no operating system or allocator       |
| Alloc    | `alloc` | Types that require a global allocator (`Box`, `Vec`, `String`, etc.) |
| Standard | `std`   | The full library, including I/O, threads, and OS abstractions        |

22.1.4 The `core` crate **MUST** be available in every environment, including `#![no_std]` freestanding environments (§3.7.3). The `alloc` crate **MUST** be available whenever a global allocator is provided. The `std` crate **MUST** be available in a hosted environment (§3.7.1).

22.1.5 The `std` crate **MUST** re-export the `core` and `alloc` crates, and `alloc` **MUST** re-export `core`. A name that is available in `core` **MUST** be available under the same path in `std` (for example, `core::mem::swap` and `std::mem::swap` name the same item).

22.1.6 The core prelude (§22.2) **MUST** include the items of the `core` crate that are required by the language itself. The standard prelude (§22.2) **MUST** extend the core prelude with the items of `std` that are commonly used in ordinary programs.

22.1.7 An implementation **MUST** document the version of the standard library that it provides. The standard library's version **MUST** be the toolchain's version (§28.1).

### 22.2 Core Prelude

22.2.1 The **core prelude** is the set of names that are in scope in every module without an explicit `use` (§15.5.1). The core prelude **MUST** contain at least the following:

**Types:**

- `bool`, `char`, `str`, `()`
- `i8`, `i16`, `i32`, `i64`, `i128`, `isize`
- `u8`, `u16`, `u32`, `u64`, `u128`, `usize`
- `f32`, `f64`
- `Option<T>`, `Result<T, E>`
- `Clone`, `Copy`, `Drop`, `Sized`

**Traits:**

- `PartialEq`, `Eq`, `PartialOrd`, `Ord`
- `Add`, `Sub`, `Mul`, `Div`, `Rem`, `Neg`
- `BitAnd`, `BitOr`, `BitXor`, `Not`, `Shl`, `Shr`
- `AddAssign`, `SubAssign`, `MulAssign`, `DivAssign`, `RemAssign`
- `BitAndAssign`, `BitOrAssign`, `BitXorAssign`, `ShlAssign`, `ShrAssign`
- `Index`, `IndexMut`
- `Fn`, `FnMut`, `FnOnce`
- `Iterator`, `IntoIterator`
- `From`, `Into`
- `TryFrom`, `TryInto`
- `AsRef`, `AsMut`
- `Borrow`, `BorrowMut`
- `Send`, `Sync`

**Macros:**

- `panic!`, `assert!`, `assert_eq!`, `assert_ne!`
- `debug_assert!`, `debug_assert_eq!`, `debug_assert_ne!`
- `unreachable!`, `todo!`, `unimplemented!`
- `write!`, `writeln!`, `format!`

  22.2.2 A name in the core prelude **MUST NOT** be shadowed by a local item without an explicit `use` or definition. The shadowing is a **deprecation warning**, not an error, under the `shadowed_prelude` lint (§24.3).

  22.2.3 A `#![no_std]` crate **MUST** have the core prelude available; a `#![no_implicit_prelude]` crate **MUST NOT** have any prelude, and every name must be explicitly imported (§23.1.6).

  22.2.4 The core prelude **MUST** be stable across recompilations of the same source with the same toolchain version. The contents of the prelude **MAY** change between toolchain versions, subject to §28.3.

### 22.3 Platform Abstraction Layer

22.3.1 The **platform abstraction layer** is the part of the standard library that provides operating system facilities. It **MUST** be implemented on every supported target and **MUST** provide a consistent API across platforms where the underlying facilities allow.

22.3.2 The platform abstraction layer provides:

- **Memory:** allocation and deallocation through the `Allocator` trait (§22.4);
- **I/O:** files, streams, and the standard input/output/error streams;
- **Process:** process creation, environment variables, and exit codes;
- **Time:** clocks, durations, and sleeping;
- **Threads:** thread creation, joining, and synchronization (§22.6);
- **Paths:** platform-specific path manipulation;
- **Networking:** sockets and addresses (where supported).

  22.3.3 The platform abstraction layer **MUST NOT** be available in a `#![no_std]` environment unless the environment provides the necessary OS facilities. A `#![no_std]` program that uses the platform abstraction layer **MUST** provide its own implementation.

  22.3.4 The platform abstraction layer's API **MUST** be documented for each supported target. The behavior of a function that is not available on a target **MUST** be specified as either an error at compile time or a runtime panic, not silently absent.

  22.3.5 A **platform-specific** module (for example, `std::os::unix` or `std::os::windows`) **MUST** be available only on the corresponding target. A use of a platform-specific module on the wrong target **MUST** be rejected at compile time.

  22.3.6 The **`std::io`** module provides the `Read`, `Write`, `Seek`, and `BufRead` traits (§22.5). These traits **MUST** be implemented for the standard streams and for files.

  22.3.7 The **`std::path`** module provides `Path` and `PathBuf`, which represent platform-native paths. The representation of a path **MUST** be the platform's native representation; the API **MUST** hide the differences between `/` and `\` separators.

### 22.4 Allocator and Memory APIs

22.4.1 The **`Allocator`** trait is the primary interface for memory allocation:

```forge
pub unsafe trait Allocator {
    fn allocate(&self, layout: Layout) -> Result<NonNull<[u8]>, AllocError>;
    unsafe fn deallocate(&self, ptr: NonNull<u8>, layout: Layout);
    fn allocate_zeroed(&self, layout: Layout) -> Result<NonNull<[u8]>, AllocError>;
    unsafe fn grow(&self, ptr: NonNull<u8>, old: Layout, new: Layout)
        -> Result<NonNull<[u8]>, AllocError>;
    unsafe fn shrink(&self, ptr: NonNull<u8>, old: Layout, new: Layout)
        -> Result<NonNull<[u8]>, AllocError>;
}
```

22.4.2 The **`GlobalAlloc`** trait is a lower-level interface used by the global allocator:

```forge
pub unsafe trait GlobalAlloc {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8;
    unsafe fn dealloc(&self, ptr: *mut u8, layout: Layout);
    unsafe fn alloc_zeroed(&self, layout: Layout) -> *mut u8;
    unsafe fn realloc(&self, ptr: *mut u8, layout: Layout, new_size: usize) -> *mut u8;
}
```

22.4.3 The **`Layout`** type describes a memory block's size and alignment. It **MUST** be constructible only with a power-of-two alignment, and its size **MUST** be a multiple of the alignment for `alloc`-allocated blocks.

22.4.4 The **`Box<T>`** type is the standard owning pointer. It allocates its value on the heap and deallocates it when dropped. `Box<T>` **MUST** be `Send` and `Sync` if `T` is `Send` and `Sync`.

22.4.5 The **`Vec<T>`** type is a growable array. It **MUST** provide amortized O(1) push and pop, O(1) indexing, and O(n) iteration. Its capacity **MUST** grow geometrically.

22.4.6 The **`String`** type is a growable UTF-8 string. It **MUST** provide the same operations as `Vec<u8>` restricted to valid UTF-8.

22.4.7 The **`Rc<T>`** and **`Arc<T>`** types are reference-counted pointers. `Rc<T>` uses non-atomic reference counting and is **not** `Send` or `Sync`; `Arc<T>` uses atomic reference counting and is `Send` and `Sync` if `T` is `Send` and `Sync`.

22.4.8 The **`Cell<T>`** and **`RefCell<T>`** types provide interior mutability. `Cell<T>` permits mutation through a shared reference for `Copy` types; `RefCell<T>` provides runtime-checked borrowing for non-`Copy` types.

22.4.9 The **`UnsafeCell<T>`** type is the primitive that enables interior mutability. It **MUST** be the only way for a type to opt out of the aliasing rules of §7.5.2.

22.4.10 The **`MaybeUninit<T>`** type represents a possibly uninitialized value. It **MUST** be used instead of `mem::uninitialized` for any value that may not be initialized; reading an uninitialized `MaybeUninit` is undefined behavior.

22.4.11 The **`ManuallyDrop<T>`** type prevents the automatic drop of its inner value. It **MUST** be used when a value's drop is managed manually, and the programmer **MUST** ensure the value is dropped exactly once.

22.4.12 The **`mem`** module provides memory operations: `size_of`, `align_of`, `swap`, `replace`, `take`, `forget`, `transmute` (unsafe), `read_unaligned`, and `write_unaligned`.

22.4.13 The **`ptr`** module provides raw pointer operations: `read`, `write`, `copy`, `copy_nonoverlapping`, `swap`, `null`, `null_mut`, and the `NonNull<T>` type.

22.4.14 The **`alloc::alloc`** module provides the `alloc`, `dealloc`, `realloc`, and `alloc_zeroed` functions for direct use with the global allocator.

### 22.5 I/O and OS Interfaces

22.5.1 The **`std::io`** module provides the `Read`, `Write`, `Seek`, and `BufRead` traits. Each **MUST** be implemented for files, sockets, and the standard streams.

22.5.2 The **`Read`** trait provides `read(&mut self, buf: &mut [u8]) -> io::Result<usize>`. It **MUST** return the number of bytes read, which **MAY** be less than `buf.len()`. A return of `0` indicates end-of-file.

22.5.3 The **`Write`** trait provides `write(&mut self, buf: &[u8]) -> io::Result<usize>` and `flush(&mut self) -> io::Result<()>`. It **MUST** return the number of bytes written, which **MAY** be less than `buf.len()`.

22.5.4 The **`BufRead`** trait provides buffered reading, including `read_line` and `lines`. It **MUST** be implemented for `BufReader<R>` and for `std::io::stdin`.

22.5.5 The **standard streams** are `io::stdin()`, `io::stdout()`, and `io::stderr()`. They **MUST** be available in a hosted environment and **MUST** be line-buffered (`stdout`) or unbuffered (`stderr`) by default.

22.5.6 The **`fs`** module provides file operations: `File::open`, `File::create`, `File::read_to_string`, `fs::read`, `fs::write`, and `fs::remove_file`. These **MUST** be implemented on every hosted target.

22.5.7 The **`env`** module provides access to command-line arguments and environment variables. `env::args()` **MUST** return an iterator over the program's arguments; `env::var(name)` **MUST** return the value of an environment variable or an error.

22.5.8 The **`process`** module provides process creation and management. `Command` **MUST** support spawning a child process, waiting for it, and capturing its standard streams.

22.5.9 The **`net`** module provides TCP and UDP sockets. It **MUST** be available on targets that support networking and **MUST** be rejected at compile time on targets that do not.

22.5.10 The **`time`** module provides `Duration`, `Instant`, and `SystemTime`. `Instant` **MUST** be monotonic; `SystemTime` **MUST** reflect the system clock and **MAY** go backwards.

22.5.11 The **`c_void`** type is an opaque type that represents C's `void`. It **MUST** be defined in `std::ffi` (and `core::ffi`) and **MUST** be FFI-safe (§18.6.1).

22.5.12 The **`CStr`** and **`CString`** types represent null-terminated C strings. They **MUST** be FFI-safe and **MUST** provide conversion to and from Forge strings.

22.5.13 The **`OsStr`** and **`OsString`** types represent platform-native strings (which may not be valid UTF-8). They **MUST** be usable in path and environment operations.

### 22.6 Concurrency APIs

22.6.1 The **`std::thread`** module provides thread creation and joining. `thread::spawn` **MUST** accept a closure that is `Send + 'static` and **MUST** return a `JoinHandle`.

22.6.2 The **`thread::scope`** function **MUST** create a scope in which threads may borrow from the parent stack. The scope **MUST NOT** return until all spawned threads have joined.

22.6.3 The **`thread::sleep`** function **MUST** suspend the current thread for at least the specified duration. It **MUST NOT** spin.

22.6.4 The **`thread::yield_now`** function **MUST** hint to the scheduler that the current thread is willing to yield its time slice. It **MUST NOT** block.

22.6.5 The **`sync`** module provides `Mutex`, `RwLock`, `Condvar`, `Barrier`, `Semaphore`, `Once`, `OnceCell`, `OnceLock`, and `mpsc::channel`. Each **MUST** implement the synchronization semantics specified in §19.4.

22.6.6 The **`atomic`** module provides the atomic types (§19.3) and the `Ordering` enum (§19.2). Each atomic operation **MUST** accept an `Ordering` and **MUST** implement the memory model of §19.2.

22.6.7 The **`Mutex<T>`** type **MUST** provide `lock()` returning a `LockResult<MutexGuard<T>>` and `try_lock()` returning a `TryLockResult<MutexGuard<T>>`. The guard **MUST** release the lock when dropped.

22.6.8 The **`RwLock<T>`** type **MUST** provide `read()` and `write()` returning guards that release the lock when dropped. Multiple readers **MAY** hold the lock simultaneously; a writer **MUST** exclude both readers and other writers.

22.6.9 The **`Condvar`** type **MUST** provide `wait(guard) -> LockResult<MutexGuard<T>>` and `notify_one()` / `notify_all()`. It **MUST** atomically release the mutex while waiting and re-acquire it when awakened.

22.6.10 The **`mpsc::channel`** function **MUST** return a `(Sender<T>, Receiver<T>)` pair. `Sender` **MUST** be `Clone` and `Send`; `Receiver` **MUST** be `Send`. Sending a value **MUST** move it into the channel.

22.6.11 The **`Future`** trait is the basis for asynchronous programming. A `Future` **MUST** provide `poll(self: Pin<&mut Self>, cx: &mut Context) -> Poll<T>`. The `Poll` type **MUST** be either `Ready(T)` or `Pending`.

22.6.12 The **`async fn`** and **`async` blocks** **MUST** be transformed into types implementing `Future` (§12.4.5). The transformation **MUST** preserve the body's evaluation order and side effects.

### 22.7 Error Types

22.7.1 The **`Error`** trait is the standard interface for error types:

```forge
pub trait Error: Debug + Display {
    fn source(&self) -> Option<&(dyn Error + 'static)> { None }
    fn backtrace(&self) -> Option<&Backtrace> { None }
}
```

22.7.2 Every type returned as the `E` parameter of `Result<T, E>` **SHOULD** implement `Error`. A type that implements `Error` **MUST** also implement `Debug` and `Display`.

22.7.3 The **`io::Error`** type is the standard error for I/O operations. It **MUST** implement `Error` and **MUST** provide an `io::ErrorKind` that classifies the error.

22.7.4 The **`ErrorKind`** enum **MUST** include at least: `NotFound`, `PermissionDenied`, `ConnectionRefused`, `ConnectionReset`, `ConnectionAborted`, `NotConnected`, `AddrInUse`, `AddrNotAvailable`, `BrokenPipe`, `AlreadyExists`, `WouldBlock`, `InvalidInput`, `InvalidData`, `TimedOut`, `WriteZero`, `Interrupted`, `Other`.

22.7.5 The **`Box<dyn Error>`** type is the canonical dynamic error type. A function that returns an arbitrary error **MAY** return `Box<dyn Error>` and use the `?` operator to convert any `Error` into it.

22.7.6 The **`From`** conversion for errors **MUST** be infallible; a conversion that can fail **MUST NOT** be used with the `?` operator.

22.7.7 The **`Termination`** trait allows a type to be used as a program entry point (§21.3.6). Its `report` method **MUST** return an `ExitCode` that is nonzero if the program failed.

---

## 23. Attributes and Directives

### 23.1 Built-in Attributes

23.1.1 The **built-in attributes** are recognized by the compiler and have semantics specified in this chapter. A user-defined attribute **MUST** be a procedural macro (§20.5) or **MUST** be registered with the compiler.

23.1.2 A **doc comment** (`///` or `//!`) is lowered to a `#[doc = "..."]` attribute. The attribute **MUST** be preserved for documentation generators and **MUST NOT** affect code generation.

23.1.3 The **`#[derive(...)]`** attribute on a struct or enum requests automatic implementation of the listed traits. The supported derives **MUST** include at least `Clone`, `Copy`, `Debug`, `PartialEq`, `Eq`, `PartialOrd`, `Ord`, `Hash`, and `Default`.

23.1.4 The **`#[repr(...)]`** attribute is specified in §17.3. It controls the layout of structs, enums, and unions.

23.1.5 The **`#[path = "..."]`** attribute on a `mod` declaration overrides the module file's path. The path is relative to the directory containing the file that contains the `mod` declaration.

23.1.6 The **`#![no_std]`** attribute on the crate root disables the standard prelude (§15.5.4). The **`#![no_implicit_prelude]`** attribute disables all preludes, requiring explicit imports for every name.

23.1.7 The **`#![feature(...)]`** attribute enables unstable language features. A program that uses a feature without enabling it **MUST** be rejected with a diagnostic.

23.1.8 The **`#[global_allocator]`** attribute on a static specifies the program's global allocator (§21.6.2). The static's type **MUST** implement `GlobalAlloc`.

23.1.9 The **`#[panic_handler]`** attribute on a function specifies the program's panic handler (§21.2.7). The attribute is permitted only in a `#![no_std]` program.

23.1.10 The **`#[start]`** attribute on a function specifies the program's entry point in a freestanding environment (§3.4.5).

23.1.11 The **`#[test]`** attribute marks a function as a test case. The test harness **MUST** call the function and report its result.

23.1.12 The **`#[bench]`** attribute marks a function as a benchmark. The benchmark harness **MUST** call the function repeatedly and report timing statistics.

23.1.13 The **`#[proc_macro]`**, **`#[proc_macro_attribute]`**, and **`#[proc_macro_derive]`** attributes declare procedural macros (§20.5). They are permitted only in a `proc-macro` crate.

### 23.2 Representation Attributes

23.2.1 The **representation attributes** are a subset of `#[repr(...)]` that control layout. They are specified in §17.3. A `#[repr(...)]` attribute **MUST** be one of the forms listed in §17.3.1.

23.2.2 The **`#[repr(C)]`** attribute specifies C-compatible layout (§17.3.2). It **MUST** be honored for FFI types and **MUST** be the default for types declared in `extern` blocks.

23.2.3 The **`#[repr(transparent)]`** attribute specifies that a struct has the same layout as its single field (§17.3.3). It **MUST NOT** be applied to a struct with zero fields or with more than one non-zero-sized field.

23.2.4 The **`#[repr(align(N))]`** attribute increases a type's alignment to at least `N` (§17.3.4). It **MUST NOT** be used with `#[repr(packed)]`.

23.2.5 The **`#[repr(packed)]`** attribute reduces a struct's alignment to `1` (§17.3.5). It **MUST NOT** be used with `#[repr(align(N))]` and **MUST NOT** be applied to a union or an enum.

23.2.6 The **integer representation attributes** (`#[repr(u8)]`, `#[repr(i32)]`, etc.) specify an enum's discriminant type (§17.3.7). They **MUST** be large enough to hold all variants' discriminants.

23.2.7 The **`#[repr(simd)]`** attribute requests a SIMD vector representation (§17.3.6). Its availability is target-specific; an implementation **MUST** reject it on targets without SIMD support.

### 23.3 Optimization and Codegen Attributes

23.3.1 The **`#[inline]`**, **`#[inline(always)]`**, and **`#[inline(never)]`** attributes control inlining (§12.7). They **MUST NOT** change the function's semantics.

23.3.2 The **`#[cold]`** attribute marks a function as unlikely to be called, allowing the optimizer to place it in a cold section (§12.7.6). It **MUST NOT** change the function's behavior.

23.3.3 The **`#[track_caller]`** attribute makes the function's caller information available through the `Location` API (§24.5). The function's body **MAY** use `Location::caller()` to obtain the caller's source location.

23.3.4 The **`#[no_mangle]`** attribute disables name mangling (§17.7.2). It **MUST** be applied only to items that have a stable external interface.

23.3.5 The **`#[export_name("...")]`** attribute overrides the exported symbol name (§17.7.5). It **MUST NOT** conflict with another exported symbol.

23.3.6 The **`#[link_name = "..."]`** attribute overrides the symbol name of an external declaration (§17.7.4).

23.3.7 The **`#[link(...)]`** attribute specifies the foreign library to link against (§18.8.2).

23.3.8 The **`#[used]`** attribute prevents the linker from discarding an item (§18.8.6). It is used for exported symbols that foreign code may reference by name.

23.3.9 The **`#[link_section("...")]`** attribute places an item in a specific linker section (§17.8.7). It **MUST** be used only for items whose layout is compatible with the section's requirements.

23.3.10 The **`#[target_feature(enable = "...")]`** attribute enables specific CPU features for a function. The function **MUST NOT** be called on a CPU that does not support the features, unless the caller checks the CPU's capabilities at runtime.

### 23.4 Conditional Compilation Attributes

23.4.1 The **`#[cfg(...)]`** attribute removes an item, statement, or expression from the program if the predicate is `false` (§11.10). It **MUST** be evaluated before name resolution.

23.4.2 The **`#[cfg_attr(predicate, attr)]`** attribute applies `attr` if `predicate` is `true` (§11.10.6). The `attr` **MUST** be a valid attribute.

23.4.3 The **`cfg!`** macro expands to a boolean literal based on the configuration (§20.7.3). It **MAY** be used in expression position.

23.4.4 The **built-in configuration atoms** are specified in §11.10.3. A custom atom **MUST** be declared with `--cfg` or the manifest's `[features]` section.

23.4.5 The **`#[cfg_attr]`** attribute **MUST** be applied before the item is otherwise processed, so that the resulting attribute (if any) is honored.

23.4.6 A **nested `#[cfg(...)]`** inside another `#[cfg(...)]` **MUST** be evaluated as a conjunction: the outer predicate must be `true` **and** the inner predicate must be `true` for the item to be included.

### 23.5 Diagnostic Attributes

23.5.1 The **`#[allow(lint)]`**, **`#[warn(lint)]`**, **`#[deny(lint)]`**, and **`#[forbid(lint)]`** attributes control lints (§24.3). They **MAY** be applied at module, item, or statement scope.

23.5.2 The **`#[must_use]`** attribute marks a value that must be used (§20.8.7). A discarded value of a `#[must_use]` type **MUST** produce a warning under the `unused_must_use` lint.

23.5.3 The **`#[deprecated(since = "...", note = "...")]`** attribute marks an item as deprecated (§23.6). Uses of the item **MUST** produce a warning under the `deprecated` lint.

23.5.4 The **`#[diagnostic::on_unimplemented]`** attribute customizes the error message for a trait bound failure. The message **MAY** reference the type that failed to implement the trait.

23.5.5 The **`#[diagnostic::do_not_recommend]`** attribute suppresses a trait from being suggested as a fix for a bound failure. It is used when the trait's implementation is not the intended fix.

23.5.6 The **`#[rustc_on_unimplemented]`** attribute is an internal attribute used by the standard library to customize error messages. It is not part of the stable language.

### 23.6 Deprecation and Stability Attributes

23.6.1 The **`#[deprecated]`** attribute marks an item as deprecated. The attribute **MAY** specify `since` (the version in which the item was deprecated) and `note` (an explanatory message).

23.6.2 A use of a deprecated item **MUST** produce a warning under the `deprecated` lint. The warning **MUST** identify the item and the replacement, if any.

23.6.3 The **`#[stable(feature = "...", since = "...")]`** attribute marks an item as stable since a specific version. A stable item **MUST NOT** be used in a context that requires an unstable feature.

23.6.4 The **`#[unstable(feature = "...", issue = "...")]`** attribute marks an item as unstable. A use of an unstable item **MUST** be rejected unless the corresponding feature is enabled with `#![feature(...)]`.

23.6.5 The **`#[rustc_const_stable]`** and **`#[rustc_const_unstable]`** attributes mark a function as stable or unstable for const evaluation. They are internal attributes used by the standard library.

23.6.6 An implementation **MUST** provide the `stable`, `unstable`, and `deprecated` attributes and **MUST** enforce their semantics. An implementation **MUST NOT** silently accept an unstable item without the enabling feature.

23.6.7 The **`#[must_use]`** attribute on a trait or a method marks all uses of the trait or method as must-use. It is a shorthand for applying `#[must_use]` to each use site.

### 23.7 User-Defined Attributes

23.7.1 A **user-defined attribute** is an attribute that is not built into the compiler. It **MUST** be registered as a procedural macro (§20.5.3) or **MUST** be ignored under the `unused_attributes` lint.

23.7.2 A user-defined attribute **MUST** be namespaced by its crate to avoid conflicts with built-in attributes and with other crates' attributes. The namespace **MUST** be the crate's name followed by `::`.

23.7.3 A user-defined attribute **MUST NOT** change the observable behavior of the item it annotates, unless the attribute is a procedural macro that expands to a replacement. A passive attribute (one that does not expand) **MAY** be used to convey metadata to tools.

23.7.4 A user-defined attribute's arguments **MUST** be a valid token sequence. The attribute's procedural macro **MUST** parse the arguments according to its specification.

23.7.5 A user-defined attribute **MUST** be applied before the item is otherwise processed, so that the procedural macro receives the item's tokens as written.

23.7.6 An implementation **MUST** reject a user-defined attribute that is not registered as a procedural macro, unless the attribute is a `#[doc]` attribute or is explicitly allowed under a lint.

---

## 24. Diagnostics and Tooling Requirements

### 24.1 Required Errors

24.1.1 A **required error** is a diagnostic that an implementation **MUST** produce for a program that is not conforming. The presence of a required error **MUST** prevent the implementation from producing an executable artifact.

24.1.2 The following are required errors:

- any syntax error (§4, §25);
- any name resolution failure (§5.6.6);
- any type error (§6, §8);
- any borrow-checker rejection (§7.5.4);
- any visibility violation (§5.4.6);
- any unresolved import (§5.5.4);
- any non-exhaustive match (§8.15.4);
- any use of an unsafe operation outside an unsafe block (§16.1.8);
- any use of an unstable feature without the enabling feature (§23.6.4);
- any use of a deprecated item under `#[deny(deprecated)]` (§23.5.3);
- any call to a function whose signature does not match (§8.11.3);
- any use of a type that does not satisfy a bound (§13.5);
- any overlapping or incoherent implementation (§14.9.2);
- any use of a moved value (§7.4.2);
- any use of an uninitialized value (§7.3.1);
- any misuse of a `static mut` outside `unsafe` (§11.3.7);
- any FFI declaration with a non-FFI-safe type without `#[allow(improper_ctypes)]` (§18.6.7);
- any use of a generic function across an ABI boundary (§13.9.1);
- any `#[no_mangle]` conflict (§17.7.2);
- any missing `;` (§4.8.3);
- any `_` in expression position (§10.4.4);
- any `return` outside a function, `break` or `continue` outside a loop (§9.8.5);
- any assignment to an immutable place (§7.7.1);
- any constant expression that would panic, overflow, or produce undefined behavior (§20.1.3);
- any use of a non-`const` operation in a `const fn` (§20.2.4);
- any use of a type parameter that does not satisfy its bounds (§13.5.7);
- any use of a trait object whose trait is not object-safe (§14.8.5).

  24.1.3 An implementation **MUST** produce at least the errors listed in §24.1.2. An implementation **MAY** produce additional errors for programs that are not listed, provided the errors are semantically justified by the specification.

  24.1.4 An implementation **MUST NOT** produce an executable artifact for a program that has any required error, regardless of whether the error is in a code path that would not be executed.

### 24.2 Required Warnings

24.2.1 A **required warning** is a diagnostic that an implementation **MUST** produce for a program that is likely incorrect. A required warning **MUST NOT** prevent the production of an executable artifact unless promoted to an error by a lint configuration (§24.4).

24.2.2 The following are required warnings:

- unreachable code (§9.11.3);
- an unreachable match arm (§8.15.5);
- an unused import (§5.5.5);
- an unused variable (§9.3);
- an unused parameter (§12.2);
- a discarded `#[must_use]` value (§20.8.7);
- a deprecated item used without `#[allow(deprecated)]` (§23.5.3);
- a shadowed prelude item (§22.2.2);
- an identifier that is not in NFC form (§4.5.4);
- a `#[no_std]` program that uses the standard library (§15.5.4);
- an unused `unsafe` block (§16.1.9);
- an unused generic parameter (§13.1.3);
- an unused lifetime parameter (§7.6.5);
- a mismatch between a match arm's type and the match's type (§8.15.8).

  24.2.3 An implementation **MUST** produce at least the warnings listed in §24.2.2. An implementation **MAY** produce additional warnings for programs that are likely incorrect, provided the warnings are semantically justified.

  24.2.4 An implementation **SHOULD** provide a way to suppress a specific warning at a specific location, such as `#[allow(...)]` (§23.5.1).

### 24.3 Lints and Configurable Diagnostics

24.3.1 A **lint** is a configurable diagnostic that the programmer may enable or disable. Lints are grouped by category and are enabled by default at a specified level.

24.3.2 The **lint levels** are:

| Level    | Effect                                                               |
| -------- | -------------------------------------------------------------------- |
| `allow`  | the lint does not produce a diagnostic                               |
| `warn`   | the lint produces a warning                                          |
| `deny`   | the lint produces an error and prevents code generation              |
| `forbid` | like `deny`, but the lint cannot be overridden by an inner attribute |

24.3.3 The **default lint levels** are specified by the implementation. An implementation **MUST** document its default levels and **MUST NOT** change them without a deprecation cycle (§28.3).

24.3.4 The **built-in lints** include at least:

- `unused_variables`, `unused_imports`, `unused_mut`, `dead_code`;
- `unreachable_code`, `unused_must_use`, `unused_results`;
- `deprecated`, `unstable_name_collisions`;
- `nonstandard_style` (naming conventions);
- `improper_ctypes`, `improper_ctypes_definitions` (FFI safety);
- `unused_unsafe`, `unused_lifetimes`, `unused_qualifications`;
- `missing_docs`, `missing_debug_implementations`;
- `private_interfaces`, `private_bounds`;
- `unsafe_op_in_unsafe_fn`.

  24.3.5 The **lint configuration** is specified with `#[allow(...)]`, `#[warn(...)]`, `#[deny(...)]`, or `#[forbid(...)]` attributes (§23.5.1), or with command-line flags. The attributes take precedence over the command-line flags.

  24.3.6 A lint's **scope** is the item, module, or statement to which its attribute is applied, and to all nested items unless overridden by a nested attribute.

  24.3.7 The **`forbid`** level **MUST NOT** be overridden by any inner attribute. An attempt to override `forbid` **MUST** produce an error under the `forbidden_lint_override` lint.

  24.3.8 An implementation **MUST** support the lint levels and the attribute-based configuration. An implementation **MAY** add implementation-specific lints, provided they are namespaced by the implementation's name.

### 24.4 Diagnostic Severity and Suppression

24.4.1 A **diagnostic** is a message that the compiler emits about the program. Its **severity** is one of `error`, `warning`, `note`, `help`, or `suggestion`.

24.4.2 An **error** is a diagnostic that prevents code generation. A **warning** is a diagnostic that does not prevent code generation but indicates a likely problem.

24.4.3 A **note** is an informational message attached to an error or warning. A **help** message suggests a fix. A **suggestion** is a machine-applicable fix that the compiler **MAY** apply automatically with a tool.

24.4.4 The **`#[allow(...)]`** attribute suppresses a lint at its scope. A suppressed lint **MUST NOT** produce a diagnostic within its scope.

24.4.5 The **`#[expect(...)]`** attribute declares that a lint is expected to fire at its scope. If the lint does not fire, the compiler **MUST** produce an `unfulfilled_expectation` warning.

24.4.6 A **diagnostic suppression** **MUST NOT** hide a required error (§24.1). A required error **MUST** be produced regardless of any `allow` attribute.

24.4.7 A **diagnostic's message** **MUST** be clear and actionable. It **SHOULD** identify the location of the problem, the nature of the problem, and a suggestion for fixing it.

24.4.8 A **diagnostic's span** **MUST** cover the smallest region of source that is relevant to the problem. The span **MUST NOT** cover unrelated code.

### 24.5 Source Locations and Spans

24.5.1 A **source location** is a (file, line, column) triple that identifies a position in the source. A **span** is a range of source locations.

24.5.2 The **`Location`** type provides the caller's source location for a function marked `#[track_caller]` (§23.3.3). Its `caller()` method **MUST** return the location of the caller, not the location of the call site.

24.5.3 The **`Span`** type is the compiler's internal representation of a source range. It is not available at runtime; only `Location` is available at runtime.

24.5.4 A **diagnostic's span** **MUST** be computed from the token spans of the program. A span **MUST NOT** include tokens that are not part of the offending construct.

24.5.5 The **`#[track_caller]`** attribute **MUST** be applied to functions that want to report the caller's location. The attribute **MUST NOT** change the function's ABI or its behavior when the caller's location is not used.

24.5.6 A **multiline span** **MUST** be rendered with a clear indication of the range. The rendering **SHOULD** include the first and last lines of the range and an ellipsis for the intervening lines if the range is large.

### 24.6 Implementation Limits

24.6.1 An implementation **MUST** document its limits for at least the following:

- maximum source file size;
- maximum number of tokens per file;
- maximum nesting depth of expressions, types, and blocks;
- maximum number of generic parameters per item;
- maximum depth of monomorphization (§13.8.6);
- maximum step budget for constant evaluation (§20.1.4);
- maximum recursion depth of macro expansion (§20.4.9);
- maximum size of a single allocation;
- maximum number of threads;
- maximum number of source files per crate.

  24.6.2 An implementation **MAY** impose limits on constructs that are not listed in §24.6.1, provided the limits are documented.

  24.6.3 An implementation **MUST** reject a program that exceeds a documented limit with a diagnostic identifying the limit and the construct that exceeded it.

  24.6.4 An implementation **MUST NOT** silently truncate, wrap, or otherwise corrupt a program that exceeds a limit. The rejection **MUST** be explicit.

  24.6.5 An implementation **SHOULD** provide a way to increase the limits for programs that legitimately need more. The mechanism **MUST** be explicit and **MUST NOT** be enabled by default.

  24.6.6 An implementation **MUST** provide a **debug runtime** that detects a subset of undefined behavior (§16.10.6), including at least:

- out-of-bounds indexing (which is already checked by the language, §8.12.4);
- integer overflow (when the overflow mode is checked, §8.7.3);
- null pointer dereference (which is undefined behavior, §16.4.2);
- misaligned pointer dereference;
- data races (under a data race detector, §19.5.7);
- use of uninitialized memory (under a memory sanitizer);
- double free and use after free (under an address sanitizer).

  24.6.7 The debug runtime **MUST NOT** be required for conformance. An implementation that does not provide a debug runtime **MUST** document its absence.

  24.6.8 An implementation **MUST** provide an **optimizer** that preserves the program's observable behavior. The optimizer **MUST NOT** introduce undefined behavior, **MUST NOT** reorder side-effecting operations in violation of §8.18.7, and **MUST NOT** eliminate volatile operations in violation of §16.7.6.

## Part VI — Formal Appendices

## 25. Formal Grammar

### 25.1 Lexical Grammar

25.1.1 The lexical grammar specifies how source characters are grouped into tokens. It is the first phase of compilation (§3.5.1).

```
source        ::= [ bom ] { source-element }
source-element::= whitespace | line-terminator | comment | token | doc-comment
whitespace    ::= " " | "\t"
line-terminator ::= "\n" | "\r\n" | "\r"
comment       ::= line-comment | block-comment
line-comment  ::= "//" { not-line-terminator } [ line-terminator ]
block-comment ::= "/*" { block-comment | not-close-comment } "*/"
doc-comment   ::= "///" { not-line-terminator } | "//!" { not-line-terminator }
```

25.1.2 **Tokens** are the atomic units of the language. Each token has a **span** that records its source location (§24.5).

```
token         ::= ident | keyword | literal | punct | delimiter
                 | raw-ident | lifetime
ident         ::= XID_Start XID_Continue* | "_" XID_Continue+
raw-ident     ::= "r#" ( ident | keyword )
lifetime      ::= "'" ident
keyword       ::= "as" | "async" | "await" | "break" | "const" | "continue"
                | "crate" | "defer" | "dyn" | "else" | "enum" | "extern"
                | "false" | "fn" | "for" | "if" | "impl" | "in" | "let"
                | "loop" | "match" | "mod" | "move" | "mut" | "pub"
                | "ref" | "return" | "self" | "Self" | "static" | "struct"
                | "super" | "trait" | "true" | "type" | "unsafe" | "use"
                | "where" | "while" | "unsized" | "yield"
delimiter     ::= "(" | ")" | "[" | "]" | "{" | "}"
punct         ::= "." | ".." | "..=" | "..." | "," | ";" | ":" | "::"
                | "->" | "=>" | "#" | "#!" | "@" | "$" | "?" | "~"
                | "+" | "-" | "*" | "/" | "%" | "^" | "!" | "&" | "|"
                | "&&" | "||" | "<<" | ">>" | "=" | "==" | "!=" | "<"
                | ">" | "<=" | ">=" | "+=" | "-=" | "*=" | "/=" | "%="
                | "&=" | "|=" | "^=" | "<<=" | ">>="
```

25.1.3 The **literal tokens** are:

```
literal       ::= int-literal | float-literal | bool-literal
                | char-literal | string-literal | byte-literal
                | byte-string-literal | raw-string-literal

int-literal   ::= dec-literal | hex-literal | oct-literal | bin-literal
dec-literal   ::= "0" | [1-9] { digit-or-underscore }
hex-literal   ::= "0x" hex-digit { hex-digit-or-underscore }
oct-literal   ::= "0o" oct-digit { oct-digit-or-underscore }
bin-literal   ::= "0b" bin-digit { bin-digit-or-underscore }
int-suffix    ::= "i8" | "i16" | "i32" | "i64" | "i128" | "isize"
                | "u8" | "u16" | "u32" | "u64" | "u128" | "usize"

float-literal ::= [0-9] { digit-or-underscore } "." [0-9] { digit-or-underscore } [exp] [float-suffix]
                | [0-9] { digit-or-underscore } exp [float-suffix]
exp           ::= ("e" | "E") ["+" | "-"] [0-9] { digit-or-underscore }
float-suffix  ::= "f32" | "f64"

bool-literal  ::= "true" | "false"

char-literal  ::= "'" ( char-content | escape ) "'"
char-content  ::= not-quote-backslash-or-line-terminator

string-literal ::= '"' { string-char | escape } '"'
string-char   ::= not-quote-backslash-or-line-terminator

raw-string-literal ::= "r" "#"* '"' { not-quote } '"' "#"*

byte-literal  ::= "b'" ( ascii-char | byte-escape ) "'"
byte-string-literal ::= 'b"' { ascii-char | byte-escape } '"'

escape        ::= "\" ( "n" | "r" | "t" | "\\" | "'" | '"' | "0"
                | "x" hex-digit hex-digit
                | "u{" hex-digit{1,6} "}" )
byte-escape   ::= "\" ( "n" | "r" | "t" | "\\" | "'" | '"' | "0"
                | "x" hex-digit hex-digit )
```

25.1.4 The lexical grammar **MUST** be applied with **maximal munch**: the longest valid token at each position is selected. When two rules produce the same token, the rule with the higher priority (as specified in §4.9) is chosen.

25.1.5 A **raw identifier** `r#name` is a single token that denotes the identifier `name` even if it is a keyword. A raw identifier **MUST NOT** be followed by a character that could continue an identifier.

25.1.6 A **lifetime token** `'a` is lexed only when it is followed by an identifier character and is not part of a character or string literal. The disambiguation between a lifetime and a character literal is performed by the lexer using the character after the quote.

### 25.2 Syntactic Grammar

25.2.1 The syntactic grammar specifies how tokens are grouped into a **syntax tree**. It is the second phase of compilation (§3.5.2).

25.2.2 The **top-level grammar** is:

```
crate         ::= { inner-attribute } { item }

item          ::= attribute* visibility? item-kind
item-kind     ::= function-item | const-item | static-item
                | struct-item | enum-item | union-item | type-alias
                | trait-item | impl-item | mod-item | use-item
                | extern-block | macro-rules-item

visibility    ::= "pub" | "pub" "(" vis-restriction ")"
vis-restriction ::= "crate" | "super" | "self" | "in" path
```

25.2.3 The **type grammar** is:

```
type          ::= "(" type { "," type } [","] ")"        // tuple
                | "[" type ";" const-expr "]"             // array
                | "[" type "]"                            // slice
                | "&" [lifetime] ["mut"] type             // reference
                | "*" ("const" | "mut") type              // raw pointer
                | "dyn" trait-bound { "+" trait-bound }   // trait object
                | "impl" trait-bound { "+" trait-bound }  // opaque type
                | "fn" "(" fn-params ")" ["->" type]      // function pointer
                | "extern" abi-string? "fn" "(" fn-params ")" ["->" type]
                | "!"                                    // never
                | "_"                                    // inferred
                | path [generic-args]                    // named type
                | "<" type ">"                           // parenthesized
```

25.2.4 The **expression grammar** is:

```
expr          ::= assign-expr
assign-expr   ::= range-expr
                | assign-expr assign-op assign-expr
assign-op     ::= "=" | "+=" | "-=" | "*=" | "/=" | "%="
                | "&=" | "|=" | "^=" | "<<=" | ">>="
range-expr    ::= or-expr [ range-op or-expr ]
range-op      ::= ".." | "..="
or-expr       ::= and-expr { "||" and-expr }
and-expr      ::= cmp-expr { "&&" cmp-expr }
cmp-expr      ::= bitor-expr { cmp-op bitor-expr }
cmp-op        ::= "==" | "!=" | "<" | "<=" | ">" | ">="
bitor-expr    ::= bitxor-expr { "|" bitxor-expr }
bitxor-expr   ::= bitand-expr { "^" bitand-expr }
bitand-expr   ::= shift-expr { "&" shift-expr }
shift-expr    ::= add-expr { shift-op add-expr }
shift-op      ::= "<<" | ">>"
add-expr      ::= mul-expr { add-op mul-expr }
add-op        ::= "+" | "-"
mul-expr      ::= cast-expr { mul-op cast-expr }
mul-op        ::= "*" | "/" | "%"
cast-expr     ::= unary-expr [ "as" type ]
unary-expr    ::= unary-op unary-expr | postfix-expr
unary-op      ::= "-" | "!" | "*" | "&" | "&mut"
postfix-expr  ::= primary-expr { postfix-op }
postfix-op    ::= "." ident
                | "." int-literal
                | "(" call-args ")"
                | "[" expr "]"
                | "?"
primary-expr  ::= literal
                | path
                | "(" expr ")"
                | "[" array-elems "]"
                | block
                | "if" expr block ["else" ( expr | block )]
                | "match" expr "{" match-arms "}"
                | "loop" block
                | "while" expr block
                | "for" pattern "in" expr block
                | "unsafe" block
                | closure
                | "return" [expr]
                | "break" [lifetime] [expr]
                | "continue" [lifetime]
                | "defer" block
                | macro-invocation
```

25.2.5 The **pattern grammar** is:

```
pattern       ::= or-pattern
or-pattern    ::= range-pattern { "|" range-pattern }
range-pattern ::= literal [ ".." literal | "..=" literal ]
                | path [ "(" pattern-list ")" | "{" field-patterns "}" ]
                | ident [ "@" pattern ]
                | "mut" ident [ "@" pattern ]
                | "ref" [ "mut" ] ident [ "@" pattern ]
                | "_"
                | "&" pattern
                | "&mut" pattern
                | "(" pattern-list ")"
                | "[" pattern-list "]"
                | "(" pattern ")"
```

25.2.6 The **statement grammar** is:

```
statement     ::= let-stmt | item-stmt | expr-stmt | empty-stmt
let-stmt      ::= "let" pattern [ ":" type ] [ "=" expr ] ";"
                | "let" pattern "=" expr "else" block ";"
item-stmt     ::= item
expr-stmt     ::= expr ";" | block-expr-without-semicolon
empty-stmt    ::= ";"
```

25.2.7 The **item grammar** for each item kind is specified in the corresponding chapter:

- functions: §11.5.1, §12.2;
- constants and statics: §11.3.1, §11.3.5;
- structs, enums, unions: §6.9.1, §6.9.4, §6.9.6;
- type aliases: §6.11.1;
- traits: §11.7.1, §14.1;
- implementations: §11.8.1, §14.3;
- modules: §11.6.1;
- use declarations: §5.5.1;
- extern blocks: §11.9.1;
- macros: §20.4.2.

### 25.3 Grammar Notation

25.3.1 The grammar uses the following notation:

- `"text"` denotes a literal token.
- `[ x ]` denotes an optional element.
- `{ x }` denotes zero or more repetitions.
- `{ x }+` denotes one or more repetitions.
- `( x )` denotes grouping.
- `x | y` denotes alternation.
- `...` denotes prose elision.
- `not-x` denotes any character or token that is not `x`.

  25.3.2 The grammar is **ambiguous** in several places that are resolved by the parser according to the rules of §4.9 and §8.6. In particular:

- `<` and `>` are ambiguous with generic delimiters (§4.9.3);
- `<<` may need to be split into two `<` in generic contexts (§4.9.2);
- `&` is ambiguous between a reference and a bitwise operator (resolved by context);
- `*` is ambiguous between a dereference and a multiplication (resolved by context);
- `-` is ambiguous between a negation and a subtraction (resolved by context).

  25.3.3 The grammar is **left-recursive** in several places. A parser **MUST** use a technique that handles left recursion, such as precedence climbing or a left-recursion elimination transformation.

  25.3.4 The grammar **MUST** be parsed deterministically: for any token sequence, there is at most one valid parse tree (after the disambiguation rules of §4.9 and §8.6). A sequence that has no valid parse tree is a syntax error.

  25.3.5 The grammar's **concrete syntax** is the text as written; the **abstract syntax** is the parse tree after desugaring. Desugarings are specified in the relevant chapters (for example, `while let` in §9.7.5, `else if` in §8.14.4).

### 25.4 Reserved Words

25.4.1 The **reserved keywords** are listed in §4.4.2 and §4.5.2. A reserved keyword **MUST NOT** be used as an identifier except when escaped as a raw identifier (§4.5.5).

25.4.2 The **contextual keywords** `self`, `Self`, `super`, and `crate` have special meaning only in specific contexts (§5.7.4). In other contexts, they may be used as identifiers.

25.4.3 The **reserved for future use** identifiers are listed in §4.5.2. They **MUST NOT** be used as identifiers, even though they are not currently keywords.

25.4.4 An implementation **MUST NOT** introduce new keywords without a deprecation cycle (§28.3). A new keyword is a breaking change for programs that use the keyword as an identifier.

25.4.5 The **`_`** identifier is reserved as the wildcard pattern (§10.4) and **MUST NOT** be used as a binding name.

### 25.5 Reserved Punctuation and Future Compatibility

25.5.1 The following punctuation tokens are **reserved for future use** and **MUST NOT** appear in a valid program:

- `...` in expression position (it is only valid in variadic parameter lists, §18.5);
- `=>` outside a match arm;
- `@` outside a binding pattern;
- `$` outside a macro pattern;
- `~` outside a future feature.

  25.5.2 The following punctuation sequences are **reserved** and **MUST NOT** be given a meaning that could conflict with a future language feature:

- `::<` for turbofish (§13.3.3);
- `<::` for a leading turbofish;
- `..=` for inclusive range;
- `->` for return type;
- `=>` for match arm.

  25.5.3 The following are **reserved for future features** and **MUST** be rejected if they appear:

- `become`, `box`, `do`, `final`, `macro`, `override`, `priv`, `try`, `typeof`, `virtual`, `yield` as identifiers (§4.5.2);
- `try` blocks;
- `yield` expressions;
- `box` expressions;
- `abstract` types.

  25.5.4 An implementation **MAY** provide unstable features for the reserved constructs, enabled with `#![feature(...)]`. A program that uses a reserved construct without the enabling feature **MUST** be rejected with a diagnostic identifying the future feature.

---

## 26. Built-in Definitions

### 26.1 Built-in Types

26.1.1 The **built-in types** are the primitive types of §6.2.1. They are available in the prelude (§22.2) and **MUST** be implemented by every conforming implementation.

26.1.2 The **scalar types** (`bool`, integers, floats, `char`) are `Copy` and have the layout specified in §6.2.1.

26.1.3 The **compound types** (`()`, tuples, arrays, slices, references, raw pointers, `str`) are specified in §6.8 and §6.17. Their layout is specified in §17.

26.1.4 The **function types** (`fn(...)`, `extern "C" fn(...)`, etc.) are specified in §6.10 and §12.6.

26.1.5 The **never type** `!` is specified in §6.6.2. It is a subtype of every type (§6.15.3).

### 26.2 Built-in Traits

26.2.1 The **built-in traits** are the traits that the language itself relies on. They are listed in the core prelude (§22.2.1) and **MUST** be available in every conforming implementation.

26.2.2 The **operator traits** are:

| Trait    | Methods  | Operator    |
| -------- | -------- | ----------- |
| `Add`    | `add`    | `+`         |
| `Sub`    | `sub`    | `-`         |
| `Mul`    | `mul`    | `*`         |
| `Div`    | `div`    | `/`         |
| `Rem`    | `rem`    | `%`         |
| `Neg`    | `neg`    | `-` (unary) |
| `BitAnd` | `bitand` | `&`         |
| `BitOr`  | `bitor`  | `\|`        |
| `BitXor` | `bitxor` | `^`         |
| `Not`    | `not`    | `!`         |
| `Shl`    | `shl`    | `<<`        |
| `Shr`    | `shr`    | `>>`        |

26.2.3 The **assignment traits** are `AddAssign`, `SubAssign`, `MulAssign`, `DivAssign`, `RemAssign`, `BitAndAssign`, `BitOrAssign`, `BitXorAssign`, `ShlAssign`, `ShrAssign`. Each provides an `op_assign` method corresponding to the compound assignment operator.

26.2.4 The **comparison traits** are:

- `PartialEq`: `eq`, `ne`;
- `Eq`: marker trait for full equality;
- `PartialOrd`: `partial_cmp`, `lt`, `le`, `gt`, `ge`;
- `Ord`: `cmp`, `min`, `max`, `clamp`.

  26.2.5 The **conversion traits** are:

- `From<T>`: `from`;
- `Into<T>`: `into` (blanket-implemented for `T: From<U>`);
- `TryFrom<T>`: `try_from`;
- `TryInto<T>`: `try_into` (blanket-implemented for `T: TryFrom<U>`);
- `AsRef<T>`: `as_ref`;
- `AsMut<T>`: `as_mut`;
- `Borrow<T>`: `borrow`;
- `BorrowMut<T>`: `borrow_mut`.

  26.2.6 The **iteration traits** are:

- `Iterator`: `next`, `size_hint`, `count`, `last`, `nth`, `map`, `filter`, `fold`, `collect`, and many others;
- `IntoIterator`: `into_iter`;
- `FromIterator`: `from_iter`;
- `Extend`: `extend`.

  26.2.7 The **function traits** are:

- `Fn`: `call` (immutable receiver);
- `FnMut`: `call_mut` (mutable receiver);
- `FnOnce`: `call_once` (owned receiver).

  26.2.8 The **memory traits** are:

- `Clone`: `clone`;
- `Copy`: marker trait for bitwise copy;
- `Drop`: `drop`;
- `Sized`: marker trait for statically-sized types;
- `Send`: marker trait for thread transfer;
- `Sync`: marker trait for shared access from multiple threads.

  26.2.9 The **`Sized`** trait is special: it is implemented for all types except unsized types, and it is a default bound on all type parameters (§13.5.7). The `?Sized` syntax relaxes it (§13.5.3).

  26.2.10 The **`Send`** and **`Sync`** traits are auto traits (§19.6.3). They are implemented automatically for types whose fields satisfy the bound, unless the type opts out with a negative implementation.

### 26.3 Built-in Macros

26.3.1 The **built-in macros** are macros that are recognized by the compiler without being declared with `macro_rules!`. They are listed in the core prelude (§22.2.1).

26.3.2 The **panic macros** are `panic!`, `unreachable!`, `todo!`, `unimplemented!`, `assert!`, `assert_eq!`, `assert_ne!`, `debug_assert!`, `debug_assert_eq!`, `debug_assert_ne!`. They are specified in §21.2 and §9.11.

26.3.3 The **format macros** are `format!`, `print!`, `println!`, `eprint!`, `eprintln!`, `write!`, `writeln!`. They format values according to the `Display` and `Debug` traits and **MUST** be available in the standard prelude.

26.3.4 The **`vec!`** macro constructs a `Vec<T>` from a list of elements or a repeated element. Its expansion **MUST** be equivalent to constructing a `Vec` and pushing the elements, except that the capacity **MUST** be at least the number of elements.

26.3.5 The **`format_args!`** macro produces a `Arguments` value that can be formatted later. It is the basis of the other format macros.

26.3.6 The **`include!`**, **`include_str!`**, and **`include_bytes!`** macros include a file's contents at compile time. The included file's path **MUST** be a string literal relative to the including file.

26.3.7 The **`concat!`** and **`stringify!`** macros concatenate string literals or convert tokens to a string literal at compile time.

26.3.8 The **`cfg!`** macro expands to a boolean literal based on the configuration (§20.7.3).

26.3.9 The **`env!`** and **`option_env!`** macros read an environment variable at compile time. `env!` panics if the variable is not set; `option_env!` returns `Option<&'static str>`.

26.3.10 The **`file!`**, **`line!`**, **`column!`**, and **`module_path!`** macros expand to the current source location's file, line, column, or module path.

### 26.4 Intrinsics

26.4.1 An **intrinsic** is a function implemented directly by the compiler (§16.8). The set of available intrinsics is implementation-defined, but the following are **REQUIRED**:

- `size_of::<T>()`, `align_of::<T>()`;
- `transmute::<Src, Dst>(value)` (§16.5);
- `copy_nonoverlapping::<T>(src, dst, count)`;
- `copy::<T>(src, dst, count)`;
- `write_bytes::<T>(dst, val, count)`;
- `volatile_load::<T>(src)`, `volatile_store::<T>(dst, val)` (§16.7);
- `compare_exchange` for atomic types (§19.3);
- `abort()`, `unreachable_unchecked()`, `assume(bool)`.

  26.4.2 An intrinsic **MUST** be semantically specified either in this document or in the implementation's documentation. An intrinsic that is not documented **MUST NOT** be used by a program that does not opt into unstable features.

  26.4.3 An intrinsic **MAY** be marked `unsafe` if its operation has safety invariants. Calling an unsafe intrinsic is an unsafe operation (§16.1.4).

  26.4.4 The **`assume(bool)`** intrinsic tells the optimizer that a condition is true. If the condition is false, the program has undefined behavior. The intrinsic **MUST** be used only when the programmer has proven the condition.

  26.4.5 The **`unreachable_unchecked()`** intrinsic tells the optimizer that the current point is unreachable. If it is reached, the program has undefined behavior. It **MUST** be used only when the programmer has proven the point is unreachable.

### 26.5 Compiler-Known Attributes

26.5.1 The **compiler-known attributes** are the built-in attributes of Chapter 23. They are recognized by the compiler and **MUST** be supported by every conforming implementation.

26.5.2 An implementation **MUST NOT** ignore a compiler-known attribute. An unknown attribute **MUST** produce an error under the `unknown_attribute` lint (§24.3).

26.5.3 A **user-defined attribute** (§23.7) **MUST NOT** conflict with a compiler-known attribute's name. The compiler-known attribute takes precedence; the user-defined attribute **MUST** be namespaced.

26.5.4 The **`#[derive(...)]`** attribute's trait names **MUST** be resolved in the macro namespace. A derive that is not registered as a procedural macro **MUST** produce an error.

### 26.6 Target-Specific Definitions

26.6.1 A **target-specific definition** is a type or function whose behavior depends on the target platform. The standard library provides target-specific definitions for each supported target.

26.6.2 The **`std::os`** module provides platform-specific APIs for Unix, Windows, and other platforms. A use of a platform-specific module on the wrong target **MUST** be rejected at compile time.

26.6.3 The **`target_os`**, **`target_arch`**, **`target_family`**, **`target_env`**, and **`target_pointer_width`** cfg atoms (§11.10.3) **MUST** be set for each supported target.

26.6.4 The **`usize`** and **`isize`** types are target-specific: their width is the target's pointer width (§27.4). An implementation **MUST** define them consistently with the target's data model.

26.6.5 The **`c_char`** type is target-specific: it is `i8` on platforms where `char` is signed and `u8` on platforms where `char` is unsigned. The `c_char` type **MUST** match the target's C ABI.

26.6.6 The **`c_long`** type is target-specific: it is `i32` on 32-bit targets and `i64` on 64-bit targets (except on Windows, where it is `i32` on both). The `c_long` type **MUST** match the target's C ABI.

---

## 27. Platform and Target Model

### 27.1 Target Triple Format

27.1.1 A **target triple** identifies a compilation target. Its format is:

```
<arch>-<vendor>-<os>[-<env>]
```

27.1.2 The **arch** component identifies the CPU architecture: `x86`, `x86_64`, `arm`, `aarch64`, `riscv32`, `riscv64`, `wasm32`, `wasm64`, `mips`, `mips64`, `powerpc`, `powerpc64`, etc.

27.1.3 The **vendor** component identifies the vendor or platform: `unknown`, `apple`, `pc`, `sun`, `nvidia`, `wrs`, `uwp`, etc. The vendor is often `unknown` for open platforms.

27.1.4 The **os** component identifies the operating system: `linux`, `windows`, `darwin`, `freebsd`, `netbsd`, `openbsd`, `android`, `ios`, `none`, `wasm`, etc.

27.1.5 The **env** component (optional) identifies the ABI environment: `gnu`, `musl`, `msvc`, `sgx`, `eabi`, `eabihf`, etc.

27.1.6 The **target triple** **MUST** be a valid identifier sequence separated by hyphens. An implementation **MUST** reject a malformed triple with a diagnostic.

27.1.7 An implementation **MUST** support at least the target triples of the platforms on which it runs. Additional targets **MAY** be supported as cross-compilation targets.

### 27.2 Data Models

27.2.1 A **data model** specifies the sizes of the primitive types. Forge supports the following data models:

| Data model | `char` | `short` | `int` | `long` | Pointer | Used by                     |
| ---------- | ------ | ------- | ----- | ------ | ------- | --------------------------- |
| LP64       | 8      | 16      | 32    | 64     | 64      | Most 64-bit Unix            |
| LLP64      | 8      | 16      | 32    | 32     | 64      | 64-bit Windows              |
| ILP32      | 8      | 16      | 32    | 32     | 32      | 32-bit Unix, 32-bit Windows |
| ILP64      | 8      | 16      | 64    | 64     | 64      | Some 64-bit platforms       |

27.2.2 The **`usize`** and **`isize`** types **MUST** have the target's pointer width, which is 32 or 64 bits depending on the data model.

27.2.3 The **`c_long`** type **MUST** be `i32` on ILP32 and LLP64, and `i64` on LP64 and ILP64. The **`c_int`** type **MUST** be `i32` on all data models. The **`c_short`** type **MUST** be `i16` on all data models.

27.2.4 The **`c_char`** type **MUST** match the target's C `char` type: `i8` if `char` is signed, `u8` if unsigned. The signedness **MUST** match the target's C ABI.

27.2.5 The **`c_float`** and **`c_double`** types **MUST** be `f32` and `f64`, respectively. The C ABI does not support other floating-point widths for these types.

27.2.6 An implementation **MUST** document the data model of each supported target. The documentation **MUST** include the sizes of `usize`, `isize`, `c_long`, and `c_char`.

### 27.3 Endianness

27.3.1 A target is **little-endian** if the least significant byte of a multi-byte value is stored at the lowest address. It is **big-endian** if the most significant byte is stored at the lowest address.

27.3.2 The **`target_endian`** cfg atom **MUST** be set to `"little"` or `"big"` for each supported target.

27.3.3 The **`to_le_bytes`**, **`to_be_bytes`**, **`from_le_bytes`**, and **`from_be_bytes`** methods on integer types **MUST** convert between the native representation and the specified byte order.

27.3.4 The **`to_ne_bytes`** and **`from_ne_bytes`** methods **MUST** use the target's native byte order.

27.3.5 A target **MAY** support **bi-endian** operation, in which the byte order is selectable at runtime. Forge **MUST** reject bi-endian targets unless the byte order is fixed at compile time.

27.3.6 The **layout** of a multi-byte value **MUST** respect the target's endianness. A program that reads a multi-byte value from a byte buffer **MUST** use the byte-order conversion methods, not a transmutation.

### 27.4 Pointer Width and Address Space

27.4.1 The **pointer width** of a target is the number of bits in a pointer. It **MUST** be 16, 32, 64, or 128. Forge 0.1 **MUST** support 32-bit and 64-bit targets; 16-bit and 128-bit targets **MAY** be supported.

27.4.2 The **address space** of a target is the set of addresses that a pointer can represent. It **MUST** be at least `2^pointer_width` for a flat address space, but may be smaller for segmented or paged address spaces.

27.4.3 The **`usize`** and **`isize`** types **MUST** have the target's pointer width. A program that uses `usize` for an index **MUST** be valid on all targets with at least that many addressable bytes.

27.4.4 A **fat pointer** (§6.17.4) **MUST** be twice the size of a thin pointer for slices and `str`, and for `dyn Trait`. The exact layout of a fat pointer is implementation-defined but **MUST** be consistent within a compilation.

27.4.5 A **null pointer** **MUST** be representable as the all-zero bit pattern for thin pointers. A program **MUST NOT** rely on the null pointer's address being zero for fat pointers, unless the implementation documents it.

27.4.6 A **non-null pointer** **MUST** have a non-zero value for thin pointers. The niche optimization (§17.5.4) uses the null value as an invalid pattern for `&T` and `Box<T>`.

### 27.5 Supported Calling Conventions

27.5.1 Each target **MUST** support the `"C"` calling convention, which **MUST** match the target's C ABI (§17.6.3).

27.5.2 Each target **MUST** support the `forge` calling convention, which is implementation-defined but **MUST** be stable within a compilation (§12.5.2).

27.5.3 Each target **MUST** support the `"system"` calling convention, which **MUST** select the platform's system ABI (§17.6.4).

27.5.4 A target **MAY** support additional calling conventions (`"fastcall"`, `"stdcall"`, `"thiscall"`, `"vectorcall"`, `"win64"`, `"sysv64"`) as specified in §12.5.1. An implementation **MUST** reject a calling convention that the target does not support.

27.5.5 A calling convention **MUST** specify the parameter passing, return value passing, register preservation, and stack alignment rules. An implementation **MUST** document its calling conventions for each supported target.

27.5.6 A **cross-language call** (§17.9.6) **MUST** use the target's C ABI or the foreign language's ABI. A cross-language call with an incompatible ABI is undefined behavior.

### 27.6 Freestanding Target Requirements

27.6.1 A **freestanding target** provides only the `core` subset of the standard library (§3.7.2). A program compiled for a freestanding target **MUST** provide its own entry point, panic handler, and allocator (if it uses allocation).

27.6.2 A freestanding program **MUST** declare `#![no_std]` at its crate root (§3.7.3). A program that uses the standard library on a freestanding target **MUST** be rejected with a diagnostic.

27.6.3 A freestanding program **MUST** provide a `#[panic_handler]` function (§23.1.9). The handler **MUST** not return; it **MUST** abort or terminate the program.

27.6.4 A freestanding program **MUST** provide a `#[global_allocator]` static (§21.6.2) if it uses any allocating type (`Box`, `Vec`, `String`, etc.).

27.6.5 A freestanding program **MUST** provide an entry point with the `#[start]` attribute (§23.1.10) or **MUST** link against a runtime that provides one.

27.6.6 The **`core`** crate **MUST** be available on every freestanding target. The `alloc` crate **MUST** be available if a global allocator is provided.

27.6.7 An implementation **MUST** document the freestanding target's requirements, including the entry point, panic handler, allocator, and any platform-specific initialization.

---

## 28. Compatibility and Evolution

### 28.1 Editions and Language Versions

28.1.1 A **language version** is a snapshot of the language's grammar and semantics. Forge 0.1 is the first version; future versions **MAY** add features without breaking backward compatibility.

28.1.2 An **edition** is a set of opt-in language changes that are not backward compatible. Editions are selected per-crate and **MUST** be specified in the package manifest.

28.1.3 A **crate** compiled with edition `E` **MUST** be compatible with crates compiled with other editions. A crate's edition **MUST NOT** affect the behavior of other crates.

28.1.4 An edition **MAY** change the meaning of existing syntax, but only for programs that opt in to the edition. A program that does not opt in **MUST** retain its original meaning.

28.1.5 An **edition migration** is a tool that rewrites a crate's source to be compatible with a new edition. The tool **SHOULD** preserve the program's behavior.

28.1.6 An implementation **MUST** support the current edition and at least one previous edition. An implementation **MAY** support older editions indefinitely.

28.1.7 The **default edition** for a new crate **MUST** be the latest edition supported by the implementation.

### 28.2 Stability Guarantees

28.2.1 The **stability guarantee** is that a program that compiles with a given toolchain version **MUST** continue to compile with the same or a later toolchain version, unless the program uses unstable features.

28.2.2 The **stable language** is the set of features that are not marked `#[unstable]`. A program that uses only stable features **MUST** compile with any later toolchain version.

28.2.3 The **unstable language** is the set of features that are marked `#[unstable]`. An unstable feature **MAY** change or be removed between toolchain versions without a deprecation cycle.

28.2.4 A **breaking change** is a change that causes a previously-compiling program to fail to compile, or a previously-correct program to behave differently. Breaking changes **MUST** go through a deprecation cycle (§28.3).

28.2.5 A **bug fix** is a change that makes a program that previously had undefined behavior conform to the specification, or fixes a soundness hole. Bug fixes **MAY** be applied without a deprecation cycle, but **SHOULD** be documented.

28.2.6 The **standard library** provides the same stability guarantee as the language: a program that uses only stable library features **MUST** continue to compile and behave correctly.

28.2.7 The **ABI stability** is not guaranteed across toolchain versions. The `forge` calling convention and the layout of types without `#[repr(...)]` **MAY** change between versions.

### 28.3 Deprecation Policy

28.3.1 A **deprecation** marks a feature as scheduled for removal. The feature **MUST** continue to work for at least one major version after deprecation.

28.3.2 A deprecated feature **MUST** produce a warning when used, under the `deprecated` lint (§23.5.3). The warning **MUST** identify the feature and suggest a replacement.

28.3.3 A feature **MUST NOT** be removed without a deprecation cycle. The cycle **MUST** include at least one stable release in which the feature is deprecated.

28.3.4 A **removed feature** **MUST** be documented in the changelog (§28.5). The changelog **MUST** describe the feature, the replacement, and the version in which the removal occurred.

28.3.5 An **unstable feature** **MAY** be removed without a deprecation cycle, because it was never stable. An unstable feature's removal **MUST** be documented in the changelog.

28.3.6 A **breaking change** to a stable feature **MUST** be approved by the language's governance process. The process **MUST** consider the impact on existing programs and provide a migration path.

### 28.4 Semver for Language Features

28.4.1 The language version **MUST** follow **semantic versioning**: `MAJOR.MINOR.PATCH`. A change in `MAJOR` indicates a breaking change; `MINOR` adds backward-compatible features; `PATCH` fixes bugs.

28.4.2 A **major version** **MAY** introduce breaking changes. A program that compiled with the previous major version **MAY** need to be updated for the new major version.

28.4.3 A **minor version** **MUST NOT** introduce breaking changes. A program that compiled with the previous minor version **MUST** continue to compile with the new minor version.

28.4.4 A **patch version** **MUST NOT** introduce new features or breaking changes. It **MAY** fix bugs, including soundness holes that cause previously-compiling programs to be rejected.

28.4.5 The **version** of the language **MUST** be specified in the package manifest. A crate **MUST NOT** use features that require a later version than it declares.

28.4.6 The **standard library** version **MUST** match the toolchain version. A program **MUST NOT** depend on a standard library version that is later than its toolchain's version.

28.4.7 The **edition** is orthogonal to the version: an edition change **MAY** be introduced in a minor version, and a crate **MAY** use an older edition with a newer toolchain.

### 28.5 Migration Rules

28.5.1 A **migration** is a change to a program that makes it compatible with a new version of the language or standard library. Migrations **MUST** be documented for each breaking change.

28.5.2 The **migration tool** is an automated tool that rewrites a program's source to be compatible with a new version. The tool **MUST** be idempotent: running it twice **MUST** produce the same result as running it once.

28.5.3 The migration tool **MUST NOT** change the program's behavior, except where the behavior was undefined or where the new version's semantics require a change.

28.5.4 A **manual migration** is required for changes that the tool cannot automate. The migration guide **MUST** describe the changes and provide examples.

28.5.5 A **deprecated feature** **MUST** have a migration path. The path **MUST** be documented and **SHOULD** be automatable by the migration tool.

28.5.6 A **breaking change** **MUST** have a migration path for at least one major version after the change. The path **MUST** be documented and **SHOULD** be automatable.

28.5.7 The **migration guide** **MUST** be published with the changelog and **MUST** be available for every supported version.
