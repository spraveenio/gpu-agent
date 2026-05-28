//
// Copyright(C) Advanced Micro Devices, Inc. All rights reserved.
//
// You may not use this software and documentation (if any) (collectively,
// the "Materials") except in compliance with the terms and conditions of
// the Software License Agreement included with the Materials or otherwise as
// set forth in writing and signed by you and an authorized signatory of AMD.
// If you do not have a copy of the Software License Agreement, contact your
// AMD representative for a copy.
//
// You agree that you will not reverse engineer or decompile the Materials,
// in whole or in part, except as allowed by applicable law.
//
// THE MATERIALS ARE DISTRIBUTED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OR
// REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.
//
//----------------------------------------------------------------------------
///
/// \file
/// implementation of smi_session - RAII helper around amdsmi_init/shut_down
///
//----------------------------------------------------------------------------

#include "nic/gpuagent/core/trace.hpp"
#include "nic/gpuagent/api/smi/gimamdsmi/smi_session.hpp"
#include "nic/gpuagent/api/smi/gimamdsmi/smi_utils.hpp"

namespace aga {

std::recursive_mutex&
smi_session::init_mutex_ (void)
{
    // Meyers' singleton - thread-safe initialization in C++11+
    static std::recursive_mutex m;
    return m;
}

uint32_t&
smi_session::depth_ (void)
{
    // protected by init_mutex_(); modified only while the lock is held
    static uint32_t d = 0;
    return d;
}

smi_session::smi_session ()
    : owner_(false), ok_(false),
      amdsmi_ret_(AMDSMI_STATUS_SUCCESS), ret_(SDK_RET_OK)
{
    init_mutex_().lock();
    if (depth_() == 0) {
        // outermost session - actually initialize amdsmi
        owner_ = true;
        amdsmi_ret_ = amdsmi_init(AMDSMI_INIT_AMD_GPUS);
        if (amdsmi_ret_ != AMDSMI_STATUS_SUCCESS) {
            ret_ = amdsmi_ret_to_sdk_ret(amdsmi_ret_);
            AGA_TRACE_ERR("Failed to amdsmi_init for smi_session, err {}",
                          amdsmi_ret_);
            // keep the lock released; nothing to balance on destruction
            init_mutex_().unlock();
            return;
        }
        ok_ = true;
        depth_() = 1;
    } else {
        // nested session - reuse the existing init
        depth_()++;
        ok_ = true;
    }
}

smi_session::~smi_session ()
{
    if (!ok_) {
        // ctor already released the lock on init failure
        return;
    }
    if (owner_) {
        amdsmi_status_t s = amdsmi_shut_down();
        if (s != AMDSMI_STATUS_SUCCESS) {
            AGA_TRACE_ERR("amdsmi_shut_down failed for smi_session, err {}",
                          s);
        }
        depth_() = 0;
    } else {
        depth_()--;
    }
    init_mutex_().unlock();
}

sdk_ret_t
smi_session::processor_handles (
    std::vector<amdsmi_processor_handle> *handles) const
{
    uint32_t socket_count = 0;
    amdsmi_status_t s;

    if (!ok_ || handles == NULL) {
        return SDK_RET_INVALID_ARG;
    }
    handles->clear();

    // enumerate sockets then processor handles per socket
    s = amdsmi_get_socket_handles(&socket_count, NULL);
    if (s != AMDSMI_STATUS_SUCCESS) {
        AGA_TRACE_ERR("amdsmi_get_socket_handles count query failed, err {}",
                      s);
        return amdsmi_ret_to_sdk_ret(s);
    }
    if (socket_count == 0) {
        return SDK_RET_OK;
    }
    std::vector<amdsmi_socket_handle> sockets(socket_count);
    s = amdsmi_get_socket_handles(&socket_count, sockets.data());
    if (s != AMDSMI_STATUS_SUCCESS) {
        AGA_TRACE_ERR("amdsmi_get_socket_handles fetch failed, err {}", s);
        return amdsmi_ret_to_sdk_ret(s);
    }
    for (uint32_t i = 0; i < socket_count; i++) {
        uint32_t proc_count = 0;
        s = amdsmi_get_processor_handles(sockets[i], &proc_count, NULL);
        if (s != AMDSMI_STATUS_SUCCESS) {
            AGA_TRACE_ERR("amdsmi_get_processor_handles count query failed "
                          "for socket {}, err {}", i, s);
            return amdsmi_ret_to_sdk_ret(s);
        }
        if (proc_count == 0) {
            continue;
        }
        std::vector<amdsmi_processor_handle> procs(proc_count);
        s = amdsmi_get_processor_handles(sockets[i], &proc_count,
                                         procs.data());
        if (s != AMDSMI_STATUS_SUCCESS) {
            AGA_TRACE_ERR("amdsmi_get_processor_handles fetch failed for "
                          "socket {}, err {}", i, s);
            return amdsmi_ret_to_sdk_ret(s);
        }
        for (uint32_t j = 0; j < proc_count; j++) {
            handles->push_back(procs[j]);
        }
    }
    return SDK_RET_OK;
}

}    // namespace aga
