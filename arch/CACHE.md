# Cora compilation cache

It is a short content-addressed namespaces, direct
hash lookups, immutable published artifacts, and a temporary directory for
atomic publication.

## Directory layout

### The Cache

The content-addressable cache is rooted at default(`.cora-cache`), `--cache` may select another.

```text
.cora-cache/
|-- h/                              # hashed frontend/CIR artifacts
|   `-- <cir-key>.cir  # one self-describing file
|-- o/                              # hashed linkable object artifacts
|   `-- <obj.o      # raw target object, no sidecar
`-- t/                            # unreferenced build attempts
    `-- <build-id>-<worker-id>/
```

### The Build

The default build root is default(`.cora-build`), `--build` may select another.

```text
.cora-build/
    |- <bui
<target-triple>/
    `-- <profile>/
        |-- bin/<program>
        `-- lib/<library>
```

# On-disk cache layout

The content-addressable cache is rooted at `.cora-cache`:

```text
.cora-cache/
  m/  manifests (SHA-256 input, content, environment, and dependency hashes)
    <>
  o/  finalized object files and executables
  c/  serialized per-file CIR artifacts
  t/  temporary atomic-write staging files
```

Artifact names are keyed by SHA-256 compilation-input digests. Every `c/` or
`o/` artifact has a matching `m/` manifest; the manifest records the artifact
content hash and is checked before a cache hit is reported. Writes always land
in `t/` first and are atomically published to their final namespace.
