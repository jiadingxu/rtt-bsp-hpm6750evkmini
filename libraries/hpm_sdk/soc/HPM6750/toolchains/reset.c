/*
 * Copyright 2021 hpmicro
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdint.h>
#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_l1c_drv.h"
#include "hpm_interrupt.h"

#ifdef __cplusplus
extern "C" {
#endif
extern void system_init(void);
extern void __libc_init_array(void);
extern void __libc_fini_array(void);
#ifdef __cplusplus
}
#endif

__attribute__((weak)) void _clean_up(void)
{
    /* clean up plic, it will help while debuging */
    disable_irq_from_intc();
    intc_m_set_threshold(0);
    for (uint32_t irq = 0; irq < 128; irq++) {
        intc_m_complete_irq(irq);
    }
    /* clear any bits left in plic enable regster */
    for (uint32_t i = 0; i < 4; i++) {
        *(volatile uint32_t *)(HPM_PLIC_BASE + HPM_PLIC_ENABLE_OFFSET + (i << 2)) = 0;
    }
}

__attribute__((weak)) void c_startup(void)
{
    uint32_t i, size;
#ifdef FLASH_XIP
    extern uint32_t __vector_ram_start__, __vector_ram_end__, __vector_load_addr__;
    size = &__vector_ram_end__ - &__vector_ram_start__;
    for (i = 0; i < size; i++) {
        *(&__vector_ram_start__ + i) = *(&__vector_load_addr__ + i);
    }
#endif

    extern uint32_t __bss_start__, __bss_end__;
    extern uint32_t __etext, __data_start__, __data_end__;
    size = &__bss_end__ - &__bss_start__;
    for (i = 0; i < size; i++) {
        *(&__bss_start__ + i) = 0;
    }

    size = &__data_end__ - &__data_start__;
    for (i = 0; i < size; i++) {
        *(&__data_start__ + i) = *(&__etext + i);
    }

    extern uint32_t __noncacheable_bss_start__, __noncacheable_bss_end__;
    extern uint32_t __noncacheable_init_start__, __noncacheable_init_end__;
    for (i = 0; i < (&__noncacheable_bss_end__ - &__noncacheable_bss_start__); i++) {
        *(&__noncacheable_bss_start__ + i) = 0;
    }

    for (i = 0; i < (&__noncacheable_init_end__ - &__noncacheable_init_start__); i++) {
        *(&__noncacheable_init_start__ + i) = *(&__etext + size + 1 + i);
    }
}

__attribute__((weak)) int main(void)
{
    while(1);
}

__attribute__((weak)) void reset_handler(void)
{
    l1c_dc_disable();
#ifndef __SEGGER_RTL_VERSION
    /*
     * Initialize LMA/VMA sections.
     * Relocation for any sections that need to be copied from LMA to VMA.
     */
    c_startup();
#endif

    /* Call platform specific hardware initialization */
    system_init();

#ifdef __cplusplus
    /* Do global constructors */
    __libc_init_array();
#endif

    /* Entry function */
    main();
}

/*
 * When compiling C++ code with static objects, the compiler inserts
 * a call to __cxa_atexit() with __dso_handle as one of the arguments.
 * The dummy versions of these symbols should be provided.
 */
void __cxa_atexit(void (*arg1)(void*), void* arg2, void* arg3)
{
}

void*   __dso_handle = (void*) &__dso_handle;

__attribute__((weak)) void _init()
{
}
