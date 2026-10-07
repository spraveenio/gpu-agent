#!/usr/bin/env bash
# Content hash of third-party inputs that affect /opt/gpuagent-deps.
# Rebuild the builder image when this value changes.
set -euo pipefail

root=$(cd "$(dirname "$0")/../.." && pwd)
cd "$root"

if ! git rev-parse --is-inside-work-tree >/dev/null 2>&1; then
	exit 1
fi

{
	git ls-tree HEAD -- \
		sw/nic/third-party/grpc \
		sw/nic/third-party/protobuf \
		sw/nic/third-party/libzmq \
		sw/nic/third-party/abseil-cpp \
		sw/nic/third-party/boost_1_88_0
	git ls-tree -r HEAD -- sw/nic/third-party/libev-4.33
	git hash-object sw/nic/gpuagent/Makefile.lib
	git hash-object tools/build-container/install-third-party.sh
} | sha256sum | awk '{print substr($1,1,12)}'
