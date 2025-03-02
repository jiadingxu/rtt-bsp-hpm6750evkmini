/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef HPM_CAN_DRV_H
#define HPM_CAN_DRV_H

#include "hpm_common.h"
#include "hpm_can_regs.h"


/**
 * @brief CAN driver APIs
 * @defgroup can_interface CAN driver APIs
 * @ingroup can_interfaces
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief CAN error codes
 */
enum {
    status_can_bit_error = MAKE_STATUS(status_group_can, 0),    /**< CAN bit error */
    status_can_form_error = MAKE_STATUS(status_group_can, 1),   /**< CAN Form error */
    status_can_stuff_error = MAKE_STATUS(status_group_can, 2),  /**< CAN stuff error */
    status_can_ack_error = MAKE_STATUS(status_group_can, 3),    /**< CAN ACK error */
    status_can_crc_error = MAKE_STATUS(status_group_can, 4),    /**< CAN CRC error */
    status_can_other_error = MAKE_STATUS(status_group_can, 5),  /**< CAN other error */
};

/**
 * @brief CAN Event definitions
 */
#define CAN_EVENT_RECEIVE (CAN_RTIF_RIF_MASK)               /**< CAN Receive event */
#define CAN_EVENT_RX_BUF_OVERRUN (CAN_RTIF_ROIF_MASK)       /**< CAN RX buffer overrun event */
#define CAN_EVENT_RX_BUF_FULL (CAN_RTIF_RFIF_MASK)          /**< CAN RX buffer full event */
#define CAN_EVENT_RX_BUF_ALMOST_FULL (CAN_RTIF_RAFIF_MASK)  /**< CAN RX buffer almost full event */
#define CAN_EVENT_TX_PRIMARY_BUF (CAN_RTIF_TPIF_MASK)       /**< CAN TX primary buffer event */
#define CAN_EVENT_TX_SECONDARY_BUF (CAN_RTIF_TSIF_MASK)     /**< CAN TX secondary buffer event */
#define CAN_EVENT_ERROR (CAN_RTIF_EIF_MASK)                 /**< CAN error event */
#define CAN_EVENT_ABORT (CAN_RTIF_AIF_MASK)                 /**< CAN abort event */


/**
 * @brief CAN loopback types
 */
typedef enum _can_loopback_mode {
    can_loopback_none,              /**< Non-loopback mode */
    can_loopback_internal,          /**< Internal loopback mode */
    can_loopback_external,          /**< External loopback mode */
} can_loopback_mode_t;

/**
 * @brief CAN bit timing options
 */
typedef enum _can_bit_timing_option {
    can_bit_timing_can2_0,          /**< CAN 2.0 bit timing option */
    can_bit_timing_canfd_norminal,  /**< CANFD norminal timing option */
    can_bit_timing_canfd_data,      /**< CANFD data timing option */
} can_bit_timing_option_t;

/**
 * @brief CAN DLC definitions
 */
enum {
    can_payload_size_0 = 0,                 /**< Payload size is 0 */
    can_payload_size_1,                     /**< Payload size is 1 */
    can_payload_size_2,                     /**< Payload size is 2 */
    can_payload_size_3,                     /**< Payload size is 3 */
    can_payload_size_4,                     /**< Payload size is 4 */
    can_payload_size_5,                     /**< Payload size is 5 */
    can_payload_size_6,                     /**< Payload size is 6 */
    can_payload_size_7,                     /**< Payload size is 7 */
    can_payload_size_8,                     /**< Payload size is 8 */
    can_payload_size_12,                    /**< Payload size is 12 */
    can_payload_size_16,                    /**< Payload size is 16 */
    can_payload_size_20,                    /**< Payload size is 20 */
    can_payload_size_24,                    /**< Payload size is 24 */
    can_payload_size_32,                    /**< Payload size is 32 */
    can_payload_size_48,                    /**< Payload size is 48 */
    can_payload_size_64,                    /**< Payload size is 64 */
    can_payload_size_max = can_payload_size_64,
};

/**
 * @brief CAN receive buffer data structure
 */
typedef union _can_rx_buf {
    uint32_t buffer[20];                        /**< CAN buffer size */
    struct {
        struct {
            uint32_t id: 29;                    /**< CAN ID */
            uint32_t : 1;
            uint32_t error_state_indicator: 1;  /**< Error state indicator */
        };
        struct {
            uint32_t dlc: 4;                   /**< Data length option */
            uint32_t bitrate_switch: 1;        /**< bitrate switch flag */
            uint32_t canfd_frame: 1;           /**< CANFD frame flag */
            uint32_t remote_frame: 1;          /**< Remote frame flag */
            uint32_t extend_id: 1;             /**< Extended ID flag */
            uint32_t : 4;
            uint32_t loopback_message: 1;      /**< Loopback message flag */
            uint32_t error_type: 3;            /**< Error type */
            uint32_t cycle_time: 16;           /**< Cycle time */
        };
        uint8_t data[];                        /**< Data pointer */
    };
} can_receive_buf_t;

/**
 * @brief CAN transmit buffer data structure
 */
typedef union _can_tx_buf {
    uint32_t buffer[18];                            /**< CAN transmit buffer */
    struct {
        struct {
            uint32_t id: 29;                       /**< CAN ID */
            uint32_t : 1;
            uint32_t transmit_timestamp_enable: 1;  /**< Timestamp enable flag */
        };
        struct {
            uint32_t dlc: 4;                        /**< Data length option */
            uint32_t bitrate_switch: 1;             /**< bitrate switch flag */
            uint32_t canfd_frame: 1;                /**< CANFD frame flag */
            uint32_t remote_frame: 1;               /**< remote_frame flag */
            uint32_t extend_id: 1;                  /**< Extended ID flag */
            uint32_t : 24;
        };
        uint8_t data[];                             /**< Data pointer */
    };
} can_transmit_buf_t;

/**
 * @brief CAN acceptance filter modes
 */
typedef enum _can_filter_mode {
    can_filter_mode_both_frames,                /**< Accept both standard frame and extended frame */
    can_filter_mode_standard_frames,            /**< Accept standard frame only */
    can_filter_mode_extended_frames,            /**< Accept extended frame only */
} can_filter_mode_t;

/**
 * @brief CAN acceptance configuration
 */
typedef struct {
    uint16_t index;                             /**< CAN filter index */
    can_filter_mode_t mode;                     /**< CAN filter mode */
    bool enable;                                /**< CAN filter enable flag */
    uint32_t code;                              /**< CAN filter code */
    uint32_t mask;                              /**< CAN filter mask */
} can_filter_config_t;

/**
 * @brief CAN configuration
 */
typedef struct {
    uint32_t baudrate;                          /**< CAN 2.0 baudrate */
    uint32_t baudrate_fd;                       /**< CANFD baudrate */
    can_loopback_mode_t loopback_mode;          /**< CAN loopback mode */
    bool enable_canfd;                          /**< CANFD enable flag */
    bool enable_self_ack;                       /**< CAN self-ack flag */
    bool disable_re_transmission_for_ptb;       /**< disable re-transmission for primary transmit buffer */
    bool disable_re_transmission_for_stb;       /**< disable re-transmission for secondary transmit buffer */
    uint32_t filter_list_num;                   /**< element number of CAN filters in filter list */
    can_filter_config_t *filter_list;           /**< CAN filter list pointer */

} can_config_t;


#ifdef __cpluspuls
extern "C" {
#endif


/**
 * @brief Reset CAN controller
 *
 * @param [in] base  CAN base address
 * @param [in] enable reset flag for CAN controller
 *  @arg true reset the CAN controller
 *  @arg false clear the CAN controller reset flag
 */
static inline void can_reset(CAN_Type *base, bool enable)
{
    if (enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_RESET_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_RESET_MASK;
    }
}

/**
 * @brief Set CAN loopback mode
 *
 * @param [in] base CAN base address
 * @param [in] mode CAN loopback mode
 *  @arg can_loopback_none Non-loopback mode
 *  @arg can_loopback_internal internal loopback mode
 *  @arg can_loopback_external external loopback mode
 */
static inline void can_set_loopback_mode(CAN_Type *base, can_loopback_mode_t mode)
{
    uint32_t cfg_stat = base->CMD_STA_CMD_CTRL & ~(CAN_CMD_STA_CMD_CTRL_LBME_MASK | CAN_CMD_STA_CMD_CTRL_LBMI_MASK);
    if (mode == can_loopback_internal) {
        cfg_stat |= CAN_CMD_STA_CMD_CTRL_LBMI_MASK;
    } else if (mode == can_loopback_external) {
        cfg_stat |= CAN_CMD_STA_CMD_CTRL_LBME_MASK;
    } else {
        // Do nothing
    }
    base->CMD_STA_CMD_CTRL = cfg_stat;
}

/**
 * @brief Enable CAN listen-only mode
 * @param [in] base CAN base address
 * @param [in] enable CAN listen-only mode enable flag
 *  @arg true enable listen-only mode
 *  @arg false disable listen-only mode
 */
static inline void can_enable_listen_only_mode(CAN_Type *base, bool enable)
{
    if (enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_LOM_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_LOM_MASK;
    }
}

/**
 * @brief Enter CAN standby mode
 * @param [in] base CAN base address
 * @param [in] enable  Can standby mode enable flag
 *  @arg true enable standby mode
 *  @arg false disable standby mode
 */
static inline void can_enter_standby_mode(CAN_Type *base, bool enable)
{
    if (enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_STBY_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_STBY_MASK;
    }
}

/**
 * @brief Select CAN TX buffer
 * @param [in] base CAN base address
 * @param [in] select_secondary_buffer CAN secondary transmit buffer selection flag
 *  @arg true Select the secondary transmit buffer
 *  @arg false Select the primary transmit buffer
 */
static inline void can_select_tx_buffer(CAN_Type *base, bool select_secondary_buffer)
{
    if (select_secondary_buffer) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TBSEL_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TBSEL_MASK;
    }
}

/**
 * @brief Start CAN high priority message transmit
 * @param [in] base CAN base address
 *
 */
static inline void can_start_high_priority_message_transmit(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TPE_MASK;
}

/**
 * @brief Abort CAN high priority message transmit
 * @param [in] base CAN base address
 */
static inline void can_abort_high_priority_message_transmit(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TPA_MASK;
}

/**
 * @brief Start CAN message transmit
 * @param [in] base CAN base address
 */
static inline void can_start_message_transmit(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSONE_MASK;
}

/**
 * @brief Start CAN all messages transmit
 * @param [in] base CAN base address
 */
static inline void can_start_all_message_transmit(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSALL_MASK;
}

/**
 * @brief Abort CAN message transmit
 * @param [in] base CAN base address
 */
static inline void can_abort_message_transmit(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSA_MASK;
}

/**
 * @brief Switch to next CAN tx buffer
 * @param [in] base CAN base address
 */
static inline void can_switch_to_next_tx_buffer(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSNEXT_MASK;
}

/**
 * @brief Select CAN tx buffer priority mode
 * @param [in] base CAN base address
 * @param [in] enable_priority_decision CAN tx buffer priority mode selection flag
 *  @arg true priority decision mode
 *  @arg false FIFO mode
 */
static inline void can_select_tx_buffer_priority_mode(CAN_Type *base, bool enable_priority_decision)
{
    if (enable_priority_decision) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_TSMODE_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_TSMODE_MASK;
    }
}

/**
 * @brief Get CAN secondary transmit buffer status
 * @param [in] base CAN base address
 * @retval secondary transmit buffer status, 0: empty, 1: <= 1/2 full, 2: > 1/2 full, 3: full
 *
 */
static inline uint8_t can_get_secondary_transmit_buffer_status(CAN_Type *base)
{
    return CAN_CMD_STA_CMD_CTRL_TSSTAT_GET(base->CMD_STA_CMD_CTRL);
}

/**
 * @brief Enable can self ack
 * @param [in] base CAN base address
 * @param [in] enable Self-ack enable flag, true or false
 *
 */
static inline void can_enable_self_ack(CAN_Type *base, bool enable)
{
    if (enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_SACK_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_SACK_MASK;
    }
}

/**
 * @brief Set CAN receive buffer overflow mode
 * @param [in] base CAN base address
 * @param [in] override_enable receive buffer overflow mode option
 *  @arg true the oldest message will be overwritten
 *  @arg false the new message will not be stored
 */
static inline void can_set_receive_buffer_overflow_mode(CAN_Type *base, bool override_enable)
{
    if (override_enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_ROM_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_ROM_MASK;
    }
}

/**
 * @brief Check whether CAN receive buffer is overflow
 * @param [in] base CAN base address
 * @retval true or false
 */
static inline bool can_is_receive_buffer_overflow(CAN_Type *base)
{
    return IS_HPM_BITMASK_SET(base->CMD_STA_CMD_CTRL, CAN_CMD_STA_CMD_CTRL_ROV_MASK);
}

/**
 * @brief Release CAN receive buffer
 *
 * @param [in] base CAN base address
 */
static inline void can_release_receive_buffer(CAN_Type *base)
{
    base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_RREL_MASK;
}

/**
 * @brief Allow can store all date in receive buffer
 * @param [in] base CAN base address
 * @param [in] allow flag to determine whether to store error data frame
 */
static inline void can_allow_store_all_data_in_receive_buffer(CAN_Type *base, bool allow)
{
    if (allow) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_RBALL_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_RBALL_MASK;
    }
}

/**
 * @brief Get CAN receive buffer status
 * @param [in] base CAN base address
 * @retval Receive buffer status, 0: empty, 1: > empty, < almost full, 2: >= almost full, 3: full
 */
static inline uint8_t can_get_receive_buffer_status(CAN_Type *base)
{
    return CAN_CMD_STA_CMD_CTRL_RSTAT_GET(base->CMD_STA_CMD_CTRL);
}


/**
 * @brief Enable CAN FD ISO mode
 * @param [in] base CAN base address
 * @param enable CAN-FD ISO mode enable flag
 */
static inline void can_enable_can_fd_iso_mode(CAN_Type *base, bool enable)
{
    if (enable) {
        base->CMD_STA_CMD_CTRL |= CAN_CMD_STA_CMD_CTRL_FD_ISO_MASK;
    } else {
        base->CMD_STA_CMD_CTRL &= ~CAN_CMD_STA_CMD_CTRL_FD_ISO_MASK;
    }
}

/**
 * @brief Enable CAN TX/RX interrupt
 * @param [in] base CAN base address
 * @param [in] mask CAN interrupt mask
 */
static inline void can_enable_tx_rx_irq(CAN_Type *base, uint8_t mask)
{
    base->RTIE |= mask;
}

/**
 * @brief Disable CAN TX/RX interrupt
 * @param [in] base CAN base address
 * @param [in] mask CAN interrupt mask
 */
static inline void can_disable_tx_rx_irq(CAN_Type *base, uint8_t mask)
{
    base->RTIE &= ~mask;
}

/**
 * @brief Clear CAN TX/RX flags
 * @param [in] base CAN base address
 * @param [in] flags CAN TX/RX interrupt flags
 */
static inline void can_clear_tx_rx_flags(CAN_Type *base, uint8_t flags)
{
    base->RTIF = flags;
}

/**
 * @brief Get CAN TX/RX flags
 *
 * @param [in] base CAN base address
 * @retval CAN Tx/RX interrupt flags
 */
static inline uint8_t can_get_tx_rx_flags(CAN_Type *base)
{
    return base->RTIF;
}

/**
 * @brief Enable CAN error interrupt
 * @param [in] base CAN base address
 * @param [in] mask CAN error interrupt mask
 */
static inline void can_enable_error_irq(CAN_Type *base, uint8_t mask)
{
    base->ERRINT |= mask;
}

/**
 * @brief Disable CAN error interrupt
 *
 * @param [in] base CAN base address
 * @param [in] mask CAN error interrupt mask
 */
static inline void can_disable_error_irq(CAN_Type *base, uint8_t mask)
{
    base->ERRINT &= ~mask;
}

/**
 * @brief Get CAN error interrupt flags
 * @param [in] base CAN base address
 * @retval CAN error interrupt flags
 */
static inline uint8_t can_get_error_interrupt_flags(CAN_Type *base)
{
    return (base->ERRINT & (uint8_t) ~(CAN_ERRINT_EPIE_MASK | CAN_ERRINT_ALIE_MASK | CAN_ERRINT_BEIE_MASK));
}

/**
 * @brief Get last CAN error kind
 * @param [in] base CAN base address
 * @retval CAN last error kind
 */
static inline uint8_t can_get_last_error_kind(CAN_Type *base)
{
    return CAN_EALCAP_KOER_GET(base->EALCAP);
}

/**
 * @brief Get CAN last arbitrary lost position
 * @param [in] base CAN base address
 * @retval CAN last arbitrary lost position
 */
static inline uint8_t can_get_last_arbitration_lost_position(CAN_Type *base)
{
    return CAN_EALCAP_ALC_GET(base->EALCAP);
}

/**
 * @brief Set CAN transimitter delay compensation
 *
 * @param [in] base CAN base address
 * @param [in] sample_point delay compensation sample point
 * @param [in] enable delay compensation enable flag
 */
static inline void can_set_transmitter_delay_compensation(CAN_Type *base, uint8_t sample_point, bool enable)
{
    base->TDC = CAN_TDC_SSPOFF_SET(sample_point) | CAN_TDC_TDCEN_SET((uint8_t) enable);
}

/**
 * @brief Set CAN warning limits
 * @param [in] base CAN base address
 * @param [in] almost_full_limit
 * @param [in] error_warning_limit
 */
static inline void can_set_warning_limits(CAN_Type *base, uint8_t almost_full_limit, uint8_t error_warning_limit)
{
    base->LIMIT = CAN_LIMIT_AFWL_SET(almost_full_limit) | CAN_LIMIT_EWL_SET(error_warning_limit);
}

/**
 * @brief Get CAN receive error count
 * @param [in] base CAN base address
 * @retval CAN receive error count
 */
static inline uint8_t can_get_receive_error_count(CAN_Type *base)
{
    return base->RECNT;
}

/**
 * @brief Get CAN transmit error count
 * @param [in] base CAN base address
 * @retval CAN transmit error count
 */
static inline uint8_t can_get_transmit_error_count(CAN_Type *base)
{
    return base->TECNT;
}

/**
 * @brief Disable CAN filter
 *
 * @param [in] base CAN base address
 * @param index  CAN filter index
 */
static inline void can_disable_filter(CAN_Type *base, uint32_t index)
{
    base->ACF_EN &= (uint16_t) ~(1U << index);
}

/**
 * @brief Get default CAN configuration parameters
 * @param [out] config CAN configuration structure
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_get_default_config(can_config_t *config);

/**
 * @brief Initialize the CAN controller
 * @param [in] base CAN base address
 * @param [in] config CAN configuration
 * @param [in] src_clk_freq CAN IP source clock frequency
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_init(CAN_Type *base, can_config_t *config, uint32_t src_clk_freq);

/**
 * @brief Configure the CAN bit timing for CAN BUS
 * @param [in] base CAN base address
 * @param [in] option CAN bit timing option
 * @param [in] src_clk_freq CAN IP source clock frequency
 * @param [in] baudrate CAN baudrate in MHz
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_set_bit_timing(CAN_Type *base, can_bit_timing_option_t option, uint32_t src_clk_freq, uint32_t baudrate);

/**
 * @brief Configure the acceptable filter
 * @param [in] base CAN base address
 * @param [in] config CAN filter configuration
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_set_filter(CAN_Type *base, const can_filter_config_t *config);

/**
 * @brief Send CAN message using blocking transfer
 * @param [in] base CAN base address
 * @param [in] message CAN message to be sent out
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_send_message_blocking(CAN_Type *base, const can_transmit_buf_t *message);

/**
 * @brief Send high-priority message using blocking transfer
 * @param [in] base CAN base address
 * @param [in] message CAN message to be sent out
 * @retval API execution status, status_success or status_invalid_argument
 */
hpm_stat_t can_send_high_priority_message_blocking(CAN_Type *base, const can_transmit_buf_t *message);

/**
 * @brief Receive CAN message using blocking transfer
 * @param [in] base CAN base address
 * @param [out] message CAN message buffer
 * @retval API execution status
 *          @arg status_success API exection is successful
 *          @arg status_invalid_argument Invalid parameters
 *          @arg status_can_bit_error CAN bit error happened during receiving message
 *          @arg status_can_form_error  CAN form error happened during receiving message
 *          @arg status_can_stuff_error CAN stuff error happened during receiving message
 *          @arg status_can_ack_error CAN ack error happened during receiving message
 *          @arg status_can_crc_error CAN crc error happened during receiving message
 *          @arg status_can_other_error Other error happened during receiving message
 */
hpm_stat_t can_receive_message_blocking(CAN_Type *base, can_receive_buf_t *message);


/**
 * @brief Read Received CAN message
 * @note  This API assumes that the received CAN message is available.
 *        It can be used in the interrupt handler
 * @param [in] base CAN base address
 * @param [out] message CAN message buffer
 * @retval status_success API exection is successful
 * @retval status_invalid_argument Invalid parameters
 * @retval status_can_bit_error CAN bit error happened during receiving message
 * @retval status_can_form_error  CAN form error happened during receiving message
 * @retval status_can_stuff_error CAN stuff error happened during receiving message
 * @retval status_can_ack_error CAN ack error happened during receiving message
 * @retval status_can_crc_error CAN crc error happened during receiving message
 * @retval status_can_other_error Other error happened during receiving message
 */
hpm_stat_t can_read_received_message(CAN_Type *base, can_receive_buf_t *message);

/**
 * @}
 *
 */


#ifdef __cpluspuls
}
#endif


#endif /* HPM_CAN_DRV_H */
