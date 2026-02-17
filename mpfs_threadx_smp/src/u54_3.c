/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application code running on U54_3
 *
 * PolarFire SoC Threadx SMP example
 */

#include <stdio.h>
#include <string.h>
#include "mpfs_hal/mss_hal.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"
#include "threadx/common_smp/inc/tx_api.h"

#include   "tx_api.h"
#include   "tx_thread.h"

/* Main function for the hart3(U54_3 processor).
 * Application code running on hart3 is placed here
 *
 * Simply enables soft interrupt and enters kernel to await scheduling.
 */

void u54_3(void)
{
    uint32_t dummy;

    set_csr(mie, MIP_MSIP);

    _tx_thread_smp_initialize_wait();

    for (;;)
    {
        dummy++;
    }

    /* never return */
}
