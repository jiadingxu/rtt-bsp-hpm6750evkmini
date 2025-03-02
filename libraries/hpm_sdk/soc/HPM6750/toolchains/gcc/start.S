/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "hpm_csr_regs.h"

    .section .start, "ax"

    .global _start
    .type _start,@function

_start:
    /* Initialize global pointer */
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop

    /* reset mstatus to 0*/
    csrrw x0, mstatus, x0

#ifdef INIT_EXT_RAM_FOR_DATA
    la t0, _stack_in_dlm
    mv sp, t0
    call _init_ext_ram
#endif

    /* Initialize stack pointer */
    la t0, _stack
    mv sp, t0

#ifndef NO_CLEANUP_AT_START
    /* clean up */
    call _clean_up
#endif

#ifdef __nds_execit
    /* Initialize EXEC.IT table */
    la t0, _ITB_BASE_
    csrw uitb, t0
#endif

#ifdef __riscv_flen
    /* Enable FPU */
    li t0, CSR_MSTATUS_FS_MASK
    csrrs t0, mstatus, t0

    /* Initialize FCSR */
    fscsr zero
#endif

#ifndef CONFIG_FREERTOS 
    #define HANDLER_TRAP irq_handler_trap
#else
    #define HANDLER_TRAP freertos_risc_v_trap_handler
#endif

#ifndef USE_NONVECTOR_MODE
    /* Initial machine trap-vector Base */
    la t0, __vector_table
    csrw mtvec, t0

    /* Enable vectored external PLIC interrupt */
    csrsi CSR_MMISC_CTL, 2
#else
    /* Initial machine trap-vector Base */
    la t0, HANDLER_TRAP
    csrw mtvec, t0
#endif

    /* System reset handler */
    call reset_handler

    /* Infinite loop, if returned accidently */
1:    j 1b

    .section .isr_vector, "ax"
    .weak nmi_handler
nmi_handler:
1:    j 1b

    .global default_irq_handler
    .weak default_irq_handler
    .align 2
default_irq_handler:
1:    j 1b

    .macro IRQ_HANDLER irq
    .weak default_isr_\irq
    .set default_isr_\irq, default_irq_handler
    .long default_isr_\irq
    .endm

#include "../vectors.S"
