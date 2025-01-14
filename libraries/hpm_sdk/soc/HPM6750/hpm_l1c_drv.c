/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_common.h"
#include "hpm_soc.h"
#include "hpm_l1c_drv.h"

static void l1c_op(uint8_t opcode, uint32_t address, uint32_t size)
{
    uint32_t i;
    uint32_t next_address = 0;
    uint32_t tmp;

#define CCTL_VERSION (3U << 18)
    address &= ~(HPM_L1C_CACHELINE_SIZE - 1);
    size += HPM_L1C_CACHELINE_SIZE;

    if ((read_csr(CSR_MMSC_CFG) & CCTL_VERSION)) {
        l1c_cctl_address(address);
        while (next_address < (address + size)) {
            l1c_cctl_cmd(opcode);
            next_address = l1c_cctl_get_address();
        }
    } else {
        for (i = 0, tmp = 0; tmp < size; i++) {
            l1c_cctl_address_cmd(opcode, address + i * HPM_L1C_CACHELINE_SIZE);
            tmp += HPM_L1C_CACHELINE_SIZE;
        }
    }
}

void l1c_dc_enable(void)
{
    if (!l1c_dc_is_enabled()) {
        clear_csr(CSR_MCACHE_CTL, HPM_MCACHE_CTL_DC_WAROUND_MASK);
        set_csr(CSR_MCACHE_CTL, 
#ifdef L1C_DC_WAROUND_VALUE
                HPM_MCACHE_CTL_DC_WAROUND(L1C_DC_WAROUND_VALUE) |
#endif
                                HPM_MCACHE_CTL_DPREF_EN_MASK
                              | HPM_MCACHE_CTL_DC_EN_MASK);
    }
}

void l1c_dc_disable(void)
{
    if (l1c_dc_is_enabled()) {
        clear_csr(CSR_MCACHE_CTL, HPM_MCACHE_CTL_DC_EN_MASK);
    }
}

void l1c_ic_enable(void)
{
    if (!l1c_ic_is_enabled()) {
        set_csr(CSR_MCACHE_CTL, HPM_MCACHE_CTL_IPREF_EN_MASK
                              | HPM_MCACHE_CTL_IC_EN_MASK);
    }
}

void l1c_ic_disable(void)
{
    if (l1c_ic_is_enabled()) {
        clear_csr(CSR_MCACHE_CTL, HPM_MCACHE_CTL_IC_EN_MASK);
    }
}

void l1c_fence_i()
{
    __asm("fence.i");
}

void l1c_dc_invalidate_all(void)
{
    l1c_cctl_cmd(HPM_L1C_CCTL_CMD_L1D_INVAL_ALL);
}

void l1c_dc_writeback_all(void)
{
    l1c_cctl_cmd(HPM_L1C_CCTL_CMD_L1D_WB_ALL);
}

void l1c_dc_fill_lock(uint32_t address, uint32_t size)
{
    l1c_op(HPM_L1C_CCTL_CMD_L1D_VA_LOCK, address, size);
}

void l1c_dc_invalidate(uint32_t address, uint32_t size)
{
    l1c_op(HPM_L1C_CCTL_CMD_L1D_VA_INVAL, address, size);
}

void l1c_dc_writeback(uint32_t address, uint32_t size)
{
    l1c_op(HPM_L1C_CCTL_CMD_L1D_VA_WB, address, size);
}

void l1c_dc_flush(uint32_t address, uint32_t size)
{
    l1c_op(HPM_L1C_CCTL_CMD_L1D_VA_WBINVAL, address, size);
}

void l1c_ic_invalidate(uint32_t address, uint32_t size)
{
    l1c_op(HPM_L1C_CCTL_CMD_L1I_VA_INVAL, address, size);
}

void l1c_ic_fill_lock(uint32_t address, uint32_t size)
{
    l1c_op(HPM_L1C_CCTL_CMD_L1I_VA_LOCK, address, size);
}
