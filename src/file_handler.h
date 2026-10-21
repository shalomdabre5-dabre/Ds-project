/**
 * ============================================================================
 * Project: Huffman Coding-Based File Compression System
 * Course:  Computer Engineering — Data Structures and Algorithms
 * Student: Shalom Dabre
 * Roll No: 11070
 * 
 * File:    src/file_handler.h
 * Purpose: Binary file encoding, decoding, header management, and file statistics.
 * ============================================================================
 */

#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

#include "huffman.h"
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Magic 4-byte signature identifying our proprietary compressed format */
#define HUFF_MAGIC_BYTE0 'H'
#define HUFF_MAGIC_BYTE1 'U'
#define HUFF_MAGIC_BYTE2 'F'
#define HUFF_MAGIC_BYTE3 '1'

/** Current file format version */
#define HUFF_VERSION 1

/** Fixed header size in bytes (excluding variable frequency table entries) */
#define HUFF_FIXED_HEADER_SIZE 20

/**
 * Representation of the binary file header.
 */
#pragma pack(push, 1)
typedef struct {
    char magic[4];               /* "HUF1" */
    uint8_t version;             /* Format version (currently 1) */
    uint8_t flags;               /* Reserved flags (0x00) */
    uint16_t distinct_count;     /* Number of unique symbols (0 - 256) */
    uint64_t original_size;      /* Exact size of uncompressed file in bytes */
    uint32_t original_crc32;     /* IEEE 802.3 CRC-32 checksum of uncompressed data */
} HuffmanHeader;
#pragma pack(pop)

/**
 * Structure holding comprehensive compression statistics.
 */
typedef struct {
    uint64_t original_size;      /* Uncompressed file size in bytes */
    uint64_t compressed_size;    /* Total compressed file size in bytes */
    uint64_t header_size;        /* Size of header + frequency table in bytes */
    uint64_t payload_size;       /* Size of bit-packed compressed data in bytes */
    double compression_ratio;    /* original_size / compressed_size */
    double space_saved_percent;  /* ((original - compressed) / original) * 100 */
    uint32_t original_crc;       /* CRC-32 of original file */
    uint32_t decompressed_crc;   /* CRC-32 of restored file */
    int crc_matches;             /* 1 if original and decompressed CRCs match */
} CompressionStats;

/**
 * Status and error codes returned by file handler operations.
 */
typedef enum {
    HUFF_SUCCESS = 0,
    HUFF_ERR_FILE_NOT_FOUND,
    HUFF_ERR_FILE_READ,
    HUFF_ERR_FILE_WRITE,
    HUFF_ERR_MEMORY,
    HUFF_ERR_INVALID_MAGIC,
    HUFF_ERR_UNSUPPORTED_VERSION,
    HUFF_ERR_CORRUPTED_HEADER,
    HUFF_ERR_TRUNCATED_FILE,
    HUFF_ERR_CRC_MISMATCH,
    HUFF_ERR_FILE_EXISTS
} HuffmanStatus;

/**
 * Converts a HuffmanStatus error code to a descriptive human-readable string.
 * @param status Error code.
 * @return Constant string description.
 */
const char* huffman_status_string(HuffmanStatus status);

/**
 * Compresses an input file to an output binary file using Huffman coding.
 * @param input_path Path to the source file to compress.
 * @param output_path Path to the target compressed file (.huf).
 * @param stats Pointer to CompressionStats structure to populate (optional, can be NULL).
 * @param allow_overwrite 1 to allow overwriting output file, 0 to fail if file exists.
 * @return HUFF_SUCCESS (0) on success, or a descriptive HuffmanStatus error code.
 */
HuffmanStatus compress_file(const char *input_path, const char *output_path,
                            CompressionStats *stats, int allow_overwrite);

/**
 * Decompresses a previously compressed binary file to its original content.
 * Reconstructs the exact Huffman tree from the file header.
 * @param input_path Path to the compressed file (.huf).
 * @param output_path Path to the restored output file.
 * @param stats Pointer to CompressionStats structure to populate (optional, can be NULL).
 * @param allow_overwrite 1 to allow overwriting output file, 0 to fail if file exists.
 * @return HUFF_SUCCESS (0) on success, or a descriptive HuffmanStatus error code.
 */
HuffmanStatus decompress_file(const char *input_path, const char *output_path,
                              CompressionStats *stats, int allow_overwrite);

/**
 * Analyzes an input file to compute character frequencies and size without compressing.
 * @param file_path Path to the file.
 * @param frequencies Array of 256 counters to populate.
 * @param file_size Pointer to uint64_t to receive file size.
 * @return HUFF_SUCCESS on success, or an error code.
 */
HuffmanStatus analyze_file_frequencies(const char *file_path, uint64_t frequencies[256],
                                       uint64_t *file_size);

/**
 * Reads header and metadata from a compressed file without decompressing the payload.
 * @param file_path Path to compressed file.
 * @param header Pointer to header struct to populate.
 * @param frequencies Array of 256 counters to receive symbol frequencies.
 * @return HUFF_SUCCESS on success, or an error code.
 */
HuffmanStatus read_compressed_header(const char *file_path, HuffmanHeader *header,
                                     uint64_t frequencies[256]);

/**
 * Performs a byte-by-byte comparison of two files.
 * @param path1 First file path.
 * @param path2 Second file path.
 * @param diff_offset If files differ, receives the byte offset of the first mismatch.
 * @return 1 if identical, 0 if different, -1 on I/O error.
 */
int compare_files_exact(const char *path1, const char *path2, uint64_t *diff_offset);

/**
 * Checks if a file exists on disk.
 * @param path Path to check.
 * @return 1 if file exists and is accessible, 0 otherwise.
 */
int file_exists(const char *path);

/**
 * Retrieves the exact file size in bytes.
 * @param path Path to check.
 * @param size Pointer to receive size.
 * @return 1 on success, 0 on failure.
 */
int get_file_size(const char *path, uint64_t *size);

/**
 * Prints formatted compression statistics to stdout.
 * @param stats Pointer to CompressionStats structure.
 */
void display_compression_stats(const CompressionStats *stats);

#ifdef __cplusplus
}
#endif

#endif /* FILE_HANDLER_H */
