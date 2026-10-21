/**
 * ============================================================================
 * Project: Huffman Coding-Based File Compression System
 * Course:  Computer Engineering — Data Structures and Algorithms
 * Student: Shalom Dabre
 * Roll No: 11070
 * 
 * File:    src/crc32.h
 * Purpose: Header for standard IEEE 802.3 CRC-32 checksum calculation.
 *          Used for verifying uncompressed file integrity and detecting
 *          corruption during decompression.
 * ============================================================================
 */

#ifndef CRC32_H
#define CRC32_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initializes the CRC-32 lookup table (called automatically if needed).
 */
void crc32_init_table(void);

/**
 * Starts a new CRC-32 calculation.
 * @return Initial CRC state (0xFFFFFFFF).
 */
uint32_t crc32_init(void);

/**
 * Updates the CRC-32 calculation with a chunk of data.
 * @param crc Current CRC accumulator.
 * @param buf Pointer to data buffer.
 * @param len Length of data in bytes.
 * @return Updated CRC state.
 */
uint32_t crc32_update(uint32_t crc, const unsigned char *buf, size_t len);

/**
 * Finalizes the CRC-32 calculation.
 * @param crc CRC accumulator after processing all data.
 * @return Final 32-bit CRC checksum.
 */
uint32_t crc32_final(uint32_t crc);

/**
 * Convenience function to compute CRC-32 for an entire memory buffer in one call.
 * @param buf Pointer to data buffer.
 * @param len Length of data in bytes.
 * @return 32-bit CRC checksum.
 */
uint32_t calculate_crc32(const unsigned char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* CRC32_H */
