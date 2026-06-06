/*
 * Copyright (c) 2006-2018, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-05-17     armink       the first version
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

//#include <rtconfig.h>
//#include <board.h>

#define FAL_DEBUG      1
#define FAL_PART_HAS_TABLE_CFG

//#define NOR_FLASH_DEV_NAME             "norflash0"

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev onchip_flash;
extern const struct fal_flash_dev nor_flash0;

/* flash device table */
#define FAL_FLASH_DEV_TABLE                                          \
{                                                                    \
    &onchip_flash,                                           \
}
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#if 1
#define FAL_PART_TABLE                                                               \
{                                                                                    \
    {FAL_PART_MAGIC_WORD, "bootloader"		,"onchip",                0,   64*1024, 0}, \
		{FAL_PART_MAGIC_WORD, "sys_conf"			,"onchip",          64*1024,   32*1024, 0}, \
    {FAL_PART_MAGIC_WORD, "fdb_kvdb1"			,"onchip",          96*1024,  224*1024, 0}, \
    {FAL_PART_MAGIC_WORD, "app"						,"onchip",         320*1024,  192*1024, 0}, \
}
#else

#define FAL_PART_TABLE                                                               \
{                                                                                    \
    {FAL_PART_MAGIC_WORD, "bootloader"		,"onchip",                0,   64*1024, 0}, \
		{FAL_PART_MAGIC_WORD, "sys_conf"			,"onchip",          64*1024,    8*1024, 0}, \
    {FAL_PART_MAGIC_WORD, "fdb_kvdb1"			,"onchip",          72*1024,  248*1024, 0}, \
    {FAL_PART_MAGIC_WORD, "app"						,"onchip",         320*1024,  192*1024, 0}, \
}

#endif

#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */
