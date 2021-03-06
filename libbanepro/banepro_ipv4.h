#ifndef _BANEPRO_IPV4_H
#define _BANEPRO_IPV4_H

#include <avr/io.h>

#include "banepro_config.h"

#ifdef BANEPRO_ENABLE_IPV4

/* IP transport protocols (our own definitions) */
#define BANEPRO_IPV4_PROTO_UNKNOWN	0
#define BANEPRO_IPV4_PROTO_ICMP		1
#define BANEPRO_IPV4_PROTO_TCP		2
#define BANEPRO_IPV4_PROTO_UDP		3

/**
 * Compares two IPv4 addresses.
 *
 * @param addr_a	First address
 * @param addr_b	Second address
 * @return		1 if equal, 0 otherwise
 */
#define BANEPRO_IPV4_CMP(addr_a, addr_b) \
	((addr_a[0] == addr_b[0]) && (addr_a[1] == addr_b[1]) && \
	(addr_a[2] == addr_b[2] && addr_a[3] == addr_b[3]))

/* IPv4 address */
typedef uint8_t banepro_ipv4_addr [4];

/* Information structure for IPv4 */
struct banepro_ipv4_info {
	uint8_t header_len;		/* header length (bytes) */
	uint16_t total_len;		/* total length (header + data) */
	uint8_t ttl;			/* time to live */
	uint8_t proto;			/* transport protocol */
	uint16_t checksum;		/* Internet checksum */
	banepro_ipv4_addr src_addr;	/* source address */
	banepro_ipv4_addr dst_addr;	/* destination address */
	uint8_t* data_start;		/* data start pointer */
};

/**
 * Analyses an IPv4 packet and gives information about it.
 *
 * @param packet	IPv4 packet (pointing to the version)
 * @param info		Information structure to be filled
 */
void banepro_ipv4_analyse(uint8_t* packet, struct banepro_ipv4_info* info);

/**
 * Formats an IPv4 header using an information structure
 *
 * @param info		Information structure (set checksum to 0 for it to
 *			be autocomputed)
 * @param buf		Buffer where to put formatted packet header (big enough)
 * @return		Formatted packet header length (0 if error)
 */
uint8_t banepro_ipv4_format_header(struct banepro_ipv4_info* info, uint8_t* buf);

#endif /* BANEPRO_ENABLE_IPV4 */

#endif /* _BANEPRO_IPV4_H */
