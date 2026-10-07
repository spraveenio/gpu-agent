#!/usr/bin/env bash
# Build pinned third-party libs into GPUAGENT_DEPS_PREFIX (default /opt/gpuagent-deps).
set -euo pipefail

PREFIX="${GPUAGENT_DEPS_PREFIX:-/opt/gpuagent-deps}"
script_dir=$(cd "$(dirname "$0")" && pwd)
repo_root=$(cd "${script_dir}/../.." && pwd)

ROOT="${THIRD_PARTY_ROOT:-${repo_root}/sw/nic/third-party}"
MAKEFILE_LIB="${MAKEFILE_LIB:-${repo_root}/sw/nic/gpuagent/Makefile.lib}"

require_dir() {
	local p=$1
	if [[ ! -d "$p" ]] || [[ -z "$(ls -A "$p" 2>/dev/null || true)" ]]; then
		echo "error: third-party sources missing or empty: $p" >&2
		echo "run: git submodule update --init --recursive" >&2
		exit 1
	fi
}

require_dir "${ROOT}/protobuf"
require_dir "${ROOT}/grpc"
require_dir "${ROOT}/libzmq"
require_dir "${ROOT}/abseil-cpp"
require_dir "${ROOT}/boost_1_88_0"
require_dir "${ROOT}/libev-4.33"

if [[ ! -f "$MAKEFILE_LIB" ]]; then
	echo "error: Makefile.lib not found: $MAKEFILE_LIB" >&2
	exit 1
fi

mkdir -p "$PREFIX"

export PREFIX
export BLD_DIR="$PREFIX"
export BLD_BIN_DIR="${BLD_BIN_DIR:-${PREFIX}/bin}"
export PROTOBUF_DIR="${ROOT}/protobuf"
export GRPC_DIR="${ROOT}/grpc"
export ZEROMQ_DIR="${ROOT}/libzmq"
export LIBEV_DIR="${ROOT}/libev-4.33"
export BOOST_DIR="${ROOT}/boost_1_88_0"
export ABSEIL_DIR="${ROOT}/abseil-cpp"

echo "Installing third-party libs into ${PREFIX}"
make -f "$MAKEFILE_LIB" third-party-libs

if [[ -n "${GPUAGENT_DEPS_HASH:-}" ]]; then
	echo "${GPUAGENT_DEPS_HASH}" > "${PREFIX}/.gpuagent-deps-hash"
	touch "${PREFIX}/.gpuagent-deps-${GPUAGENT_DEPS_HASH}"
fi
