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
/// RAII helper that owns one amdsmi_init/amdsmi_shut_down lifetime
///
/// This is foundational scaffolding for moving the gim amdsmi backend from a
/// persistent (init-once-at-startup, never-shut-down) model to a per-request
/// session model that releases /dev/gim_smi between calls.  This file alone
/// does not change runtime behavior; subsequent PRs wire it into
/// smi_state::init() and the public smi_api.cc entry points.
///
//----------------------------------------------------------------------------

#ifndef __AGA_API_SMI_GIMAMDSMI_SESSION_HPP__
#define __AGA_API_SMI_GIMAMDSMI_SESSION_HPP__

#include <mutex>
#include <vector>
extern "C" {
#include "nic/third-party/rocm/gim_amd_smi_lib/include/amd_smi/amdsmi.h"
}
#include "nic/sdk/include/sdk/base.hpp"

namespace aga {

/// \defgroup AGA_SMI_SESSION - gim amdsmi RAII session helper
/// \ingroup AGA
/// @{

/// \brief RAII wrapper around one amdsmi_init / amdsmi_shut_down lifetime
///
/// On construction, acquires a process-wide mutex and calls amdsmi_init().
/// On destruction, calls amdsmi_shut_down() and releases the mutex.  Nested
/// sessions on the same thread are reference counted - only the outermost
/// session actually initializes or shuts down the library.
///
/// Typical use (per-request):
/// \code
///     smi_session s;
///     if (!s.ok()) {
///         return s.ret();
///     }
///     // use s.processor_handles() to get the live handles for this session
/// \endcode
class smi_session {
public:
    /// \brief ctor - acquires the global init lock and runs amdsmi_init
    smi_session();

    /// \brief dtor - runs amdsmi_shut_down and releases the global init lock
    ~smi_session();

    /// non-copyable / non-movable - sessions are tied to a scope
    smi_session(const smi_session &) = delete;
    smi_session &operator=(const smi_session &) = delete;
    smi_session(smi_session &&) = delete;
    smi_session &operator=(smi_session &&) = delete;

    /// \brief    true iff amdsmi_init succeeded for this session
    bool ok(void) const { return ok_; }

    /// \brief    sdk_ret_t form of the underlying amdsmi_init status
    sdk_ret_t ret(void) const { return ret_; }

    /// \brief    raw amdsmi_init status (for logging / mapping)
    amdsmi_status_t amdsmi_ret(void) const { return amdsmi_ret_; }

    /// \brief    enumerate processor handles owned by this session
    /// \param[out] handles   vector populated with live processor handles
    /// \return SDK_RET_OK or error status; handles is valid only for the
    ///         lifetime of this smi_session instance
    sdk_ret_t processor_handles(
                  std::vector<amdsmi_processor_handle> *handles) const;

private:
    /// process-wide guard so concurrent threads do not race on
    /// amdsmi_init / amdsmi_shut_down (the library is not re-entrant)
    static std::mutex& init_mutex_(void);

    /// process-wide nesting depth so that nested sessions on the same
    /// outermost scope skip re-init and re-shutdown
    static uint32_t& depth_(void);

    /// true if this session owns the underlying init (depth went 0 -> 1)
    bool owner_;
    /// true if amdsmi_init for this session succeeded
    bool ok_;
    /// raw amdsmi status from amdsmi_init
    amdsmi_status_t amdsmi_ret_;
    /// sdk_ret_t form of amdsmi_ret_
    sdk_ret_t ret_;
};

/// \@}

}    // namespace aga

#endif    // __AGA_API_SMI_GIMAMDSMI_SESSION_HPP__
