#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "m16_rs232.h"
#include "enc28j60defs.h"
#include "enc28j60_driver.h"
#include "enc28j60_fw.h"
#include "enc28j60_fw_macros.h"
#include "banepro_utils.h"
#include "banepro_ethernet.h"
#include "banepro_arp.h"
#include "testcases.h"

void e28j60_spi_cs_on(void) {
	PORTB &= ~_BV(PB4);
}

void e28j60_spi_cs_off(void) {
	PORTB |= _BV(PB4);
}

uint8_t e28j60_spi_rw(const uint8_t c) {
	uint8_t rec;
	
	SPDR = c;
	while (!(SPSR & _BV(SPIF)));
	rec = SPDR;
	
	return rec;
}

#if 0
/**
 * Tests LED blinking (device communication okay).
 */
void testcase_led(void) {
	printf("configuring LEDs...\n");
	e28j60fw_write_phy_reg(E28J60_PHLCON, 0x0aa0);	
}

/**
 * Simple transmit - malformed packet.
 */
void testcase_simple_send(void) {
	uint8_t buf;
	uint16_t mamxfl = 1000;
	
	printf("initializing MAC (phase 1)...\n");
	e28j60fw_bitfield_clr(E28J60_MACON2, _BV(E28J60_MARST));
	
	printf("initializing MAC (phase 3)...\n");
	e28j60fw_bitfield_set(E28J60_MACON3, _BV(E28J60_PADCFG0) |
		_BV(E28J60_PADCFG1) | _BV(E28J60_TXCRCEN));
	
	printf("initializing MAC (phase 5)...\n");
	e28j60fw_write_ctrl_reg_8(E28J60_MAMXFLL, E28J60FW_LOW(mamxfl));
	e28j60fw_write_ctrl_reg_8(E28J60_MAMXFLH, E28J60FW_HIGH(mamxfl));
	
	printf("initializing MAC (phase 6)...\n");
	e28j60fw_write_ctrl_reg_8(E28J60_MABBIPG, 0x12);
	
	printf("initializing MAC (phase 7)...\n");
	e28j60fw_write_ctrl_reg_8(E28J60_MAIPGL, 0x12);
	
	printf("initializing MAC (phase 8)...\n");
	e28j60fw_write_ctrl_reg_8(E28J60_MAIPGH, 0x0c);
	
	printf("setting MAC address...\n");
	e28j60fw_write_ctrl_reg_8(E28J60_MAADR0, 0xaa);
	e28j60fw_write_ctrl_reg_8(E28J60_MAADR1, 0xbb);
	e28j60fw_write_ctrl_reg_8(E28J60_MAADR2, 0xcc);
	e28j60fw_write_ctrl_reg_8(E28J60_MAADR3, 0xdd);
	e28j60fw_write_ctrl_reg_8(E28J60_MAADR4, 0xee);
	e28j60fw_write_ctrl_reg_8(E28J60_MAADR5, 0xff);
	
	printf("programming ETXST...\n");
	e28j60fw_write_ctrl_reg_8(E28J60_ETXSTL, 0);
	e28j60fw_write_ctrl_reg_8(E28J60_ETXSTH, 0);
	
	printf("writing PPCB...\n");
	e28j60_wbm(0x00);
	
	printf("writing to send buffer...\n");
	e28j60_wbm('p');
	e28j60_wbm('h');
	e28j60_wbm('i');
	e28j60_wbm('l');
	e28j60_wbm('z');
	
	printf("programming ETXND...\n");
	e28j60fw_write_ctrl_reg_8(E28J60_ETXNDL, 5);
	e28j60fw_write_ctrl_reg_8(E28J60_ETXNDH, 0);
	
	printf("starting transmission loop...\n");
	for (;;) {
		e28j60fw_bitfield_clr(E28J60_EIR, _BV(E28J60_TXIF));
		e28j60fw_bitfield_clr(E28J60_EIE, _BV(E28J60_TXIE));
		e28j60fw_bitfield_set(E28J60_ECON1, _BV(E28J60_TXRTS));
	
		for (;;) {
			_delay_ms(1.0);
			buf = e28j60fw_read_ctrl_reg_8(E28J60_ECON1);
			if (!(buf & _BV(E28J60_TXRTS))) {
				break;
			}		
		}
		printf("sent!\n");
	
		_delay_ms(1000.0);
	}
}
#endif

/**
 * This one will reply to ARP requests.
 */
void testcase_arp_reply(void) {
	e28j60fw_mac_address_t mac;
	struct e28j60fw_rx_info rx_info;
	struct e28j60fw_tx_info tx_info;
	uint16_t i;
	uint8_t packet [64];
	uint8_t buf;
	struct banepro_eth_info eth_info;
	struct banepro_arp_info arp_info;
	uint8_t eth_head_len, arp_len;
	
	mac[0] = 0xaa;
	mac[1] = 0xbb;
	mac[2] = 0xcc;
	mac[3] = 0xdd;
	mac[4] = 0xee;
	mac[5] = 0xff;
	e28j60fw_easy_init(mac, 6144, 1518);
	tx_info.rx_size = 6144;
	
	printf("starting reception loop...\n");
	for (;;) {
		while (!e28j60fw_has_new_packet()) {
			_delay_ms(1.0);
		}
		e28j60fw_handle_rx_beg(&rx_info);
				
		/* read first 60 bytes of received packet (should be enough
		for ARP */
		if (rx_info.size >= 60 && rx_info.size <= 1518) {
			for (i = 0; i < 60; ++i) {
				packet[i] = e28j60fw_read_byte();
			}
			banepro_eth_analyse(packet, &eth_info);
			switch (eth_info.net_type) {
				case BANEPRO_ETH_TYPE_ARP:
				banepro_arp_analyse(eth_info.data_start, &arp_info);
				if (arp_info.is_valid && arp_info.op == BANEPRO_ARP_OP_REQUEST) {
					if (arp_info.tpa[0] == 132 && arp_info.tpa[1] == 207 &&
					arp_info.tpa[2] == 89 && arp_info.tpa[3] == 181) {
	 					banepro_arp_inplace_reply(&arp_info, mac);
						memcpy(eth_info.dst_addr, eth_info.src_addr, 6);
						memcpy(eth_info.src_addr, mac, 6);
						eth_head_len = banepro_eth_format_header(&eth_info, packet);
						arp_len = banepro_arp_format_packet(&arp_info, packet + eth_head_len);
					
						e28j60fw_prepare_tx(&tx_info);
						for (i = 0; i < (eth_head_len + arp_len); ++i) {
							e28j60fw_write_byte(packet[i]);
						}
						tx_info.len = eth_head_len + arp_len;
						e28j60fw_transmit(&tx_info);
						while (!e28j60fw_all_packets_sent()) {
							_delay_ms(1.0);
						}
						printf("ARP reply to [%d.%d.%d.%d]!\n",
							arp_info.tpa[0], arp_info.tpa[1],
							arp_info.tpa[2], arp_info.tpa[3]);
					}
				}
				break;
				
				default:
				break;
			} 
		}
		
		e28j60fw_handle_rx_end(&rx_info);
	}
}
