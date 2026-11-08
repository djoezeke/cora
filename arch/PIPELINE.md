# Cora compilation pipeline

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
