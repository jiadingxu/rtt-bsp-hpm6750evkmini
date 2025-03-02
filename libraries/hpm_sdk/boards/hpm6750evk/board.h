/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef _HPM_BOARD_H
#define _HPM_BOARD_H
#include <stdio.h>
#include "hpm_common.h"
#include "hpm_clock_drv.h"
#include "hpm_soc.h"
#include "hpm_soc_feature.h"
#include "pinmux.h"

#define BOARD_NAME "hpm6750evk"

/* uart section */
#ifndef BOARD_RUNNING_CORE
#define BOARD_RUNNING_CORE HPM_CORE0
#endif
#ifndef BOARD_APP_UART_BASE
#define BOARD_APP_UART_BASE HPM_UART0
#define BOARD_APP_UART_IRQ  IRQn_UART0
#else
#ifndef BOARD_APP_UART_IRQ
#warning no IRQ specified for applicaiton uart
#endif
#endif

#define BOARD_APP_UART_BAUDRATE (115200UL)
#define BOARD_APP_UART_CLK_NAME clock_uart0

#ifndef BOARD_CONSOLE_TYPE
#define BOARD_CONSOLE_TYPE console_type_uart
#endif

#if BOARD_CONSOLE_TYPE == console_type_uart
#ifndef BOARD_CONSOLE_BASE
#if BOARD_RUNNING_CORE == HPM_CORE0
#define BOARD_CONSOLE_BASE HPM_UART0
#define BOARD_CONSOLE_CLK_NAME clock_uart0
#else
#define BOARD_CONSOLE_BASE HPM_UART13
#define BOARD_CONSOLE_CLK_NAME clock_uart13
#endif
#endif
#define BOARD_CONSOLE_BAUDRATE (115200UL)
#endif


#define BOARD_FREEMASTER_UART_BASE HPM_UART0
#define BOARD_FREEMASTER_UART_IRQ IRQn_UART0
#define BOARD_FREEMASTER_UART_CLK_NAME clock_uart0

/* sdram section */
#define BOARD_SDRAM_ADDRESS  (0x40000000UL)
#define BOARD_SDRAM_SIZE     (32*SIZE_1MB)
#define BOARD_SDRAM_CS       DRAM_SDRAM_CS0
#define BOARD_SDRAM_PORT_SIZE DRAM_SDRAM_PORT_SIZE_32_BITS
#define BOARD_SDRAM_REFRESH_COUNT (8192UL)
#define BOARD_SDRAM_REFRESH_IN_MS (64UL)
#define BOARD_SDRAM_DATA_WIDTH_IN_BYTE (4UL)

/* lcd section */
#define BOARD_LCD_BASE HPM_LCDC
#define BOARD_LCD_IRQ  IRQn_LCDC_D0
#define BOARD_LCD_POWER_GPIO_BASE HPM_GPIO0
#define BOARD_LCD_POWER_GPIO_INDEX GPIO_DO_GPIOB
#define BOARD_LCD_POWER_GPIO_PIN 16
#define BOARD_LCD_BACKLIGHT_GPIO_BASE HPM_GPIO0
#define BOARD_LCD_BACKLIGHT_GPIO_INDEX GPIO_DO_GPIOB
#define BOARD_LCD_BACKLIGHT_GPIO_PIN 10

/* i2c section */
#define BOARD_APP_I2C_BASE HPM_I2C0
#define BOARD_APP_I2C_CLK_NAME clock_i2c0

#define BOARD_APP_I2C_SLAVE_BASE HPM_I2C0
#define BOARD_APP_I2C_SLAVE_IRQ IRQn_I2C0
#define BOARD_APP_I2C_SLAVE_CLK_NAME clock_i2c0

#define BOARD_CAM_I2C_BASE HPM_I2C0
#define BOARD_CAM_I2C_CLK_NAME clock_i2c0

#define BOARD_CAP_I2C_BASE (HPM_I2C0)
#define BOARD_CAP_I2C_CLK_NAME clock_i2c0
#define BOARD_CAP_RST_GPIO (HPM_GPIO0)
#define BOARD_CAP_RST_GPIO_INDEX (GPIO_DI_GPIOB)
#define BOARD_CAP_RST_GPIO_PIN (9)
#define BOARD_CAP_RST_GPIO_IRQ (IRQn_GPIO0_B)
#define BOARD_CAP_INTR_GPIO (HPM_GPIO0)
#define BOARD_CAP_INTR_GPIO_INDEX (GPIO_DI_GPIOB)
#define BOARD_CAP_INTR_GPIO_PIN (8)
#define BOARD_CAP_INTR_GPIO_IRQ (IRQn_GPIO0_B)
#define BOARD_CAP_I2C_SDA_GPIO_INDEX (GPIO_DI_GPIOZ)
#define BOARD_CAP_I2C_SDA_GPIO_PIN (10)
#define BOARD_CAP_I2C_CLK_GPIO_INDEX (GPIO_DI_GPIOZ)
#define BOARD_CAP_I2C_CLK_GPIO_PIN (11)

/* dma section */
#define BOARD_APP_XDMA_IRQ IRQn_XDMA
#define BOARD_APP_HDMA_IRQ IRQn_HDMA

/* gpio section */
#define BOARD_R_GPIO_CTRL HPM_GPIO0
#define BOARD_R_GPIO_INDEX GPIO_DI_GPIOB
#define BOARD_R_GPIO_PIN 11
#define BOARD_G_GPIO_CTRL HPM_GPIO0
#define BOARD_G_GPIO_INDEX GPIO_DI_GPIOB
#define BOARD_G_GPIO_PIN 12
#define BOARD_B_GPIO_CTRL HPM_GPIO0
#define BOARD_B_GPIO_INDEX GPIO_DI_GPIOB
#define BOARD_B_GPIO_PIN 13

#define BOARD_LED_GPIO_CTRL HPM_GPIO0

#define BOARD_LED_GPIO_INDEX GPIO_DI_GPIOB
#define BOARD_LED_GPIO_PIN 12
#define BOARD_LED_OFF_LEVEL 1
#define BOARD_LED_ON_LEVEL 0

#define BOARD_LED_TOGGLE_RGB 1

#define BOARD_APP_GPIO_INDEX GPIO_DI_GPIOZ
#define BOARD_APP_GPIO_PIN 2

/* pinmux section */
#define USING_GPIO0_FOR_GPIOZ
#ifndef USING_GPIO0_FOR_GPIOZ
#define BOARD_APP_GPIO_CTRL HPM_BGPIO
#define BOARD_APP_GPIO_IRQ IRQn_BGPIO
#else
#define BOARD_APP_GPIO_CTRL HPM_GPIO0
#define BOARD_APP_GPIO_IRQ IRQn_GPIO0_Z
#endif

/* gpiom section */
#define BOARD_APP_GPIOM_BASE            HPM_GPIOM
#define BOARD_APP_GPIOM_USING_CTRL      HPM_FGPIO
#define BOARD_APP_GPIOM_USING_CTRL_NAME gpiom_core0_fast

/* spi section */
#define BOARD_APP_SPI_BASE HPM_SPI2
#define BOARD_APP_SPI_CLK_SRC_FREQ      (24000000UL)
#define BOARD_APP_SPI_SCLK_FREQ         (1562500UL)
#define BOARD_APP_SPI_ADDR_LEN_IN_BYTES (1U)
#define BOARD_APP_SPI_DATA_LEN_IN_BITS  (8U)

/* mtimer section */
#define BOARD_MCHTMR_FREQ_IN_HZ (1000000UL)

/* Flash section */
#define BOARD_APP_XPI_NOR_XPI_BASE            (HPM_XPI0)
#define BOARD_APP_XPI_NOR_CFG_OPT_HDR         (0xfcf90001U)
#define BOARD_APP_XPI_NOR_CFG_OPT_OPT0        (0x00000005U)
#define BOARD_APP_XPI_NOR_CFG_OPT_OPT1        (0x00001000U)

/* lcd section */
#ifndef BOARD_LCD_WIDTH
#define BOARD_LCD_WIDTH (800)
#endif
#ifndef BOARD_LCD_HEIGHT
#define BOARD_LCD_HEIGHT (480)
#endif

/* pdma section */
#define BOARD_PDMA_BASE HPM_PDMA

/* i2s section */
#define BOARD_APP_I2S_BASE HPM_I2S0
#define BOARD_APP_I2S_DATA_LINE      (2U)
#define BOARD_APP_I2S_CLK_NAME clock_i2s0

/* enet section */
#define BOARD_APP_ENET_BASE HPM_ENET0

/* adc section */
#define BOARD_APP_ADC12_BASE HPM_ADC0
#define BOARD_APP_ADC16_BASE HPM_ADC3
#define BOARD_APP_ADC12_IRQn IRQn_ADC0
#define BOARD_APP_ADC16_IRQn IRQn_ADC3
#define BOARD_APP_ADC_CH                         (0U)
#define BOARD_APP_ADC_SEQ_DMA_SIZE_IN_4BYTES     (1024U)
#define BOARD_APP_ADC_PREEMPT_DMA_SIZE_IN_4BYTES (192U)
#define BOARD_APP_ADC_PREEMPT_TRIG_LEN           (1U)
#define BOARD_APP_ADC_SINGLE_CONV_CNT            (6)
#define BOARD_APP_ADC_TRIG_PWMT0                 HPM_PWM0
#define BOARD_APP_ADC_TRIG_PWMT1                 HPM_PWM1
#define BOARD_APP_ADC_TRIG_TRGM0                 HPM_TRGM0
#define BOARD_APP_ADC_TRIG_TRGM1                 HPM_TRGM1
#define BOARD_APP_ADC_TRIG_PWM_SYNC              HPM_SYNT

#define BOARD_APP_CAN_BASE                       HPM_CAN0
#define BOARD_APP_CAN_IRQn                       IRQn_CAN0


/*
 * timer for board delay
 */
#define BOARD_DELAY_TIMER (HPM_GPTMR7)
#define BOARD_DELAY_TIMER_CH 0
#define BOARD_DELAY_TIMER_CLK_NAME (clock_gptmr7)

#define BOARD_CALLBACK_TIMER (HPM_GPTMR7)
#define BOARD_CALLBACK_TIMER_CH 1
#define BOARD_CALLBACK_TIMER_IRQ IRQn_GPTMR7
#define BOARD_CALLBACK_TIMER_CLK_NAME (clock_gptmr7)

/* SDXC section */
#define BOARD_APP_SDCARD_SDXC_BASE            (HPM_SDXC1)
#define BOARD_APP_SDCARD_CDN_GPIO_CTRL        (HPM_GPIO0)
#define BOARD_APP_SDCARD_CDN_GPIO_PIN         (15UL)
#define BOARD_APP_SDCARD_SUPPORT_1V8          (0)

/* USB section */
#define BOARD_USB0_VBUS_PORT       (HPM_GPIO0)
#define BOARD_USB0_VBUS_GPIO_INDEX (GPIO_DO_GPIOF)
#define BOARD_USB0_VBUS_GPIO_PIN   (9)

#define BOARD_USB0_OC_PORT         (HPM_GPIO0)
#define BOARD_USB0_OC_GPIO_INDEX   (GPIO_DI_GPIOF)
#define BOARD_USB0_OC_GPIO_PIN     (8)

#define BOARD_USB1_VBUS_PORT       (HPM_GPIO0)
#define BOARD_USB1_VBUS_GPIO_INDEX (GPIO_DO_GPIOF)
#define BOARD_USB1_VBUS_GPIO_PIN   (6)

#define BOARD_USB1_OC_PORT         (HPM_GPIO0)
#define BOARD_USB1_OC_GPIO_INDEX   (GPIO_DI_GPIOF)
#define BOARD_USB1_OC_GPIO_PIN     (5)

/*BLDC pwm*/

/*PWM define*/
#define BOARD_BLDCPWM                     HPM_PWM2
#define BOARD_BLDC_UH_PWM_OUTPIN         (0U)
#define BOARD_BLDC_UL_PWM_OUTPIN         (1U)
#define BOARD_BLDC_VH_PWM_OUTPIN         (2U)
#define BOARD_BLDC_VL_PWM_OUTPIN         (3U)
#define BOARD_BLDC_WH_PWM_OUTPIN         (4U)
#define BOARD_BLDC_WL_PWM_OUTPIN         (5U)
#define BOARD_BLDCPWM_TRGM                HPM_TRGM2
#define BOARD_BLDCAPP_PWM_IRQ             IRQn_PWM2
#define BOARD_BLDCPWM_CMP_INDEX_0         (0U)
#define BOARD_BLDCPWM_CMP_INDEX_1         (1U)
#define BOARD_BLDCPWM_CMP_INDEX_2         (2U)
#define BOARD_BLDCPWM_CMP_INDEX_3         (3U)
#define BOARD_BLDCPWM_CMP_INDEX_4         (4U)
#define BOARD_BLDCPWM_CMP_INDEX_5         (5U)

/*HALL define*/

#define BOARD_BLDC_HALL_BASE                 HPM_HALL2
#define BOARD_BLDC_HALL_TRGM                 HPM_TRGM2
#define BOARD_BLDC_HALL_IRQ                  IRQn_HALL2
#define BOARD_BLDC_HALL_TRGM_HALL_U_SRC      HPM_TRGM2_INPUT_SRC_TRGM2_P6
#define BOARD_BLDC_HALL_TRGM_HALL_V_SRC      HPM_TRGM2_INPUT_SRC_TRGM2_P7
#define BOARD_BLDC_HALL_TRGM_HALL_W_SRC      HPM_TRGM2_INPUT_SRC_TRGM2_P8
#define BOARD_BLDC_HALL_MOTOR_PHASE_COUNT_PER_REV        (1000U)



/*QEI*/

#define BOARD_BLDC_QEI_BASE              HPM_QEI2
#define BOARD_BLDC_QEI_IRQ               IRQn_QEI2
#define BOARD_BLDC_QEI_TRGM              HPM_TRGM2
#define BOARD_BLDC_QEI_TRGM_QEI_A_SRC    HPM_TRGM2_INPUT_SRC_TRGM2_P9
#define BOARD_BLDC_QEI_TRGM_QEI_B_SRC    HPM_TRGM2_INPUT_SRC_TRGM2_P10
#define BOARD_BLDC_QEI_MOTOR_PHASE_COUNT_PER_REV     (16U)
#define BOARD_BLDC_QEI_CLOCK_SOURCE      clock_mot2
#define BOARD_BLDC_QEI_FOC_PHASE_COUNT_PER_REV       (4000U)

/*Timer define*/

#define BOARD_BLDC_TMR_1MS                       HPM_GPTMR2
#define BOARD_BLDC_TMR_CH                        0
#define BOARD_BLDC_TMR_CMP                       0
#define BOARD_BLDC_TMR_IRQ                       IRQn_GPTMR2
#define BOARD_BLDC_TMR_RELOAD                    (100000U)

/*adc*/
#define BOARD_BLDC_ADC_BASE                    HPM_ADC0
#define BOARD_BLDC_ADC_V_BASE                  HPM_ADC1
#define BOARD_BLDC_ADC_W_BASE                  HPM_ADC2

#define BOARD_BLDC_ADC_CH_U                    (7U)
#define BOARD_BLDC_ADC_CH_V                    (10U)
#define BOARD_BLDC_ADC_CH_W                    (11U)
#define BOARD_BLDC_ADC_IRQn                    IRQn_ADC0
#define BOARD_BLDC_ADC_SEQ_DMA_SIZE_IN_4BYTES  (40U)
#define BOARD_BLDC_ADC_TRG                    ADC12_CONFIG_TRG2A
#define BOARD_BLDC_ADC_PREEMPT_TRIG_LEN        (1U)
#define BOARD_BLDC_PWM_TRIG_CMP_INDEX          (8U)
#define BOARD_BLDC_TRIGMUX_IN_NUM              HPM_TRGM2_INPUT_SRC_PWM2_CH8REF
#define BOARD_BLDC_TRG_NUM                     TRGM_TRGOCFG_ADCX_PTRGI0A

/* APP PWM */
#define BOARD_APP_PWM HPM_PWM2
#define BOARD_APP_PWM_CLOCK_NAME clock_mot2
#define BOARD_APP_PWM_OUT1 0
#define BOARD_APP_PWM_OUT2 1
#define BOARD_APP_TRGM HPM_TRGM2

/* RGB LED Section */
#define BOARD_RED_PWM_IRQ IRQn_PWM1
#define BOARD_RED_PWM HPM_PWM1
#define BOARD_RED_PWM_OUT 8
#define BOARD_RED_PWM_CMP 8
#define BOARD_RED_PWM_CMP_INITIAL_ZERO true
#define BOARD_RED_PWM_CLOCK_NAME clock_mot1

#define BOARD_GREEN_PWM_IRQ IRQn_PWM0
#define BOARD_GREEN_PWM HPM_PWM0
#define BOARD_GREEN_PWM_OUT 8
#define BOARD_GREEN_PWM_CMP 8
#define BOARD_GREEN_PWM_CMP_INITIAL_ZERO true
#define BOARD_GREEN_PWM_CLOCK_NAME clock_mot0

#define BOARD_BLUE_PWM_IRQ IRQn_PWM1
#define BOARD_BLUE_PWM HPM_PWM1
#define BOARD_BLUE_PWM_OUT 9
#define BOARD_BLUE_PWM_CMP 9
#define BOARD_BLUE_PWM_CMP_INITIAL_ZERO true
#define BOARD_BLUE_PWM_CLOCK_NAME clock_mot1

#define BOARD_RGB_RED 0
#define BOARD_RGB_GREEN (BOARD_RGB_RED + 1)
#define BOARD_RGB_BLUE  (BOARD_RGB_RED + 2)

#define BOARD_CPU_FREQ (816000000UL)

#ifndef BOARD_SHOW_CLOCK
#define BOARD_SHOW_CLOCK 1
#endif
#ifndef BOARD_SHOW_BANNER
#define BOARD_SHOW_BANNER 1
#endif

#if defined(__cplusplus)
extern "C" {
#endif /* __cplusplus */

typedef void (*board_timer_cb)(void);

void board_init(void);
void board_init_console(void);

void board_init_uart(UART_Type *ptr);
void board_init_i2c(I2C_Type *ptr);
void board_init_lcd(void);

void board_init_can(CAN_Type *ptr);

uint32_t board_init_dram_clock(void);

void board_init_sdram_pins(void);
void board_init_gpio_pins(void);
void board_init_spi_pins(SPI_Type *ptr);
void board_init_led_pins(void);

/* cap touch */
void board_init_cap_touch(void);

void board_led_write(bool state);
void board_led_toggle(void);

void board_fpga_power_enable(void);

void board_init_cam_pins(void);

#ifdef CAMREA_RESET_PWDN_CONFIGURABLE
void board_power_down_camera(bool active);
void board_reset_camera(bool active);
#endif

/* Initialize SoC overall clocks */
void board_init_clock(void);

/* Initialize the CAM(camera) dot clock */
uint32_t board_init_cam_clock(CAM_Type *ptr);

/* Initialize the LCD pixel clock */
uint32_t board_init_lcd_clock(void);

uint32_t board_init_spi_clock(SPI_Type *ptr);

uint32_t board_init_adc12_clock(ADC12_Type *ptr);

uint32_t board_init_adc16_clock(ADC16_Type *ptr);

uint32_t board_init_can_clock(CAN_Type *ptr);

uint32_t board_init_i2s_clock(I2S_Type *ptr);
uint32_t board_init_pdm_clock(void);
uint32_t board_init_dao_clock(void);

void board_init_sd_pins(SDXC_Type *ptr);
uint32_t board_sd_configure_clock(SDXC_Type *ptr, uint32_t freq);
void board_sd_switch_pins_to_1v8(SDXC_Type *ptr);
bool board_sd_detect_card(SDXC_Type *ptr);

void board_init_usb_pins(void);
void board_usb_vbus_ctrl(uint8_t usb_index, uint8_t level);

hpm_stat_t board_init_enet_rmii_reference_clock(ENET_Type *ptr);

/*
 * @brief Initialize PMP and PMA for but not limited to the following purposes:
 *      -- non-cacheable memory initialization
 */
void board_init_pmp(void);

void board_delay_ms(uint32_t ms);

void board_timer_create(uint32_t ms, void *cb);

void board_init_rgb_pwm_pins(void);
void board_enable_output_rgb_led(uint8_t color);
void board_disable_output_rgb_led(uint8_t color);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_BOARD_H */
