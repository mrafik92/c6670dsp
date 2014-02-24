/******************************************************************************
 * Copyright (c) 2011 Texas Instruments Incorporated - http://www.ti.com
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 *****************************************************************************/

/******************************************************************************	
 *
 * File	Name:	evmc66x_nand.h
 *
 * Description:	This is	the header file	for Nand flash API's.
 ******************************************************************************/
#ifndef	NAND_H_	
#define	NAND_H_	

/*********************************
 * Defines and Macros and globals
 *********************************/
/* GPIO pin mapping to the NAND */
#define NAND_CLE_GPIO_PIN               GPIO_8      /* NAND Command Latch Enable pin */
#define NAND_ALE_GPIO_PIN               GPIO_9      /* NAND Address Latch Enable pin */
#define NAND_NWE_GPIO_PIN               GPIO_10     /* NAND Write Enable pin */
#define NAND_BSY_GPIO_PIN               GPIO_11     /* NAND Ready/Busy pin */
#define NAND_NRE_GPIO_PIN               GPIO_12     /* NAND Read Enable pin */
#define NAND_NCE_GPIO_PIN               GPIO_13     /* NAND Chip Enable pin */

/* NAND Flash Command Definitions */
#define NAND_READ_LO_PAGE               (0)     /* Read the lo page CMD */
#define NAND_READ_HI_PAGE               (1)     /* Read the hi page CMD */
#define NAND_READ_SPARE_AREA            (0x50)  /* Read the Spare area CMD */
#define NAND_BLOCK_ERASE                (0x60)  /* Block Erase Command */
#define NAND_ERASE_CONFIRM              (0xD0)  /* Erase Confirm Command */
#define NAND_PROG_PAGE                  (0x80)  /* Program Page Command */
#define NAND_CONFIRM_PAGE_PROGRAM       (0x10)  /* Page Program Confirm Command */
#define NAND_READ_ELEC_SIG              (0x90)  /* Read Electronic Signature, e.g. manufacture and device code */
#define NAND_STATUS                     (0x70)  /* Read Status command */
#define NAND_RST                        (0xFF)  /* Reset Command */

#define NAND_ADD_00H                    (0x00)  /* Address 0x00 */
#define NAND_ADD_08H                    (0x08)  /* Address 0x08 */

#define NAND_MICRON_MAN_ID              (0x2C)  /* Micron manufacture code */
#define NAND_MICRON_DEVICE_ID           (0xA1)  /* MT29F1G08ABB device code */
#define NAND_MAN_ID                     (0x20)  /* Numonyx NAND512W3A2D manufacture code */
#define NAND_DEVICE_ID                  (0x36)  /* Numonyx NAND512W3A2D device code */

 /**
 @}
 */

#if 1
/* NAND address pack macro for NUMONYX PART*/
#define PACK_ADDR(col, page, block) \
        ((col & 0x000000ff) | ((page & 0x0000001f)<<9) | ((block & 0x00000fff) << 14 ))
#else
/* NAND address pack macro for MICRON PART*/
#define	PACK_ADDR(col, page, block) \
        ((col &	0x00000fff) | ((page & 0x0000003f)<<16)	| ((block & 0x000003ff)	<< 22 ))
#endif

/* NAND delay in Nano Sec */
#define NAND_STD_DELAY              (25)
#define NAND_PROG_TIMEOUT           (2000000)
#define NAND_RESET_TIMEOUT          (100000)
#define NAND_BLOCK_ERASE_TIMEOUT    (2000000)
#define NAND_WAIT_PIN_POLL_ST_DLY   (10)


/* NAND error code */
#define INVALID_NAND_ADDR           (8)
#define NAND_PAGE_WRITE_FAIL        (9)
#define NAND_SPARE_AREA_WRITE_FAIL  (10)
#define NAND_PAGE_READ_FAIL         (11)
#define NAND_SPARE_AREA_READ_FAIL   (12)
#define NAND_BLOCK_ERASE_FAIL       (13)
#define NAND_ID_READ_ERROR          (14)
#define NAND_TWO_BIT_ERROR          (15)
#define INVALID_NAND_DEVICE         (16)

/* NAND flash block/page info definitions */
#define NAND_BYTES_PER_PAGE         (512)
#define NAND_SPARE_BYTES_PER_PAGE   (16)
#define NAND_PAGES_PER_BLOCK        (32)
#define NAND_TOTAL_BYTES_PER_PAGE   (NAND_BYTES_PER_PAGE+NAND_SPARE_BYTES_PER_PAGE)
#define NAND_BLOCKS_PER_DEVICE      (4096)


/* NAND ECC related definitionss */
#define ECC_BLOCK_SIZE              (256)   /* in Bytes */
#define ECC_SPARE_OFFSET            (NAND_SPARE_BYTES_PER_PAGE-3*(NAND_BYTES_PER_PAGE/ECC_BLOCK_SIZE))

/* NAND information structure */
typedef struct _NAND_INFO
{
    uint32_t uiManufacuterID; /* NAND	manufacture ID */
    uint32_t uiDeviceID;      /* NAND	Device ID */
    uint32_t uiNandWidth;     /* NAND	width 1->16 bits 0->8 bits */
    uint32_t uiBlockCount;    /* Total NAND blocks */
    uint32_t uiPageCount;     /* Page	count per block */
    uint32_t uiPageSize;      /* Number of bytes in a	page including extra data */
    uint32_t uiBBlockOffset;  /* Bad Block byte offset in the spare area */
}NAND_INFO;

typedef struct _NAND_ADDR
{
    uint32_t uiColumnAddr;
    uint32_t uiPageAddr;
    uint32_t uiBlockAddr;
} NAND_ADDR;

/************************
 * Function declarations
 ************************/
uint32_t nandInit(void);
uint32_t readNandPage(PLATFORM_DEVICE_info* pNandInfo, NAND_ADDR address, uint8_t* puchBuffer);
uint32_t writeNandPage(PLATFORM_DEVICE_info* pNandInfo, NAND_ADDR address,	uint8_t* puchBuffer);
uint32_t enableNandECC(uint32_t uiUseSoftEcc);
uint32_t nandFlashBlockErase(PLATFORM_DEVICE_info* pNandInfo, uint32_t uiBlockNumber);
uint32_t NandGetDetails(PLATFORM_DEVICE_info* pNandInfo);
uint32_t NandReadSpareArea(PLATFORM_DEVICE_info* pNandInfo, uint32_t uiBlkAddr, uint32_t uiPage, uint8_t *pBuffer);
uint32_t NandWriteSpareArea (PLATFORM_DEVICE_info* pNandInfo, uint32_t uiBlkAddr, uint32_t uiPage, uint8_t *pBuffer);

int32_t  eccCorrectData(uint8_t *puchData, uint8_t *puchEccRead, uint8_t *puchEccCalc);
uint32_t eccComputeECC(const uint8_t *puchData, uint8_t *puchEccCode);

#endif /* NAND_H_ */





