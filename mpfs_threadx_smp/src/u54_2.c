/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application code running on U54_2
 *
 * PolarFire SoC Threadx SMP example
 */

#include "mpfs_hal/mss_hal.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"
#include "threadx/common_smp/inc/tx_api.h"
#include   "tx_api.h"
#include   "tx_thread.h"

/* Main function for the hart2(U54_2 processor).
 * Application code running on hart2 is placed here
 *
 * Simply enables soft interrupt and enters kernel to await scheduling.
 */
void u54_2(void)
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

