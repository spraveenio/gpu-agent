# create container image for development environment with all necessary 
# compilation and development tools pre-installed.
.PHONY: all rhel-builder ubuntu-builder
all: rhel-builder ubuntu-builder

# create rhel9 based builder/developer container
rhel-builder:
	docker build --build-arg USER=$(shell id -un) \
		--build-arg GROUP=$(shell id -gn) \
		--build-arg BUILD_BASE_IMAGE=$(BUILD_BASE_IMAGE) \
		--build-arg GO_VERSION=$(GO_VERSION) \
		--build-arg UID=$(shell id -u) \
		--build-arg GID=$(shell id -g) \
		-t ${GPUAGENT_BLD_CONTAINER_IMAGE} \
		. -f Dockerfile.rhel9
	echo "dev container build complete : ${GPUAGENT_BLD_CONTAINER_IMAGE}"

# create ubuntu based builder/developer container
ubuntu-builder:
	docker build --build-arg USER=$(shell id -un) \
		--build-arg GROUP=$(shell id -gn) \
		--build-arg GO_VERSION=$(GO_VERSION) \
		--build-arg UID=$(shell id -u) \
		--build-arg GID=$(shell id -g) \
		-t ${GPUAGENT_BLD_CONTAINER_IMAGE_UBUNTU} \
		. -f Dockerfile.ubu2204
	echo "builder container build complete : ${GPUAGENT_BLD_CONTAINER_IMAGE_UBUNTU}"
