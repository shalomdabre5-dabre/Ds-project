/**
 * ============================================================================
 * Project: Huffman Coding-Based File Compression System
 * Course:  Computer Engineering — Data Structures and Algorithms
 * Student: Shalom Dabre
 * Roll No: 11070
 * 
 * File:    src/bit_io.h
 * Purpose: Bit-level stream reader and writer for real binary compression.
 *          Packs variable-length prefix codes into 8-bit bytes (MSB to LSB).
 * ============================================================================
 */

#ifndef BIT_IO_H
#define BIT_IO_H

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * BitWriter packs individual bits into bytes and writes them to a binary stream.
 */
typedef struct {
    FILE *file;                  /* Output file pointer */
    uint8_t buffer;              /* Byte buffer accumulating up to 8 bits */
    int bit_count;               /* Number of bits currently accumulated (0-7) */
    uint64_t total_bytes_written;/* Total bytes written to the stream */
} BitWriter;

/**
 * BitReader reads individual bits from a binary stream packed MSB to LSB.
 */
typedef struct {
    FILE *file;                  /* Input file pointer */
    uint8_t buffer;              /* Current byte being read */
    int bit_count;               /* Number of unread bits remaining in buffer (0-7) */
    uint64_t total_bytes_read;   /* Total bytes read from the stream */
    int eof_reached;             /* Flag indicating whether end-of-file was encountered */
} BitReader;

/**
 * Creates and initializes a BitWriter for the given open file.
 * @param file Open writable binary file pointer.
 * @return Pointer to BitWriter or NULL on error.
 */
BitWriter* bit_writer_create(FILE *file);

/**
 * Writes a single bit (0 or 1) into the bit stream.
 * @param writer Pointer to BitWriter.
 * @param bit The bit value (0 or 1; non-zero is treated as 1).
 * @return 1 on success, 0 on I/O error.
 */
int bit_writer_write_bit(BitWriter *writer, int bit);

/**
 * Writes a sequence of bits given as a null-terminated string of '0' and '1' chars.
 * @param writer Pointer to BitWriter.
 * @param bit_str C-string of '0' and '1' characters.
 * @return 1 on success, 0 on I/O error.
 */
int bit_writer_write_code(BitWriter *writer, const char *bit_str);

/**
 * Flushes any remaining bits in the buffer, padding with zeros to complete the byte.
 * @param writer Pointer to BitWriter.
 * @return 1 on success, 0 on error.
 */
int bit_writer_flush(BitWriter *writer);

/**
 * Destroys the BitWriter. Does NOT close the underlying file.
 * @param writer Pointer to BitWriter.
 */
void bit_writer_destroy(BitWriter *writer);

/**
 * Creates and initializes a BitReader for the given open file.
 * @param file Open readable binary file pointer.
 * @return Pointer to BitReader or NULL on error.
 */
BitReader* bit_reader_create(FILE *file);

/**
 * Reads a single bit from the bit stream (MSB to LSB).
 * @param reader Pointer to BitReader.
 * @return 0 or 1 on success, -1 on EOF or error.
 */
int bit_reader_read_bit(BitReader *reader);

/**
 * Destroys the BitReader. Does NOT close the underlying file.
 * @param reader Pointer to BitReader.
 */
void bit_reader_destroy(BitReader *reader);

#ifdef __cplusplus
}
#endif

#endif /* BIT_IO_H */
