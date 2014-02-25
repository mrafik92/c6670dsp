/*
 * main.c
 */

#include "xdcDeff.h"
#include "DescriptorsLayout.h"
#include "QueueFun.h"
#include "QMSSr.h"

#pragma DATA_ALIGN (host_region, 16)
Uint8 host_region[64 * 64];
#pragma DATA_ALIGN (mono_region, 16)
Uint8 mono_region[32 * 160];
#pragma DATA_ALIGN (buffers, 16)
Uint32 buffers[64 * 256]; //these buffers are for Host Packets
#pragma DATA_ALIGN (hostList, 16)
Uint32 hostList[34]; // ping/pong of (16 + 1 word for list count)
#pragma DATA_ALIGN (monoList, 16)
Uint32 monoList[34]; // ping/pong of (16 + 1 word for list count)



MNAV_HostPacketDescriptor *host_pkt;
MNAV_MonolithicPacketDescriptor *mono_pkt;
Qmss_AccCmd cmd;


int main(void) {
	int idx;
	Uint16 qm_map[4];
	/* Setup Memory Region 0 for 40 56 byte Host descriptors. Our
	 * Host descriptors will be 32 bytes plus up to 6 words of PS data,
	 * but the next best size is 64 bytes times 64 descriptors. */
	set_memory_region(0, (Uint32) host_region, 0, 0x00030001);
	/* Setup Memory Region 1 for 8 148B Monolithic descriptors. Our
	 * Mono descriptors will be 12 bytes plus 16 bytes of EPIB Info, plus
	 * 128 bytes of payload, but the next best size is 160 bytes times
	 * 32 descriptors. (dead space is possible) */
	set_memory_region(1, (Uint32) mono_region, 64, 0x00090000);

	/*****************************************************************
	 * Configure Linking RAM 0 to use the 16k entry internal link ram.
	 */
	set_link_ram(0, 0x00080000, 0x3FFF);

	/* Initialize descriptor regions to zero */
	memset(host_region, 0, 64 * 64);
	memset(mono_region, 0, 32 * 160);

	/* Push Host Descriptors to Tx Completion Queue (FDQ) 5000 */
	for (idx = 0; idx < 20; idx ++)
	{
		host_pkt = (MNAV_HostPacketDescriptor *)(host_region + (idx * 64));
		host_pkt->pkt_return_qmgr = 1;
		host_pkt->pkt_return_qnum = 0;
		host_pkt->orig_buff0_len = 64 * 4;
		host_pkt->orig_buff0_ptr = (Uint32)(buffers + (idx * 128)); // ??
		host_pkt->next_desc_ptr = NULL;
		push_queue(5000, 1, 0, (Uint32)(host_pkt));
	}
	/* Push Monolithic packets to Tx Completion Queue (FDQ) 5001 */
	for (idx = 0; idx < 16; idx ++)
	{
		mono_pkt = (MNAV_MonolithicPacketDescriptor *)(mono_region + (idx * 160));
		mono_pkt->pkt_return_qmgr = 1;
		mono_pkt->pkt_return_qnum = 1;
		push_queue(5001, 1, 0, (Uint32)(mono_pkt));
	}

	/* Push Host Descriptors to Rx FDQ 7000 */
	for (idx = 20; idx < 64; idx ++)
	{
		host_pkt = (MNAV_HostPacketDescriptor *)(host_region + (idx * 64));
		/* Set non-Rx overwrite fields */
		host_pkt->orig_buff0_len = 64 * 4;
		host_pkt->orig_buff0_ptr = (Uint32)(buffers + (idx * 128));
		host_pkt->next_desc_ptr = NULL; //don’t link Host buffers in Rx FDQ
		push_queue(7000, 1, 0, (Uint32)(host_pkt));
	}
	/* Push Monolithic packets to Rx FDQ 2001 */
	for (idx = 16; idx < 32; idx ++)
	{
		mono_pkt = (MNAV_MonolithicPacketDescriptor *)(mono_region + (idx * 160));
		push_queue(7001, 1, 0, (Uint32)(mono_pkt));
	}

	/*****************************************************************
	 * Program a hi-pri accumulation channel for queue 712.
	 */
	cmd.command = 0x81; //enable
	cmd.channel = 8; //will trigger qmss_intr1_8 to core 0
	cmd.queue_mask = 0; //not used in single mode
	cmd.list_address = (uint32_t)hostList; //address of ping buffer
	cmd.max_entries = 17; //list can hold up to 16 (max-1)
	cmd.qm_index = 712; //que to monitor for channel 8
	cmd.cfg_multi_q = 0; //0=single queue mode
	cmd.cfg_list_mode = 1; //1=list count in first entry
	cmd.cfg_list_size = 0; //0="D" Reg
	cmd.cfg_int_delay = 1; //1=delay since last interrupt (pacing mode)
	cmd.timer_count = 1; //number of timer ticks to delay interrupt
	program_accumulator(1, &cmd);
	/*****************************************************************
	 * Program a lo-pri accumulation channel for queue 32.
	 */
	cmd.command = 0x81; //enable
	cmd.channel = 1; //will trigger qmss_intr0_1 to all cores
	cmd.queue_mask = 0x00000001; //look only at queue 32 for this example
	cmd.list_address = (uint32_t)monoList; //address of ping buffer
	cmd.max_entries = 17; //list can hold up to 16 (max-1)
	cmd.qm_index = 32; //first que to monitor for this channel
	cmd.cfg_multi_q = 1; //1=multi queue mode
	cmd.cfg_list_mode = 0; //0=NULL terminated list
	cmd.cfg_list_size = 0; //0="D" Reg
	cmd.cfg_int_delay = 1; //1=delay since last interrupt (pacing mode)
	cmd.timer_count = 1; //number of timer ticks to delay interrupt
	program_accumulator(0, &cmd);
	/* Clear the Accumulator lists. */
	memset(hostList, 0, 34 * 4);
	memset(monoList, 0, 34 * 4);





	/* Program the logical queue managers for QMSS PKTDMA: */
	qm_map[0] = 0;
	qm_map[1] = 5000;
	qm_map[2] = 7000;
	qm_map[3] = 0xffff; /* unused */
	config_pktdma_qm(QMSS_PKTDMA_GBL_CFG_REGION, qm_map);

	/* Configure Tx channel 0 */
	config_tx_sched(QMSS_PKTDMA_TX_SCHD_REGION, 0, 0); //high priority
	/* Configure Tx channel 1 */
	config_tx_sched(QMSS_PKTDMA_TX_SCHD_REGION, 1, 0); //high priority

	/* Configure Rx flow 0 for channel 0 (Host Descriptors to Hi Pri Acc.) */
	config_rx_flow(QMSS_PKTDMA_RX_FLOW_REGION, 0,
	0x040002c8, 0, 0, 0x20002000, 0x20002000, 0, 0, 0);
	/* Configure Rx flow 1 for channel 1 (Mono Descriptors to Lo Pri Acc.) */
	config_rx_flow(QMSS_PKTDMA_RX_FLOW_REGION, 1,
	0x080c0020, 0, 0, 0x20012001, 0, 0, 0, 0);

	/* Enable the Tx and Rx channels */
	enable_tx_chan(QMSS_PKTDMA_TX_CHAN_REGION, 0, 0x80000000);
	enable_tx_chan(QMSS_PKTDMA_TX_CHAN_REGION, 1, 0x80000000);
	enable_rx_chan(QMSS_PKTDMA_RX_CHAN_REGION, 0, 0x80000000);
	enable_rx_chan(QMSS_PKTDMA_RX_CHAN_REGION, 1, 0x80000000);


	//merwesh el ggggg fash555555555555555555555555555555555555555555
	return 0;

}
