/******************************************************************************
 * Copyright (c) 2010-2011 Texas Instruments Incorporated - http://www.ti.com
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
 * File	Name:       evm66x_nand.c
 *
 * Description: This contains   TMS320C6670 specific nand functions.
 *
 *
 ******************************************************************************/
#include "platform_internal.h"

#if (PLATFORM_NAND_IN)

/************************
 * Include Files
 ************************/

/************************
 * Globals
 ************************/

/************************
 * Defines and Macros
 ************************/
#define NAND_DEBUG  0

#pragma DATA_SECTION(nandPageBuf,"platform_lib");
static uint8_t nandPageBuf[NAND_BYTES_PER_PAGE + NAND_SPARE_BYTES_PER_PAGE];

static void NandWriteDataByte(uint8_t uchData);

 /************************
  * Globals
  ************************/

 /******************************************************************************
 *
 * Function:    ndelay
 *
 * Description: This function implements a delay loop in nano sec
 *
 * Parameters:  uiDelay - delay
 *
 * Return Value: void
 *
 *****************************************************************************/
void
ndelay
(
    uint32_t      uiDelay
)
{
    uint32_t t;

    for (t=0 ; t< uiDelay ; t++) 
    {
        asm ( "  nop  " ) ;
    }

}

 /******************************************************************************
  *
  * Function:    NandCmdSet
  *
  * Description: This function is used to indicates command cycle occurring,
  *              and to send command to NAND device
  *
 * Parameters:  uint32_t cmd - Command to NAND
  *
  * Return Value: None
  *
  *****************************************************************************/
static void NandCmdSet(uint32_t cmd)
{
    /* Commands are written to the command register on the rising edge of WE# when
       CE# and ALE are LOW, and CLE is HIGH, CLE HIGH indicates command cycle occurring
       and the device is not busy */
     gpioSetOutput(NAND_CLE_GPIO_PIN);
     NandWriteDataByte((uint8_t)cmd);
     gpioClearOutput(NAND_CLE_GPIO_PIN);
 }

 /******************************************************************************
  *
  * Function:    NandAleSet
  *
  * Description: This function is used to indicates Address cycle occurring,
  *              and to send address value to NAND device
  *
 * Parameters:  uint32_t addr - Address to NAND
 *
 * Return Value: None
 *
 *****************************************************************************/
static void NandAleSet(uint32_t addr)
{
    /* ALE HIGH indicates address cycle occurring */
    gpioSetOutput(NAND_ALE_GPIO_PIN);
    NandWriteDataByte(addr);
    gpioClearOutput(NAND_ALE_GPIO_PIN);
}

 /******************************************************************************
  *
  * Function:    NandWaitRdy
  *
  * Description: This function waits for the NAND status to be ready
  *
  * Parameters:  uint32_t in_timeout - time out value in micro seconds
  *
  * Return Value: Failure if Ready Pin is not high for prescribed time
  *
  *****************************************************************************/
 static uint32_t NandWaitRdy(uint32_t in_timeout)
{
     uint32_t count = 0;

    while(!gpioReadInput(NAND_BSY_GPIO_PIN))
    {
        platform_delay(1);
        count ++;        
        if (count > in_timeout)
            return FAIL;
    }
    return SUCCESS;
}

 /******************************************************************************
  *
  * Function:    NandReadDataByte
  *
  * Description: This function is used to read Nand data byte
  *
  * Parameters:  uint8_t* puchValue - Pointer to data buffer
  *
  * Return Value: None
  *
  *****************************************************************************/
static void NandReadDataByte(uint8_t* puchValue)
{
    /* Set Data Bus direction as IN */
    gpioSetDataBusDirection(GPIO_IN);

    gpioClearOutput(NAND_NRE_GPIO_PIN);
    ndelay(NAND_STD_DELAY);

    *puchValue = gpioReadDataBus();
    gpioSetOutput(NAND_NRE_GPIO_PIN);
    ndelay(NAND_STD_DELAY);

    /* Set Data Bus direction as OUT */
    gpioSetDataBusDirection(GPIO_OUT);
    ndelay(NAND_STD_DELAY);
}

 /******************************************************************************
  *
  * Function:    NandReadDataBytes
  *
  * Description: This function is used to read data bytes from the NAND device
  *
  * Parameters:  uiNumBytes - Number of bytes to be read
  *              pBuffer - Data buffer
  *
  * Return Value: Error/Success codes
  *
  *****************************************************************************/
 static uint32_t NandReadDataBytes(uint32_t uiNumBytes, uint8_t *pBuffer)
 {
     uint32_t i;

     /* 8-bit NAND */
     for (i = 0; i < uiNumBytes; i++)
     {
         /* NANDRead done directly without checking for nand width */
         NandReadDataByte((uint8_t *)pBuffer);
         pBuffer++;
     }
     return SUCCESS;
 }

/******************************************************************************
*
* Function:    NandReadSpareArea
*
* Description: Function to read Nand spare area
*
* Parameters:  uiBlkAddr - Block Address
*              uiPage - Page Number
*              pBuffer - Data Buffer
*
* Return Value: Error/Success codes
*
*****************************************************************************/
 uint32_t NandReadSpareArea(PLATFORM_DEVICE_info *p_device, uint32_t uiBlkAddr, uint32_t uiPage, uint8_t *pBuffer)
{
     uint32_t uiAddr, ret_val = SUCCESS;
     uint32_t uiColumn;


    /* Read the data to the destination buffer and detect error */
     uiColumn = p_device->column;

    ndelay(NAND_STD_DELAY*10);

    gpioClearOutput(NAND_NCE_GPIO_PIN);
    ndelay(NAND_STD_DELAY*5);

    /* Send 0x50h command to read the spare area */
    NandCmdSet(NAND_READ_SPARE_AREA); /* First cycle send 0 */

    /* Send address of the block + page to be read */
    /* Address cycles = 4, Block shift = 22, Page Shift = 16, Bigblock = 0 */
    uiAddr = PACK_ADDR(uiColumn, uiPage, uiBlkAddr);

    NandAleSet((uiAddr >>  0u) & 0xFF);   /* A0-A7  1st Cycle;  column addr */
    ndelay(NAND_STD_DELAY);
    NandAleSet((uiAddr >>  9u) & 0xFF);   /* A9-A16 2nd Cycle;  page addr & blk */
    ndelay(NAND_STD_DELAY);
    NandAleSet((uiAddr >> 17u) & 0xFF);   /* A17-A24 3rd Cycle; Block addr */
    ndelay(NAND_STD_DELAY);
    NandAleSet((uiAddr >> 25u) & 0x1);    /* A25    4th Cycle;  Plane addr */
    ndelay(NAND_STD_DELAY);

	/* Wait for Ready Busy Pin to go HIGH */
    ret_val = NandWaitRdy(NAND_PROG_TIMEOUT);

    if (ret_val != SUCCESS)
    {
    	platform_errno = PLATFORM_ERRNO_DEV_TIMEOUT;
        return FAIL;
    }

    /* Read the data to the destination buffer and detect error */
     NandReadDataBytes(p_device->spare_size, pBuffer);

    /* Set Chip select disable */
    gpioSetOutput(NAND_NCE_GPIO_PIN);
    ndelay(NAND_STD_DELAY*5);

    return SUCCESS;
}

 /******************************************************************************
  *
  * Function:    NandReadPage
  *
  * Description: Function to read a page from NAND
  *
  * Parameters:  uiBlkAddr - Block Address
  *              uiPage - Page Number
  *              pBuffer - Data Buffer
  *
  * Return Value: Error/Success codes
  *
  *****************************************************************************/
 #if (PLATFORM_NAND_READ_IN)
  uint32_t NandReadPage(PLATFORM_DEVICE_info *p_device, uint32_t uiBlkAddr, uint32_t uiPage, uint8_t *pBuffer)
{
      uint32_t uiAddr, ret_val = SUCCESS;
#if NAND_DEBUG
      uint8_t  status;
#endif


    ndelay(NAND_STD_DELAY*10);

    gpioClearOutput(NAND_NCE_GPIO_PIN);
    ndelay(NAND_STD_DELAY*5);

/***********************READ A Command***************************************/
    NandCmdSet(NAND_READ_LO_PAGE); /* First cycle send 0 */

    /* Send address of the block + page to be read
       Address cycles = 4, Block shift = 14, Page Shift = 9, Bigblock = 0 */
    uiAddr = PACK_ADDR(0x0, uiPage, uiBlkAddr);

    NandAleSet((uiAddr >>  0u) & 0xFF);   /* A0-A7  1st Cycle;  column addr */
    ndelay(NAND_STD_DELAY);
    NandAleSet((uiAddr >>  9u) & 0xFF);   /* A9-A16 2nd Cycle;  page addr & blk */
    ndelay(NAND_STD_DELAY);
    NandAleSet((uiAddr >> 17u) & 0xFF);   /* A17-A24 3rd Cycle; Block addr */
    ndelay(NAND_STD_DELAY);
    NandAleSet((uiAddr >> 25u) & 0x1);    /* A25    4th Cycle;  Plane addr */
    ndelay(NAND_STD_DELAY);


    /* Wait for Ready Busy Pin to go HIGH */
    ret_val = NandWaitRdy(NAND_PROG_TIMEOUT);

    if (ret_val != SUCCESS)
    {
       return FAIL;
    }

     /* Read the data to the destination buffer and detect error */
     NandReadDataBytes(p_device->page_size+p_device->spare_size, pBuffer);

    /* Set Chip select disable */
    gpioSetOutput(NAND_NCE_GPIO_PIN);
    ndelay(NAND_STD_DELAY*5);

#if NAND_DEBUG
    ndelay(NAND_STD_DELAY*10);
    gpioClearOutput(NAND_NCE_GPIO_PIN);     /* Chip EN = 0 */
    ndelay(NAND_STD_DELAY*5);
    NandCmdSet(NAND_STATUS);
    ndelay(NAND_STD_DELAY*2);

    NandReadDataByte(&status);
    ndelay(NAND_STD_DELAY*3);
    gpioSetOutput(NAND_NCE_GPIO_PIN);
#endif

/***********************READ A Command END************************************/

    return SUCCESS;
}
 #endif

 /******************************************************************************
  *
  * Function:    readNandPage
  *
  * Description: This function reads a page from NAND flash and detects and
  *              corrects the bit errors if ECC is enabled
  *
  * Parameters:  NAND_ADDR address - Block Address/Page address of NAND flash
  *              uint8_t* puchBuffer - Pointer to buffer
  *
  * Return Value: status
  *
  * Assumptions: puchBuffer points to a 2KB buffer
  *
  ******************************************************************************/
#if (PLATFORM_NAND_READ_IN)
 uint32_t readNandPage(PLATFORM_DEVICE_info *p_device, NAND_ADDR address, uint8_t* puchBuffer)
 {
     int32_t iErrors = SUCCESS;
     int32_t iCount = 0;
     uint8_t *puchSpareAreaBuf = NULL;
     uint8_t uchaEcc[3];
     uint8_t eccBytes[3*(NAND_BYTES_PER_PAGE/ECC_BLOCK_SIZE)];

     if(puchBuffer == NULL)
         return NULL_POINTER_ERROR;

     puchSpareAreaBuf = nandPageBuf + NAND_BYTES_PER_PAGE;

     /* Read NAND flash page */
     iErrors = NandReadPage(p_device, address.uiBlockAddr, address.uiPageAddr, nandPageBuf);

     if (iErrors != SUCCESS) {
    	 platform_errno = PLATFORM_ERRNO_DEV_FAIL;
    	 return iErrors;
     }

     /* Copy the ECC bytes from spare area to temp buf */
     memcpy(eccBytes, puchSpareAreaBuf, 4);
     eccBytes[4] = puchSpareAreaBuf[6];
     eccBytes[5] = puchSpareAreaBuf[7];

     /* Verify ECC */
     for(iCount = 0; iCount < NAND_BYTES_PER_PAGE/ECC_BLOCK_SIZE; iCount++)
     {
         /* Correct ecc error for each 256 byte blocks */
         eccComputeECC(nandPageBuf + iCount*ECC_BLOCK_SIZE, uchaEcc);
         iErrors = eccCorrectData(nandPageBuf + (iCount * ECC_BLOCK_SIZE),
                   &eccBytes[iCount*3], uchaEcc);

         if(iErrors != SUCCESS) {
 			platform_errno = PLATFORM_ERRNO_ECC_FAIL;
             return iErrors;
			}
     }

     memcpy(puchBuffer, nandPageBuf, p_device->page_size);

     return SUCCESS;
 }
#endif

 /******************************************************************************
  *
  * Function:    NandWriteDataByte
  *
  * Description: This function is used to write Nand data byte
  *
  * Parameters:  uint8_t uchData - Data to be written
  *
  * Return Value: None
  *
  *****************************************************************************/
static void NandWriteDataByte(uint8_t uchData)
{
    /* Data is written to the data register on the rising edge of WE# when
       CE#, CLE, and ALE are LOW, and the device is not busy.*/
    gpioClearOutput(NAND_NWE_GPIO_PIN);
    ndelay(NAND_STD_DELAY);
    gpioClearDataBus(GPIO_DATAMASK);
    gpioWriteDataBus(uchData);
    ndelay(NAND_STD_DELAY);
    gpioSetOutput(NAND_NWE_GPIO_PIN);   /* At posedge clock, make WE# = 1. */
    ndelay(NAND_STD_DELAY);
}

 /******************************************************************************
  *
  * Function:    NandWriteDataBytes
  *
  * Description: This function is used to write data bytes to the NAND device
  *
  * Parameters:  PLATFORM_DEVICE_info *p_device - Nand device
  * 			  uiNumBytes - Number of bytes to be written
  *              pBuffer - Data buffer
  *
  * Return Value: Error/Success codes
  *
  *****************************************************************************/
#if (PLATFORM_NAND_WRITE_IN)
uint32_t NandWriteDataBytes(PLATFORM_DEVICE_info *p_device, uint32_t uiNumBytes, uint8_t *pBuffer)
{
     uint32_t i;


     if (p_device->width == 8)
    {
        gpioClearOutput(NAND_ALE_GPIO_PIN);
        gpioClearOutput(NAND_CLE_GPIO_PIN);

        /* 8-bit NAND */
        for (i = 0; i < uiNumBytes; i++)
        {
            NandWriteDataByte( (uint8_t)*pBuffer );
            pBuffer++;
        }
    }
    return SUCCESS;
}
#endif

 /******************************************************************************
  *
  * Function:    NandWritePage
  *
  * Description: Function to write a page from NAND
  *
  * Parameters:  uiBlkAddr - Block Address
  *              uiPage - Page Number
  *              pBuffer - Data Buffer
  *
  * Return Value: Error/Success codes
  *
  *****************************************************************************/
#if (PLATFORM_NAND_WRITE_IN)
 uint32_t NandWritePage(PLATFORM_DEVICE_info *p_device, uint32_t uiBlkAddr, uint32_t uiPage, uint8_t *pBuffer)
{
     uint32_t addr;
     uint32_t ret_val = SUCCESS;
#if NAND_DEBUG
     uint8_t  status;
#endif
     uint8_t *pBuffer_loc;


    /* Set Data Bus direction as OUT */
    gpioSetDataBusDirection(GPIO_OUT);

    ndelay(NAND_STD_DELAY*10);
    gpioClearOutput(NAND_NCE_GPIO_PIN);
    ndelay(NAND_STD_DELAY*7);

    /* 1st half page*/
    NandCmdSet(NAND_READ_LO_PAGE);
    ndelay(NAND_STD_DELAY);

    NandCmdSet(NAND_PROG_PAGE);
    ndelay(NAND_STD_DELAY);

    /* Send address of the block + page to be read */
    /* Address cycles = 4, Block shift = 22, Page Shift = 16, Bigblock = 0 */
    addr = PACK_ADDR(0x0, uiPage, uiBlkAddr);

    NandAleSet((addr >>  0u) & 0xFF);   /* A0-A7  1st Cycle;  column addr */
    ndelay(NAND_STD_DELAY);
    NandAleSet((addr >>  9u) & 0xFF);   /* A9-A16 2nd Cycle;  page addr & blk */
    ndelay(NAND_STD_DELAY);
    NandAleSet((addr >> 17u) & 0xFF);   /* A17-A24 3rd Cycle; Block addr */
    ndelay(NAND_STD_DELAY);
    NandAleSet((addr >> 25u) & 0x1);    /* A25    4th Cycle;  Plane addr */
    ndelay(NAND_STD_DELAY);

    /* Write the data */
     pBuffer_loc = pBuffer;
     NandWriteDataBytes(p_device, p_device->page_size, (uint8_t *)pBuffer_loc);
     pBuffer_loc = pBuffer + p_device->page_size;
     NandWriteDataBytes(p_device, p_device->spare_size, (uint8_t *)pBuffer_loc);

    /* Wait for Ready Busy Pin to go HIGH */
    ndelay(NAND_STD_DELAY*10);

    NandCmdSet(NAND_CONFIRM_PAGE_PROGRAM);

    ret_val = NandWaitRdy(NAND_PROG_TIMEOUT);
    gpioSetOutput(NAND_NCE_GPIO_PIN);

    if (ret_val != SUCCESS)
    {
    	platform_errno = PLATFORM_ERRNO_DEV_TIMEOUT;
        return FAIL;
    }

#if NAND_DEBUG
    ndelay(NAND_STD_DELAY*10);
    gpioClearOutput(NAND_NCE_GPIO_PIN);     /* Chip EN = 0 */
    ndelay(NAND_STD_DELAY*5);
    NandCmdSet(NAND_STATUS);
    ndelay(NAND_STD_DELAY*2);

    NandReadDataByte(&status);
    ndelay(NAND_STD_DELAY*3);
    gpioSetOutput(NAND_NCE_GPIO_PIN);
#endif

    return SUCCESS;
}
#endif

 /******************************************************************************
  *
  * Function:    writeNandPage
  *
  * Description: This function a page to NAND flash. It computes ECC and
  *              and writes it to spare area if ECC is enabled
  *
  * Parameters:  NAND_ADDR address - Block Address/Page address of NAND flash
  *              uint8_t* puchBuffer - Pointer to buffer
  *
  * Return Value: status
  *
  * Assumptions: puchBuffer points to a 2KB buffer
  *
  ******************************************************************************/
#if (PLATFORM_NAND_WRITE_IN)
 uint32_t  writeNandPage(PLATFORM_DEVICE_info *p_device, NAND_ADDR address, uint8_t* puchBuffer)
 {
     int32_t iErrors = SUCCESS;
     int32_t iCount = 0;
     uint8_t *puchSpareAreaBuf = NULL;
     uint8_t  tempSpareAreaBuf[3*(NAND_BYTES_PER_PAGE/ECC_BLOCK_SIZE)];
     /* ECC locations for the Numonyx nand device */
     uint8_t   eccLoc[3*(NAND_BYTES_PER_PAGE/ECC_BLOCK_SIZE)] = {0, 1, 2, 3, 6, 7};


     memcpy (nandPageBuf, puchBuffer, NAND_BYTES_PER_PAGE);

     puchSpareAreaBuf = nandPageBuf + NAND_BYTES_PER_PAGE;

     /* Write single page data to NAND */

     /* Init the buffer by reading the existing values in the spare area */
    iErrors = NandReadSpareArea(p_device, address.uiBlockAddr, address.uiPageAddr,
                      puchSpareAreaBuf);

    if(iErrors != SUCCESS) {
        return iErrors;
    }

    /* Compute the ECC for the partial pages */
     for(iCount = 0; iCount < NAND_BYTES_PER_PAGE/ECC_BLOCK_SIZE; iCount++)
     {
         /* Compute ecc for each 256 byte block */
         eccComputeECC(nandPageBuf + iCount*ECC_BLOCK_SIZE,
                                 tempSpareAreaBuf + iCount*3);
     }

     /* Store the ECC values in the designated locations */
     for(iCount = 0; iCount < 3*(NAND_BYTES_PER_PAGE/ECC_BLOCK_SIZE); iCount++) {
        puchSpareAreaBuf[eccLoc[iCount]] = tempSpareAreaBuf[iCount];
     }

     iErrors = NandWritePage( p_device, address.uiBlockAddr,
                                address.uiPageAddr, nandPageBuf);
     if(iErrors != SUCCESS) {
         return iErrors;
     }

      return SUCCESS;
 }
#endif

 /******************************************************************************
  *
 * Function:    NandWriteSpareArea
 *
 * Description: Function to write a spare area of the NAND
 *
 * Parameters:  uiBlkAddr - Block Address
 *              uiPage - Page Number
 *              pBuffer - Data Buffer
 *
 * Return Value: Error/Success codes
 *
 *****************************************************************************/
#if (PLATFORM_NAND_WRITE_IN)
uint32_t NandWriteSpareArea (PLATFORM_DEVICE_info *p_device, uint32_t uiBlkAddr, uint32_t uiPage, uint8_t *pBuffer)
{
    uint32_t addr;
    uint32_t ret_val = SUCCESS;
#if NAND_DEBUG
    uint8_t  status;
#endif
    uint8_t *pBuffer_loc;
    uint32_t uiColumn;

    /* Read the data to the destination buffer and detect error */
    uiColumn = p_device->column;

    /* Set Data Bus direction as OUT */
    gpioSetDataBusDirection(GPIO_OUT);

    ndelay(NAND_STD_DELAY*10);
    gpioClearOutput(NAND_NCE_GPIO_PIN);
    ndelay(NAND_STD_DELAY*7);

    /* Spare Area*/
    NandCmdSet(NAND_READ_SPARE_AREA);
    ndelay(NAND_STD_DELAY);

    NandCmdSet(NAND_PROG_PAGE);
    ndelay(NAND_STD_DELAY);

    /*
     * Send address of the block + page to be read
     * Address cycles = 4, Block shift = 22, Page Shift = 16, Bigblock = 0
     */
    addr = PACK_ADDR(uiColumn, uiPage, uiBlkAddr);

    NandAleSet((addr >>  0u) & 0xFF);   // A0-A7  1st Cycle;  column addr
    ndelay(NAND_STD_DELAY);
    NandAleSet((addr >>  9u) & 0xFF);   // A9-A16 2nd Cycle;  page   addr & blk
    ndelay(NAND_STD_DELAY);
    NandAleSet((addr >> 17u) & 0xFF);   // A17-A24 3rd Cycle; Block addr
    ndelay(NAND_STD_DELAY);
    NandAleSet((addr >> 25u) & 0x1);    // A25-A26  4th Cycle;  Plane addr
    ndelay(NAND_STD_DELAY);

    /* Write the data */
    pBuffer_loc = pBuffer;
    NandWriteDataBytes(p_device, p_device->spare_size, (uint8_t *)pBuffer_loc);

    /* Wait for Ready Busy Pin to go HIGH */
    ndelay(NAND_STD_DELAY*10);

    NandCmdSet(NAND_CONFIRM_PAGE_PROGRAM);

    ret_val = NandWaitRdy(NAND_PROG_TIMEOUT);
    gpioSetOutput(NAND_NCE_GPIO_PIN);

    if (ret_val != SUCCESS)
    {
		platform_errno = PLATFORM_ERRNO_DEV_TIMEOUT;
        return FAIL;
    }

#if NAND_DEBUG
    ndelay(NAND_STD_DELAY*10);
    gpioClearOutput(NAND_NCE_GPIO_PIN);     /* Chip EN = 0 */
    ndelay(NAND_STD_DELAY*5);
    NandCmdSet(NAND_STATUS);
    ndelay(NAND_STD_DELAY*2);

    NandReadDataByte(&status);
    ndelay(NAND_STD_DELAY*3);
    gpioSetOutput(NAND_NCE_GPIO_PIN);
#endif

    return SUCCESS;


}
#endif

 /******************************************************************************
  *
  * Function:    nandFlashBlockErase
  *
  * Description: This function erases the specified block of NAND flash
  *
  * Parameters:  NAND_ADDR address - Block Address of NAND flash
  *
  * Return Value: status
  *
  *****************************************************************************/
#if (PLATFORM_NAND_WRITE_IN)
 uint32_t nandFlashBlockErase(PLATFORM_DEVICE_info *p_device, uint32_t uiBlockNumber)
 {
     uint32_t addr = 0, ret_val = SUCCESS;
#if NAND_DEBUG
     uint8_t  status;
#endif

    ndelay(NAND_STD_DELAY*10);
    gpioClearOutput(NAND_NCE_GPIO_PIN);
    ndelay(NAND_STD_DELAY*7);
    NandCmdSet(NAND_BLOCK_ERASE); /* Block erase command */
    ndelay(NAND_STD_DELAY);

    /* Send address of the block + page to be read
       Address cycles = 2, Block shift = 22, Page shiht = 16 */
    addr = PACK_ADDR(0x0, 0x0, uiBlockNumber);

    /* Properly adjust the shifts to match to the data sheet */
    NandAleSet((addr >>  9u) & 0xFF);   // A9-A16 2nd Cycle;  page   addr & blk
    ndelay(NAND_STD_DELAY);
    NandAleSet((addr >> 17u) & 0xFF);   // A17-A24 3rd Cycle; Block addr
    ndelay(NAND_STD_DELAY);
    NandAleSet((addr >> 25u) & 0x3);    // A25-A26  4th Cycle;  Plane addr
    ndelay(NAND_STD_DELAY*7);

    NandCmdSet(NAND_ERASE_CONFIRM); /* Erase confirm */

    ret_val = NandWaitRdy(NAND_BLOCK_ERASE_TIMEOUT);

    gpioSetOutput(NAND_NCE_GPIO_PIN);

    if (ret_val != SUCCESS)
    {
        return FAIL;
    }

#if NAND_DEBUG
    ndelay(NAND_STD_DELAY*10);
    gpioClearOutput(NAND_NCE_GPIO_PIN);     /* Chip EN = 0 */
    ndelay(NAND_STD_DELAY*5);
    NandCmdSet(NAND_STATUS);
    ndelay(NAND_STD_DELAY*2);

    NandReadDataByte(&status);
    ndelay(NAND_STD_DELAY*3);
    gpioSetOutput(NAND_NCE_GPIO_PIN);
#endif

    return SUCCESS;

 }
#endif

 /******************************************************************************
  *
  * Function:    NandConfig
  *
  * Description: This function is used to congigure the NAND Device
  *
  * Parameters:  None
  *
  * Return Value: Err Status
  *
  *****************************************************************************/
 static uint32_t NandConfig (void)
{
    /* Set direction of control signals as OUT */
    gpioSetDirection(NAND_CLE_GPIO_PIN, GPIO_OUT);
    gpioSetDirection(NAND_NCE_GPIO_PIN, GPIO_OUT);
    gpioSetDirection(NAND_NWE_GPIO_PIN, GPIO_OUT);
    gpioSetDirection(NAND_ALE_GPIO_PIN, GPIO_OUT);
    gpioSetDirection(NAND_NRE_GPIO_PIN, GPIO_OUT);

    /* Set Data Bus direction as OUT */
    gpioSetDataBusDirection(GPIO_OUT);

    gpioSetOutput(NAND_NCE_GPIO_PIN);   /* Chip Enable = 1 */
    gpioClearOutput(NAND_CLE_GPIO_PIN); /* Command Latch enable = 0 */
    gpioClearOutput(NAND_ALE_GPIO_PIN); /* Address latch Enable = 0 */
    gpioSetOutput(NAND_NWE_GPIO_PIN);   /* Write Enable = 1 */
    gpioSetOutput(NAND_NRE_GPIO_PIN);   /* Read Enable = 1 */

    return SUCCESS;
}

 /******************************************************************************
  *
  * Function:    NandOpenDevice
  *
  * Description: This function is used to open the NAND device and configure it
  *
  * Parameters:  None
  *
  * Return Value: Error/Success codes
  *
  *****************************************************************************/
 static uint32_t NandOpenDevice(void)
{
     uint32_t ret_val;
#if NAND_DEBUG
     uint8_t  status;
#endif

    /* Initialize NAND interface */
    ret_val = NandConfig();
    ndelay(NAND_STD_DELAY*10);

    if (ret_val != SUCCESS)
    {
       return FAIL;
    }

    /* Send reset command to NAND */
    gpioClearOutput(NAND_NCE_GPIO_PIN);     /* Chip EN = 0 */
    NandCmdSet(NAND_RST);
    gpioSetOutput(NAND_NCE_GPIO_PIN);

    ret_val = NandWaitRdy(NAND_RESET_TIMEOUT);

    if (ret_val != SUCCESS)
    {
     	platform_errno = PLATFORM_ERRNO_DEV_TIMEOUT;
    	IFPRINT(platform_write("NandOpenDevice ... Nand wait ready failed. \n"));
      	return FAIL;
    }

#if NAND_DEBUG
    ndelay(NAND_STD_DELAY*10);
    gpioClearOutput(NAND_NCE_GPIO_PIN);     /* Chip EN = 0 */
    ndelay(NAND_STD_DELAY*5);
    NandCmdSet(NAND_STATUS);
    ndelay(NAND_STD_DELAY*2);

    NandReadDataByte(&status);
    ndelay(NAND_STD_DELAY*3);
    gpioSetOutput(NAND_NCE_GPIO_PIN);
#endif

    return SUCCESS;
}


 /******************************************************************************
  *
  * Function:    NandGetDetails
  *
  * Description: Get details of the NAND flash used from the id and the
  *              table of NAND
  *
  * Parameters:  pNandInfo - Pointer to Nand Info structure
  *
  * Return Value: Error/Success codes
  *
  *****************************************************************************/
 uint32_t NandGetDetails(PLATFORM_DEVICE_info *pNandInfo)
{
	 uint32_t	i;
	 uint32_t 	uiStatus;
     NAND_ADDR  address;

     /* Clear the Information */
     pNandInfo->device_id = pNandInfo->manufacturer_id = 0x0;

    /* Set Data Bus direction as OUT */
    gpioSetDataBusDirection(GPIO_OUT);

    /* Read device ID */
    gpioClearOutput(NAND_NCE_GPIO_PIN);
    ndelay(NAND_STD_DELAY*5);
    NandCmdSet(NAND_READ_ELEC_SIG);
    ndelay(NAND_STD_DELAY*2);
    NandAleSet(NAND_ADD_00H);

    ndelay(NAND_STD_DELAY*3);

    /* Always reading the ID alone in 8 bit mode */
     NandReadDataByte((uint8_t *)&pNandInfo->manufacturer_id);
     NandReadDataByte((uint8_t *)&pNandInfo->device_id);

    ndelay(NAND_STD_DELAY*3);
    gpioSetOutput(NAND_NCE_GPIO_PIN);

     /* Get the bad block table */
     address.uiPageAddr 	= 0;
     address.uiColumnAddr 	= 0;

     for (i=0; i < NAND_BLOCKS_PER_DEVICE; i++) {
         address.uiBlockAddr     = i;

         /* Clear the Spare Area */
         memset(nandPageBuf, 0, NAND_SPARE_BYTES_PER_PAGE);

         // Read the spare area in to buffer
         uiStatus = NandReadSpareArea(pNandInfo, address.uiBlockAddr,
                                         address.uiPageAddr, nandPageBuf);
         if(uiStatus != SUCCESS) {
            return PLATFORM_ERRNO_NANDBBT;
         }
         else { // Success in reading the NAND spare area
             if (nandPageBuf[pNandInfo->bboffset] == 0xFF) {
            	 pNandInfo->bblist[i]  = 0xFF;
             } else {
            	 pNandInfo->bblist[i]  = 0x00;
             }
         }
     }
     return SUCCESS;
 }

 /******************************************************************************
  * 
  * Function:    nandInit
  *
  * Description: This function configures the GPIO pins for communication
  *              between DSP and FPGA NAND flash controller.
  *
  * Parameters:  None
  *
  * Return Value: status
  * 
  ******************************************************************************/
 uint32_t nandInit(void)
 {
   /* Initialize GPIO module */
   gpioInit();

   /* Open the NAND Device */
   return NandOpenDevice();
 }
 
#endif /*(PLATFORM_NAND_IN)*/
 

