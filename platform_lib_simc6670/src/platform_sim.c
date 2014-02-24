/****************************************************************************
 * Copyright (c) 2010-2011 Texas Instruments Incorporated - http://www.ti.com
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
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
#include "platform_internal.h"


/* Errno value */
uint32_t platform_errno = 0;

/* Platform Library Version*/
#if (PLATFORM_VERSTRING_IN)
#pragma DATA_SECTION(platform_library_version,"platform_lib");
static char platform_library_version[16] = PLATFORM_LIB_VERSION;
#endif

/* Information we need to keep around for access */
#pragma DATA_SECTION(platform_mcb,"platform_lib");
static struct platform_mcb_t {
    uint32_t	frequency;
    int32_t  	board_version;
    int32_t		mastercore;
} platform_mcb = {0, 0, 0};

#if (PLATFORM_WRITE_IN)
#pragma DATA_SECTION(write_type,"platform_lib");
static WRITE_info	write_type;
#pragma DATA_SECTION(write_buffer,"platform_lib");
static char			write_buffer[MAX_WRITE_LEN];
#endif


/* This structure holds information about the devices on the platform */
#if (PLATFORM_NAND_IN)
#pragma DATA_SECTION(gDeviceNandBBlist,"platform_lib");
uint8_t gDeviceNandBBlist[NAND_BLOCKS_PER_DEVICE];

#pragma DATA_SECTION(gDeviceNand,"platform_lib");
PLATFORM_DEVICE_info gDeviceNand = {0x20,	0x36,PLATFORM_DEVICE_NAND, 8, NAND_BLOCKS_PER_DEVICE, NAND_PAGES_PER_BLOCK, NAND_BYTES_PER_PAGE, NAND_SPARE_BYTES_PER_PAGE, PLATFORM_DEVID_NAND512R3A2D, 5, 0x200, 0, NULL, NULL};
#endif

#if (PLATFORM_NOR_IN)
#pragma DATA_SECTION(gDeviceNor,"platform_lib");
PLATFORM_DEVICE_info gDeviceNor  = {0, 0, PLATFORM_DEVICE_NOR, 8, SPI_NOR_SECTOR_COUNT, (SPI_NOR_PAGE_COUNT / SPI_NOR_SECTOR_COUNT), SPI_NOR_PAGE_SIZE, 0, PLATFORM_DEVID_NORN25Q128, 0, 0, 0, NULL, NULL};
#endif

#if (PLATFORM_EEPROM_IN)
#pragma DATA_SECTION(gDeviceEeprom0,"platform_lib");
PLATFORM_DEVICE_info gDeviceEeprom0 = {PLATFORM_EEPROM_MANUFACTURER_ID,PLATFORM_EEPROM_DEVICE_ID_1,PLATFORM_DEVICE_EEPROM,	8, 1, 1, 65536, 0, PLATFORM_DEVID_EEPROM50, 0, 0, 0, NULL, NULL};
#pragma DATA_SECTION(gDeviceEeprom1,"platform_lib");
PLATFORM_DEVICE_info gDeviceEeprom1 = {PLATFORM_EEPROM_MANUFACTURER_ID,PLATFORM_EEPROM_DEVICE_ID_2,PLATFORM_DEVICE_EEPROM,	8, 1, 1, 65536, 0, PLATFORM_DEVID_EEPROM51, 0, 0, 0, NULL, NULL};
#endif

/* This structure holds information about the EMAC port on the platform */
#pragma DATA_SECTION(emac_port_mode,"platform_lib");
PLATFORM_EMAC_PORT_MODE emac_port_mode[PLATFORM_MAX_EMAC_PORT_NUM] =
{
    PLATFORM_EMAC_PORT_MODE_NONE,
    PLATFORM_EMAC_PORT_MODE_NONE
};

/******************************************************************************
 * platform_get_frequency
 *
 * Internal function to read frequency from PLL.
 *
 ******************************************************************************/
static inline uint32_t platform_get_frequency(void)
{
    uint32_t        dsp_freq = 983000;

    return (dsp_freq);

}


/******************************************************************************
 * platform_get_info
 ******************************************************************************/
#if (PLATFORM_GETINFO_IN)
void platform_get_info(platform_info * p_info)
{
    uint32_t 			csr 		= CSL_chipReadCSR(); //CSL_chipReadReg(CSL_CHIP_CSR);
    volatile uint32_t 	*megm_rev   = (uint32_t *) (MEGM_REV_ID_REGISTER);
    uint32_t 			i;

    if (p_info == 0) {
        IFPRINT(platform_write("p_info argument is NULL\n"));
        platform_errno = PLATFORM_ERRNO_INVALID_ARGUMENT;
        return;
    }

    memset(p_info, 0, sizeof(platform_info));

    strncpy(p_info->version, platform_library_version, 16);

    p_info->cpu.core_count 	= PLATFORM_CORE_COUNT;
    p_info->cpu.id 			= (Uint16) CSL_FEXT(csr, CHIP_CSR_CPU_ID);
    p_info->cpu.revision_id = (Uint16) CSL_FEXT(csr, CHIP_CSR_REV_ID);
    strncpy(p_info->cpu.name, PLATFORM_INFO_CPU_NAME, 32);

    p_info->cpu.megamodule_revision_major =
        (uint16_t)(((*megm_rev) & MEGM_REV_ID_MAJ_MASK) >> MEGM_REV_ID_MAJ_SHIFT);
    p_info->cpu.megamodule_revision_minor =
        (uint16_t)(((*megm_rev) & MEGM_REV_ID_MIN_MASK) >> MEGM_REV_ID_MIN_SHIFT);

    strncpy(p_info->board_name, PLATFORM_INFO_BOARD_NAME, 32);


    if ( TRUE == CSL_BootCfgIsLittleEndian() ) {
        p_info->cpu.endian = PLATFORM_LE;
    }
    else {
        p_info->cpu.endian = PLATFORM_BE;
    }

    p_info->emac.port_count = PLATFORM_NUM_ETH_PORTS;

    p_info->frequency 			= platform_get_frequency();
    platform_mcb.frequency 		= p_info->frequency;
    p_info->board_rev 			= 0;
    platform_mcb.board_version 	= p_info->board_rev;

    platform_get_macaddr(PLATFORM_MAC_TYPE_EFUSE, &(p_info->emac.efuse_mac_address[0]));
    platform_get_macaddr(PLATFORM_MAC_TYPE_EEPROM, &(p_info->emac.eeprom_mac_address[0]));

    for (i = PLATFORM_USER_LED_CLASS; i < PLATFORM_END_LED_CLASS; i++ ) {
        switch (i) {
            case PLATFORM_USER_LED_CLASS:
                p_info->led[i].count     = PLATFORM_FPGA_LED_COUNT;
                break;
            case PLATFORM_SYSTEM_LED_CLASS:
                p_info->led[i].count     = PLATFORM_I2C_LED_COUNT;
                break;
            default:
                IFPRINT(platform_write("Can't read LED Class information\n"));
                platform_errno = PLATFORM_ERRNO_LED;
                break;
        }
    }
}
#endif

/******************************************************************************
 * platform_init
 ******************************************************************************/
#if (PLATFORM_INIT_IN)

/* Registers to enable or disable memory ECC for L1, L2 and MSMC memories */
#define L1PEDSTAT	0x01846404
#define L1PEDCMD	0x01846408
#define L1PEDADDR	0x0184640C
#define L2EDSTAT	0x01846004
#define L2EDCMD		0x01846008
#define L2EDADDR	0x0184600C
#define L2EDCPEC	0x01846018
#define L2EDCNEC	0x0184601C
#define L2EDCEN		0x01846030
#define SMCERRAR	0x0BC00008
#define	SMCERRXR	0x0BC0000C
#define SMEDCC		0x0BC00010
#define SMCEA		0x0BC00014
#define SMSECC		0x0BC00018


Platform_STATUS platform_init(platform_init_flags  * p_flags,
        					  platform_init_config * p_config)
{
    if ((p_flags == 0) || (p_config == 0)){
        platform_errno = PLATFORM_ERRNO_INVALID_ARGUMENT;
        return ( (Platform_STATUS) Platform_EFAIL);
    }

    CSL_chipWriteTSCL(0);

    /* PLLC module handle structure */
    if (p_flags->pll) {

    }

    CSL_chipWriteTSCL(0);
    if(!platform_mcb.frequency) {
       platform_mcb.frequency = platform_get_frequency();
    }

    /* Initialize DDR */
    if (p_flags->ddr) {

    }

    /* Enable Error Correction for memory */
    if (p_flags->ecc) {
    	/*Nothing to do on simulator */
    }


    if (p_flags->phy) {
        configSerdes();
        /*Init_SGMII(1);*/
    }


    return Platform_EOK;
}
#endif

/******************************************************************************
 * platform_get_coreid
 ******************************************************************************/
#if (PLATFORM_GETCOREID_IN)
uint32_t platform_get_coreid(void)
{
    return (CSL_chipReadDNUM());
}
#endif

/******************************************************************************
 * platform_getmacaddr
 ******************************************************************************/
#if (PLATFORM_GETMACADDR_IN)
Platform_STATUS platform_get_emac_info(uint32_t port_num, PLATFORM_EMAC_EXT_info * emac_info)
{
	IFPRINT(platform_write("platform_get_emac_info called \n"));

    emac_info->port_num       = port_num;
    emac_info->mode           = emac_port_mode[port_num];

    emac_info->mac_address[0] = 0x10;
    emac_info->mac_address[1] = 0x11;

    emac_info->mac_address[2] = 0x12;
    emac_info->mac_address[3] = 0x13;
    emac_info->mac_address[4] = 0x14;
    emac_info->mac_address[5] = 0x15;

    return Platform_EOK;
}

/*
 * August 15, 2011 - platform_get_macaddr() is deprecated, application needs to call
 * the new API platform_get_emac_info() to get the MAC address of the port
 */
Platform_STATUS platform_get_macaddr(PLATFORM_MAC_TYPE type, uint8_t * p_mac_address)
{
	IFPRINT(platform_write("platform_get_macaddr called \n"));
    switch (type) {

        case PLATFORM_MAC_TYPE_EFUSE:
        {
            p_mac_address[0] = 0x10;
            p_mac_address[1] = 0x11;

            p_mac_address[2] = 0x12;
            p_mac_address[3] = 0x13;
            p_mac_address[4] = 0x14;
            p_mac_address[5] = 0x15;

            return Platform_EOK;
        }

        default:
        case PLATFORM_MAC_TYPE_EEPROM:
        {
            memset(p_mac_address, 0, 6);
            return ((Platform_STATUS) Platform_EUNSUPPORTED);
        }
    }
}
#endif

/******************************************************************************
 * platform_get_phy_addr
 ******************************************************************************/
#if (PLATFORM_GETPHYADDR_IN)
int32_t platform_get_phy_addr(uint32_t port_num)
{
	IFPRINT(platform_write("platform_get_phy_addr called \n"));
	return port_num;
}
#endif

/******************************************************************************
 * platform_phy_link_status
 ******************************************************************************/
#if (PLATFORM_PHYLINKSTATUS_IN)
Platform_STATUS platform_phy_link_status(uint32_t port_num)
{
    return Platform_EOK;
}
#endif

/******************************************************************************
 * platform_get_switch_state
 ******************************************************************************/
#if (PLATFORM_GETSWITCHSTATE_IN)
uint32_t platform_get_switch_state(uint32_t id)
{
   IFPRINT(platform_write("platform_get_switch_state(id=%d) called \n", id));

   return (0);
}
#endif


/******************************************************************************
 * platform_uart_read
 ******************************************************************************/
#if (PLATFORM_UART_IN)
Platform_STATUS platform_uart_read(uint8_t *buf, uint32_t delay)
{
   return ((Platform_STATUS) Platform_EUNSUPPORTED);
}

/******************************************************************************
 * platform_uart_write
 ******************************************************************************/
Platform_STATUS platform_uart_write(uint8_t buf)
{

	return ((Platform_STATUS) Platform_EOK);
}

/******************************************************************************
 * platform_uart_set_baudrate
 ******************************************************************************/
Platform_STATUS platform_uart_set_baudrate(uint32_t baudrate) {

	return ((Platform_STATUS) Platform_EOK);
}

/******************************************************************************
 * platform_uart_init
 ******************************************************************************/
Platform_STATUS platform_uart_init(void) {

	return ((Platform_STATUS) Platform_EOK);
}

#endif


/******************************************************************************
 * platform_led
 ******************************************************************************/
#if (PLATFORM_LED_IN)
Platform_STATUS platform_led(uint32_t led_id, PLATFORM_LED_OP operation, LED_CLASS_E led_class)
{
	return ((Platform_STATUS) Platform_EOK);
}
#endif

/******************************************************************************
 * platform_delay
 ******************************************************************************/
#if (PLATFORM_DELAY_IN)
Platform_STATUS platform_delay(uint32_t usecs)
{
    uint32_t i;

    for ( i = 0 ; i < usecs ; i++ ){ asm (" nop "); };

	return ((Platform_STATUS) Platform_EOK);
}

void platform_delaycycles(uint32_t cycles)
{
	uint32_t start_val  = CSL_chipReadTSCL();

	while ((CSL_chipReadTSCL() - start_val) < cycles);

    return;
}
#endif

/******************************************************************************
 * platform_memory_test
 ******************************************************************************/
#if (PLATFORM_EXTMEMTEST_IN || PLATFORM_INTMEMTEST_IN)
static inline int32_t platform_memory_test (uint32_t start_address, uint32_t end_address)
{
    uint32_t index, value;

    /* Write a pattern */
    for (index = start_address; index < end_address; index += 4) {
        *(volatile uint32_t *) index = (uint32_t)index;
    }

    /* Read and check the pattern */
    for (index = start_address; index < end_address; index += 4) {

    	value = *(uint32_t *) index;

        if (value  != index) {
            IFPRINT(platform_write("platform_memory_test: Failed at address index = 0x%x value = 0x%x *(index) = 0x%x\n",
                index, value, *(volatile uint32_t *) index));
            platform_errno = index;
            return (Platform_EFAIL);
        }
    }

    return Platform_EOK;
}
#endif

/******************************************************************************
 * platform_external_memory_test
 ******************************************************************************/
#if (PLATFORM_EXTMEMTEST_IN)
Platform_STATUS platform_external_memory_test(uint32_t start_address, uint32_t end_address)
{
	IFPRINT(platform_write("platform_external_memory_test(start=0x%x,end=0x%x) called \n", start_address, end_address));
    return Platform_EOK;
}
#endif

/******************************************************************************
 * platform_internal_memory_test
 ******************************************************************************/
#if (PLATFORM_INTMEMTEST_IN)
Platform_STATUS platform_internal_memory_test(uint32_t id)
{
    uint32_t start_address, end_address;
    CSL_PSC_MODSTATE state_prev, state_val;
    CSL_PSC_MDLRST   loc_rst_prev, loc_rst_val;
    uint32_t rstiso_prev_flag;
    uint32_t module_number = 15;
    int32_t status = Platform_EOK;

	IFPRINT(platform_write("platform_internal_memory_test(core=%d) called \n", id));

    if (platform_get_coreid() == id) {
        IFPRINT(platform_write("platform_internal_memory_test: Can't run the test own core(%d)\n",
            id));
        platform_errno = PLATFORM_ERRNO_INVALID_ARGUMENT;
        return ((Platform_STATUS) Platform_EUNSUPPORTED);
    }

    if (id >= PLATFORM_CORE_COUNT) {
        IFPRINT(platform_write("platform_internal_memory_test: core id should be in range [0..%d]\n",
            PLATFORM_CORE_COUNT - 1));
        platform_errno = PLATFORM_ERRNO_INVALID_ARGUMENT;
        return ((Platform_STATUS) Platform_EINVALID);
    }

    /* For 6670L */
    switch (id) {
        case 0:
            module_number = 23; //(C66x Core 0 and Timer 0)
            break;
        case 1:
            module_number = 24; //(C66x Core 1 and Timer 1)
            break;
        case 2:
            module_number = 26; //(C66x Core 2 and Timer 2)
            break;
        case 3:
            module_number = 28; //(C66x Core 3 and Timer 3)
            break;
    }

    /* Preserve the state, local reset and resetiso values */
    state_prev          = CSL_PSC_getModuleNextState(module_number);
    loc_rst_prev        = CSL_PSC_getModuleLocalReset(module_number);
    rstiso_prev_flag    = CSL_PSC_isModuleResetIsolationEnabled(module_number);

    /* Clear the state, local reset and resetiso values */
    state_val           = PSC_MODSTATE_SWRSTDISABLE;
    loc_rst_val         = PSC_MDLRST_ASSERTED;

    CSL_PSC_setModuleNextState(module_number, state_val);
    CSL_PSC_setModuleLocalReset(module_number, loc_rst_val);
    CSL_PSC_disableModuleResetIsolation(module_number);

    /* Test L1P memory region*/
    start_address = PLATFORM_L1P_BASE_ADDRESS | (id << 24);
    end_address   = start_address + PLATFORM_L1P_SIZE;
    if (platform_memory_test(start_address, end_address) != Platform_EOK) {
        status = Platform_EFAIL;
    }

    /* Test L1D memory region*/
    start_address = PLATFORM_L1D_BASE_ADDRESS | (id << 24);
    end_address   = start_address + PLATFORM_L1D_SIZE;
    if (platform_memory_test(start_address, end_address) != Platform_EOK) {
        status = Platform_EFAIL;
    }

    /* Restore the previous state, local reset and resetiso values */
    CSL_PSC_setModuleNextState(module_number, state_prev);
    CSL_PSC_setModuleLocalReset(module_number, loc_rst_prev);
    if (rstiso_prev_flag) {
         CSL_PSC_enableModuleResetIsolation(module_number);
    } else {
         CSL_PSC_disableModuleResetIsolation(module_number);
    }

    return status;
}
#endif

/******************************************************************************
 * platform_write  - Printf or echo to UART or both
 ******************************************************************************/
#if (PLATFORM_WRITE_IN)
WRITE_info platform_write_configure (WRITE_info	wtype) {
	WRITE_info original;
	IFPRINT(platform_write("platform_write_configure(write_type=%d) called \n", wtype));
	original = write_type;
	write_type = wtype;
	return original;
}

void platform_write(const char *fmt, ... )
{
    va_list 	arg_ptr;

    /* Initial platform_write to temporary buffer.. at least try some sort of sanity check so we don't write all over
     * memory if the print is too large.
     */
    if (strlen(fmt) > MAX_WRITE_LEN) {platform_errno = PLATFORM_ERRNO_INVALID_ARGUMENT; return;}

    va_start( arg_ptr, fmt );
    vsprintf( (char *)write_buffer, fmt, arg_ptr );
    va_end( arg_ptr );

	if ((write_type == PLATFORM_WRITE_PRINTF) || (write_type == PLATFORM_WRITE_ALL)) {
		printf( "%s", write_buffer );
	}

	if ((write_type == PLATFORM_WRITE_UART) || (write_type == PLATFORM_WRITE_ALL)) {
		/* Log to console port (routine only sends a byte at a time) */
	    platform_errno = PLATFORM_ERRNO_UNSUPPORTED;
	}

	return;
}
#endif


/******************************************************************************
 * platform_flash_open
 ******************************************************************************/
#if (PLATFORM_NAND_IN) || (PLATFORM_NOR_IN)  || (PLATFORM_EEPROM_IN)
PLATFORM_DEVICE_info *platform_device_open(uint32_t deviceid, uint32_t flags ) {

	IFPRINT(platform_write("platform_device_open(deviceid=0x%x,flags=0x%x) called \n", deviceid, flags));

	platform_errno = PLATFORM_ERRNO_UNSUPPORTED;
	return NULL;
}
#endif

/******************************************************************************
 * platform_flash_close
 ******************************************************************************/
#if (PLATFORM_NAND_IN) || (PLATFORM_NOR_IN)  || (PLATFORM_EEPROM_IN)
Platform_STATUS platform_device_close(PLATFORM_DEVHANDLE deviceid) {

	IFPRINT(platform_write("platform_device_close(handle=0x%x) called \n", deviceid));

	return Platform_EOK;
}
#endif

/******************************************************************************
 * platform_flash_read_spare_data
 ******************************************************************************/
#if (PLATFORM_NAND_IN) || (PLATFORM_NOR_IN) || (PLATFORM_EEPROM_IN)
Platform_STATUS platform_device_read_spare_data(PLATFORM_DEVHANDLE deviceid,
												uint32_t block_number,
												uint32_t page_number,
												uint8_t *buf) {

	IFPRINT(platform_write("platform_device_read_spare_data(handle=0x%x, block=%d, page=%d,buf=0x%x) called \n",
			deviceid, block_number, page_number, buf));

	return Platform_EOK;

}
#endif

/******************************************************************************
 * platform_device_mark_block_bad
 ******************************************************************************/
#if (PLATFORM_NAND_WRITE_IN) || (PLATFORM_NOR_WRITE_IN) || (PLATFORM_EEPROM_WRITE_IN)
Platform_STATUS platform_device_mark_block_bad(PLATFORM_DEVHANDLE deviceid,
												uint32_t block_number) {
    return Platform_EOK;

}
#endif

/******************************************************************************
 * platform_flash_write_spare_data
 ******************************************************************************/
#if (PLATFORM_NAND_IN) || (PLATFORM_NOR_IN) || (PLATFORM_EEPROM_IN)
Platform_STATUS platform_device_write_spare_data(PLATFORM_DEVHANDLE deviceid,
												uint32_t block_number,
												uint32_t page_number,
												uint8_t *buf) {
	IFPRINT(platform_write("platform_device_write_spare_data(handle=0x%x, block=%d, page=%d,buf=0x%x) called \n",
			deviceid, block_number, page_number, buf));

	return Platform_EOK;

}
#endif

/******************************************************************************
 * platform_flash_read
 ******************************************************************************/
#if (PLATFORM_NAND_IN) || (PLATFORM_NOR_IN) || (PLATFORM_EEPROM_IN)
Platform_STATUS platform_device_read(PLATFORM_DEVHANDLE 	deviceid,
									 uint32_t 	offset,
                                     uint8_t 	*buf,
                                     uint32_t	len) {

	IFPRINT(platform_write("platform_device_read(handle=0x%x, offset=%d, len=%d, buf=0x%x) called \n",
			deviceid, offset, len, buf));

	return Platform_EUNSUPPORTED;

}
#endif

/******************************************************************************
 * Computes a block and page based on an offset
 ******************************************************************************/
#if (PLATFORM_NAND_IN) || (PLATFORM_NOR_IN)

Platform_STATUS platform_offset_to_blocknpage(PLATFORM_DEVHANDLE 	deviceid,
									 uint32_t 	offset,
                                     uint32_t 	*block,
                                     uint32_t	*page) {
	uint32_t	leftover;
	uint32_t	block_size;
	uint32_t	block_count, page_size, page_count;

	IFPRINT(platform_write("platform_offset_to_blocknpage(handle=0x%x, offset=%d) called \n",
			deviceid, offset));

	if (deviceid == PLATFORM_DEVID_NAND512R3A2D) {
		block_count = gDeviceNand.block_count;
		page_size	= gDeviceNand.page_size;
		page_count 	= gDeviceNand.page_count;
	}
	else if (deviceid == PLATFORM_DEVID_NORN25Q128) {
		block_count = gDeviceNor.block_count;
		page_size	= gDeviceNor.page_size;
		page_count 	= gDeviceNor.page_count;
	}
	else {
		return Platform_EUNSUPPORTED;
	}

	block_size = (page_count * page_size);

	*block 	  = offset / block_size;
	leftover  = offset % block_size;
	*page 	  = leftover / page_size;
	if (leftover % page_size) {
		/* All writes must be page aligned for now */
		return Platform_EUNSUPPORTED;
	}
	if (*block > block_count) {
		return Platform_EINVALID;
	}
	if (*page > page_count) {
		return Platform_EINVALID;
	}

	IFPRINT(platform_write("platform_offset_to_blocknpage: offset = %d block = %d page = %d \n", offset, *block, *page));

	return Platform_EOK;

}

#endif

/******************************************************************************
 * Computes a block and page based on an offset
 ******************************************************************************/
#if (PLATFORM_NAND_IN) || (PLATFORM_NOR_IN)

Platform_STATUS platform_blocknpage_to_offset(PLATFORM_DEVHANDLE 	deviceid,
									 uint32_t 	*offset,
                                     uint32_t 	block,
                                     uint32_t	page) {

	uint32_t	block_count, page_size, page_count;

	IFPRINT(platform_write("platform_blocknpage_to_offset(handle=0x%x, block=%d, page=%d) called \n",
			deviceid, block, page));

	if (deviceid == PLATFORM_DEVID_NAND512R3A2D) {
		block_count = gDeviceNand.block_count;
		page_size	= gDeviceNand.page_size;
		page_count 	= gDeviceNand.page_count;
	}
	else if (deviceid == PLATFORM_DEVID_NORN25Q128) {
		block_count = gDeviceNor.block_count;
		page_size	= gDeviceNor.page_size;
		page_count 	= gDeviceNor.page_count;
	}
	else {
		return Platform_EUNSUPPORTED;
	}

	if (block >block_count || page > page_count) {
			return Platform_EINVALID;
	}

	*offset = 	(block * (page_count * page_size)) + (page * page_size);

	IFPRINT(platform_write("platform_blocknpage_to_offset: offset = %d block = %d page = %d \n", *offset, block, page));

	return Platform_EOK;
}

#endif

/******************************************************************************
 * platform_flash_write
 ******************************************************************************/

#if (PLATFORM_NAND_IN) || (PLATFORM_NOR_IN) || (PLATFORM_EEPROM_IN)

Platform_STATUS platform_device_write(PLATFORM_DEVHANDLE 	deviceid,
									 uint32_t 	offset,
                                     uint8_t 	*buf,
                                     uint32_t	len) {

	IFPRINT(platform_write("platform_device_write(handle=0x%x, offset=%d, len=%d, buf=0x%x) called \n",
			deviceid, offset, len, buf));

	return Platform_EOK;

}
#endif

/******************************************************************************
 * platform_flash_erase_block
 ******************************************************************************/
#if (PLATFORM_NAND_IN) || (PLATFORM_NOR_IN) || (PLATFORM_EEPROM_IN)
Platform_STATUS platform_device_erase_block(PLATFORM_DEVHANDLE	deviceid,
                                           uint32_t block_number) {

	IFPRINT(platform_write("platform_device_erase_block(handle=0x%x, blockt=%d) called \n",
			deviceid, block_number));

	return Platform_EOK;
}
#endif


#if PLATFORM_CACHE_IN
uint32_t Convert_CoreLocal2GlobalAddr (uint32_t  addr)
{
    uint32_t coreNum;

    /* Get the core number. */
    coreNum = CSL_chipReadDNUM();

    /* Check if the address is a valid Local L2 address to convert */
    if ( (addr >= (uint32_t) 0x800000) &&
         (addr <  (uint32_t) 0x880000) ) {
         /* Compute the global address. */
         return ((1 << 28) | (coreNum << 24) | (addr & 0x00ffffff));
    }
    else {
        return (addr);
    }
}
#endif

