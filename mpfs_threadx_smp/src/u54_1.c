/*******************************************************************************
 * Copyright 2026 Microchip FPGA Embedded Systems Solutions.
 *
 * SPDX-License-Identifier: MIT
 *
 * @file u54_1.cpp
 *
 * @author Microchip FPGA Embedded Systems Solutions
 *
 * @brief Threadx SMP code running on U54_1.
 */

/*
 * The MPFSoC Threadx SMP port relies on atomic instructions for some
 * functionality in relation to task scheduling and resource protection. For
 * this reason it is important that any data structures and code which require
 * the use of atomic instructions are held in cacheable memory such as
 * the LIM scratchpad memory or cached DDR memory.
 *
 * This example targets the Polarfire SoC Discovoery Kit board with the default
 * design and uses LIM scratchpad memory to keep it as self contained as
 * possible. The default hardware design for the Discovery Kit does support
 * DDR and some testing has been carried out with the data elements requiring
 * atomic instructions located in DDR.
 *
 * The e51 is responsible for hardware initialisation including the DDR and
 * GPIOs and synchronising the startup of the U54s. There is an additional
 * synchronisation required to make sure U54 1 does enter the kernel until the
 * application level init code on the e51 is complete and the GPIOs are ready
 * for use.
 *
 * The example makes use of the Discovery Kit LEDs to provide some basic visual
 * indication of system operation as follows:
 *
 *  - LED 1 is toggled every 16 times the timer interrupt routine is called.
 *  - LED 2 is toggled each time thread 0 sleeps.
 *  - LED 3 is toggled every 15,000 iterations of task 1.
 *  - LEDs 4-7 indicate which core thread 0 is currently running on and are
 *    updated each time the task wakes.
 *
 * There is optional instrumentation available using the Raspberry PI GPIO pins
 * to allow following code execution via an oscilloscope or logic analyser. See
 * e51().c for details of the GPIO 2 bits used and their pin numbers on the RPi
 * connector.
 *
 * Threadx main source changes made to support SMP:
 *
 * tx_thread_initialize.c - _tx_thread_smp_protection structure is located in
 *                          scratchpad to allow atomics work correctly.
 *
 * tx_timer_initialize.c  - _tx_timer_thread and _tx_timer_thread_stack_area are
 *                          located in scratchpad to allow atomics work
 *                          correctly.
 *
 * There is one code sequence which I am a little suspicious of that is used in
 * the Threadx kernel where the _tx_thread_preempt_disable is set, the
 * TX_RESTORE macro invoked and finally  tx_thread_system_return() is called.
 * This leaves a brief window before calling tx_thread_system_return() where
 * interrupts can be enabled and particularly on core 0, this gives a
 * possibility of other kernel code being executed. I've changed this sequence
 * to use the TX_RESTORE_INT_OFF macro instead to leave interrupts disabled but
 * I'm not sure if it is really needed... Here are the files where the changes
 * are made.
 *
 * tx_thread_relinquish.c           - enter tx_thread_system_return() with
 *                                    interrupts off.
 *
 * tx_thread_smp_core_exclude.c     - enter tx_thread_system_return() with
 *                                    interrupts off.
 *
 * tx_thread_system_preempt_check.c - enter tx_thread_system_return() with
 *                                    interrupts off.
 *
 * tx_thread_system_resume.c        - enter tx_thread_system_return() with
 *                                    interrupts off.
 *
 * tx_thread_system_suspend.c       - enter tx_thread_system_return() with
 *                                    interrupts off.
 *
 * HAL changes made to support Threadx SMP
 *
 * mss_entry.S - added copy_scratch_code and copy_ddr_code functions to allow
 *               placing atomic code in cacheable memory.
 *
 *               interrupt vector set to trap_entry which is in
 *               tx_trap_handling.S
 *
 * system_startup.c - Call copy_ddr_code() once DDR is initialised. Also call
 *                    init_global_constructors().
 *
 * mss_clint.c - Don't enable interrupts when setting up timer. We do it later
 *               after kernel is initialised.
 *
 * mss_pll.c - copy-scratch_code called.
 *
 * linker script - this has sections added to support the copying to scratchpad
 *                 and ddr.
 */


#include "mpfs_hal/mss_hal.h"
#include "drivers/mss/mss_timer/mss_timer.h"
#include "drivers/mss/mss_mmuart/mss_uart.h"
#include "drivers/mss/mss_gpio/mss_gpio.h"
#include "inc/uart_mapping.h"

/*
 * This is a small demo of the high-performance ThreadX SMP kernel.  It includes
 * examples of eight threads of different priorities, using a message queue,
 * semaphore, mutex, event flags group, byte pool, and block pool.
 */

#include   "tx_api.h"
#include   "tx_thread.h"
#include   <stdio.h>

#define     DEMO_STACK_SIZE         4096
#define     DEMO_BYTE_POOL_SIZE     (4096 * 20)
#define     DEMO_BLOCK_POOL_SIZE    100
#define     DEMO_QUEUE_SIZE         100


/*
 * Define the ThreadX object control blocks...
 *
 * The thread control blocks need to be located in cacheable memory for the
 * AMOxx instructions to work correctly.
 */
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


/*
 * Define the counters used in the demo application...
 *
 * ULONG64 needed as the thread 1 and 2 counts will overflow 32 bits in a few
 * hours and mess up the printing in task 0.
 *
 */

ULONG64           thread_0_counter;
ULONG64           thread_1_counter;
ULONG64           thread_1_messages_sent;
ULONG64           thread_2_counter;
ULONG64           thread_2_messages_received;
ULONG64           thread_3_counter;
ULONG64           thread_4_counter;
ULONG64           thread_5_counter;
ULONG64           thread_6_counter;
ULONG64           thread_7_counter;


/* Define thread prototypes.  */

void    thread_0_entry(ULONG thread_input);
void    thread_1_entry(ULONG thread_input);
void    thread_2_entry(ULONG thread_input);
void    thread_3_and_4_entry(ULONG thread_input);
void    thread_5_entry(ULONG thread_input);
void    thread_6_and_7_entry(ULONG thread_input);

#if defined (DBG_PROT_LOG)
/*
 * Data for debug logging of protection functionality.
 */
uint64_t pro_in[257];
uint64_t pro_out[257];
uint64_t pro_in_count = 0;
uint64_t pro_out_count = 0;
uint64_t pro_in_total = 0;
uint64_t pro_out_total = 0;
#endif

extern uint64_t startup_rtos;

/* Define main entry point.  */

void u54_1(void)
{
    uint32_t hartid = read_csr(mhartid);
    uint32_t timer_load_value ;
    volatile uint64_t dummy;

    /*
     * Wait around until e51 is done setting up HW etc.
     */
    while(0 == startup_rtos)
    {
        dummy++;
    }

    set_csr(mie, MIP_MSIP);
    SysTick_Config();

    tx_kernel_enter();

    while(1); /* Should never get here... */
}


/* Define what the initial system looks like.  */

void    tx_application_define(void *first_unused_memory)
{
    CHAR    *pointer = TX_NULL;

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
}


/* Define the test threads.  */

void    thread_0_entry(ULONG thread_input)
{
    volatile uint64_t dummy;
    TX_INTERRUPT_SAVE_AREA
    UINT    status;

    /* This thread simply sits in while-forever-sleep loop.  */
    while(1)
    {
        /* Increment the thread counter.  */
        thread_0_counter++;

        /* Print results.  */
        printf("**** ThreadX SMP MPFSoC Demonstration ****\r\n\r\n");
        printf("           thread 0 events sent:          %lu\r\n", thread_0_counter);
        printf("           thread 1 messages sent:        %lu\r\n", thread_1_counter);
        printf("           thread 2 messages received:    %lu\r\n", thread_2_counter);
        printf("           thread 3 obtained semaphore:   %lu\r\n", thread_3_counter);
        printf("           thread 4 obtained semaphore:   %lu\r\n", thread_4_counter);
        printf("           thread 5 events received:      %lu\r\n", thread_5_counter);
        printf("           thread 6 mutex obtained:       %lu\r\n", thread_6_counter);
        printf("           thread 7 mutex obtained:       %lu\r\n\n", thread_7_counter);

        /*
         * Provide visible display of task 0 activity.
         *
         * LEDs 4-7 are used to display which hart task 0 is currently running
         * on and give a visible indication of how the task moves from hart to
         * hart as the scheduling operates.
         *
         * LED 2 is turned on before sleeping and off when we return to show how
         * frequently the task executes.
         */
        TX_DISABLE
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_18, 1);
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_20, 0);
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_21, 0);
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_22, 0);
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_23, 0);
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_20 + _tx_thread_smp_core_get(), 1);
        TX_RESTORE

        /* Sleep for 10 ticks.  */
        tx_thread_sleep(10);

        TX_DISABLE
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_18, 0);
        TX_RESTORE

        /* Set event flag 0 to wakeup thread 5.  */
        status =  tx_event_flags_set(&event_flags_0, 0x1, TX_OR);

        /* Check status.  */
        if (status != TX_SUCCESS)
            break;
    }

    while(1)
    {
        dummy++;
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

        /* Send message to queue 0.  */
        status =  tx_queue_send(&queue_0, &thread_1_messages_sent, TX_WAIT_FOREVER);

        /* Check completion status.  */
        if (status != TX_SUCCESS)
            break;

        /*
         * We need a forced delay here because on a 4 core system, thread 1 and
         * 2 will flood the system with messages and cause the scheduling to
         * break down. I believe this is due to the amount of time that the
         * system spends in the protection code with scheduling locked out
         * because the tasks can generate thousands (or even tens of thousands)
         * of messages per second.
         */
        while((dummy % 1024) != 0)
        {
            dummy++;
        }
        dummy++;

        /* Increment the message sent.  */
        thread_1_messages_sent++;

        /* Flash LED 1 periodically to show timer interrupt activity */
        MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_19, ((thread_1_counter % 30000) > 15000) & 1);
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
        /* Increment the thread counter.  */
        thread_2_counter++;

        /* Retrieve a message from the queue.  */
        status = tx_queue_receive(&queue_0, &received_message, TX_WAIT_FOREVER);

        /* Check completion status and make sure the message is what we
           expected.  */
        if ((status != TX_SUCCESS) || (received_message != thread_2_messages_received))
            break;

        /* Otherwise, all is okay.  Increment the received message count.  */
        thread_2_messages_received++;
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
        status =  tx_semaphore_get(&semaphore_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if ((status != TX_SUCCESS) && (status != TX_NO_INSTANCE))
            break;

        if (status != TX_NO_INSTANCE)
        {
            /* Sleep for 2 ticks to hold the semaphore.  */
            tx_thread_sleep(2);

            /* Release the semaphore.  */
            status =  tx_semaphore_put(&semaphore_0);
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
        status =  tx_event_flags_get(&event_flags_0, 0x1, TX_OR_CLEAR,
                                                &actual_flags, TX_WAIT_FOREVER);

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
        status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);

        /* Check status.  */
        if ((status != TX_SUCCESS) && (status != TX_NOT_AVAILABLE))
            break;

        if (status != TX_NOT_AVAILABLE)
        {
            /* Get the mutex again with suspension.  This shows
               that an owning thread may retrieve the mutex it
               owns multiple times.  */
            status =  tx_mutex_get(&mutex_0, TX_WAIT_FOREVER);
            /* Check status.  */
            if (status != TX_SUCCESS)
                break;
            /* Sleep for 2 ticks to hold the mutex.  */
            tx_thread_sleep(2);
            /* Release the mutex.  */
            status =  tx_mutex_put(&mutex_0);

            /* Check status.  */
            if (status != TX_SUCCESS)
                break;

            /* Release the mutex again.  This will actually
               release ownership since it was obtained twice.  */
            status =  tx_mutex_put(&mutex_0);

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

    /* Flash LED 1 periodically to show timer interrupt activity */
    MSS_GPIO_set_output(GPIO2_LO, MSS_GPIO_17, (value >> 4) & 1);
}

