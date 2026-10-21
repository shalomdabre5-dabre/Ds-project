/**
 * ============================================================================
 * Project: Huffman Coding-Based File Compression System
 * Course:  Computer Engineering — Data Structures and Algorithms
 * Student: Shalom Dabre
 * Roll No: 11070
 * 
 * File:    src/crc32.c
 * Purpose: Implementation of IEEE 802.3 32-bit Cyclic Redundancy Check (CRC-32).
 * ============================================================================
 */

#include "crc32.h"

/* Standard IEEE 802.3 CRC-32 polynomial: 0xEDB88320 in reversed bit notation */
#define CRC32_POLYNOMIAL 0xEDB88320UL

static uint32_t s_crc_table[256];
static int s_table_initialized = 0;

void crc32_init_table(void) {
    if (s_table_initialized) {
        return;
    }
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (int j = 0; j < 8; j++) {
            if (c & 1) {
                c = CRC32_POLYNOMIAL ^ (c >> 1);
            } else {
                c >>= 1;
            }
        }
        s_crc_table[i] = c;
    }
    s_table_initialized = 1;
}

uint32_t crc32_init(void) {
    if (!s_table_initialized) {
        crc32_init_table();
    }
    return 0xFFFFFFFFUL;
}

uint32_t crc32_update(uint32_t crc, const unsigned char *buf, size_t len) {
    if (!s_table_initialized) {
        crc32_init_table();
    }
    for (size_t i = 0; i < len; i++) {
        crc = s_crc_table[(crc ^ buf[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc;
}

uint32_t crc32_final(uint32_t crc) {
    return crc ^ 0xFFFFFFFFUL;
}

uint32_t calculate_crc32(const unsigned char *buf, size_t len) {
    uint32_t crc = crc32_init();
    crc = crc32_update(crc, buf, len);
    return crc32_final(crc);
}
