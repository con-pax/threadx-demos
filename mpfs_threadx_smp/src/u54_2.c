/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * Application code running on U54_2
 *
 */

#include "mpfs_hal/mss_hal.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"
// #include "threadx/common_smp/inc/tx_api.h"
#include "tx_api.h"
#include "tx_thread.h"

volatile uint32_t count_sw_ints_h2 = 0U;

/* Main function for the hart2(U54_2 processor).
 * Application code running on hart2 is placed here
 *
 * The hart2 goes into WFI. hart0 brings it out of WFI when it raises the first
 * Software interrupt to this hart
 */
void u54_2(void)
{
#if 1
    _tx_thread_smp_initialize_wait();
#else
    uint64_t hartid = read_csr(mhartid);
    volatile uint32_t icount = 0U;

    /* Clear pending software interrupt in case there was any.
       Enable only the software interrupt so that the E51 core can bring this
       core out of WFI by raising a software interrupt. */

    clear_soft_interrupt();
    set_csr(mie, MIP_MSIP);
#if (IMAGE_LOADED_BY_BOOTLOADER == 0)
    /* Put this hart in WFI */

    do
    {
        __asm("wfi");
    }while(0 == (read_csr(mip) & MIP_MSIP));
#endif
    /* The hart is out of WFI, clear the SW interrupt. Here onwards application
     * can enable and use any interrupts as required */

    clear_soft_interrupt();

    __enable_irq();

    cout<<"Hello World from u54 core 2 - hart2.\n\r"<<endl;

    /* Raise software interrupt to wake hart 3 */
    raise_soft_interrupt(3U);
#endif
    for (;;)
    {

    }

    /* never return */
}

/* hart2 Software interrupt handler */

void Software_h2_IRQHandler(void)
{
    uint64_t hart_id = read_csr(mhartid);
    count_sw_ints_h2++;
}
