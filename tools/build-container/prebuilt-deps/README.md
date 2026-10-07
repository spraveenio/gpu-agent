This directory holds an optional `gpuagent-deps.tar.gz` sysroot used by CI as a
bridge cache when the builder-image layer cache misses. The tarball is gitignored
and produced from `/opt/gpuagent-deps` after a successful image build.
