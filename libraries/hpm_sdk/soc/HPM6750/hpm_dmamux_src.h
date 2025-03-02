/*
 * Copyright (c) 2021-2022 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#ifndef HPM_DMAMUX_H
#define HPM_DMAMUX_H

/* dma mux definitions */
#define HPM_DMA_SRC_SPI0_RX                    (0x0UL)
#define HPM_DMA_SRC_SPI0_TX                    (0x1UL)
#define HPM_DMA_SRC_SPI1_RX                    (0x2UL)
#define HPM_DMA_SRC_SPI1_TX                    (0x3UL)
#define HPM_DMA_SRC_SPI2_RX                    (0x4UL)
#define HPM_DMA_SRC_SPI2_TX                    (0x5UL)
#define HPM_DMA_SRC_SPI3_RX                    (0x6UL)
#define HPM_DMA_SRC_SPI3_TX                    (0x7UL)
#define HPM_DMA_SRC_UART0_RX                   (0x8UL)
#define HPM_DMA_SRC_UART0_TX                   (0x9UL)
#define HPM_DMA_SRC_UART1_RX                   (0xAUL)
#define HPM_DMA_SRC_UART1_TX                   (0xBUL)
#define HPM_DMA_SRC_UART2_RX                   (0xCUL)
#define HPM_DMA_SRC_UART2_TX                   (0xDUL)
#define HPM_DMA_SRC_UART3_RX                   (0xEUL)
#define HPM_DMA_SRC_UART3_TX                   (0xFUL)
#define HPM_DMA_SRC_UART4_RX                   (0x10UL)
#define HPM_DMA_SRC_UART4_TX                   (0x11UL)
#define HPM_DMA_SRC_UART5_RX                   (0x12UL)
#define HPM_DMA_SRC_UART5_TX                   (0x13UL)
#define HPM_DMA_SRC_UART6_RX                   (0x14UL)
#define HPM_DMA_SRC_UART6_TX                   (0x15UL)
#define HPM_DMA_SRC_UART7_RX                   (0x16UL)
#define HPM_DMA_SRC_UART7_TX                   (0x17UL)
#define HPM_DMA_SRC_UART8_RX                   (0x18UL)
#define HPM_DMA_SRC_UART8_TX                   (0x19UL)
#define HPM_DMA_SRC_UART9_RX                   (0x1AUL)
#define HPM_DMA_SRC_UART9_TX                   (0x1BUL)
#define HPM_DMA_SRC_UART10_RX                  (0x1CUL)
#define HPM_DMA_SRC_UART10_TX                  (0x1DUL)
#define HPM_DMA_SRC_UART11_RX                  (0x1EUL)
#define HPM_DMA_SRC_UART11_TX                  (0x1FUL)
#define HPM_DMA_SRC_UART12_RX                  (0x20UL)
#define HPM_DMA_SRC_UART12_TX                  (0x21UL)
#define HPM_DMA_SRC_UART13_RX                  (0x22UL)
#define HPM_DMA_SRC_UART13_TX                  (0x23UL)
#define HPM_DMA_SRC_UART14_RX                  (0x24UL)
#define HPM_DMA_SRC_UART14_TX                  (0x25UL)
#define HPM_DMA_SRC_UART15_RX                  (0x26UL)
#define HPM_DMA_SRC_UART15_TX                  (0x27UL)
#define HPM_DMA_SRC_I2S0_RX                    (0x28UL)
#define HPM_DMA_SRC_I2S0_TX                    (0x29UL)
#define HPM_DMA_SRC_I2S1_RX                    (0x2AUL)
#define HPM_DMA_SRC_I2S1_TX                    (0x2BUL)
#define HPM_DMA_SRC_I2S2_RX                    (0x2CUL)
#define HPM_DMA_SRC_I2S2_TX                    (0x2DUL)
#define HPM_DMA_SRC_I2S3_RX                    (0x2EUL)
#define HPM_DMA_SRC_I2S3_TX                    (0x2FUL)
#define HPM_DMA_SRC_MOT0_0                     (0x30UL)
#define HPM_DMA_SRC_MOT0_1                     (0x31UL)
#define HPM_DMA_SRC_MOT0_2                     (0x32UL)
#define HPM_DMA_SRC_MOT0_3                     (0x33UL)
#define HPM_DMA_SRC_MOT1_0                     (0x34UL)
#define HPM_DMA_SRC_MOT1_1                     (0x35UL)
#define HPM_DMA_SRC_MOT1_2                     (0x36UL)
#define HPM_DMA_SRC_MOT1_3                     (0x37UL)
#define HPM_DMA_SRC_MOT2_0                     (0x38UL)
#define HPM_DMA_SRC_MOT2_1                     (0x39UL)
#define HPM_DMA_SRC_MOT2_2                     (0x3AUL)
#define HPM_DMA_SRC_MOT2_3                     (0x3BUL)
#define HPM_DMA_SRC_MOT3_0                     (0x3CUL)
#define HPM_DMA_SRC_MOT3_1                     (0x3DUL)
#define HPM_DMA_SRC_MOT3_2                     (0x3EUL)
#define HPM_DMA_SRC_MOT3_3                     (0x3FUL)
#define HPM_DMA_SRC_NTMR0_0                    (0x40UL)
#define HPM_DMA_SRC_NTMR0_1                    (0x41UL)
#define HPM_DMA_SRC_NTMR0_2                    (0x42UL)
#define HPM_DMA_SRC_NTMR0_3                    (0x43UL)
#define HPM_DMA_SRC_NTMR1_0                    (0x44UL)
#define HPM_DMA_SRC_NTMR1_1                    (0x45UL)
#define HPM_DMA_SRC_NTMR1_2                    (0x46UL)
#define HPM_DMA_SRC_NTMR1_3                    (0x47UL)
#define HPM_DMA_SRC_GPTMR0_0                   (0x48UL)
#define HPM_DMA_SRC_GPTMR0_1                   (0x49UL)
#define HPM_DMA_SRC_GPTMR0_2                   (0x4AUL)
#define HPM_DMA_SRC_GPTMR0_3                   (0x4BUL)
#define HPM_DMA_SRC_GPTMR1_0                   (0x4CUL)
#define HPM_DMA_SRC_GPTMR1_1                   (0x4DUL)
#define HPM_DMA_SRC_GPTMR1_2                   (0x4EUL)
#define HPM_DMA_SRC_GPTMR1_3                   (0x4FUL)
#define HPM_DMA_SRC_GPTMR2_0                   (0x50UL)
#define HPM_DMA_SRC_GPTMR2_1                   (0x51UL)
#define HPM_DMA_SRC_GPTMR2_2                   (0x52UL)
#define HPM_DMA_SRC_GPTMR2_3                   (0x53UL)
#define HPM_DMA_SRC_GPTMR3_0                   (0x54UL)
#define HPM_DMA_SRC_GPTMR3_1                   (0x55UL)
#define HPM_DMA_SRC_GPTMR3_2                   (0x56UL)
#define HPM_DMA_SRC_GPTMR3_3                   (0x57UL)
#define HPM_DMA_SRC_GPTMR4_0                   (0x58UL)
#define HPM_DMA_SRC_GPTMR4_1                   (0x59UL)
#define HPM_DMA_SRC_GPTMR4_2                   (0x5AUL)
#define HPM_DMA_SRC_GPTMR4_3                   (0x5BUL)
#define HPM_DMA_SRC_GPTMR5_0                   (0x5CUL)
#define HPM_DMA_SRC_GPTMR5_1                   (0x5DUL)
#define HPM_DMA_SRC_GPTMR5_2                   (0x5EUL)
#define HPM_DMA_SRC_GPTMR5_3                   (0x5FUL)
#define HPM_DMA_SRC_GPTMR6_0                   (0x60UL)
#define HPM_DMA_SRC_GPTMR6_1                   (0x61UL)
#define HPM_DMA_SRC_GPTMR6_2                   (0x62UL)
#define HPM_DMA_SRC_GPTMR6_3                   (0x63UL)
#define HPM_DMA_SRC_GPTMR7_0                   (0x64UL)
#define HPM_DMA_SRC_GPTMR7_1                   (0x65UL)
#define HPM_DMA_SRC_GPTMR7_2                   (0x66UL)
#define HPM_DMA_SRC_GPTMR7_3                   (0x67UL)
#define HPM_DMA_SRC_I2C0                       (0x68UL)
#define HPM_DMA_SRC_I2C1                       (0x69UL)
#define HPM_DMA_SRC_I2C2                       (0x6AUL)
#define HPM_DMA_SRC_I2C3                       (0x6BUL)
#define HPM_DMA_SRC_XPI0_RX                    (0x6CUL)
#define HPM_DMA_SRC_XPI0_TX                    (0x6DUL)
#define HPM_DMA_SRC_XPI1_RX                    (0x6EUL)
#define HPM_DMA_SRC_XPI1_TX                    (0x6FUL)
#define HPM_DMA_SRC_ACMP_0                     (0x70UL)
#define HPM_DMA_SRC_ACMP_1                     (0x71UL)
#define HPM_DMA_SRC_ACMP_2                     (0x72UL)
#define HPM_DMA_SRC_ACMP_3                     (0x73UL)



#endif /* HPM_DMAMUX_H */