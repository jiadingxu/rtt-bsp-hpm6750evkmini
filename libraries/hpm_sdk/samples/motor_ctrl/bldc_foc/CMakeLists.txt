# Copyright 2021 hpmicro
# SPDX-License-Identifier: BSD-3-Clause
cmake_minimum_required(VERSION 3.13)
set(CONFIG_FREEMASTER 1)
set(CONFIG_HPM_DSP 1)
set(CONFIG_MOTORCTRL 1)

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})
# sdk_compile_options("-Os")
project(bldc_foc)
sdk_app_inc(src)
sdk_app_src(src/bldc_foc_callback.c)
sdk_app_src(src/bldc_foc.c)
sdk_ld_options("-lm")
# sdk_compile_options("-mabi=ilp32f")
# sdk_compile_options("-march=rv32imafc")
# sdk_compile_options("-mabi=ilp32d")
generate_ses_project()