/* ========================================================================== */
/**
 *  @file   platform_internal.h
 *
 *  @brief  Private definitions for the Platform Library.
 *
 *  ============================================================================
 */
/* --COPYRIGHT--,BSD
 * Copyright (c) 2010-2011, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

#ifndef PLATFORM_INTERNAL_H_
#define PLATFORM_INTERNAL_H_

/********************************************************************************************
 * 					BUILD OPTIONS FOR THE LIBRARY											*
 *******************************************************************************************/

/***
 * @brief  The following flags are used for controlling the build of Platform Library
 */

/*  brief Platform Library version number */
#define PLATFORM_LIB_VERSION "2.00.00.07"

/* Turn on and off debug statements (may not be safe in certain contexts) */
#define	PLATFORM_DEBUG	0

/* These flags compile in and out functionality offered by the library allowing you to control
 * the size of the code that is included.
 */
#ifdef _PLATFORM_LITE_
	/*
	 * Build a version of Platform Library suitable for use in an EEPROM or other small application.
	 * (likely needs tailoring to the specific application if size matters)
	 */
	#define	PLATFORM_VERSTRING_IN		1
	#define	PLATFORM_INIT_IN			1
	#define	PLATFORM_GETINFO_IN			1
	#define PLATFORM_GETCOREID_IN		1
	#define PLATFORM_GETSWITCHSTATE_IN	0
	#define PLATFORM_GETMACADDR_IN		1
	#define PLATFORM_GETPHYADDR_IN		0
	#define PLATFORM_PHYLINKSTATUS_IN	0
	#define PLATFORM_DELAY_IN			1
	#define PLATFORM_LED_IN				1
	#define PLATFORM_WRITE_IN			0
	#define	PLATFORM_EXTMEMTEST_IN		1
	#define PLATFORM_INTMEMTEST_IN		0
	#define	PLATFORM_EEPROM_IN			1
	#define	PLATFORM_EEPROM_WRITE_IN	1
	#define	PLATFORM_UART_IN			1
	#define	PLATFORM_NOR_IN				1
	#define	PLATFORM_NOR_READ_IN		1
	#define	PLATFORM_NOR_WRITE_IN		0
	#define PLATFORM_NAND_IN			1
	#define PLATFORM_NAND_READ_IN		1
	#define PLATFORM_NAND_WRITE_IN		0
	#define PLATFORM_SEMLOCK_IN			0
	#define PLATFORM_CACHE_IN			0
#else
	/*
	 * Build the FULL version of Platform Library
	 */
	#define	PLATFORM_VERSTRING_IN		1
	#define	PLATFORM_INIT_IN			1
	#define	PLATFORM_GETINFO_IN			1
	#define PLATFORM_GETCOREID_IN		1
	#define PLATFORM_GETSWITCHSTATE_IN	1
	#define PLATFORM_GETMACADDR_IN		1
	#define PLATFORM_GETPHYADDR_IN		1
	#define PLATFORM_PHYLINKSTATUS_IN	1
	#define PLATFORM_DELAY_IN			1
	#define PLATFORM_LED_IN				1
	#define PLATFORM_WRITE_IN			1
	#define	PLATFORM_EXTMEMTEST_IN		1
	#define PLATFORM_INTMEMTEST_IN		1
	#define	PLATFORM_EEPROM_WRITE_IN	1
	#define	PLATFORM_EEPROM_IN			1
	#define	PLATFORM_UART_IN			1
	#define	PLATFORM_NOR_IN				1
	#define	PLATFORM_NOR_READ_IN		1
	#define	PLATFORM_NOR_WRITE_IN		1
	#define PLATFORM_NAND_IN			1
	#define PLATFORM_NAND_READ_IN		1
	#define PLATFORM_NAND_WRITE_IN		1
	#define PLATFORM_SEMLOCK_IN			0
	#define PLATFORM_CACHE_IN			1
#endif

#if (PLATFORM_DEBUG && !PLATFORM_WRITE_IN)
#error	You must enable PLATFORM_WRITE to turn on DEBUG
#endif

#if (PLATFORM_DEBUG)
#define IFPRINT(x)   (x)
#else
#define IFPRINT(x)
#endif

/********************************************************************************************
 * 					Includes for the Library Routines										*
 *******************************************************************************************/

#include "types.h"
#include "csl_types.h"

#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "csl_chip.h"
#include "csl_chipAux.h"
#include "csl_semAux.h"
#include "cslr_device.h"
#include "cslr_psc.h"
#include "csl_psc.h"
#include "csl_emif4f.h"
#include "csl_emif4fAux.h"

#include "evmc66x_fpga.h"
#include "evmc66x_i2c.h"
#include "cslr_i2c.h"

#include "csl_cpsw.h"
#include "csl_cpsgmii.h"
#include "csl_cpsgmiiAux.h"
#include "csl_mdio.h"
#include "csl_mdioAux.h"

#include "evmc66x_uart.h"
#include "cslr_uart.h"

#include "evmc66x_gpio.h"
#include "csl_gpioAux.h"

#include "evmc66x_i2c_eeprom.h"

#include "evmc66x_nand.h"
#include "csl_pscAux.h"
#include "csl_bootcfg.h"
#include "csl_bootcfgAux.h"

#include "evmc66x_nor.h"
#include "evmc66x_spi.h"
#include "cslr_spi.h"

#include "csl_pllcAux.h"
#include "csl_xmcAux.h"
#include "evmc66x_pllc.h"

/********************************************************************************************
 * 					Platform Specific Declarations											*
 *******************************************************************************************/

#define PLATFORM_INFO_CPU_NAME 		"TMS320C6670"
#define PLATFORM_INFO_BOARD_NAME 	"TMDXEVM6670L"

#define MEGM_REV_ID_REGISTER   (0x01812000)
#define MEGM_REV_ID_MAJ_MASK   (0xFFFF0000)
#define MEGM_REV_ID_MAJ_SHIFT  (16)
#define MEGM_REV_ID_MIN_MASK   (0x0000FFFF)
#define MEGM_REV_ID_MIN_SHIFT  (0)

/* Number of Ethernet Ports (not the number of silver ports) */
#define PLATFORM_NUM_ETH_PORTS	3

#define PLATFORM_BASE_CLK_RATE_MHZ (122.88)    /* External 122.88 MHz clock */

/* PREDIV */
#define PLATFORM_PLL_PREDIV_val (1)

/* POSTDIV */
#define PLATFORM_PLL_POSTDIV_val (2)

/* Default PLL PLLM value (122.88/1*(16/2)) = 983MHz) */
#define  PLATFORM_PLL1_PLLM_val (16)

/* Default PLL PLLD value for 1.0GHz) */
#define  PLATFORM_PLL1_PLLD_val (1)

/* Default UART baudrate value */
#define  PLATFORM_UART_BAUDRATE_val (19200)

/* Input crystal frequency 100 MHz */
#define PLATFORM_UART_INPUT_CLOCK_RATE ((PLATFORM_BASE_CLK_RATE_MHZ * PLATFORM_PLL1_PLLM_val * 1000000)/(PLATFORM_PLL_PREDIV_val * 12 * PLATFORM_PLL1_PLLD_val)) /* SYSCLK7 = CPU_CLK/7 in Hz */

/* 1/x-rate clock for CorePac (emulation) and the ADTF module */
#define  PLATFORM_PLLDIV2_val  (3)
/* 1/y-rate clock for system trace module only */
#define  PLATFORM_PLLDIV5_val (5)
/* 1/z-rate clock is used as slow_sysclck in the system */
#define  PLATFORM_PLLDIV8_val (64)


/* LED Definitions */
#define PLATFORM_FPGA_LED_COUNT  (4)
#define PLATFORM_I2C_LED_COUNT   (0)
#define PLATFORM_TOTAL_LED_COUNT (PLATFORM_FPGA_LED_COUNT + PLATFORM_I2C_LED_COUNT)

/* Number of cores on the platform */
#define PLATFORM_CORE_COUNT 4

/* Mmeory Sections */
#define PLATFORM_L1P_BASE_ADDRESS 0x10E00000
#define PLATFORM_L1P_SIZE         0x8000    /* 32K bytes */
#define PLATFORM_L1D_BASE_ADDRESS 0x10F00000
#define PLATFORM_L1D_SIZE         0x8000    /* 32K bytes */
#define PLATFORM_DDR3_SDRAM_START 0x80000000
#define PLATFORM_DDR3_SDRAM_END   0x9FFFFFFF /* 512 MB */

/* 24AA1025 EEPROM */
#define PLATFORM_EEPROM_MANUFACTURER_ID (0x01)
#define PLATFORM_EEPROM_DEVICE_ID_1     (0x50)
#define PLATFORM_EEPROM_DEVICE_ID_2     (0x51)

/********************************************************************************************
 * 					General Declarations													*
 *******************************************************************************************/

/* Size of a string we can output with platform_write */
#define	MAX_WRITE_LEN	80

/* Device Tables */
#if (PLATFORM_NAND_IN)
extern PLATFORM_DEVICE_info gDeviceNand;
#endif

#if (PLATFORM_NOR_IN)
extern  PLATFORM_DEVICE_info gDeviceNor;
#endif

#if (PLATFORM_EEPROM_IN)
extern PLATFORM_DEVICE_info gDeviceEeprom0;
extern PLATFORM_DEVICE_info gDeviceEeprom1;
#endif


/********************************************************************************************
 * 					Function Prototypes														*
 *******************************************************************************************/
#if (PLATFORM_SEMLOCK_IN)
#define PLIBSPILOCK() Osal_platformSpiCsEnter();
#define PLIBSPIRELEASE() Osal_platformSpiCsExit ();
#else
#define PLIBSPILOCK()
#define PLIBSPIRELEASE()
#endif

/* Function prototypes that don't live anywhere else */
extern void configSerdes();
extern void Init_SGMII(uint32_t macport);
extern void PowerUpDomains (void);
extern void xmc_setup();

#endif
