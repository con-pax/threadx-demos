/***********************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application code running on U54_4
 *
 * PolarFire SoC Threadx SMP example
 */

#include <stdio.h>
#include <string.h>
#include "mpfs_hal/mss_hal.h"

#include   "tx_api.h"
#include   "tx_thread.h"
/* Main function for the hart1(U54_4 processor).
 * Application code running on hart1 is placed here.
 *
 * Simply enables soft interrupt and enters kernel to await scheduling.
 */
void u54_4(void)
{
    uint32_t dummy;

    _tx_thread_smp_initialize_wait();

    set_csr(mie, MIP_MSIP);

    for (;;)
    {
        dummy++;
    }

    /* never return */
}
