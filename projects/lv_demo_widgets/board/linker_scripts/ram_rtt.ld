/*
 * Copyright 2021 hpmicro
 * SPDX-License-Identifier: BSD-3-Clause
 */

ENTRY(_start)

STACK_SIZE = DEFINED(_stack_size) ? _stack_size : 0x4000;
HEAP_SIZE = DEFINED(_heap_size) ? _heap_size : 1M;
SDRAM_SIZE = DEFINED(_sdram_size) ? _sdram_size : 16M;
NONCACHEABLE_SIZE = DEFINED(_noncacheable_size) ? _noncacheable_size : 4M;

MEMORY
{
    ILM (wx) : ORIGIN = 0, LENGTH = 256K
    DLM (w) : ORIGIN = 0x80000, LENGTH = 256K
    /* It's alias address of core0 ILM+DLM, but accessing via system bus */
    CORE0_LM_SLV (wx) : ORIGIN = 0x1000000, LENGTH = 512K
    /* It's alias address of core1 ILM+DLM, but accessing via system bus */
    CORE1_LM_SLV (wx) : ORIGIN = 0x1180000, LENGTH = 512K
    AXI_SRAM (wx) : ORIGIN = 0x1080000, LENGTH = 1536K
    SDRAM (wx) : ORIGIN = 0x40000000, LENGTH = SDRAM_SIZE - NONCACHEABLE_SIZE
    NONCACHEABLE (wx) : ORIGIN = 0x40000000 + SDRAM_SIZE - NONCACHEABLE_SIZE, LENGTH = NONCACHEABLE_SIZE
}

SECTIONS
{
    .vectors : {
        . = ALIGN(8);
        KEEP(*(.isr_vector))
        KEEP(*(.vector_table))
        . = ALIGN(8);
    } > AXI_SRAM

    .start : {
        . = ALIGN(8);
        KEEP(*(.start))
    } > AXI_SRAM

    .text : {
        . = ALIGN(8);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.srodata)
        *(.srodata*)

        *(.hash)
        *(.dyn*)
        *(.gnu*)
        *(.pl*)

        KEEP(*(.eh_frame))
        *(.eh_frame*)

        KEEP (*(.init))
        KEEP (*(.fini))
        . = ALIGN(8);

        /*********************************************
         *
         *      RT-Thread related sections - Start
         *
        *********************************************/
        /* section information for finsh shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;
        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;
        . = ALIGN(4);

        . = ALIGN(4);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;
        . = ALIGN(4);

        /* section information for modules */
        . = ALIGN(4);
        __rtmsymtab_start = .;
        KEEP(*(RTMSymTab))
        __rtmsymtab_end = .;

        /* RT-Thread related sections - end */

        PROVIDE (__etext = .);
        PROVIDE (_etext = .);
        PROVIDE (etext = .);
    } > AXI_SRAM

    .rel : {
        KEEP(*(.rel*))
    } > AXI_SRAM


    .data : AT(etext) {
        . = ALIGN(8);
        __data_start__ = .;
        __global_pointer$ = . + 0x800;
        *(.data)
        *(.data*)
        *(.sdata)
        *(.sdata*)
        *(.tdata)
        *(.tdata*)

        KEEP(*(.jcr))
        KEEP(*(.dynamic))
        KEEP(*(.got*))
        KEEP(*(.got))
        __data_end__ = .;
        KEEP(*(.gcc_execpt_table))
        KEEP(*(.gcc_execpt_table.*))

        . = ALIGN(8);
        PROVIDE(__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE(__preinit_array_end = .);

        . = ALIGN(8);
        PROVIDE(__init_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE(__init_array_end = .);

        . = ALIGN(8);
        PROVIDE(__finit_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.finit_array.*)))
        KEEP(*(.finit_array))
        PROVIDE(__finit_array_end = .);

        . = ALIGN(8);
        KEEP(*crtbegin*.o(.ctors))
        KEEP(*(EXCLUDE_FILE (*crtend*.o) .ctors))
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))

        . = ALIGN(8);
        KEEP(*crtbegin*.o(.dtors))
        KEEP(*(EXCLUDE_FILE (*crtend*.o) .dtors))
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))

        . = ALIGN(8);
        PROVIDE (__edata = .);
        PROVIDE (_edata = .);
        PROVIDE (edata = .);
    } > DLM

    .noncacheable (NOLOAD) : {
        . = ALIGN(1024);
        KEEP(*(.noncacheable))
        . = ALIGN(1024);

    } > NONCACHEABLE
    __noncacheable_start__ = ORIGIN(NONCACHEABLE);
    __noncacheable_end__ = ORIGIN(NONCACHEABLE) + LENGTH(NONCACHEABLE);

    .bss : {
        . = ALIGN(8);
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(.tbss*)
        *(.sbss*)
        *(.scommon)
        *(.scommon*)
        *(.tcommon*)
        *(.dynsbss*)
        *(COMMON)
        . = ALIGN(8);
        _end = .;
        __bss_end__ = .;
    } > DLM

    .stack : {
        . = ALIGN(8);
        __stack_base__ = .;
        . += STACK_SIZE;
        PROVIDE (_stack = .);
        PROVIDE (_stack_in_dlm = .);
    } > DLM

    .framebuffer (NOLOAD) : {
        KEEP(*(.framebuffer))
    } > SDRAM

    .heap : {
        . = ALIGN(8);
        __heap_start__ = .;
        . += HEAP_SIZE;
        __heap_end__ = .;

    } > SDRAM
}
