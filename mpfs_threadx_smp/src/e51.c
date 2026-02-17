/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
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

uint64_t startup_rtos = 0; /* Holds off other harts until HW is setup */

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
    __disable_all_irqs();

    /* Reset the peripherals turn on the clocks */

    mss_config_clk_rst(MSS_PERIPH_MMUART_U54_1, 1, PERIPHERAL_ON);
    mss_config_clk_rst(MSS_PERIPH_GPIO2,        1, PERIPHERAL_ON) ;

    /*
     * MSS GPIO 2 is used to drive the LEDs on the Discovery Kit and also to
     * drive 17 GPIO pins on the Raspberry Pi connector for code instrumentation
     */
    MSS_GPIO_init(GPIO2_LO) ;

    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_0,  MSS_GPIO_OUTPUT_MODE);  // E12 - RPi-GPIO4  - J10 Pin 07
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_1,  MSS_GPIO_OUTPUT_MODE);  // G18 - RPi-GPIO17 - J10 Pin 11
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_2,  MSS_GPIO_OUTPUT_MODE);  // E10 - RPi-GPIO18 - J10 Pin 12
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_3,  MSS_GPIO_OUTPUT_MODE);  // F21 - RPi-GPIO27 - J10 Pin 13
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_4,  MSS_GPIO_OUTPUT_MODE);  // F20 - RPi-GPIO22 - J10 Pin 15
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_5,  MSS_GPIO_OUTPUT_MODE);  // E21 - RPi-GPIO23 - J10 Pin 16
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_6,  MSS_GPIO_OUTPUT_MODE);  // F19 - RPi-GPIO24 - J10 Pin 18
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_7,  MSS_GPIO_OUTPUT_MODE);  // A19 - RPi-GPIO25 - J10 Pin 22
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_8,  MSS_GPIO_OUTPUT_MODE);  // D17 - RPi-GPIO5  - J10 Pin 29
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_9,  MSS_GPIO_OUTPUT_MODE);  // D18 - RPi-GPIO6  - J10 Pin 31
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_10, MSS_GPIO_OUTPUT_MODE);  // B20 - RPi-GPIO12 - J10 Pin 32
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_11, MSS_GPIO_OUTPUT_MODE);  // A15 - RPi-GPIO13 - J10 Pin 33
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_12, MSS_GPIO_OUTPUT_MODE);  // B19 - RPi-GPIO19 - J10 Pin 35
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_13, MSS_GPIO_OUTPUT_MODE);  // B15 - RPi-GPIO16 - J10 Pin 36
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_14, MSS_GPIO_OUTPUT_MODE);  // B17 - RPi-GPIO26 - J10 Pin 37
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_15, MSS_GPIO_OUTPUT_MODE);  // B14 - RPi-GPIO20 - J10 Pin 38
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_16, MSS_GPIO_OUTPUT_MODE);  // E13 - RPi-GPIO21 - J10 Pin 40

    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_17, MSS_GPIO_OUTPUT_MODE); // T18  - LED1
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_18, MSS_GPIO_OUTPUT_MODE); // V17  - LED2
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_19, MSS_GPIO_OUTPUT_MODE); // U20  - LED3
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_20, MSS_GPIO_OUTPUT_MODE); // U21  - LED4
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_21, MSS_GPIO_OUTPUT_MODE); // AA18 - LED5
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_22, MSS_GPIO_OUTPUT_MODE); // V16  - LED6
    MSS_GPIO_config(GPIO2_LO, MSS_GPIO_23, MSS_GPIO_OUTPUT_MODE); // U15  - LED7

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

    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_17, 1);
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_18, 1);
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_19, 1);
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_20, 1);
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_21, 1);
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_22, 1);
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_23, 1);

    startup_rtos = 1;   /* let the other harts move on */

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
