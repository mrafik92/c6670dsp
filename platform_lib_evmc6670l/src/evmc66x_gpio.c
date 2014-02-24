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
 * File Name:  	evmc66x_gpio.c
 *
 * Description:	This file contains the lower level function to access GPIO 
 * 				
 * 
 *****************************************************************************/

/************************
 * Include Files
 ************************/
#include "platform_internal.h"

#define GPIO_BANK_INDEX     (0)

/* GPIO instance handle */
static CSL_GpioHandle   hGpio_handle;

/******************************************************************************
 * 
 * Function:        gpioInit  
 *
 * Description:     Initializes the GPIO peripheral
 *
 * Parameters:      void
 *
 * Return Value:    void
 * 
 *****************************************************************************/
void 
gpioInit
(
    void
)
{
     /* Open the CSL GPIO Module 0 */
    hGpio_handle = (CSL_GpioHandle) CSL_GPIO_REGS;    
    
    /* Disable the GPIO global interrupts */
    CSL_GPIO_bankInterruptDisable(hGpio_handle, GPIO_BANK_INDEX);
    
	// Clear all falling edge trigger
	hGpio_handle->BANK_REGISTERS[0].CLR_FAL_TRIG = 0x0000ffff;
	
	// Clear all rising edge trigger
	hGpio_handle->BANK_REGISTERS[0].CLR_RIS_TRIG = 0x0000ffff;
	
	// Set all GPIO as input
	hGpio_handle->BANK_REGISTERS[0].DIR = hGpio_handle->BANK_REGISTERS[0].DIR & 0xffffffff;
    
    /* Configure the GPIOs for NAND flash controller communication
       Configure data bus as output */
    gpioSetDataBusDirection(GPIO_OUT);

    /* Configure the signal pins direction */
    gpioSetDirection(NAND_CLE_GPIO_PIN, GPIO_OUT );
    gpioSetDirection(NAND_ALE_GPIO_PIN, GPIO_OUT );
    
    gpioSetDirection(NAND_NWE_GPIO_PIN, GPIO_OUT );
    gpioSetDirection(NAND_BSY_GPIO_PIN, GPIO_IN );

}

/******************************************************************************
 * 
 * Function:        gpioSetDirection  
 *
 * Description:     This function configures the specified GPIO's direction  
 *
 * Parameters:      uiNumber - 	GPIO number to configure
 *                  direction - GPIO_OUT or GPIO_IN
 *
 * Return Value:    void
 * 
 *****************************************************************************/
void 
gpioSetDirection
( 
    uint32_t          uiNumber, 
    GpioDirection   direction 
)
{   
    if ( direction == GPIO_OUT ) 
    {
        CSL_GPIO_setPinDirOutput(hGpio_handle, uiNumber); /* Set to OUTPUT */
    }
    else 
    {
        CSL_GPIO_setPinDirInput (hGpio_handle, uiNumber); /* Set to INPUT */
    }
}

/******************************************************************************
 * 
 * Function:        gpioSetDataBusDirection  
 *
 * Description:     This function configures the GPIO[7:0]'s direction  
 *
 * Parameters:      direction - GPIO_OUT or GPIO_IN
 * 
 * Return Value:    void
 * 
 *****************************************************************************/
void 
gpioSetDataBusDirection
( 
    GpioDirection   direction 
)
{
    if ( direction == GPIO_OUT ) 
    {
        hGpio_handle->BANK_REGISTERS[GPIO_BANK_INDEX].DIR &= 0xFFFFFF00;
    }
    else 
    {
        hGpio_handle->BANK_REGISTERS[GPIO_BANK_INDEX].DIR |= 0x000000FF;
    }
}

/******************************************************************************
 * 
 * Function:        gpioSetOutput  
 *
 * Description:     This function sets the specified GPIO's pin state to 1
 *
 * Parameters:      uiNumber - 	GPIO number to configure
 *
 * Return Value:    void
 * 
 * Pre-Condition:   The specified GPIO should be configured as output 
 * 
 *****************************************************************************/
void 
gpioSetOutput
( 
    uint32_t      uiNumber
)
{
    CSL_GPIO_setOutputData(hGpio_handle, uiNumber);
}

/******************************************************************************
 * 
 * Function:        gpioClearOutput  
 *
 * Description:     This function Clears the specified GPIO's pin state to 0  
 *
 * Parameters:      uiNumber - GPIO number to configure
 *
 * Return Value:    void
 * 
 * Pre-Condition:   The specified GPIO should be configured as output 
 * 
 *****************************************************************************/
void 
gpioClearOutput
( 
    uint32_t      uiNumber
)
{
    CSL_GPIO_clearOutputData(hGpio_handle, uiNumber);
}

/******************************************************************************
 * 
 * Function:        gpioReadInput  
 *
 * Description:     This function gets the specified GPIO's pin state  
 *
 * Parameters:      uiNumber - 	GPIO number to configure
 *
 * Return Value:    uint32_t - Input state of GPIO if success else GPIO status
 * 
 * Pre-Condition:   The specified GPIO should be configured as input
 * 
 *****************************************************************************/
uint32_t 
gpioReadInput
( 
    uint32_t      uiNumber 
)
{   
    uint8_t inData = 0;

    if(uiNumber > GPIO_MAX_NUMBER)
    {
        return INVALID_GPIO_NUMBER;
    }
    
    CSL_GPIO_getInputData(hGpio_handle, uiNumber, &inData);

    if( ( inData & GPIO_HIGH ) == GPIO_HIGH )
    {
        return GPIO_HIGH;
    }
    else
    {
        return GPIO_LOW;
    }
}

/******************************************************************************
 * 
 * Function:        gpioWriteDataBus  
 *
 * Description:     This function sets the GPIO[7:0] pins state  
 *
 * Parameters:      uchValue - 	Value to set as output
 *
 * Return Value:    void
 * 
 * Pre-Condition:   The GPIO[7:0] should be configured as output
 * 
 *****************************************************************************/
void 
gpioWriteDataBus 
( 
    uint8_t   uchValue 
)
{
    hGpio_handle->BANK_REGISTERS[GPIO_BANK_INDEX].OUT_DATA = 
        ( (hGpio_handle->BANK_REGISTERS[GPIO_BANK_INDEX].OUT_DATA & (~GPIO_DATAMASK)) | uchValue );
}

/******************************************************************************
 * 
 * Function:        gpioClearDataBus  
 *
 * Description:     This function sets the value to the Clear data register
 *
 * Parameters:      uchValue - 	Value to set as output
 *
 * Return Value:    void
 * 
 * Pre-Condition:   None
 * 
 *****************************************************************************/
void 
gpioClearDataBus
(
    uint8_t       uchValue
)
{
    hGpio_handle->BANK_REGISTERS[GPIO_BANK_INDEX].CLR_DATA = uchValue;
}

/******************************************************************************
 * 
 * Function:        gpioSetDataBus  
 *
 * Description:     This function sets the value to the Set data register
 *
 * Parameters:      uchValue - 	Value to set as output
 *
 * Return Value:    void
 * 
 * Pre-Condition:   None
 * 
 *****************************************************************************/
void 
gpioSetDataBus
(
    uint8_t       uchValue
)
{
    hGpio_handle->BANK_REGISTERS[GPIO_BANK_INDEX].SET_DATA = uchValue;
}

/******************************************************************************
 * 
 * Function:        gpioReadDataBus  
 *
 * Description:     This function gets the GPIO[7:0] pins state
 * 
 * Parameters:      void
 *
 * Return Value:    uint8_t - Input state of GPIO[7:0]
 * 
 * Pre-Condition:   The GPIO[7:0] should be configured as input
 * 
 *****************************************************************************/
uint8_t 
gpioReadDataBus
( 
    void 
)
{    
    uint8_t value;

    value = (uint8_t)(hGpio_handle->BANK_REGISTERS[GPIO_BANK_INDEX].IN_DATA);


    return (value) ; 
}

/******************************************************************************
 * 
 * Function:        gpioEnableGlobalInterrupt  
 *
 * Description:     This function Enables GPIO interrupts to CPU
 * 
 * Parameters:      void
 *
 * Return Value:    void
 * 
 *****************************************************************************/
void 
gpioEnableGlobalInterrupt
( 
    void 
)
{
    CSL_GPIO_bankInterruptEnable(hGpio_handle, GPIO_BANK_INDEX); 
}

/******************************************************************************
 * 
 * Function:        gpioDisableGlobalInterrupt 
 *
 * Description:     This function Disables GPIO interrupts to CPU
 * 
 * Parameters:      void
 *
 * Return Value:    void
 * 
 *****************************************************************************/
void 
gpioDisableGlobalInterrupt
( 
    void 
)
{
    CSL_GPIO_bankInterruptDisable(hGpio_handle, GPIO_BANK_INDEX);
}

/******************************************************************************
 * 
 * Function         gpioSetRisingEdgeInterrupt 
 *
 * Description:     This function sets specified GPIO's rising edge interrupt
 * 
 * Parameters:      uiNumber - 	GPIO number to configure
 *
 * Return Value:    void
 * 
 *****************************************************************************/
void 
gpioSetRisingEdgeInterrupt
( 
    uint32_t      uiNumber 
)
{
    CSL_GPIO_setRisingEdgeDetect(hGpio_handle, uiNumber); 
}

/******************************************************************************
 * 
 * Function:        gpioClearRisingEdgeInterrupt 
 *
 * Description:     This function clears specified GPIO's rising edge interrupt
 * 
 * Parameters:      uiNumber - GPIO number to configure
 *
 * Return Value:    void
 * 
 *****************************************************************************/
void 
gpioClearRisingEdgeInterrupt
( 
    uint32_t      uiNumber 
)
{
    CSL_GPIO_clearRisingEdgeDetect(hGpio_handle, uiNumber); 
}

/******************************************************************************
 * 
 * Function:        gpioSetFallingEdgeInterrupt 
 *
 * Description:     This function sets specified GPIO's falling edge interrupt
 * 
 * Parameters:      uiNumber - GPIO number to configure
 *
 * Return Value:    void
 * 
 *****************************************************************************/
void 
gpioSetFallingEdgeInterrupt
( 
    uint32_t      uiNumber 
)
{
    CSL_GPIO_setFallingEdgeDetect(hGpio_handle, uiNumber); 
}

/******************************************************************************
 * 
 * Function:        gpioClearFallingEdgeInterrupt 
 *
 * Description:     This function clears specified GPIO's falling edge interrupt
 * 
 * Parameters:      uiNumber - GPIO number to configure
 *
 * Return Value:    void
 * 
 *****************************************************************************/
void 
gpioClearFallingEdgeInterrupt
( 
    uint32_t      uiNumber 
)
{
    CSL_GPIO_clearFallingEdgeDetect(hGpio_handle, uiNumber); 
}


