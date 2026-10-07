# create container image for development environment with all necessary
# compilation and development tools pre-installed, plus pinned third-party
# libs at /opt/gpuagent-deps. Rebuild when `deps-hash.sh` changes.
.PHONY: all rhel-builder ubuntu-builder
all: rhel-builder ubuntu-builder

REPO_ROOT := $(abspath ../..)
DEPS_HASH := $(shell bash $(REPO_ROOT)/tools/build-container/deps-hash.sh)

# create rhel9 based builder/developer container
rhel-builder:
	docker build --build-arg USER=$(shell id -un) \
		--build-arg GROUP=$(shell id -gn) \
		--build-arg BUILD_BASE_IMAGE=$(BUILD_BASE_IMAGE) \
		--build-arg GO_VERSION=$(GO_VERSION) \
		--build-arg GPUAGENT_DEPS_HASH=$(DEPS_HASH) \
		--build-arg UID=$(shell id -u) \
		--build-arg GID=$(shell id -g) \
		--target with-deps \
		-t ${GPUAGENT_BLD_CONTAINER_IMAGE} \
		-t ${GPUAGENT_BLD_CONTAINER_IMAGE}-$(DEPS_HASH) \
		-f Dockerfile.rhel9 \
		$(REPO_ROOT)
	echo "dev container build complete : ${GPUAGENT_BLD_CONTAINER_IMAGE} (${GPUAGENT_BLD_CONTAINER_IMAGE}-$(DEPS_HASH))"

# create ubuntu based builder/developer container
ubuntu-builder:
	docker build --build-arg USER=$(shell id -un) \
		--build-arg GROUP=$(shell id -gn) \
		--build-arg BUILD_BASE_IMAGE=$(BUILD_BASE_IMAGE) \
		--build-arg GO_VERSION=$(GO_VERSION) \
		--build-arg GPUAGENT_DEPS_HASH=$(DEPS_HASH) \
		--build-arg UID=$(shell id -u) \
		--build-arg GID=$(shell id -g) \
		--target with-deps \
		-t ${GPUAGENT_BLD_CONTAINER_IMAGE_UBUNTU} \
		-t ${GPUAGENT_BLD_CONTAINER_IMAGE_UBUNTU}-$(DEPS_HASH) \
		-f Dockerfile.ubu2204 \
		$(REPO_ROOT)
	echo "builder container build complete : ${GPUAGENT_BLD_CONTAINER_IMAGE_UBUNTU} (${GPUAGENT_BLD_CONTAINER_IMAGE_UBUNTU}-$(DEPS_HASH))"
