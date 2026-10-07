# Developer Guide

This document different components for developers working on the AMD GPU-Agent enhancement.

## Build Prerequisites

Before starting, ensure you have Docker installed and running with the user permissions set appropriately.

## Environment and Build Setup

Refer to the [README.md](README.md).

Third-party C++ libraries (protobuf, gRPC, Abseil, ZeroMQ, libev, Boost) are
baked into the builder image at `/opt/gpuagent-deps`. Rebuild that image when
those pins change; do not expect `make` inside the container to compile them on
the default path. Details: [sw/nic/third-party/README.md](sw/nic/third-party/README.md).

# Architecture

## API layer
- North Bound [API definitions](sw/nic/gpuagent/protos)
- Internal gpuagent [Model Definitions](sw/nic/gpuagent/api)

## Service layer
gRPC services are being handled through service layer [svc](sw/nic/gpuagent/svc)

## Data Abstraction layer
[smi](sw/nic/gpuagent/api/smi) layer is responsible for 
populating/retrieving data from the clients through libraries. 
This takes care of translating internal data
[models](sw/nic/gpuagent/api/include) to respective protobuf payloads for the
northbound definitions.

### Data clients
- amdsmi : data obtained through [libamdsmi](sw/nic/gpuagent/api/smi/amdsmi/smi_api.cc)

```mermaid
sequenceDiagram
   actor user/client
   user/client ->> gpuagent : gRPC request
   gpuagent ->> svc : data marshal
   svc ->> smi : smi call
   smi ->> libamdsmi : amdsmi call
   libamdsmi ->> gpudriver : AMD GPU HW
   gpudriver -->> libamdsmi : driver response
   libamdsmi -->> smi : amdsmi response
   smi -->> svc : smi response
   svc -->> gpuagent : data unmarshal
   gpuagent -->> user/client : gRPC response
```

