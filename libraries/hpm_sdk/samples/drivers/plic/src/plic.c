/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "board.h"
#include "hpm_clock_drv.h"
#include "hpm_gpio_drv.h"
#include "hpm_mchtmr_drv.h"
#include "hpm_ptpc_drv.h"

void test_mchtmr_interrupt(void);
void test_plicsw_interrupt(void);
uint32_t mchtmr_freq = 0;

void isr_gpio(void)
{
    printf("gpio interrupt start\n");
    gpio_clear_pin_interrupt_flag(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
    gpio_toggle_pin(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
    test_plicsw_interrupt();
    /* test_mchtmr_interrupt */
    printf("gpio interrupt end\n");
}
SDK_DECLARE_EXT_ISR_M(BOARD_APP_GPIO_IRQ, isr_gpio)

void test_gpio_input_interrupt(void)
{
    gpio_interrupt_trigger_t trigger;

    gpio_enable_pin_output(BOARD_LED_GPIO_CTRL, BOARD_LED_GPIO_INDEX, BOARD_LED_GPIO_PIN);
    gpio_disable_pin_output(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);

    trigger = gpio_interrupt_trigger_edge_falling;
    gpio_config_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN, trigger);
    gpio_enable_pin_interrupt(BOARD_APP_GPIO_CTRL, BOARD_APP_GPIO_INDEX, BOARD_APP_GPIO_PIN);
    intc_m_enable_irq_with_priority(BOARD_APP_GPIO_IRQ, 5);
}

void isr_ptpc(void)
{
    printf("ptpc interrupt start\n");
    board_delay_ms(50000); /* could press button here */
    ptpc_clear_irq_status(HPM_PTPC, PTPC_EVENT_COMPARE0_MASK);
    printf("ptpc interrupt end\n");
}
SDK_DECLARE_EXT_ISR_M(IRQn_PTPC, isr_ptpc)

void test_ptpc_interrupt(void)
{
    uint32_t freq = clock_get_frequency(clock_ptpc);
    ptpc_config_t config = {0};

    ptpc_get_default_config(HPM_PTPC, &config);
    config.coarse_increment = false;
    config.src_frequency = freq;
    config.capture_keep = false;
    config.ns_rollover_mode = ptpc_ns_counter_rollover_digital;
    ptpc_init(HPM_PTPC, PTPC_PTPC_0, &config);

    ptpc_config_compare(HPM_PTPC, PTPC_PTPC_0, 3, 0); /* 3 seconds */
    ptpc_init_timer(HPM_PTPC, PTPC_PTPC_0);

    printf("%s done\n", __func__);
    /* enable interrupt */
    ptpc_irq_enable(HPM_PTPC, PTPC_EVENT_COMPARE0_MASK);
    intc_m_enable_irq_with_priority(IRQn_PTPC, 2);
}

void delay_ms(uint32_t ms)
{
    mchtmr_delay(HPM_MCHTMR, (uint64_t) ms * mchtmr_freq / 1000);
}

void isr_mchtmr(void)
{
    printf("mchtmr start\n");
    disable_mchtmr_irq();
    printf("mchtmr end\n");
}
SDK_DECLARE_MCHTMR_ISR(isr_mchtmr)

void test_mchtmr_interrupt(void)
{
    enable_mchtmr_irq();
    delay_ms(1000);
}

void isr_plicsw(void)
{
    printf("plicsw start\n");
    printf("plicsw end\n");
    intc_m_complete_swi();
    intc_m_disable_swi();
}
SDK_DECLARE_MSWI_ISR(isr_plicsw)

void test_plicsw_interrupt(void)
{
    intc_m_enable_swi();
    intc_m_init_swi();
    intc_m_trigger_swi();
}

int main(void)
{
    board_init();
    board_init_gpio_pins();
    mchtmr_freq = clock_get_frequency(clock_mchtmr0);
    printf("interrupt test\n");

    test_gpio_input_interrupt();
    test_ptpc_interrupt();

    while(1);
    return 0;
}
