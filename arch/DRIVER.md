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

| Option                             | Meaning                                           | Key effect                            |
| ---------------------------------- | ------------------------------------------------- | ------------------------------------- | ----------------------------------- | ------------------------- | ---------------------- |
| `--manifest-path <file>`           | use a specific project manifest                   | selects project inputs                |
| `--target <triple>`                | target triple, such as `x86_64-unknown-linux-gnu` | object                                |
| `--profile <name>`                 | `debug`, `release`, `test`, or custom profile     | object when flags differ              |
| `-O <0..3>` / `--opt-level <0..3>` | optimization level                                | object                                |
| `--cpu <name>`                     | target CPU model                                  | object                                |
| `--features <list>`                | enabled target features                           | object                                |
| `--debug <none                     | line                                              | full>`                                | debug information                   | object                    |
| `--sanitize <list>`                | sanitizer modes                                   | object                                |
| `--pic` / `--pie`                  | position-independent code/executable              | object or link as applicable          |
| `--lto <off                        | thin                                              | full>`                                | link-time/cross-module optimization | object and link           |
| `--backend <llvm                   | c                                                 | native>`                              | backend implementation              | object                    |
| `--emit <exe                       | lib                                               | obj                                   | cir>`                               | final artifact to produce | selects stopping point |
| `-o <path>` / `--output <path>`    | explicit final output path                        | output only                           |
| `--jobs <n>`                       | maximum parallel workers                          | not a cache key                       |
| `--cache-dir <path>`               | cache root; default `.cora-cache`                 | cache location, not artifact identity |
| `--no-cache`                       | disable lookup and publication                    | no cache reads/writes                 |
| `--cache-read-only`                | allow hits but do not publish misses              | no cache writes                       |
| `--explain-cache`                  | print normalized keys and hit/miss reasons        | diagnostics only                      |
| `-v` / `--verbose`                 | show phase and external-tool information          | diagnostics only                      |
| `--diagnostic-format <text         | json>`                                            | diagnostic encoding                   | not a cache key                     |

The target triple, ABI, CPU, target features, backend, optimization/debug/
sanitizer/PIC/unwind/LTO settings, and backend toolchain identity are normalized
into `ObjectInput`. Link-only options such as library search order, soname,
entry point, and linker arguments are passed to the linker and do not invalidate
objects unless they change code generation.
