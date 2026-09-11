# GPU Agent provides programmable APIs to configure and monitor AMD Instinct GPUs
[![Trivy security scan](https://github.com/spraveenio/gpu-agent/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/spraveenio/gpu-agent/actions/workflows/ci.yml?query=branch%3Amain)

## To build GPU Agent, follow the steps below:

### setup workspace (required once)

```bash
$ git submodule update --init  --recursive -f
```

### create build container image (required once)

```bash
$ make build-container
```

### Building artifacts
  
  Follow either of the two methods below to build gpuagent and gpuctl binaries
  
  #### Manual Steps
  
  vendor setup workspace for manual building (required once)

  - choose build/developer environment
      - rhel9 
      ```bash
      [user@host]# GPUAGENT_BLD_CONTAINER_IMAGE=gpuagent-builder-rhel:9 make docker-shell
      ```

      - ubuntu 22.04
      ```bash
       [user@host]# GPUAGENT_BLD_CONTAINER_IMAGE=gpuagent-bldr-ubuntu:22.04 make docker-shell
      ```

  - golang dependency setup (required once)
      ```bash
      [user@build-container ]# make gopkglist
      ```
  - golang vendor setup
      ```bash
      [user@build-container ]# cd sw/nic/gpuagent/
      [user@build-container ]# go mod vendor
      ```

  - bild gpuagent (within build-container)
      ```bash
      [user@build-container ]# make
      ```

  #### Full target build in single step (from host)

  Choose build base os

  - rhel9
    ```bash
    [user@host]# GPUAGENT_BLD_CONTAINER_IMAGE=gpuagent-builder-rhel:9 make gpuagent
    ```

  - ubuntu 22.04
    ```bash
    [user@host]# GPUAGENT_BLD_CONTAINER_IMAGE=gpuagent-bldr-ubuntu:22.04 make gpuagent
    ```

### Artifacts location
  - gpuagent binary can be found at - ${TOP_DIR}/sw/nic/build/x86_64/sim/bin/gpuagent
  - gpuctl binary can be found at - ${TOP_DIR}/sw/nic/build/x86_64/sim/bin/gpuctl

### To clean the build artifacts (run it within build-container)

```bash
[root@dev gpu-agent]# make -C sw/nic/gpuagent clean
[root@dev gpu-agent]# 
```

# Things to note
 - For updating any amdsmi library to any other version, make sure the libamdsmi.so libraries are built correctly and are available in sw/nic/build/x86_64/sim/lib/ path. These are required during runtime, mismatch in library version may lead to runtime issues. These libraries are built from [amdsmi git](https://github.com/rocm/amdsmi/). The commit/tag the current gpuagent is built on can be found in [file](sw/nic/third-party/rocm/amd_smi_lib/version.txt)
 - apply patches on amdsmi found in [here](patch/amdsmi)
 - amdsmi build instructions are available [here](sw/nic/gpuagent/api/smi/amdsmi/README.md)

# Troubleshooting
 - If you face any issue with golang dependencies, re-run `make gopkglist` and `go mod vendor` command.
 - some go files are generated during build time, if you face any issue related to missing files, run `make gpuagent` command within build-container, then re-run `go mod vendor` command.
