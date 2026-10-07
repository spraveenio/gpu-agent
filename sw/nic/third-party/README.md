# Third-party libraries

These libraries are compiled **once** into the builder image at
`/opt/gpuagent-deps`. Everyday `make` / CI compiles gpuagent against that
prefix and does not rebuild them.

| Library   | Location                         | Pin |
|-----------|----------------------------------|-----|
| protobuf  | submodule `protobuf`             | v3.19.6 (`5cba162a5d93`) |
| gRPC      | submodule `grpc`                 | v1.44.0 (`591d56e1300b`) |
| Abseil    | submodule `abseil-cpp`           | gitlink `76bb24329e8b` (not `master`) |
| ZeroMQ    | submodule `libzmq`               | v4.3.4 (`4097855ddaaa`) |
| Boost     | submodule `boost_1_88_0`         | boost-1.88.0 (`199ef13d6034`) |
| libev     | in-tree `libev-4.33`             | 4.33 |
| spdlog    | in-tree `spdlog` (header-only)   | not installed into the prefix |
| AMD SMI   | prebuilt `rocm/`                 | not compiled here |

A content hash of the compiled set is `tools/build-container/deps-hash.sh`.
The builder image is tagged with that hash and `/opt/gpuagent-deps/.gpuagent-deps-<hash>`
must match before gpuagent skips a source rebuild.

## Bumping a dependency

1. Update the submodule (or libev tree) and commit the gitlink.
2. Rebuild the builder image: `make build-container`.
3. Do **not** expect `make gpuagent` to compile third-party on the default path.
   To test a bump without rebuilding the image:

   ```bash
   make -C sw/nic/gpuagent rebuild-deps
   ```

   That installs into the workspace `sw/nic/build` prefix.
