# Cora compilation cache

It is a short content-addressed namespaces, direct
hash lookups, immutable published artifacts, and a temporary directory for
atomic publication.

## Directory layout

### The Cache

The content-addressable cache is rooted at default(`.cora-cache`), `--cache` may select another.

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

### The Build

The default build root is default(`.cora-build`), `--build` may select another.

```text
.cora-build/
    |- <build-id>/
    |   `-- <target-triple>/
    `-- <profile>/
        |-- bin/<program>
        `-- lib/<library>
```
