/**
 * ============================================================================
 * Project: Huffman Coding-Based File Compression System
 * Course:  Computer Engineering — Data Structures and Algorithms
 * Student: Shalom Dabre
 * Roll No: 11070
 * 
 * File:    src/file_handler.c
 * Purpose: Complete implementation of binary file compression, decompression,
 *          header serialization, integrity checks, and file comparisons.
 * ============================================================================
 */

#include "file_handler.h"
#include "bit_io.h"
#include "crc32.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHUNK_BUFFER_SIZE 65536

const char* huffman_status_string(HuffmanStatus status) {
    switch (status) {
        case HUFF_SUCCESS:
            return "Operation completed successfully.";
        case HUFF_ERR_FILE_NOT_FOUND:
            return "File not found or cannot be opened.";
        case HUFF_ERR_FILE_READ:
            return "Error reading from file.";
        case HUFF_ERR_FILE_WRITE:
            return "Error writing to output file.";
        case HUFF_ERR_MEMORY:
            return "Memory allocation failure.";
        case HUFF_ERR_INVALID_MAGIC:
            return "Invalid file signature. File is not a valid HUF1 compressed file.";
        case HUFF_ERR_UNSUPPORTED_VERSION:
            return "Unsupported compression format version.";
        case HUFF_ERR_CORRUPTED_HEADER:
            return "Corrupted header metadata (invalid frequencies or symbol counts).";
        case HUFF_ERR_TRUNCATED_FILE:
            return "Compressed file is truncated or abruptly ended.";
        case HUFF_ERR_CRC_MISMATCH:
            return "Data integrity check failed: CRC-32 checksum mismatch.";
        case HUFF_ERR_FILE_EXISTS:
            return "Target output file already exists. Overwrite not permitted.";
        default:
            return "Unknown error occurred.";
    }
}

int file_exists(const char *path) {
    if (!path || path[0] == '\0') {
        return 0;
    }
    FILE *f = fopen(path, "rb");
    if (f) {
        fclose(f);
        return 1;
    }
    return 0;
}

int get_file_size(const char *path, uint64_t *size) {
    if (!path || !size) {
        return 0;
    }
    FILE *f = fopen(path, "rb");
    if (!f) {
        return 0;
    }
    if (fseek(f, 0L, SEEK_END) != 0) {
        fclose(f);
        return 0;
    }
    long sz = ftell(f);
    if (sz < 0) {
        fclose(f);
        return 0;
    }
    *size = (uint64_t)sz;
    fclose(f);
    return 1;
}

HuffmanStatus analyze_file_frequencies(const char *file_path, uint64_t frequencies[256],
                                       uint64_t *file_size) {
    if (!file_path || !frequencies) {
        return HUFF_ERR_FILE_NOT_FOUND;
    }

    memset(frequencies, 0, 256 * sizeof(uint64_t));
    if (file_size) {
        *file_size = 0;
    }

    FILE *f = fopen(file_path, "rb");
    if (!f) {
        return HUFF_ERR_FILE_NOT_FOUND;
    }

    unsigned char *buffer = (unsigned char*)malloc(CHUNK_BUFFER_SIZE);
    if (!buffer) {
        fclose(f);
        return HUFF_ERR_MEMORY;
    }

    uint64_t total = 0;
    size_t bytes_read = 0;
    while ((bytes_read = fread(buffer, 1, CHUNK_BUFFER_SIZE, f)) > 0) {
        count_frequencies(buffer, bytes_read, frequencies);
        total += bytes_read;
    }

    free(buffer);
    fclose(f);

    if (file_size) {
        *file_size = total;
    }
    return HUFF_SUCCESS;
}

HuffmanStatus read_compressed_header(const char *file_path, HuffmanHeader *header,
                                     uint64_t frequencies[256]) {
    if (!file_path || !header) {
        return HUFF_ERR_FILE_NOT_FOUND;
    }

    FILE *f = fopen(file_path, "rb");
    if (!f) {
        return HUFF_ERR_FILE_NOT_FOUND;
    }

    /* Read fixed header */
    if (fread(header, sizeof(HuffmanHeader), 1, f) != 1) {
        fclose(f);
        return HUFF_ERR_TRUNCATED_FILE;
    }

    /* Validate magic signature */
    if (header->magic[0] != HUFF_MAGIC_BYTE0 ||
        header->magic[1] != HUFF_MAGIC_BYTE1 ||
        header->magic[2] != HUFF_MAGIC_BYTE2 ||
        header->magic[3] != HUFF_MAGIC_BYTE3) {
        fclose(f);
        return HUFF_ERR_INVALID_MAGIC;
    }

    if (header->version != HUFF_VERSION) {
        fclose(f);
        return HUFF_ERR_UNSUPPORTED_VERSION;
    }

    if (header->distinct_count > 256) {
        fclose(f);
        return HUFF_ERR_CORRUPTED_HEADER;
    }

    if (frequencies) {
        memset(frequencies, 0, 256 * sizeof(uint64_t));
    }

    /* Read frequency table entries */
    uint64_t sum_freq = 0;
    for (uint16_t i = 0; i < header->distinct_count; i++) {
        uint8_t byte_val = 0;
        uint64_t freq = 0;
        if (fread(&byte_val, 1, 1, f) != 1 || fread(&freq, sizeof(uint64_t), 1, f) != 1) {
            fclose(f);
            return HUFF_ERR_TRUNCATED_FILE;
        }
        if (frequencies) {
            frequencies[byte_val] = freq;
        }
        sum_freq += freq;
    }

    if (header->original_size > 0 && sum_freq != header->original_size) {
        fclose(f);
        return HUFF_ERR_CORRUPTED_HEADER;
    }

    fclose(f);
    return HUFF_SUCCESS;
}

HuffmanStatus compress_file(const char *input_path, const char *output_path,
                            CompressionStats *stats, int allow_overwrite) {
    if (!input_path || !output_path) {
        return HUFF_ERR_FILE_NOT_FOUND;
    }

    if (!allow_overwrite && file_exists(output_path)) {
        return HUFF_ERR_FILE_EXISTS;
    }

    FILE *in_file = fopen(input_path, "rb");
    if (!in_file) {
        return HUFF_ERR_FILE_NOT_FOUND;
    }

    unsigned char *buffer = (unsigned char*)malloc(CHUNK_BUFFER_SIZE);
    if (!buffer) {
        fclose(in_file);
        return HUFF_ERR_MEMORY;
    }

    /* Pass 1: Compute frequencies and CRC-32 of original file */
    uint64_t frequencies[256];
    memset(frequencies, 0, sizeof(frequencies));
    uint32_t orig_crc = crc32_init();
    uint64_t original_size = 0;
    size_t bytes_read = 0;

    while ((bytes_read = fread(buffer, 1, CHUNK_BUFFER_SIZE, in_file)) > 0) {
        count_frequencies(buffer, bytes_read, frequencies);
        orig_crc = crc32_update(orig_crc, buffer, bytes_read);
        original_size += bytes_read;
    }
    orig_crc = crc32_final(orig_crc);

    /* Count distinct symbols */
    uint16_t distinct_count = 0;
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            distinct_count++;
        }
    }

    FILE *out_file = fopen(output_path, "wb");
    if (!out_file) {
        free(buffer);
        fclose(in_file);
        return HUFF_ERR_FILE_WRITE;
    }

    /* Construct header */
    HuffmanHeader header;
    header.magic[0] = HUFF_MAGIC_BYTE0;
    header.magic[1] = HUFF_MAGIC_BYTE1;
    header.magic[2] = HUFF_MAGIC_BYTE2;
    header.magic[3] = HUFF_MAGIC_BYTE3;
    header.version = HUFF_VERSION;
    header.flags = 0;
    header.distinct_count = distinct_count;
    header.original_size = original_size;
    header.original_crc32 = orig_crc;

    if (fwrite(&header, sizeof(HuffmanHeader), 1, out_file) != 1) {
        free(buffer);
        fclose(in_file);
        fclose(out_file);
        return HUFF_ERR_FILE_WRITE;
    }

    /* Write frequency table entries */
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            uint8_t byte_val = (uint8_t)i;
            uint64_t freq = frequencies[i];
            if (fwrite(&byte_val, 1, 1, out_file) != 1 ||
                fwrite(&freq, sizeof(uint64_t), 1, out_file) != 1) {
                free(buffer);
                fclose(in_file);
                fclose(out_file);
                return HUFF_ERR_FILE_WRITE;
            }
        }
    }

    uint64_t header_size = sizeof(HuffmanHeader) + (distinct_count * (1 + sizeof(uint64_t)));
    uint64_t payload_size = 0;

    /* If original file is non-empty, encode payload */
    if (original_size > 0) {
        HuffmanNode *root = build_huffman_tree(frequencies);
        if (!root) {
            free(buffer);
            fclose(in_file);
            fclose(out_file);
            return HUFF_ERR_MEMORY;
        }

        HuffmanCodeTable code_table;
        generate_huffman_codes(root, &code_table);

        BitWriter *writer = bit_writer_create(out_file);
        if (!writer) {
            free_huffman_tree(root);
            free(buffer);
            fclose(in_file);
            fclose(out_file);
            return HUFF_ERR_MEMORY;
        }

        /* Rewind input file for Pass 2: Encoding */
        rewind(in_file);

        if (distinct_count == 1) {
            /* Edge case: only one distinct symbol repeated */
            for (uint64_t i = 0; i < original_size; i++) {
                if (!bit_writer_write_bit(writer, 0)) {
                    bit_writer_destroy(writer);
                    free_huffman_tree(root);
                    free(buffer);
                    fclose(in_file);
                    fclose(out_file);
                    return HUFF_ERR_FILE_WRITE;
                }
            }
        } else {
            /* Standard case: stream encoding using code table */
            while ((bytes_read = fread(buffer, 1, CHUNK_BUFFER_SIZE, in_file)) > 0) {
                for (size_t i = 0; i < bytes_read; i++) {
                    const char *code = code_table.codes[buffer[i]].bits;
                    if (!bit_writer_write_code(writer, code)) {
                        bit_writer_destroy(writer);
                        free_huffman_tree(root);
                        free(buffer);
                        fclose(in_file);
                        fclose(out_file);
                        return HUFF_ERR_FILE_WRITE;
                    }
                }
            }
        }

        if (!bit_writer_flush(writer)) {
            bit_writer_destroy(writer);
            free_huffman_tree(root);
            free(buffer);
            fclose(in_file);
            fclose(out_file);
            return HUFF_ERR_FILE_WRITE;
        }

        payload_size = writer->total_bytes_written;
        bit_writer_destroy(writer);
        free_huffman_tree(root);
    }

    free(buffer);
    fclose(in_file);
    fclose(out_file);

    uint64_t total_compressed_size = header_size + payload_size;

    if (stats) {
        stats->original_size = original_size;
        stats->compressed_size = total_compressed_size;
        stats->header_size = header_size;
        stats->payload_size = payload_size;
        stats->compression_ratio = (total_compressed_size > 0)
            ? ((double)original_size / (double)total_compressed_size) : 0.0;
        stats->space_saved_percent = (original_size > 0)
            ? ((((double)original_size - (double)total_compressed_size) / (double)original_size) * 100.0) : 0.0;
        stats->original_crc = orig_crc;
        stats->decompressed_crc = orig_crc;
        stats->crc_matches = 1;
    }

    return HUFF_SUCCESS;
}

HuffmanStatus decompress_file(const char *input_path, const char *output_path,
                              CompressionStats *stats, int allow_overwrite) {
    if (!input_path || !output_path) {
        return HUFF_ERR_FILE_NOT_FOUND;
    }

    if (!allow_overwrite && file_exists(output_path)) {
        return HUFF_ERR_FILE_EXISTS;
    }

    FILE *in_file = fopen(input_path, "rb");
    if (!in_file) {
        return HUFF_ERR_FILE_NOT_FOUND;
    }

    /* Read fixed header */
    HuffmanHeader header;
    if (fread(&header, sizeof(HuffmanHeader), 1, in_file) != 1) {
        fclose(in_file);
        return HUFF_ERR_TRUNCATED_FILE;
    }

    /* Validate magic signature */
    if (header.magic[0] != HUFF_MAGIC_BYTE0 ||
        header.magic[1] != HUFF_MAGIC_BYTE1 ||
        header.magic[2] != HUFF_MAGIC_BYTE2 ||
        header.magic[3] != HUFF_MAGIC_BYTE3) {
        fclose(in_file);
        return HUFF_ERR_INVALID_MAGIC;
    }

    if (header.version != HUFF_VERSION) {
        fclose(in_file);
        return HUFF_ERR_UNSUPPORTED_VERSION;
    }

    if (header.distinct_count > 256) {
        fclose(in_file);
        return HUFF_ERR_CORRUPTED_HEADER;
    }

    /* Read frequency table */
    uint64_t frequencies[256];
    memset(frequencies, 0, sizeof(frequencies));
    uint64_t sum_freq = 0;
    uint8_t single_byte = 0;

    for (uint16_t i = 0; i < header.distinct_count; i++) {
        uint8_t byte_val = 0;
        uint64_t freq = 0;
        if (fread(&byte_val, 1, 1, in_file) != 1 ||
            fread(&freq, sizeof(uint64_t), 1, in_file) != 1) {
            fclose(in_file);
            return HUFF_ERR_TRUNCATED_FILE;
        }
        frequencies[byte_val] = freq;
        sum_freq += freq;
        single_byte = byte_val;
    }

    if (header.original_size > 0 && sum_freq != header.original_size) {
        fclose(in_file);
        return HUFF_ERR_CORRUPTED_HEADER;
    }

    if (fseek(in_file, 0L, SEEK_END) != 0) {
        fclose(in_file);
        return HUFF_ERR_FILE_READ;
    }
    long sz = ftell(in_file);
    uint64_t in_file_size = (sz >= 0) ? (uint64_t)sz : 0;

    uint64_t header_size = sizeof(HuffmanHeader) + (header.distinct_count * (1 + sizeof(uint64_t)));
    uint64_t payload_size = (in_file_size >= header_size) ? (in_file_size - header_size) : 0;

    /* Seek back to start of payload */
    if (fseek(in_file, (long)header_size, SEEK_SET) != 0) {
        fclose(in_file);
        return HUFF_ERR_FILE_READ;
    }

    FILE *out_file = fopen(output_path, "wb");
    if (!out_file) {
        fclose(in_file);
        return HUFF_ERR_FILE_WRITE;
    }

    unsigned char *chunk_buf = (unsigned char*)malloc(CHUNK_BUFFER_SIZE);
    if (!chunk_buf) {
        fclose(in_file);
        fclose(out_file);
        return HUFF_ERR_MEMORY;
    }

    uint32_t decomp_crc = crc32_init();

    /* Edge Case 1: Empty original file */
    if (header.original_size == 0) {
        decomp_crc = crc32_final(decomp_crc);
        free(chunk_buf);
        fclose(in_file);
        fclose(out_file);

        if (stats) {
            stats->original_size = 0;
            stats->compressed_size = in_file_size;
            stats->header_size = header_size;
            stats->payload_size = payload_size;
            stats->compression_ratio = 0.0;
            stats->space_saved_percent = 0.0;
            stats->original_crc = header.original_crc32;
            stats->decompressed_crc = decomp_crc;
            stats->crc_matches = (decomp_crc == header.original_crc32);
        }
        return HUFF_SUCCESS;
    }

    /* Edge Case 2: Exactly one distinct character */
    if (header.distinct_count == 1) {
        memset(chunk_buf, single_byte, CHUNK_BUFFER_SIZE);
        uint64_t remaining = header.original_size;
        while (remaining > 0) {
            size_t to_write = (remaining > CHUNK_BUFFER_SIZE) ? CHUNK_BUFFER_SIZE : (size_t)remaining;
            if (fwrite(chunk_buf, 1, to_write, out_file) != to_write) {
                free(chunk_buf);
                fclose(in_file);
                fclose(out_file);
                return HUFF_ERR_FILE_WRITE;
            }
            decomp_crc = crc32_update(decomp_crc, chunk_buf, to_write);
            remaining -= to_write;
        }
        decomp_crc = crc32_final(decomp_crc);
        free(chunk_buf);
        fclose(in_file);
        fclose(out_file);

        if (decomp_crc != header.original_crc32) {
            return HUFF_ERR_CRC_MISMATCH;
        }

        if (stats) {
            stats->original_size = header.original_size;
            stats->compressed_size = in_file_size;
            stats->header_size = header_size;
            stats->payload_size = payload_size;
            stats->compression_ratio = (in_file_size > 0) ? ((double)header.original_size / (double)in_file_size) : 0.0;
            stats->space_saved_percent = (header.original_size > 0)
                ? ((((double)header.original_size - (double)in_file_size) / (double)header.original_size) * 100.0) : 0.0;
            stats->original_crc = header.original_crc32;
            stats->decompressed_crc = decomp_crc;
            stats->crc_matches = 1;
        }
        return HUFF_SUCCESS;
    }

    /* Standard Case: Rebuild tree and decode bitstream */
    HuffmanNode *root = build_huffman_tree(frequencies);
    if (!root) {
        free(chunk_buf);
        fclose(in_file);
        fclose(out_file);
        return HUFF_ERR_CORRUPTED_HEADER;
    }

    BitReader *reader = bit_reader_create(in_file);
    if (!reader) {
        free_huffman_tree(root);
        free(chunk_buf);
        fclose(in_file);
        fclose(out_file);
        return HUFF_ERR_MEMORY;
    }

    uint64_t decoded_bytes = 0;
    size_t chunk_pos = 0;
    HuffmanNode *current = root;

    while (decoded_bytes < header.original_size) {
        int bit = bit_reader_read_bit(reader);
        if (bit < 0) {
            /* Unexpected EOF */
            bit_reader_destroy(reader);
            free_huffman_tree(root);
            free(chunk_buf);
            fclose(in_file);
            fclose(out_file);
            return HUFF_ERR_TRUNCATED_FILE;
        }

        current = (bit == 0) ? current->left : current->right;
        if (!current) {
            bit_reader_destroy(reader);
            free_huffman_tree(root);
            free(chunk_buf);
            fclose(in_file);
            fclose(out_file);
            return HUFF_ERR_CORRUPTED_HEADER;
        }

        if (current->is_leaf) {
            chunk_buf[chunk_pos++] = current->data;
            decoded_bytes++;

            if (chunk_pos == CHUNK_BUFFER_SIZE) {
                if (fwrite(chunk_buf, 1, chunk_pos, out_file) != chunk_pos) {
                    bit_reader_destroy(reader);
                    free_huffman_tree(root);
                    free(chunk_buf);
                    fclose(in_file);
                    fclose(out_file);
                    return HUFF_ERR_FILE_WRITE;
                }
                decomp_crc = crc32_update(decomp_crc, chunk_buf, chunk_pos);
                chunk_pos = 0;
            }
            current = root; /* Reset back to root for the next symbol */
        }
    }

    if (chunk_pos > 0) {
        if (fwrite(chunk_buf, 1, chunk_pos, out_file) != chunk_pos) {
            bit_reader_destroy(reader);
            free_huffman_tree(root);
            free(chunk_buf);
            fclose(in_file);
            fclose(out_file);
            return HUFF_ERR_FILE_WRITE;
        }
        decomp_crc = crc32_update(decomp_crc, chunk_buf, chunk_pos);
    }

    decomp_crc = crc32_final(decomp_crc);

    bit_reader_destroy(reader);
    free_huffman_tree(root);
    free(chunk_buf);
    fclose(in_file);
    fclose(out_file);

    /* Verify data integrity */
    if (decomp_crc != header.original_crc32) {
        return HUFF_ERR_CRC_MISMATCH;
    }

    if (stats) {
        stats->original_size = header.original_size;
        stats->compressed_size = in_file_size;
        stats->header_size = header_size;
        stats->payload_size = payload_size;
        stats->compression_ratio = (in_file_size > 0) ? ((double)header.original_size / (double)in_file_size) : 0.0;
        stats->space_saved_percent = (header.original_size > 0)
            ? ((((double)header.original_size - (double)in_file_size) / (double)header.original_size) * 100.0) : 0.0;
        stats->original_crc = header.original_crc32;
        stats->decompressed_crc = decomp_crc;
        stats->crc_matches = 1;
    }

    return HUFF_SUCCESS;
}

int compare_files_exact(const char *path1, const char *path2, uint64_t *diff_offset) {
    if (!path1 || !path2) {
        return -1;
    }

    FILE *f1 = fopen(path1, "rb");
    if (!f1) return -1;
    FILE *f2 = fopen(path2, "rb");
    if (!f2) {
        fclose(f1);
        return -1;
    }

    unsigned char *buf1 = (unsigned char*)malloc(CHUNK_BUFFER_SIZE);
    unsigned char *buf2 = (unsigned char*)malloc(CHUNK_BUFFER_SIZE);
    if (!buf1 || !buf2) {
        if (buf1) free(buf1);
        if (buf2) free(buf2);
        fclose(f1);
        fclose(f2);
        return -1;
    }

    uint64_t offset = 0;
    int identical = 1;

    while (1) {
        size_t r1 = fread(buf1, 1, CHUNK_BUFFER_SIZE, f1);
        size_t r2 = fread(buf2, 1, CHUNK_BUFFER_SIZE, f2);

        size_t min_r = (r1 < r2) ? r1 : r2;
        for (size_t i = 0; i < min_r; i++) {
            if (buf1[i] != buf2[i]) {
                if (diff_offset) *diff_offset = offset + i;
                identical = 0;
                break;
            }
        }

        if (!identical) break;

        if (r1 != r2) {
            /* Files have different lengths */
            if (diff_offset) *diff_offset = offset + min_r;
            identical = 0;
            break;
        }

        if (r1 == 0) {
            /* Both files reached EOF simultaneously and matched completely */
            break;
        }

        offset += r1;
    }

    free(buf1);
    free(buf2);
    fclose(f1);
    fclose(f2);
    return identical;
}

void display_compression_stats(const CompressionStats *stats) {
    if (!stats) return;

    printf("\n=======================================================\n");
    printf("               COMPRESSION STATISTICS                  \n");
    printf("=======================================================\n");
    printf("  Original File Size    : %llu bytes\n", (unsigned long long)stats->original_size);
    printf("  Compressed File Size  : %llu bytes\n", (unsigned long long)stats->compressed_size);
    printf("  - Metadata/Header     : %llu bytes\n", (unsigned long long)stats->header_size);
    printf("  - Encoded Payload     : %llu bytes\n", (unsigned long long)stats->payload_size);
    printf("-------------------------------------------------------\n");
    if (stats->compressed_size > 0) {
        printf("  Compression Ratio     : %.3f:1\n", stats->compression_ratio);
    } else {
        printf("  Compression Ratio     : N/A\n");
    }
    printf("  Space Saved           : %.2f%%\n", stats->space_saved_percent);
    printf("  Original CRC-32       : 0x%08X\n", stats->original_crc);
    printf("  Restored CRC-32       : 0x%08X\n", stats->decompressed_crc);
    printf("  Integrity Check       : %s\n",
           stats->crc_matches ? "MATCHED (100%% Lossless Recovery)" : "MISMATCH (Corrupted!)");
    printf("=======================================================\n");

    if (stats->original_size > 0 && stats->compressed_size > stats->original_size) {
        printf("  * Note: The compressed file is larger than the original\n");
        printf("    due to header overhead (frequency table). This is normal\n");
        printf("    and expected for very small files or high-entropy inputs.\n");
        printf("=======================================================\n");
    }
    printf("\n");
}
