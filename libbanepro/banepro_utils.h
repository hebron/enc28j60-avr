#ifndef _BANEPRO_UTILS_H
#define _BANEPRO_UTILS_H

#include <avr/io.h>

/**
 * Computes and returns the Internet checksum of a buffer.
 *
 * @param buf	Buffer
 * @param len	Buffer length (in bytes - must be even)
 */
uint16_t banepro_u_checksum(uint8_t* buf, uint16_t buf_len);

#endif /* _BANEPRO_UTILS_H */
