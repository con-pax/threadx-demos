/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file e51.cpp
 *
 * @author Microchip FPGA Embedded Systems Solutions
 *
 * @brief Application code running on E51
 *
 * PolarFire SoC MSS  C++ example project
 */

#include "mpfs_hal/mss_hal.h"
#include "inc/common.h"
#include "drivers/mss/mss_timer/mss_timer.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"
#include "drivers/mss/mss_gpio/mss_gpio.h"
#include "inc/uart_mapping.h"


volatile uint32_t count_sw_ints_h0 = 0U;
uint64_t startup_rtos = 0;

/* Main function for the hart0(E51 processor).
 * Application code running on hart0 is placed here
 *
 * The hart0 is used in the application for bootup and wakeup the U54_1 when
 * the application is running from LIM/eNVM memory. In case of DDR the
 * bootloader application will perform the booting and start executing this
 * application from U54_1.
 */
void e51(void)
{
volatile uint32_t dummy;

PLIC_init();
//__enable_irq();
__disable_all_irqs();
/* Reset the peripherals turn on the clocks */

mss_config_clk_rst(MSS_PERIPH_MMUART_U54_1, (uint8_t) MPFS_HAL_LAST_HART, PERIPHERAL_ON);
mss_config_clk_rst(MSS_PERIPH_TIMER, (uint8_t) MPFS_HAL_LAST_HART, PERIPHERAL_ON);

/* GPIO2 Pad Interrupt initializarion */
/*------------------------------------------*/
mss_config_clk_rst(MSS_PERIPH_GPIO2, 1, PERIPHERAL_ON) ;
MSS_GPIO_init(GPIO2_LO) ;
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_13, MSS_GPIO_INPUT_MODE |
                MSS_GPIO_IRQ_EDGE_POSITIVE );
MSS_GPIO_enable_irq(GPIO2_LO, MSS_GPIO_13);
SYSREG->GPIO_INTERRUPT_FAB_CR = 0xFFFFFFFFUL;

/*------------------------------------------*/
//    MSS_UART_init( p_uartmap_u54_1,
//                   MSS_UART_115200_BAUD,
//                   MSS_UART_DATA_8_BITS | MSS_UART_NO_PARITY | MSS_UART_ONE_STOP_BIT);

//    MSS_UART_polled_tx_string(p_uartmap_u54_1, "U54_4 UART \r\n");

PLIC_SetPriority(TIMER1_PLIC, 2);
PLIC_SetPriority(TIMER2_PLIC, 2);
PLIC_SetPriority(GPIO0_BIT13_or_GPIO2_BIT13_PLIC_13,7) ;
PLIC_EnableIRQ(GPIO0_BIT13_or_GPIO2_BIT13_PLIC_13) ;

//    MSS_GPIO_init(GPIO2_LO);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_17, MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_18, MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_19, MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_20, MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_21, MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_22, MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_23, MSS_GPIO_OUTPUT_MODE);

MSS_GPIO_config(GPIO2_LO, MSS_GPIO_0,  MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_1,  MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_2,  MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_3,  MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_4,  MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_5,  MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_6,  MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_7,  MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_8,  MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_9,  MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_10, MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_11, MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_12, MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_13, MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_14, MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_15, MSS_GPIO_OUTPUT_MODE);
MSS_GPIO_config(GPIO2_LO, MSS_GPIO_16, MSS_GPIO_OUTPUT_MODE);

MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_17, 1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_18, 1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_19, 1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_20, 1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_21, 1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_22, 1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_23, 1);

MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_0,  1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_1,  1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_2,  1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_3,  1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_4,  1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_5,  1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_6,  1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_7,  1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_8,  1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_9,  1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_10, 1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_11, 1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_12, 1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_13, 1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_14, 1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_15, 1);
MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_16, 1);

startup_rtos = 1;
/* Raise software interrupt to wake hart 1 */
//    raise_soft_interrupt(1U);
    for (;;)
    {
        dummy++;
    }
    /* never return */
}

/* hart0 Software interrupt handler */
void Software_h0_IRQHandler(void)
{
    uint64_t hart_id = read_csr(mhartid);
    count_sw_ints_h0++;
}
