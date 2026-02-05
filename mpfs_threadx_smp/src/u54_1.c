/*******************************************************************************
 * Copyright 2023 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file u54_1.cpp
 *
 * @author Microchip FPGA Embedded Systems Solutions
 *
 * @brief Application code running on U54_1.
 */

#include "mpfs_hal/mss_hal.h"
#include "drivers/mss/mss_timer/mss_timer.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"
#include "drivers/mss/mss_gpio/mss_gpio.h"
#include "inc/uart_mapping.h"
/* This is a small demo of the high-performance ThreadX SMP kernel.  It includes examples of eight
   threads of different priorities, using a message queue, semaphore, mutex, event flags group,
   byte pool, and block pool.  */

#include   "tx_api.h"
#include   "tx_thread.h"
#include   <stdio.h>

#define     DEMO_STACK_SIZE         4096
#define     DEMO_BYTE_POOL_SIZE     (4096 * 20)
#define     DEMO_BLOCK_POOL_SIZE    100
#define     DEMO_QUEUE_SIZE         100


/* Define the ThreadX object control blocks...  */

TX_THREAD               thread_0  __attribute__ ((section (".l2_scratchpad")));
TX_THREAD               thread_1  __attribute__ ((section (".l2_scratchpad")));
TX_THREAD               thread_2  __attribute__ ((section (".l2_scratchpad")));
TX_THREAD               thread_3  __attribute__ ((section (".l2_scratchpad")));
TX_THREAD               thread_4  __attribute__ ((section (".l2_scratchpad")));
TX_THREAD               thread_5  __attribute__ ((section (".l2_scratchpad")));
TX_THREAD               thread_6  __attribute__ ((section (".l2_scratchpad")));
TX_THREAD               thread_7  __attribute__ ((section (".l2_scratchpad")));
TX_QUEUE                queue_0;
TX_SEMAPHORE            semaphore_0;
TX_MUTEX                mutex_0;
TX_EVENT_FLAGS_GROUP    event_flags_0;
TX_BYTE_POOL            byte_pool_0;
TX_BLOCK_POOL           block_pool_0;


/* Define the counters used in the demo application...  */

ULONG           thread_0_counter;
ULONG           thread_1_counter;
ULONG           thread_1_messages_sent;
ULONG           thread_2_counter;
ULONG           thread_2_messages_received;
ULONG           thread_3_counter;
ULONG           thread_4_counter;
ULONG           thread_5_counter;
ULONG           thread_6_counter;
ULONG           thread_7_counter;


/* Define thread prototypes.  */

void    thread_0_entry(ULONG thread_input);
void    thread_1_entry(ULONG thread_input);
void    thread_2_entry(ULONG thread_input);
void    thread_3_and_4_entry(ULONG thread_input);
void    thread_5_entry(ULONG thread_input);
void    thread_6_and_7_entry(ULONG thread_input);

uint64_t pro_in[257];
uint64_t pro_out[257];
uint64_t pro_in_count = 0;
uint64_t pro_out_count = 0;
uint64_t pro_in_total = 0;
uint64_t pro_out_total = 0;

extern uint64_t startup_rtos;

/* Define main entry point.  */

void u54_1(void)
{
    uint32_t hartid = read_csr(mhartid);
    uint32_t timer_load_value ;
    volatile uint64_t dummy;
    clear_soft_interrupt();
    set_csr(mie, MIP_MSIP);
#if 0
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

    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_17, 1);
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_18, 1);
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_19, 1);
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_20, 1);
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_21, 1);
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_22, 1);
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_23, 1);
#endif
//    set_csr(mie, MIP_MEIP);
    SysTick_Config();

    while(0 == startup_rtos)
    {
        dummy++;
    }
    tx_kernel_enter();

    while(1);

}


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{

CHAR    *pointer = TX_NULL;
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a0) = 256;
#endif

    /* Create a byte memory pool from which to allocate the thread stacks.  */
    tx_byte_pool_create(&byte_pool_0, "byte pool 0", first_unused_memory, DEMO_BYTE_POOL_SIZE);

    /* Put system definition stuff in here, e.g. thread creates and other assorted
       create information.  */

    /* Allocate the stack for thread 0.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create the main thread.  */
    tx_thread_create(&thread_0, "thread 0", thread_0_entry, 0,
            pointer, DEMO_STACK_SIZE,
            1, 1, TX_NO_TIME_SLICE, TX_AUTO_START);


    /* Allocate the stack for thread 1.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 1 and 2. These threads pass information through a ThreadX
       message queue.  It is also interesting to note that these threads have a time
       slice.  */
    tx_thread_create(&thread_1, "thread 1", thread_1_entry, 1,
            pointer, DEMO_STACK_SIZE,
            16, 16, 4, TX_AUTO_START);

    /* Allocate the stack for thread 2.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&thread_2, "thread 2", thread_2_entry, 2,
            pointer, DEMO_STACK_SIZE,
            16, 16, 4, TX_AUTO_START);

    /* Allocate the stack for thread 3.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 3 and 4.  These threads compete for a ThreadX counting semaphore.
       An interesting thing here is that both threads share the same instruction area.  */
    tx_thread_create(&thread_3, "thread 3", thread_3_and_4_entry, 3,
            pointer, DEMO_STACK_SIZE,
            8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for thread 4.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&thread_4, "thread 4", thread_3_and_4_entry, 4,
            pointer, DEMO_STACK_SIZE,
            8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for thread 5.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create thread 5.  This thread simply pends on an event flag which will be set
       by thread_0.  */
    tx_thread_create(&thread_5, "thread 5", thread_5_entry, 5,
            pointer, DEMO_STACK_SIZE,
            4, 4, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for thread 6.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    /* Create threads 6 and 7.  These threads compete for a ThreadX mutex.  */
    tx_thread_create(&thread_6, "thread 6", thread_6_and_7_entry, 6,
            pointer, DEMO_STACK_SIZE,
            8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the stack for thread 7.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_STACK_SIZE, TX_NO_WAIT);

    tx_thread_create(&thread_7, "thread 7", thread_6_and_7_entry, 7,
            pointer, DEMO_STACK_SIZE,
            8, 8, TX_NO_TIME_SLICE, TX_AUTO_START);

    /* Allocate the message queue.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_QUEUE_SIZE*sizeof(ULONG), TX_NO_WAIT);

    /* Create the message queue shared by threads 1 and 2.  */
    tx_queue_create(&queue_0, "queue 0", TX_1_ULONG, pointer, DEMO_QUEUE_SIZE*sizeof(ULONG));

    /* Create the semaphore used by threads 3 and 4.  */
    tx_semaphore_create(&semaphore_0, "semaphore 0", 1);

    /* Create the event flags group used by threads 1 and 5.  */
    tx_event_flags_create(&event_flags_0, "event flags 0");

    /* Create the mutex used by thread 6 and 7 without priority inheritance.  */
    tx_mutex_create(&mutex_0, "mutex 0", TX_NO_INHERIT);

    /* Allocate the memory for a small block pool.  */
    tx_byte_allocate(&byte_pool_0, (VOID **) &pointer, DEMO_BLOCK_POOL_SIZE, TX_NO_WAIT);

    /* Create a block memory pool to allocate a message buffer from.  */
    tx_block_pool_create(&block_pool_0, "block pool 0", sizeof(ULONG), pointer, DEMO_BLOCK_POOL_SIZE);

    /* Allocate a block and release the block memory.  */
    tx_block_allocate(&block_pool_0, (VOID **) &pointer, TX_NO_WAIT);

    /* Release the block back to the pool.  */
    tx_block_release(pointer);
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a4) = 256;
#endif

}



/* Define the test threads.  */

void    thread_0_entry(ULONG thread_input)
{
    TX_INTERRUPT_SAVE_AREA

UINT    status;


    /* This thread simply sits in while-forever-sleep loop.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_0_counter++;

        /* Print results.  */
        printf("**** ThreadX SMP Linux Demonstration **** (c) 1996-2020 Microsoft Corporation\r\n\r\n");
        printf("           thread 0 events sent:          %lu\r\n", thread_0_counter);
        printf("           thread 1 messages sent:        %lu\r\n", thread_1_counter);
        printf("           thread 2 messages received:    %lu\r\n", thread_2_counter);
        printf("           thread 3 obtained semaphore:   %lu\r\n", thread_3_counter);
        printf("           thread 4 obtained semaphore:   %lu\r\n", thread_4_counter);
        printf("           thread 5 events received:      %lu\r\n", thread_5_counter);
        printf("           thread 6 mutex obtained:       %lu\r\n", thread_6_counter);
        printf("           thread 7 mutex obtained:       %lu\r\n\n", thread_7_counter);

        /* Sleep for 10 ticks.  */

        TX_DISABLE
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_18, 1);
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_20, 0);
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_21, 0);
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_22, 0);
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_23, 0);
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_20 + _tx_thread_smp_core_get(), 1);
        TX_RESTORE

#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a0) = 4;
#endif

        tx_thread_sleep(10);

#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a4) = 4;
#endif
        TX_DISABLE
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_18, 0);
        TX_RESTORE

        /* Set event flag 0 to wakeup thread 5.  */
        status =  tx_event_flags_set(&event_flags_0, 0x1, TX_OR);
        if((_tx_thread_smp_protection.tx_thread_smp_protect_count == 1) && (_tx_thread_smp_protection.tx_thread_smp_protect_core == _tx_thread_smp_core_get()))
        {
            volatile int dummy;
#if defined(DBG_ANNO_2H)
            *((uint32_t *)0x201220a0) = 4;
#endif
            dummy++;
        }


        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }
}


void    thread_1_entry(ULONG thread_input)
{
volatile uint64_t dummy;
UINT    status;
TX_INTERRUPT_SAVE_AREA


    /* This thread simply sends messages to a queue shared by thread 2.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_1_counter++;

//        dummy = 0;
//        while(dummy!=50)
//            dummy++;

//        TX_DISABLE
//        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_19, (thread_1_counter & 1024) >> 10);
//        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_20, (thread_1_counter & 2048) >> 11);
//        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_21, (thread_1_counter & 4096) >> 12);
//        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_19, (thread_1_counter & 8192) >> 13);
//        TX_RESTORE
        /* Send message to queue 0.  */
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a0) = 8;
#endif
        status =  tx_queue_send(&queue_0, &thread_1_messages_sent, TX_WAIT_FOREVER);
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a4) = 8;
#endif
//        if((_tx_thread_smp_protection.tx_thread_smp_protect_count == 1) && (_tx_thread_smp_protection.tx_thread_smp_protect_core == _tx_thread_smp_core_get()))
//        {
//            volatile int dummy;
//#if defined(DBG_ANNO_2H)
//            *((uint32_t *)0x201220a0) = 4;
//#endif
//            dummy++;
//        }

        /* Check completion status.  */
        if (status != TX_SUCCESS)
            break;

        while((dummy % 1024) != 0)
        {
            dummy++;
        }
        dummy++;

        /* Increment the message sent.  */
        thread_1_messages_sent++;
    }

    while(1)
    {
        dummy++;
    }

}


void    thread_2_entry(ULONG thread_input)
{
    volatile uint32_t dummy;

ULONG   received_message;
UINT    status;

    /* This thread retrieves messages placed on the queue by thread 1.  */
    while(1)
    {
 //               dummy = 0;
//                while(dummy!=50)
//                    dummy++;

        /* Increment the thread counter.  */
        thread_2_counter++;

        /* Retrieve a message from the queue.  */
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a0) = 16;
#endif
        status = tx_queue_receive(&queue_0, &received_message, TX_WAIT_FOREVER);
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a4) = 16;
#endif
        if((_tx_thread_smp_protection.tx_thread_smp_protect_count == 1) && (_tx_thread_smp_protection.tx_thread_smp_protect_core == _tx_thread_smp_core_get()))
        {
            volatile int dummy;
#if defined(DBG_ANNO_2H)
            *((uint32_t *)0x201220a0) = 4;
#endif
            dummy++;
        }


        /* Check completion status and make sure the message is what we
           expected.  */
        if ((status != TX_SUCCESS) || (received_message != thread_2_messages_received))
            break;

        /* Otherwise, all is okay.  Increment the received message count.  */
        thread_2_messages_received++;
//        tx_thread_sleep(1);
    }

    while(1)
    {
        dummy++;
    }

}


void    thread_3_and_4_entry(ULONG thread_input)
{
    volatile uint32_t dummy;

UINT    status;


    /* This function is executed from thread 3 and thread 4.  As the loop
       below shows, these function compete for ownership of semaphore_0.  */
    while(1)
    {

        /* Increment the thread counter.  */
        if (thread_input == 3)
            thread_3_counter++;
        else
            thread_4_counter++;

        /*
         * Get the semaphore with suspension.
         *
         * Potential issue here as this call will fail if another core has preemption
         * disabled momentarily...
         *
         *  */
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a0) = 32;
#endif
        status =  tx_semaphore_get(&semaphore_0, TX_WAIT_FOREVER);
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a4) = 32;
#endif
        if((_tx_thread_smp_protection.tx_thread_smp_protect_count == 1) && (_tx_thread_smp_protection.tx_thread_smp_protect_core == _tx_thread_smp_core_get()))
        {
            volatile int dummy;
#if defined(DBG_ANNO_2H)
            *((uint32_t *)0x201220a0) = 4;
#endif
            dummy++;
        }


        /* Check status.  */
        if ((status != TX_SUCCESS) && (status != TX_NO_INSTANCE))
            break;

        if (status != TX_NO_INSTANCE)
        {
            /* Sleep for 2 ticks to hold the semaphore.  */
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a0) = 32;
#endif
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a4) = 32;
#endif
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a0) = 32;
#endif
            tx_thread_sleep(2);
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a4) = 32;
#endif

            /* Release the semaphore.  */
            status =  tx_semaphore_put(&semaphore_0);
            if((_tx_thread_smp_protection.tx_thread_smp_protect_count == 1) && (_tx_thread_smp_protection.tx_thread_smp_protect_core == _tx_thread_smp_core_get()))
            {
                volatile int dummy;
#if defined(DBG_ANNO_2H)
            *((uint32_t *)0x201220a0) = 4;
#endif
                dummy++;
            }


            /* Check status.  */
//            if (status != TX_SUCCESS)
//                break;
        }

        /* Check status.  */
        if ((status != TX_SUCCESS) && (status != TX_NO_INSTANCE))
            break;
    }
    while(1)
    {
        dummy++;
    }

}


void    thread_5_entry(ULONG thread_input)
{
    volatile uint32_t dummy;

UINT    status;
ULONG   actual_flags;


    /* This thread simply waits for an event in a forever loop.  */
    while(1)
    {

        /* Increment the thread counter.  */
        thread_5_counter++;

        /* Wait for event flag 0.  */
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a0) = 64;
#endif
        status =  tx_event_flags_get(&event_flags_0, 0x1, TX_OR_CLEAR,
                                                &actual_flags, TX_WAIT_FOREVER);
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a4) = 64;
#endif
        if((_tx_thread_smp_protection.tx_thread_smp_protect_count == 1) && (_tx_thread_smp_protection.tx_thread_smp_protect_core == _tx_thread_smp_core_get()))
        {
            volatile int dummy;
#if defined(DBG_ANNO_2H)
            *((uint32_t *)0x201220a0) = 4;
#endif
            dummy++;
        }


        /* Check status.  */
        if (((status != TX_SUCCESS) && (status != TX_NO_EVENTS)) || (actual_flags != 0x1))
            break;
    }
    while(1)
    {
        dummy++;
    }

}


void    thread_6_and_7_entry(ULONG thread_input)
{
    volatile uint32_t dummy;

UINT    status;


    /* This function is executed from thread 6 and thread 7.  As the loop
       below shows, these function compete for ownership of mutex_0.  */
    while(1)
    {

        /* Increment the thread counter.  */
        if (thread_input == 6)
            thread_6_counter++;
        else
            thread_7_counter++;

        /* Get the mutex with suspension.  */
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a0) = 128;
#endif
        status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a4) = 128;
#endif

        if((_tx_thread_smp_protection.tx_thread_smp_protect_count == 1) && (_tx_thread_smp_protection.tx_thread_smp_protect_core == _tx_thread_smp_core_get()))
        {
            volatile int dummy;
#if defined(DBG_ANNO_2H)
            *((uint32_t *)0x201220a0) = 4;
#endif
            dummy++;
        }

        /* Check status.  */
        if ((status != TX_SUCCESS) && (status != TX_NOT_AVAILABLE))
            break;

        if (status != TX_NOT_AVAILABLE)
        {
            /* Get the mutex again with suspension.  This shows
               that an owning thread may retrieve the mutex it
               owns multiple times.  */
            status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
            if((_tx_thread_smp_protection.tx_thread_smp_protect_count == 1) && (_tx_thread_smp_protection.tx_thread_smp_protect_core == _tx_thread_smp_core_get()))
            {
                volatile int dummy;
#if defined(DBG_ANNO_2H)
            *((uint32_t *)0x201220a0) = 4;
#endif
                dummy++;
            }

            /* Check status.  */
            if (status != TX_SUCCESS)
                break;

            if((_tx_thread_smp_protection.tx_thread_smp_protect_count == 1) && (_tx_thread_smp_protection.tx_thread_smp_protect_core == _tx_thread_smp_core_get()))
            {
                volatile int dummy;
#if defined(DBG_ANNO_2H)
            *((uint32_t *)0x201220a0) = 4;
#endif
                dummy++;
            }

            /* Sleep for 2 ticks to hold the mutex.  */
            tx_thread_sleep(2);
            if((_tx_thread_smp_protection.tx_thread_smp_protect_count == 1) && (_tx_thread_smp_protection.tx_thread_smp_protect_core == _tx_thread_smp_core_get()))
            {
                volatile int dummy;
#if defined(DBG_ANNO_2H)
            *((uint32_t *)0x201220a0) = 4;
#endif
                dummy++;
            }

            /* Release the mutex.  */
            status =  tx_mutex_put(&mutex_0);
            if(_tx_thread_smp_protection.tx_thread_smp_protect_count > 1)
            {
                volatile int dummy;
#if defined(DBG_ANNO_2H)
            *((uint32_t *)0x201220a0) = 4;
#endif
                dummy++;
            }


            /* Check status.  */
            if (status != TX_SUCCESS)
                break;

            /* Release the mutex again.  This will actually
               release ownership since it was obtained twice.  */
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a0) = 128;
#endif
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a4) = 128;
#endif
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a0) = 128;
#endif
            status =  tx_mutex_put(&mutex_0);
#if defined(DBG_ANNO_4H)
        *((uint32_t *)0x201220a4) = 128;
#endif
            if((_tx_thread_smp_protection.tx_thread_smp_protect_count == 1) && (_tx_thread_smp_protection.tx_thread_smp_protect_core == _tx_thread_smp_core_get()))
            {
                volatile int dummy;
#if defined(DBG_ANNO_2H)
            *((uint32_t *)0x201220a0) = 4;
#endif
                dummy++;
            }

            /* Check status.  */
            if (status != TX_SUCCESS)
                break;
        }
    }

    while(1)
    {
        dummy++;
    }

}


void SysTick_Handler_h1_IRQHandler()
{
    static volatile uint64_t value;

    MSS_TIM1_clear_irq(TIMER_LO);

    value++;
/*
    if(0u == value)
    {
        value = 0x01u;
    }
    else
    {
        value = 0x00u;
    }
*/

    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_17, (value >> 4) & 1);
}

/* hart1 Software interrupt handler */
void Software_h1_IRQHandler(void)
{
    uint64_t hart_id = read_csr(mhartid);
}
