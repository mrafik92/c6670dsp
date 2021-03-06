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
 * File Name:  	evmc66x_ecc.c
 *
 * Description:	This file implements ECC algorithm used on micron NAND flash
 * 				
 *****************************************************************************/
 
/************************
 * Include Files
 ************************/
#include "platform_internal.h"

#if (PLATFORM_NAND_IN)

/************************
 * Globals
 ************************/
/* Pre-calculated 256-way 1 byte column parity */
#pragma DATA_SECTION(nand_ecc_precalc_table,"platform_lib");
static const uint8_t nand_ecc_precalc_table[] = {
	0x00, 0x55, 0x56, 0x03, 0x59, 0x0c, 0x0f, 0x5a, 0x5a, 0x0f, 0x0c, 0x59, 0x03, 0x56, 0x55, 0x00,
	0x65, 0x30, 0x33, 0x66, 0x3c, 0x69, 0x6a, 0x3f, 0x3f, 0x6a, 0x69, 0x3c, 0x66, 0x33, 0x30, 0x65,
	0x66, 0x33, 0x30, 0x65, 0x3f, 0x6a, 0x69, 0x3c, 0x3c, 0x69, 0x6a, 0x3f, 0x65, 0x30, 0x33, 0x66,
	0x03, 0x56, 0x55, 0x00, 0x5a, 0x0f, 0x0c, 0x59, 0x59, 0x0c, 0x0f, 0x5a, 0x00, 0x55, 0x56, 0x03,
	0x69, 0x3c, 0x3f, 0x6a, 0x30, 0x65, 0x66, 0x33, 0x33, 0x66, 0x65, 0x30, 0x6a, 0x3f, 0x3c, 0x69,
	0x0c, 0x59, 0x5a, 0x0f, 0x55, 0x00, 0x03, 0x56, 0x56, 0x03, 0x00, 0x55, 0x0f, 0x5a, 0x59, 0x0c,
	0x0f, 0x5a, 0x59, 0x0c, 0x56, 0x03, 0x00, 0x55, 0x55, 0x00, 0x03, 0x56, 0x0c, 0x59, 0x5a, 0x0f,
	0x6a, 0x3f, 0x3c, 0x69, 0x33, 0x66, 0x65, 0x30, 0x30, 0x65, 0x66, 0x33, 0x69, 0x3c, 0x3f, 0x6a,
	0x6a, 0x3f, 0x3c, 0x69, 0x33, 0x66, 0x65, 0x30, 0x30, 0x65, 0x66, 0x33, 0x69, 0x3c, 0x3f, 0x6a,
	0x0f, 0x5a, 0x59, 0x0c, 0x56, 0x03, 0x00, 0x55, 0x55, 0x00, 0x03, 0x56, 0x0c, 0x59, 0x5a, 0x0f,
	0x0c, 0x59, 0x5a, 0x0f, 0x55, 0x00, 0x03, 0x56, 0x56, 0x03, 0x00, 0x55, 0x0f, 0x5a, 0x59, 0x0c,
	0x69, 0x3c, 0x3f, 0x6a, 0x30, 0x65, 0x66, 0x33, 0x33, 0x66, 0x65, 0x30, 0x6a, 0x3f, 0x3c, 0x69,
	0x03, 0x56, 0x55, 0x00, 0x5a, 0x0f, 0x0c, 0x59, 0x59, 0x0c, 0x0f, 0x5a, 0x00, 0x55, 0x56, 0x03,
	0x66, 0x33, 0x30, 0x65, 0x3f, 0x6a, 0x69, 0x3c, 0x3c, 0x69, 0x6a, 0x3f, 0x65, 0x30, 0x33, 0x66,
	0x65, 0x30, 0x33, 0x66, 0x3c, 0x69, 0x6a, 0x3f, 0x3f, 0x6a, 0x69, 0x3c, 0x66, 0x33, 0x30, 0x65,
	0x00, 0x55, 0x56, 0x03, 0x59, 0x0c, 0x0f, 0x5a, 0x5a, 0x0f, 0x0c, 0x59, 0x03, 0x56, 0x55, 0x00
};


/******************************************************************************
 * 
 * Function:	nandTransResult  
 *
 * Description:	Creates non-inverted ECC code from line parity
 *
 * Parameters:	uint8_t uchReg2 - line parity reg 2
 * 				uint8_t uchReg3 - line parity reg 3
 * 				uint8_t *puchEccCode - ecc
 *
 * Return Value: void
 ******************************************************************************/
static void nandTransResult(uint8_t uchReg2, uint8_t uchReg3, uint8_t *puchEccCode)
{
	uint8_t a, b, i, tmp1, tmp2;

	/* Initialize variables */
	a = b = 0x80;
	tmp1 = tmp2 = 0;

	/* Calculate first ECC byte */
	for (i = 0; i < 4; i++) {
		if (uchReg3 & a)		/* LP15,13,11,9 --> ecc_code[0] */
			tmp1 |= b;
		b >>= 1;
		if (uchReg2 & a)		/* LP14,12,10,8 --> ecc_code[0] */
			tmp1 |= b;
		b >>= 1;
		a >>= 1;
	}

	/* Calculate second ECC byte */
	b = 0x80;
	for (i = 0; i < 4; i++) {
		if (uchReg3 & a)		/* LP7,5,3,1 --> ecc_code[1] */
			tmp2 |= b;
		b >>= 1;
		if (uchReg2 & a)		/* LP6,4,2,0 --> ecc_code[1] */
			tmp2 |= b;
		b >>= 1;
		a >>= 1;
	}

	/* Store two of the ECC bytes */
	puchEccCode[0] = tmp1;
	puchEccCode[1] = tmp2;
}
/******************************************************************************
 * 
 * Function:	eccComputeECC  
 *
 * Description:	Compute 3 byte ECC code for 256 byte block
 *
 * Parameters:	uint8_t* puchData - pointer to raw data
 * 				uint8_t *puchEccCode - pointer to ECC buffer
 *
 * Return Value: status
 ******************************************************************************/
uint32_t eccComputeECC(const uint8_t *puchData, uint8_t *puchEccCode)
{
	uint8_t uchIndex, uchReg1, uchReg2, uchReg3;
	int j;
	if(puchData == NULL || puchEccCode == NULL)
		return FAIL;
		
	/* Initialize variables */
	uchReg1 = uchReg2 = uchReg3 = 0;
	puchEccCode[0] = puchEccCode[1] = puchEccCode[2] = 0;

	/* Build up column parity */
	for(j = 0; j < 256; j++) {

		/* Get CP0 - CP5 from table */
		uchIndex = nand_ecc_precalc_table[puchData[j]];
		uchReg1 ^= (uchIndex & 0x3f);

		/* All bit XOR = 1 ? */
		if (uchIndex & 0x40) {
			uchReg3 ^= (uint8_t) j;
			uchReg2 ^= ~((uint8_t) j);
		}
	}

	/* Create non-inverted ECC code from line parity */
	nandTransResult(uchReg2, uchReg3, puchEccCode);

	/* Calculate final ECC code */
	puchEccCode[0] = ~puchEccCode[0];
	puchEccCode[1] = ~puchEccCode[1];
	puchEccCode[2] = ((~uchReg1) << 2) | 0x03;
	return SUCCESS;
}

/******************************************************************************
 * 
 * Function:	eccCorrectData  
 *
 * Description:	Detect and correct a 1 bit error for 256 byte block
 *
 * Parameters:	uint8_t* puchData - Raw data read from the chip
 * 				uint8_t *puchEccRead - ECC from the chip
 * 				uint8_t *puchEccCalc - ECC calculated from raw data
 * 				
 * Return Value: status
 ******************************************************************************/
int32_t eccCorrectData(uint8_t *puchData, uint8_t *puchEccRead, uint8_t *puchEccCalc)
{
	uint8_t a, b, c, d1, d2, d3, add, bit, i;
	
	if(puchData == NULL || puchEccRead == NULL || puchEccCalc == NULL)
		return FAIL;
	
	/* Do error detection */
	d1 = puchEccCalc[0] ^ puchEccRead[0];
	d2 = puchEccCalc[1] ^ puchEccRead[1];
	d3 = puchEccCalc[2] ^ puchEccRead[2];

	if ((d1 | d2 | d3) == 0) {
		/* No errors */
		return SUCCESS;
	}
	else {
		a = (d1 ^ (d1 >> 1)) & 0x55;
		b = (d2 ^ (d2 >> 1)) & 0x55;
		c = (d3 ^ (d3 >> 1)) & 0x54;

		/* Found and will correct single bit error in the data */
		if ((a == 0x55) && (b == 0x55) && (c == 0x54)) {
			c = 0x80;
			add = 0;
			a = 0x80;
			for (i=0; i<4; i++) {
				if (d1 & c)
					add |= a;
				c >>= 2;
				a >>= 1;
			}
			c = 0x80;
			for (i=0; i<4; i++) {
				if (d2 & c)
					add |= a;
				c >>= 2;
				a >>= 1;
			}
			bit = 0;
			b = 0x04;
			c = 0x80;
			for (i=0; i<3; i++) {
				if (d3 & c)
					bit |= b;
				c >>= 2;
				b >>= 1;
			}
			b = 0x01;
			a = puchData[add];
			a ^= (b << bit);
			puchData[add] = a;
			return SUCCESS;
		} else {
			i = 0;
			while (d1) {
				if (d1 & 0x01)
					++i;
				d1 >>= 1;
			}
			while (d2) {
				if (d2 & 0x01)
					++i;
				d2 >>= 1;
			}
			while (d3) {
				if (d3 & 0x01)
					++i;
				d3 >>= 1;
			}
			if (i == 1) {
				/* ECC Code Error Correction */
				puchEccRead[0] = puchEccCalc[0];
				puchEccRead[1] = puchEccCalc[1];
				puchEccRead[2] = puchEccCalc[2];
				return SUCCESS;
			}
			else {
				/* Uncorrectable Error */
				return FAIL;
			}
		}
	}
}

#endif /*PLATFORM_NAND_IN*/
