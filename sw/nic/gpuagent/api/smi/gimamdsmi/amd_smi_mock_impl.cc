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
/// smi layer mock API definitions using amd-smi apis
///
//----------------------------------------------------------------------------

#include "time.h"
#include "nic/sdk/include/sdk/base.hpp"
#include "nic/gpuagent/api/smi/smi.hpp"
#include "nic/gpuagent/api/smi/smi_api_mock_impl.hpp"
#include "nic/gpuagent/api/smi/gimamdsmi/smi_utils.hpp"

/// global array of GPU handles
static aga_gpu_handle_t g_gpu_handles[AGA_MAX_GPU];

/// amdsmi init/shutdown call counters (for test assertions)
static uint32_t g_amdsmi_init_count = 0;
static uint32_t g_amdsmi_shut_down_count = 0;

namespace aga {

uint32_t
smi_mock_get_init_count (void)
{
    return g_amdsmi_init_count;
}

uint32_t
smi_mock_get_shut_down_count (void)
{
    return g_amdsmi_shut_down_count;
}

void
smi_mock_reset_init_counters (void)
{
    g_amdsmi_init_count = 0;
    g_amdsmi_shut_down_count = 0;
}

void
smi_mock_increment_init_count (void)
{
    g_amdsmi_init_count++;
}

void
smi_mock_increment_shut_down_count (void)
{
    g_amdsmi_shut_down_count++;
}

aga_gpu_handle_t
event_buffer_get_gpu_handle (void *event_buffer_, uint32_t event_idx)
{
    amdsmi_evt_notification_data_t *event_buffer;

    event_buffer = (amdsmi_evt_notification_data_t *)event_buffer_;
    return event_buffer[event_idx].processor_handle;
}

aga_event_id_t
event_buffer_get_event_id (void *event_buffer_, uint32_t event_idx)
{
    amdsmi_evt_notification_data_t *event_buffer;

    event_buffer = (amdsmi_evt_notification_data_t *)event_buffer_;
    return aga_event_id_from_smi_event_id(event_buffer[event_idx].event);
}

char *
event_buffer_get_message (void *event_buffer_, uint32_t event_idx)
{
    amdsmi_evt_notification_data_t *event_buffer;

    event_buffer = (amdsmi_evt_notification_data_t *)event_buffer_;
    return event_buffer[event_idx].message;
}

void
gpu_gen_unique_ids (void)
{
    uint64_t high, low;

    // seed the random number generator
    srand((unsigned int)time(NULL));
    for (uint32_t i = 0; i < AGA_MOCK_NUM_GPU; i++) {
        high = (uint64_t)rand();
        low = (uint64_t)rand();
        g_gpu_handles[i] = (aga_gpu_handle_t)((high << 32) | low);
    }
}

aga_gpu_handle_t
gpu_get_handle (uint32_t gpu_idx)
{
    return g_gpu_handles[gpu_idx];
}

uint64_t
gpu_get_unique_id (uint32_t gpu_idx)
{
    return (uint64_t)g_gpu_handles[gpu_idx];
}

void *
event_get (void)
{
    static amdsmi_evt_notification_data_t event_ntfn_data;
    static uint8_t dev = 0;

    event_ntfn_data.processor_handle = g_gpu_handles[dev % AGA_MOCK_NUM_GPU];
    switch (dev%5) {
    case 0:
        event_ntfn_data.event = AMDSMI_EVT_NOTIF_RING_HANG;
        break;
    case 1:
        event_ntfn_data.event = AMDSMI_EVT_NOTIF_GPU_POST_RESET;
        break;
    case 2:
        event_ntfn_data.event = AMDSMI_EVT_NOTIF_GPU_PRE_RESET;
        break;
    case 3:
        event_ntfn_data.event = AMDSMI_EVT_NOTIF_THERMAL_THROTTLE;
        break;
    case 4:
        event_ntfn_data.event = AMDSMI_EVT_NOTIF_VMFAULT;
        break;
    default:
        break;
    }
    strncpy(event_ntfn_data.message, "test event",
            MAX_EVENT_NOTIFICATION_MSG_SIZE);
    dev++;

    return &event_ntfn_data;
}

}    // namespace aga
