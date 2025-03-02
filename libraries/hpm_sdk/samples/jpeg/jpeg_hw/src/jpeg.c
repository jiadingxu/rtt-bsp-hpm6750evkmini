/*
 * Copyright (c) 2021 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
/*---------------------------------------------------------------------
 * Includes
 ---------------------------------------------------------------------
 */
#include "board.h"
#include "hpm_lcdc_drv.h"
#include "hpm_l1c_drv.h"
#include "hpm_sysctl_drv.h"
#include "jpeglib.h"
#include "hpm_jpeg_drv.h"
#include "qlook.h"
/*--------------------------------------------------------------------
 * Macro Definitions
 ---------------------------------------------------------------------
 */
#if defined SD_FATFS_MODE
#include "sd_fatfs.h"
#elif defined TINYUSB_HOST_MODE
#include "msc_app.h"
#endif
/*JPEG Definitions*/
#ifndef TEST_JPEG
#define TEST_JPEG HPM_JPEG
#endif
/*LCD Definitions*/
#ifndef LCD
#define LCD BOARD_LCD_BASE
#define LCD_IRQ BOARD_LCD_IRQ
#endif
/*SOS Maximum number of scans*/
#define MAX_COMPS_IN_ONE_SCAN 4
/*huffmin data buff length*/
#define HUFFMINLEN 16
/*huffbase data buff length*/
#define HUFFBASELEN 64
/*huffsymb data buff length*/
#define HUFFSYMBLEN 336
/*qdtable data buff length*/
#define QDTABLELEN 256
/*qetable data buff length*/
#define QETABLELEN 256
/*huffenc data buff length*/
#define HUFFENCLEN 384
/*decode rgb-data buff length*/
#define OUT_IMG_LEN 100000
/*Pixel format of LCD display*/
#define PIXEL_FORMAT display_pixel_format_rgb565
/*---------------------------------------------------------------------*
 * Define variables
 *---------------------------------------------------------------------
 */
/*JPG file data buff*/
uint8_t filebuff[FILEBUFFLEN];
/*JPG data buff size*/
int32_t jpg_size;
/*JPG file Width and height*/
int32_t rgbwidth = 0, rgbheight = 0;
/*JPG file data esc-data buff*/
uint8_t in_ecs[FILEBUFFLEN];
/*JPG file data esc-data length*/
uint16_t in_ecs_length;
/*huffmin data buff*/
uint32_t huffmin[HUFFMINLEN];
/*huffbase data buff*/
uint16_t huffbase[HUFFBASELEN];
/*huffsymb data buff*/
uint8_t huffsymb[HUFFSYMBLEN];
/*qdtable data buff*/
uint16_t qdtable[QDTABLELEN];
/*qetable data buff*/
uint16_t qetable[QETABLELEN];
/*huffenc data buff*/
uint16_t huffenc[HUFFENCLEN] = {0};
/*encode esc-data buff*/
uint8_t out_ecs[FILEBUFFLEN];
/*decode rgb-data buff*/
uint8_t out_img[OUT_IMG_LEN];

/*---------------------------------------------------------------------*
 * Display picture data on LCD
 *---------------------------------------------------------------------
 */
void lcdc_display_picture(int32_t rgb_width, int32_t rgb_heihgt)
{
    lcdc_config_t config = {0};
    lcdc_layer_config_t layer = {0};

    /*LCD default parameter settings*/
    lcdc_get_default_config(LCD, &config);
    config.resolution_x = BOARD_LCD_WIDTH;
    config.resolution_y = BOARD_LCD_HEIGHT;
    lcdc_init(LCD, &config);
    lcdc_get_default_layer_config(LCD, &layer, PIXEL_FORMAT);

    /*LCD layer parameter configuration*/
    layer.position_x = 0;
    layer.position_y = 0;
    layer.width = rgb_width;
    layer.height = rgb_heihgt;
    layer.buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)out_img);
    layer.alphablend.src_alpha = 0xF4; /* src */
    layer.alphablend.dst_alpha = 0xF0; /* dst */
    layer.alphablend.src_alpha_op = display_alpha_op_override;
    layer.alphablend.dst_alpha_op = display_alpha_op_override;
    layer.background.u = 0xFFFF0000;
    layer.alphablend.mode = display_alphablend_mode_xor;
    if (status_success != lcdc_config_layer(LCD, 0, &layer, true)) {
        printf("failed to configure layer\n");
        while (1) {
        };
    }

    /*LCD display function enable*/
    lcdc_turn_on_display(LCD);
}

/*---------------------------------------------------------------------*
 * extract tables from a JPEG image
 *---------------------------------------------------------------------
 */
void tabgen(void)
{
    uint8_t filebyte;
    uint32_t i, j, n, v, rst, nm, code, hid, z;
    uint32_t dc, ht, abase, bbase, hbase, hb;
    uint32_t l[16];
    uint8_t tmp[4];
    uint32_t nTemp;
    uint16_t fp = 0;

    /*BASE mem (base.dat) - 9 bits array*/
    uint32_t hea[64] = {0};
    /*SYMB mem (symb.dat) - 8 bits array*/
    uint32_t heb[336] = {0};
    /*min values (min.dat)*/
    uint32_t min[64];
    /*QT (qdtable.dat)*/
    uint32_t qt[4][64];

    /* Initialize HuffEnc with default values */
    for (i = 0; i < 384; i++) {
        huffenc[i] = 0xfff;
    }
    for (i = 168, j = 0xfd0; i < 176; i++, j++) {
        huffenc[i] = j;
    }
    for (i = 344, j = 0xfd0; i < 352; i++, j++) {
        huffenc[i] = j;
    }
    ht = nm = rst = 0;
    for (i = 162; i < 174; i++) {
        heb[i] = 0;
    }
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 64; j++) {
            qt[i][j] = 0;
        }
    }

    /* scanning and extract tables from jpeg-data*/
    while (jpg_size - fp) {
        if (filebuff[fp++] != 0xff) {
            continue;
        }

        while
        ((filebyte = filebuff[fp++]) == 0xff);
        if (filebyte == 0) {
            continue;
        }
marker:
        switch (filebyte) {
        /*Baseline (0, 0)*/
        case 0xc0:
            fp += 3;
            i = filebuff[fp++];
            rgbheight = (i << 8) | filebuff[fp++];
            i = filebuff[fp++];
            rgbwidth = (i << 8) | filebuff[fp++];
            n = filebuff[fp++];
            fp += 3 * n;
            printf("SOF");
            break;
        /*Ext. Sequential, Huffman (0, 0)*/
        case 0xc1:
        /*Progressive, Huffman (1, 0)*/
        case 0xc2:
        /*Lossless, Huffman*/
        case 0xc3:
        /*Differential Sequential, Huffman*/
        case 0xc5:
        /*Differential Progressive, Huffman*/
        case 0xc6:
        /*Differential Lossless, Huffman*/
        case 0xc7:
        /*Extended Sequential, Arithmetic (0, 1)*/
        case 0xc9:
        /*Progressive, Arithmetic (1, 1)*/
        case 0xca:
        /*Lossless, Huffman */
        case 0xcb:
        /*Differential Sequential, Arithmetic*/
        case 0xcd:
        /*Differential Progressive, Arithmetic*/
        case 0xce:
        /*Differential Lossless, Arithmetic*/
        case 0xcf:
            fp += 7;
            n = filebuff[fp++];
            fp += 3 * n;
            printf("SOF");
            break;
        /* Get the length of the marker segment */
        case 0xc4:
            printf("DHT");
            /* Lh : HT length (16b)*/
            v = filebuff[fp++];
            n = (v << 8) | filebuff[fp++];
            printf("Lh %d\n", n);
            /* Reduce marker segment byte count */
            n -= 2;
            while (n) {
                /* Get the type of table */
                v = filebuff[fp++]; /* Tc & Th*/
                n--;
                printf("Tc %d\n", v >> 4);
                printf("Th %d\n", v & 15);
                hid = v >> 4 ? 2 : 0;
                hid |= v & 15 ? 1 : 0;
                switch (hid) {
                case 1:
                    hbase = 368;
                    break;
                case 2:
                    hbase = 0;
                    break;
                case 3:
                    hbase = 176;
                    break;
                default:
                    hbase = 352;
                    break;
                }
                if ((v >> 4)) {
                    abase = 0;
                } else {
                    abase = 1;
                }
                dc = abase;
                ht = v & 15;
                abase |= ht << 1;
                switch (abase) {
                case 1:
                case 3:
                    bbase = 162;
                    break;
                case 2:
                    bbase = 174;
                    break;
                default:
                    bbase = 0;
                    break;
                }
                abase <<= 4;
                /* Memory initialization */
                for (i = abase; i < abase + 16; i++) {
                    hea[i] = 255;
                }
                /* Get the number of codes for each length */
                /* Lj : # of Huffman codes of length i
                 *- specifies the # of Huffman codes for each of 16 possible lengths
                 *allowed by spec. BITS
                 */
                for (i = 0; i < 16; i++) {
                    l[i] = filebuff[fp++];
                }
                /* Reduce marker segment byte count */
                n -= 16;
                code = 0;
                for (i = 0; i < 16; i++, abase++) {
                    min[abase] = code;
                    hea[abase] = bbase - code;
                    if (l[i]) {
                        /* Vi,j : associated with each Huffman code
                         *- specifies, for each i the value associated with each Huffman code
                         *of length i.  HUFFVAL
                         */
                        for (j = 0; j < l[i]; j++, bbase++) {
                            v = filebuff[fp++];
                            /* Reduce marker segment byte count */
                            n--;
                            if (dc) {
                                huffenc[hbase + v] = (i << 8) | (code & 0xff);
                                v &= 15;
                                if (ht) {
                                    v <<= 4;
                                }
                                heb[bbase] |= v;
                            } else {
                                if (v == 0) {
                                    hb = 160;
                                } else if (v == 0xf0) {
                                    hb = 161;
                                } else {
                                    hb = (v >> 4) * 10 + (v & 0xf) - 1;
                                }
                                huffenc[hbase + hb] = (i << 8) | (code & 0xff);
                                heb[bbase] = v;
                            }
                            code++;
                        }
                    }
                    code <<= 1;
                }
            }
            break;
        /*JPG extensions*/
        case 0xc8:
            printf("JPG extensions !!!\n");
            break;
        /*arithmatic*/
        case 0xcc:
            printf("DAC !!! (arithmatic coding not supported)\n");
            break;
        /*reset*/
        case 0xd0:
        case 0xd1:
        case 0xd2:
        case 0xd3:
        case 0xd4:
        case 0xd5:
        case 0xd6:
        case 0xd7:
            printf("reset %d\n", filebyte & 15);
            rst++;
            break;
        /*SOI*/
        case 0xd8:
            printf("SOI\n");
            break;
        /*EOI*/
        case 0xd9:
            printf("EOI\n");
            in_ecs[in_ecs_length++] = 0xff;
            in_ecs[in_ecs_length++] = 0xd9;
            break;
        /*data scan*/
        case 0xda:
            /*Start of Scan*/
            printf("SOS\n");
            tmp[0] = filebuff[fp++];
            tmp[1] = filebuff[fp++];
            /*Ls (scan header length)*/
            nTemp = (tmp[0] << 8) | (tmp[1]);
            /*Ns (# of image components)*/
            n = filebuff[fp++];
            printf("Ls = %d, Ns = %02x\n", nTemp, n);

            if ((nTemp != (n * 2 + 6)) || (n < 1) || (n > MAX_COMPS_IN_ONE_SCAN)) {
                printf("bad scan length Ls = %d vs. (%d * 2 + 6)\n", nTemp, n);
            }
            for (i = 0; i < n; i++) {
                /*Cs, Td&Ta*/
                fp += 2;
            }
            /*ss se Ah&Al*/
            fp += 3;
            for (;;) {
                filebyte = filebuff[fp++];
                if (filebyte == 0xff) {
                    filebyte = filebuff[fp++];
                    if ((filebyte != 0x00) && ((filebyte & 0xf8) != 0xd0)) {
                        goto marker;
                    } else {
                        in_ecs[in_ecs_length++] = 0xff;
                    }
                }
                in_ecs[in_ecs_length++] = filebyte;
            }
            break;
        case 0xdb:
            /*Quantization Table  */
            printf("DQT\n");
            /*Lq : QT Length (16b)*/
            v = filebuff[fp++];
            v = (v << 8) | filebuff[fp++];
            printf("Lq %d\n", v);
            v = filebuff[fp++];
            /* Pq : QT element precision (4b)
             *- specifies the precision of the Qk values.
             *  0 indicates 8-bits Qk values.
               1 indicates 16-bits Qk values
            */
            printf("Pq %d\n", v >> 4);
            /* Tq : QT destination identifier (4b)
             *- specifies one of 4 possible destnations at the decoder into
             *which the QT shall be installed.
             */
            printf("Tq %d\n", v & 15);
            n = v & 15;
            for (i = 0; i < 64; i++) {
                /* Qk: Quantization table element
                 *k is the index in the zigzag ordering of the DCT coeff
                 *JPC only do 8-bit Qk! (ie, Pq shall be 0)
                 */
                qt[n][i] = filebuff[fp++];
            }
            break;
        case 0xdd:
            /*Restart Interval Definition*/
            printf("DRI\n");
            /* Lr : restart interval segment length (16b)
             *- specifies the length of the paramenters in the DRI segment
             */
            v = filebuff[fp++];
            v = (v << 8) | filebuff[fp++];
            printf("Lr %d\n", v);
            /* Ri : restart interval (16b)
             *- specifies the number of MCU in the restart interval.
             */
            v = filebuff[fp++];
            v = (v << 8) | filebuff[fp++];
            printf("Ri %d\n", v);
            break;
        /* All these markers are ignored */
        case 0xe0:
        case 0xe1:
        case 0xe2:
        case 0xe3:
        case 0xe4:
        case 0xe5:
        case 0xe6:
        case 0xe7:
        case 0xe8:
        case 0xe9:
        case 0xea:
        case 0xeb:
        case 0xec:
        case 0xed:
        case 0xee:
        case 0xef:
        case 0xf0:
        case 0xf1:
        case 0xf2:
        case 0xf3:
        case 0xf4:
        case 0xf5:
        case 0xf6:
        case 0xf7:
        case 0xf8:
        case 0xf9:
        case 0xfa:
        case 0xfb:
        case 0xfc:
        case 0xfd:
        case 0xfe:
            v = filebuff[fp++];
            v = (v << 8) | filebuff[fp++];
            v -= 2;
            for (i = 0; i < v; i++) {
                fp++;
            }
            break;
        default:
            printf("Unknown marker %x !\n", filebuff);
            break;
        }
    }

    /* Outputs the min values */
    for (i = 0, j = 3; i < 64; j += 7) {
        v = min[i++] & 1;
        v <<= 2;
        v |= min[i++] & 3;
        v <<= 3;
        v |= min[i++] & 7;
        huffmin[j--] = v >> 2;

        v <<= 4;
        v |= min[i++] & 15;
        v <<= 5;
        v |= min[i++] & 31;
        v <<= 6;
        v |= min[i++] & 63;
        v <<= 7;
        v |= min[i++] & 127;
        v <<= 8;
        v |= min[i++] & 255;
        huffmin[j--] = v;

        v = min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        huffmin[j--] = v;

        v = min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        v <<= 8;
        v |= min[i++] & 255;
        huffmin[j] = v;
    }

    /* Outputs the BASE mem values */
    for (i = 0; i < 64; i++) {
        huffbase[i] = hea[i] & 511;
    }

    /* Outputs the SYMB mem values */
    for (i = 0; i < 336; i++) {
        huffsymb[i] = heb[i] & 255;
    }

    /* Outputs the quantization table */
    for (i = 0, z = 0; i < 4; i++) {
        for (j = 0; j < 64; j++, z++) {
            qdtable[z] = qt[i][j];
        }
    }

    /* Outputs the encoder quantization table */
    for (i = 0, z = 0; i < 4; i++) {
        for (j = 0; j < 64; j++, z++) {
            qetable[z] = qlook[qt[i][j]];
        }
    }
}

/*---------------------------------------------------------------------*
 * Initialize JPEG module
 *---------------------------------------------------------------------
 */
void init_jpeg(void)
{
    jpeg_enable(TEST_JPEG);
    /*Fill huffmin table data*/
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffmin, (uint8_t *)huffmin, HUFFMINLEN);
    /*Fill huffbase table data*/
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffbase, (uint8_t *)huffbase, HUFFBASELEN);
    /*Fill huffsymb table data*/
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffsymb, (uint8_t *)huffsymb, HUFFSYMBLEN);
    /*Fill huffenc table data*/
    jpeg_fill_table(TEST_JPEG, jpeg_table_huffenc, (uint8_t *)huffenc, HUFFENCLEN);
    jpeg_disable(TEST_JPEG);
}

/*---------------------------------------------------------------------*
 * Fill decode table data
 *---------------------------------------------------------------------
 */
void fill_jpeg_decode_table(void)
{
    jpeg_enable(TEST_JPEG);
    /*Fill Decoder Q. values*/
    jpeg_fill_table(TEST_JPEG, jpeg_table_qmem, (uint8_t *)qdtable, QDTABLELEN);
    jpeg_disable(TEST_JPEG);
}

/*---------------------------------------------------------------------*
 * Fill encode table data
 *---------------------------------------------------------------------
 */
void fill_jpeg_encode_table(void)
{
    jpeg_enable(TEST_JPEG);
    /*Fill encoder Q. values*/
    jpeg_fill_table(TEST_JPEG, jpeg_table_qmem, (uint8_t *)qetable, QETABLELEN);
    jpeg_disable(TEST_JPEG);
}

/*---------------------------------------------------------------------*
 * Wait for codec to complete
 *---------------------------------------------------------------------
 */
bool wait_jpeg_finish(void)
{
    do {
        if (jpeg_get_status(TEST_JPEG) & JPEG_EVENT_OUT_DMA_FINISH) {
            jpeg_clear_status(TEST_JPEG, JPEG_EVENT_OUT_DMA_FINISH);
            return true;
        }
        if (jpeg_get_status(TEST_JPEG) & JPEG_EVENT_ERROR) {
            jpeg_clear_status(TEST_JPEG, JPEG_EVENT_ERROR);
            return false;
        }
        board_delay_ms(100);
    } while (1);
}

/*---------------------------------------------------------------------*
 * JPEG decoding
 *---------------------------------------------------------------------
 */
void decode_jpeg(void)
{
    jpeg_job_config_t config = {0};

    /*JPEG default parameter table settings*/
    TEST_JPEG->CFG = 0;
    jpeg_init(TEST_JPEG);
    fill_jpeg_decode_table();

    /*jpeg parameter configuration*/
    config.jpeg_format = JPEG_SUPPORTED_FORMAT_420;
    config.in_pixel_format = JPEG_PIXEL_FORMAT_YUV422H1P;
    config.out_pixel_format = JPEG_PIXEL_FORMAT_RGB565;
    config.byte_order = JPEG_BYTE_ORDER_2301;
    config.enable_csc = true;
    config.enable_ycbcr = true;
    config.width_in_pixel = rgbwidth;
    config.height_in_pixel = rgbheight;
    config.in_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)in_ecs);
    config.out_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)out_img);

    /* Start decompressor */
    printf("start decoding\n");
    if (status_success != jpeg_start_decode(TEST_JPEG, &config, in_ecs_length)) {
        printf("failed to decode\n");
        while (1) {
        };
    }
    if (wait_jpeg_finish()) {
        printf("decoding done\n");
    } else {
        printf("decoding failed\n");
    }
}

/*---------------------------------------------------------------------*
 * JPEG encoding and decoding
 *---------------------------------------------------------------------
 */
void encode_decode_jpeg(void)
{
    uint32_t encoded_length;
    jpeg_job_config_t config = {0};
    printf("start encoding and decoding\n");

    /*JPEG encode default parameter table settings*/
    TEST_JPEG->CFG = 0;
    jpeg_init(TEST_JPEG);
    fill_jpeg_encode_table();

    /*jpeg encode parameter configuration*/
    config.jpeg_format = JPEG_SUPPORTED_FORMAT_420;
    config.in_pixel_format = JPEG_PIXEL_FORMAT_RGB565;
    config.out_pixel_format = JPEG_PIXEL_FORMAT_YUV422H1P;
    config.byte_order = JPEG_BYTE_ORDER_3210;
    config.enable_csc = true;
    config.enable_ycbcr = true;
    config.width_in_pixel = rgbwidth;
    config.height_in_pixel = rgbheight;
    config.in_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)out_img);
    config.out_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)out_ecs);

    /* Start compressor */
    if (status_success != jpeg_start_encode(TEST_JPEG, &config)) {
        printf("failed to endcode\n");
        while (1) {
        };
    }
    if (!wait_jpeg_finish()) {
        printf("encoding failed\n");
        while (1) {
        };
    }
    encoded_length = jpeg_get_encoded_length(TEST_JPEG);
    printf("complete encoding length %d bytes\n", encoded_length);

    /*JPEG decode default parameter table settings*/
    TEST_JPEG->CFG = 0;
    jpeg_init(TEST_JPEG);
    fill_jpeg_decode_table();

    /*jpeg decode parameter configuration*/
    config.jpeg_format = JPEG_SUPPORTED_FORMAT_420;
    config.in_pixel_format = JPEG_PIXEL_FORMAT_YUV422H1P;
    config.out_pixel_format = JPEG_PIXEL_FORMAT_RGB565;
    config.byte_order = JPEG_BYTE_ORDER_2301;
    config.enable_csc = true;
    config.enable_ycbcr = false;
    config.width_in_pixel = rgbwidth;
    config.height_in_pixel = rgbheight;
    config.in_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)out_ecs);
    config.out_buffer = core_local_mem_to_sys_address(BOARD_RUNNING_CORE, (uint32_t)out_img);

    /* Start decompressor */
    if (status_success != jpeg_start_decode(TEST_JPEG, &config, encoded_length)) {
        printf("failed to decode\n");
        while (1) {
        };
    }
    if (wait_jpeg_finish()) {
        printf("encoding and decoding done\n");
    } else {
        printf("encoding and decoding failed\n");
    }
}

/*---------------------------------------------------------------------*
 * MAIN.C
 *---------------------------------------------------------------------
 */
int main(void)
{
    /*System initialization*/
    board_init();

    do {
#if defined SD_FATFS_MODE
        /*Read picture data by SD card*/
        printf("Read picture data by SD card\n");
        sdfatfs_task();
#elif defined TINYUSB_HOST_MODE
        /*Reading picture data in tinyusb mode*/
        printf("Read picture data by usb-mode\n");
        tinyusb_task();
#endif
    } while (!jpg_size);

    /*extract tables from a JPEG image*/
    tabgen();
    /*LCD initialization*/
    board_init_lcd();
    /*Initialize JPEG module*/
    init_jpeg();

    /*Display picture data on LCD*/
    lcdc_display_picture(rgbwidth, rgbheight);

    /*jpeg conversion to convert JPG data into rgb565 data*/
    decode_jpeg();
    board_delay_ms(1000);
    /*jpeg conversion to convert rgb565 data into JPG data and  JPG data into rgb565 data*/
    encode_decode_jpeg();
    while (1) {
    };
}