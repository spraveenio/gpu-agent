# Third-party libraries

These libraries are compiled **once** into the builder image at
`/opt/gpuagent-deps`. Everyday `make` / CI compiles gpuagent against that
prefix and does not rebuild them.

| Library   | Location                         | Pin (gitlink SHA; no `branch`) |
|-----------|----------------------------------|-----|
| protobuf  | submodule `protobuf`             | `5cba162a5d93f8df786d828621019e03e50edb4f` (v3.19.6) |
| gRPC      | submodule `grpc`                 | `591d56e1300b6d11948e1b821efac785a295989c` (v1.44.0) |
| Abseil    | submodule `abseil-cpp`           | `76bb24329e8bf5f39704eb10d21b9a80befa7c81` |
| ZeroMQ    | submodule `libzmq`               | `4097855ddaaa65ed7b5e8cb86d143842a594eebd` (v4.3.4) |
| Boost     | submodule `boost_1_88_0`         | `199ef13d6034c85232431130142159af3adfce22` (boost-1.88.0) |
| libev     | in-tree `libev-4.33`             | 4.33 |
| spdlog    | in-tree `spdlog` (header-only)   | not installed into the prefix |
| AMD SMI   | prebuilt `rocm/`                 | not compiled here |

`.gitmodules` has no `branch` keys. `git submodule update --init --recursive` checks out the gitlink SHA. Do not use `--remote`.

A content hash of the compiled set is `tools/build-container/deps-hash.sh`.
The builder image is tagged with that hash and `/opt/gpuagent-deps/.gpuagent-deps-<hash>`
must match before gpuagent skips a source rebuild.

## Bumping a dependency

1. Check out the new commit in the submodule, commit the gitlink SHA, and
   update the SHA comments in `.gitmodules`. Do not add a `branch` key.
2. Rebuild the builder image: `make build-container`.
3. Do **not** expect `make gpuagent` to compile third-party on the default path.
   To test a bump without rebuilding the image:

   ```bash
   make -C sw/nic/gpuagent rebuild-deps
   ```

   That installs into the workspace `sw/nic/build` prefix.
