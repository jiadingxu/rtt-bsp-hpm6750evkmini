/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_can_drv.h"
#include <assert.h>

/***********************************************************************************************************************
 *
 *  Definitions
 *
 **********************************************************************************************************************/
#define TSEG1_MIN_FOR_CAN2_0 (2U)
#define TSEG1_MAX_FOR_CAN2_0 (65U)

#define TSEG1_MIN_FOR_CANFD_NORMINAL (2U)
#define TSEG1_MAX_FOR_CANFD_NORMINAL (65U)

#define TSEG1_MIN_FOR_CANFD_DATA (2U)
#define TSEG1_MAX_FOR_CANFD_DATA (17U)

#define TSEG2_MIN_FOR_CAN2_0 (1U)
#define TSEG2_MAX_FOR_CAN2_0 (8U)

#define TSEG2_MIN_FOR_CANFD_NORMINAL (1U)
#define TSEG2_MAX_FOR_CANFD_NORMINAL (32U)

#define TSEG2_MIN_FOR_CANFD_DATA (1U)
#define TSEG2_MAX_FOR_CANFD_DATA (8U)

#define TSJW_MIN_FOR_CAN2_0 (1U)
#define TSJW_MAX_FOR_CAN2_0 (16U)

#define TSJW_MIN_FOR_CANFD_NORMINAL (1U)
#define TSJW_MAX_FOR_CANFD_NORMINAL (16U)

#define TSJW_MIN_FOR_CANFD_DATA (1U)
#define TSJW_MAX_FOR_CANFD_DATA (8U)

#define NUM_TQ_MIN_FOR_CAN2_0 (8U)
#define NUM_TQ_MAX_FOR_CAN2_0 (TSEG1_MAX_FOR_CAN2_0 + TSEG2_MAX_FOR_CAN2_0)

#define NUM_TQ_MIN_FOR_CANFD_NOMINAL (8U)
#define NUM_TQ_MAX_FOR_CANFD_NOMINAL (TSEG1_MAX_FOR_CANFD_NORMINAL + TSEG2_MAX_FOR_CANFD_NORMINAL)

#define NUM_TQ_MIN_FOR_CANFD_DATA (8U)
#define NUM_TQ_MAX_FOR_CANFD_DATA (TSEG1_MAX_FOR_CANFD_DATA + TSEG2_MAX_FOR_CANFD_DATA)

#define MIN_TQ_MUL_PRESCALE (10U)

#define CAN_FILTER_INDEX_MAX (15U)


#define PRESCALER_MAX (256U)


#define CAN_STB_IS_EMPTY (0U)
#define CAN_STB_LESS_EAUL_HALF_FULL (1U)
#define CAN_STB_MORE_THAN_HALF_FULL (2U)
#define CAN_STB_IS_FULL (3U)

#define CAN_RXBUF_IS_EMPTY (0U)
#define CAN_RXBUF_HAS_DATA (1U)
#define CAN_RXBUF_ALMOST_FULL (2U)
#define CAN_RXBUF_IS_FULL (3U)

#define CAN_TIMEOUT_CNT (0xFFFFFFUL)


/*
 * @brief CAN bit timing table
 */
typedef struct {
    uint8_t tq_min;
    uint8_t tq_max;
    uint8_t seg1_min;
    uint8_t seg1_max;
    uint8_t seg2_min;
    uint8_t seg2_max;
    uint8_t sjw_min;
    uint8_t sjw_max;
    uint8_t min_diff_seg1_minus_seg2;
} can_bit_timing_table_t;

/**
 * @brief CAN bit timing list for all supported bit timing modes
 */
static const can_bit_timing_table_t s_can_bit_timing_tbl[3] =
        {
                {
                        .tq_min = NUM_TQ_MIN_FOR_CAN2_0,
                        .tq_max = NUM_TQ_MAX_FOR_CAN2_0,
                        .seg1_min = TSEG1_MIN_FOR_CAN2_0,
                        .seg1_max = TSEG1_MAX_FOR_CAN2_0,
                        .seg2_min = TSEG2_MIN_FOR_CAN2_0,
                        .seg2_max = TSEG2_MAX_FOR_CAN2_0,
                        .sjw_min = TSJW_MIN_FOR_CAN2_0,
                        .sjw_max = TSJW_MAX_FOR_CAN2_0,
                        .min_diff_seg1_minus_seg2 = 2,
                },
                {
                        .tq_min = NUM_TQ_MIN_FOR_CANFD_NOMINAL,
                        .tq_max = NUM_TQ_MAX_FOR_CANFD_NOMINAL,
                        .seg1_min = TSEG1_MIN_FOR_CANFD_NORMINAL,
                        .seg1_max = TSEG1_MAX_FOR_CANFD_NORMINAL,
                        .seg2_min = TSEG2_MIN_FOR_CANFD_NORMINAL,
                        .seg2_max = TSEG2_MAX_FOR_CANFD_NORMINAL,
                        .sjw_min = TSJW_MIN_FOR_CANFD_NORMINAL,
                        .sjw_max = TSJW_MAX_FOR_CANFD_NORMINAL,
                        .min_diff_seg1_minus_seg2 = 2,
                },
                {
                        .tq_min = NUM_TQ_MIN_FOR_CANFD_DATA,
                        .tq_max = NUM_TQ_MAX_FOR_CANFD_DATA,
                        .seg1_min = TSEG1_MIN_FOR_CANFD_DATA,
                        .seg1_max = TSEG1_MAX_FOR_CANFD_DATA,
                        .seg2_min = TSEG2_MIN_FOR_CANFD_DATA,
                        .seg2_max = TSEG2_MAX_FOR_CANFD_DATA,
                        .sjw_min = TSJW_MIN_FOR_CANFD_DATA,
                        .sjw_max = TSJW_MAX_FOR_CANFD_DATA,
                        .min_diff_seg1_minus_seg2 = 1,
                }
        };

/***********************************************************************************************************************
 *
 *  Prototypes
 */
static uint32_t find_closest_prescaler(uint32_t num_tq_mul_prescaler, uint32_t start_prescaler,
                                       uint32_t max_tq, uint32_t min_tq);


static uint8_t can_get_data_words_from_dlc(uint32_t dlc);

static void can_fill_tx_buffer(CAN_Type *base, const can_transmit_buf_t *message);


/***********************************************************************************************************************
 *
 *  Codes
 */
static uint32_t find_closest_prescaler(uint32_t num_tq_mul_prescaler, uint32_t start_prescaler,
                                       uint32_t max_tq, uint32_t min_tq)
{
    bool has_found = false;

    uint32_t prescaler = start_prescaler;

    while (!has_found) {

        if (num_tq_mul_prescaler / prescaler > max_tq) {
            ++prescaler;
        } else {
            has_found = true;
            break;
        }
        uint32_t tq = num_tq_mul_prescaler / prescaler;
        if (tq * prescaler == num_tq_mul_prescaler) {
            has_found = true;
            break;
        } else if (tq < min_tq) {
            has_found = false;
            break;
        }
    }

    return has_found ? prescaler : 0U;
}


hpm_stat_t can_set_bit_timing(CAN_Type *base, can_bit_timing_option_t option, uint32_t src_clk_freq, uint32_t baudrate)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((base == NULL) || (option > can_bit_timing_canfd_data) || (baudrate == 0U) ||
            (src_clk_freq / baudrate < MIN_TQ_MUL_PRESCALE)) {
            break;
        }

        const can_bit_timing_table_t *tbl = &s_can_bit_timing_tbl[(uint8_t) option];

        /* According to the CAN specification 2.0,
         * the Tq must be in range specified in the above can bit timing table
         */
        if (src_clk_freq / baudrate < tbl->tq_min) {
            break;
        }

        uint32_t num_tq_mul_prescaler = src_clk_freq / baudrate;
        uint32_t start_prescaler = 1U;
        uint32_t num_seg1, num_seg2;
        bool has_found = false;

        /* Find out the minimum prescaler */
        uint32_t current_prescaler;
        while (!has_found) {

            current_prescaler = find_closest_prescaler(num_tq_mul_prescaler, start_prescaler,
                                                       tbl->tq_max,
                                                       tbl->tq_min);
            if (current_prescaler < start_prescaler) {
                break;
            }
            uint32_t num_tq = num_tq_mul_prescaler / current_prescaler;

            num_seg2 = (num_tq - tbl->min_diff_seg1_minus_seg2) / 2U;
            num_seg1 = num_tq - num_seg2;
            while (num_seg2 > tbl->seg2_max) {
                num_seg2--;
                num_seg1++;
            }

            /* Recommended sample point is 75% - 87.5% */
            while ((num_seg1 * 100U) / num_tq < 75U) {
                ++num_seg1;
                --num_seg2;
            }

            if ((num_seg2 >= tbl->seg2_min) && (num_seg1 <= tbl->seg1_max)) {
                has_found = true;
            } else {
                start_prescaler = current_prescaler + 1U;
            }
        }

        if (has_found) {

            uint32_t num_sjw = MIN(tbl->sjw_max, (num_seg2 + 1U) / 2U);

            if (option < can_bit_timing_canfd_data) {
                base->S_PRESC = CAN_S_PRESC_S_PRESC_SET(current_prescaler - 1U) | CAN_S_PRESC_S_SEG_1_SET(num_seg1 - 2U) |
                                CAN_S_PRESC_S_SEG_2_SET(num_seg2 - 1U) | CAN_S_PRESC_S_SJW_SET(num_sjw - 1U);
            } else {
                base->F_PRESC = CAN_F_PRESC_F_PRESC_SET(current_prescaler - 1U) | CAN_F_PRESC_F_SEG_1_SET(num_seg1 - 2U) |
                                CAN_F_PRESC_F_SEG_2_SET(num_seg2 - 1U) | CAN_F_PRESC_F_SJW_SET(num_sjw - 1U);
            }
            status = status_success;
        }

    } while (false);

    return status;
}

hpm_stat_t can_set_filter(CAN_Type *base, const can_filter_config_t *config)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        if ((base == NULL) || (config == NULL)) {
            break;
        }

        /* Configure acceptance code */
        base->ACFCTRL = CAN_ACFCTRL_ACFADR_SET(config->index);
        base->ACF = CAN_ACF_CODE_MASK_SET(config->code);

        /* Configure acceptance mask */
        uint32_t acf_value = CAN_ACF_CODE_MASK_SET(config->mask);
        if (config->mode == can_filter_mode_standard_frames) {
            acf_value |= CAN_ACF_AIDEE_MASK;
        } else if (config->mode == can_filter_mode_extended_frames) {
            acf_value |= CAN_ACF_AIDEE_MASK | CAN_ACF_AIDE_MASK;
        } else {
            /* Treat it as the default mode */
        }

        base->ACFCTRL = CAN_ACFCTRL_SELMASK_MASK | CAN_ACFCTRL_ACFADR_SET(config->index);
        base->ACF = acf_value;

        if (config->enable) {
            base->ACF_EN = (1U << config->index);
        } else {
            base->ACF_EN &= (uint16_t) ~(1U << config->index);
        }
        status = status_success;
    } while (false);

    return status;
}

static uint8_t can_get_data_words_from_dlc(uint32_t dlc)
{
    uint32_t copy_words = 0;

    dlc &= 0xFU;
    if (dlc <= 8U) {
        copy_words = (dlc + 3U) / sizeof(uint32_t);
    } else {
        switch (dlc) {
            case can_payload_size_12:
                copy_words = 3U;
                break;
            case can_payload_size_16:
                copy_words = 4U;
                break;
            case can_payload_size_20:
                copy_words = 5U;
                break;
            case can_payload_size_24:
                copy_words = 6U;
                break;
            case can_payload_size_32:
                copy_words = 8U;
                break;
            case can_payload_size_48:
                copy_words = 12U;
                break;
            case can_payload_size_64:
                copy_words = 16U;
                break;
            default:
                /* Code should never touch here */
                break;
        }
    }

    return copy_words;
}

static void can_fill_tx_buffer(CAN_Type *base, const can_transmit_buf_t *message)
{
    base->TBUF[0] = message->buffer[0];
    base->TBUF[1] = message->buffer[1];

    uint32_t copy_words = can_get_data_words_from_dlc(message->dlc);
    for (uint32_t i = 0U; i < copy_words; i++) {
        base->TBUF[2U + i] = message->buffer[2U + i];
    }
}

hpm_stat_t can_send_message_blocking(CAN_Type *base, const can_transmit_buf_t *message)
{
    hpm_stat_t status = status_invalid_argument;

    do {

        if ((base == NULL) || (message == NULL)) {
            break;
        }

        status = status_success;
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TBSEL_MASK;

        can_fill_tx_buffer(base, message);

        /* Wait until STB is not full */
        int32_t timeout_cnt = CAN_TIMEOUT_CNT;
        while (CAN_CMD_STA_CMD_CTRL_TSSTAT_GET(base->CMD_STA_CMD_CTRL) == CAN_STB_IS_FULL) {
            timeout_cnt--;
            if (timeout_cnt <= 0) {
                status = status_timeout;
                break;
            }
        }
        if (status != status_success) {
            break;
        }

        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSNEXT_MASK | CAN_CMD_STA_CMD_CTRL_TSONE_MASK;
        timeout_cnt = CAN_TIMEOUT_CNT;
        while (CAN_CMD_STA_CMD_CTRL_TSSTAT_GET(base->CMD_STA_CMD_CTRL) != CAN_STB_IS_EMPTY) {
            timeout_cnt--;
            if (timeout_cnt <= 0) {
                status = status_timeout;
                break;
            }
        }

    } while (false);

    return status;
}

hpm_stat_t can_send_high_priority_message_blocking(CAN_Type *base, const can_transmit_buf_t *message)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF ((base == NULL) || (message == NULL));
        status = status_success;

        /* Select the high-priority buffer */
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TBSEL_MASK;

        can_fill_tx_buffer(base, message);

        /* Send the data out */
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TPE_MASK;
        /* Wait until the data is sent out */
        int32_t timeout_cnt = CAN_TIMEOUT_CNT;
        while (IS_HPM_BITMASK_SET(base->CMD_STA_CMD_CTRL, CAN_CMD_STA_CMD_CTRL_TPE_MASK)) {
            timeout_cnt--;
            if (timeout_cnt <= 0) {
                status = status_timeout;
                break;
            }
        }
    } while (false);

    return status;
}

hpm_stat_t can_receive_message_blocking(CAN_Type *base, can_receive_buf_t *message)
{
    hpm_stat_t status = status_invalid_argument;

    do {
        HPM_BREAK_IF ((base == NULL) || (message == NULL));

        while (CAN_CMD_STA_CMD_CTRL_RSTAT_GET(base->CMD_STA_CMD_CTRL) == CAN_RXBUF_IS_EMPTY) {

        }

        /* Get the first 2 words (including CAN ID, Data length and other control bits) */
        message->buffer[0] = base->RBUF[0];
        message->buffer[1] = base->RBUF[1];

        if (message->error_type != 0U) {
            switch (message->error_type) {
            case 1:
                status = status_can_bit_error;
                break;
            case 2:
                status = status_can_form_error;
                break;
            case 3:
                status = status_can_stuff_error;
                break;
            case 4:
                status = status_can_ack_error;
                break;
            case 5:
                status = status_can_crc_error;
                break;
            default:
                status = status_can_other_error;
                break;
            }
            break;
        }

        if (message->remote_frame == 0U) {
            uint32_t copy_words = can_get_data_words_from_dlc(message->dlc);

            for (uint32_t i = 0; i < copy_words; i++) {
                message->buffer[2U + i] = base->RBUF[2U + i];
            }
        }
        /* Release the current buffer */
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_RREL_MASK;

        status = status_success;

    } while (false);

    return status;
}

hpm_stat_t can_read_received_message(CAN_Type *base, can_receive_buf_t *message)
{
    hpm_stat_t status;

    assert((base != NULL) && (message != NULL));

    do {
        /* Get the first 2 words (including CAN ID, Data length and other control bits) */
        message->buffer[0] = base->RBUF[0];
        message->buffer[1] = base->RBUF[1];

        if (message->error_type != 0U) {
            switch (message->error_type) {
            case 1:
                status = status_can_bit_error;
                break;
            case 2:
                status = status_can_form_error;
                break;
            case 3:
                status = status_can_stuff_error;
                break;
            case 4:
                status = status_can_ack_error;
                break;
            case 5:
                status = status_can_crc_error;
                break;
            default:
                status = status_can_other_error;
                break;
            }
            break;
        }

        if (message->remote_frame == 0U) {
            uint32_t copy_words = can_get_data_words_from_dlc(message->dlc);

            for (uint32_t i = 0; i < copy_words; i++) {
                message->buffer[2U + i] = base->RBUF[2U + i];
            }
        }
        /* Release the current buffer */
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_RREL_MASK;

        status = status_success;

    } while (false);

    return status;
}

hpm_stat_t can_get_default_config(can_config_t *config)
{
    hpm_stat_t status = status_invalid_argument;
    if (config != NULL) {
        config->baudrate = 1000000UL; /* 1Mbit/s */
        config->baudrate_fd = 0U;
        config->loopback_mode = can_loopback_none;
        config->enable_canfd = false;
        config->enable_self_ack = false;
        config->disable_re_transmission_for_stb = false;
        config->disable_re_transmission_for_ptb = false;
        config->filter_list_num = 0;
        config->filter_list = NULL;

        status = status_success;
    }

    return status;
}

hpm_stat_t can_init(CAN_Type *base, can_config_t *config, uint32_t src_clk_freq)
{
    hpm_stat_t status = status_invalid_argument;

    do {

        HPM_BREAK_IF((base == NULL) || (config == NULL) || (src_clk_freq == 0U) || (config->filter_list_num > 16U));

        can_reset(base, true);

        base->TTCFG &= ~CAN_TTCFG_TTEN_MASK;
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TTTBM_MASK;

        if (config->enable_canfd) {
            status = can_set_bit_timing(base, can_bit_timing_canfd_norminal, src_clk_freq, config->baudrate);
            HPM_BREAK_IF(status != status_success);
            status = can_set_bit_timing(base, can_bit_timing_canfd_data, src_clk_freq, config->baudrate_fd);
        } else {
            status = can_set_bit_timing(base, can_bit_timing_can2_0, src_clk_freq, config->baudrate);
        }
        HPM_BREAK_IF(status != status_success);

        if (config->disable_re_transmission_for_ptb) {
            base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TPSS_MASK;
        } else {
            base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TPSS_MASK;
        }

        if (config->disable_re_transmission_for_stb) {
            base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSSS_MASK;
        } else {
            base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TSSS_MASK;
        }

        can_enable_self_ack(base, config->enable_self_ack);

        for (uint32_t i = 0; i < config->filter_list_num; i++) {
            (void) can_set_filter(base, &config->filter_list[i]);
        }

        can_reset(base, false);

        can_set_loopback_mode(base, config->loopback_mode);

        can_disable_tx_rx_irq(base, 0xFFU);
        can_disable_error_irq(base, 0xFFU);

        status = status_success;
    } while (false);

    return status;
}
