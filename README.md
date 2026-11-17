# cora
The Cora Language 

## Compiler pipeline

The compiler lowers a source file through lexer tokens, AST, CIR, semantic
analysis, AIR, and the portable C backend. The semantic stage performs name
resolution, type checking, basic ownership/borrow checks, comptime integer
evaluation, and generic-instance discovery.

Build and inspect a source file:

```text
cora --emit stats source.cora
cora --emit check source.cora
cora --emit c -o source.c source.cora
```

When `-o` is omitted, `cora --emit c source.cora` writes the generated output
next to the input as `source.c`. Generic parameters are represented by their
portable C erased form (`int64_t` for unconstrained values), while generic
type aliases are emitted as C typedefs.

The generated C source uses only standard C headers and can be compiled by a
conforming C compiler.
