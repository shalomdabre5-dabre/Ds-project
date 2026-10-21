/**
 * ============================================================================
 * Project: Huffman Coding-Based File Compression System
 * Course:  Computer Engineering — Data Structures and Algorithms
 * Student: Shalom Dabre
 * Roll No: 11070
 * 
 * File:    src/bit_io.c
 * Purpose: Bit-level input and output operations for reading/writing bitstreams.
 * ============================================================================
 */

#include "bit_io.h"
#include <stdlib.h>

BitWriter* bit_writer_create(FILE *file) {
    if (!file) {
        return NULL;
    }
    BitWriter *writer = (BitWriter*)malloc(sizeof(BitWriter));
    if (!writer) {
        return NULL;
    }
    writer->file = file;
    writer->buffer = 0;
    writer->bit_count = 0;
    writer->total_bytes_written = 0;
    return writer;
}

int bit_writer_write_bit(BitWriter *writer, int bit) {
    if (!writer || !writer->file) {
        return 0;
    }

    /* Shift buffer left and place incoming bit in the LSB position */
    writer->buffer = (uint8_t)((writer->buffer << 1) | (bit ? 1 : 0));
    writer->bit_count++;

    /* When 8 bits have accumulated, write the complete byte */
    if (writer->bit_count == 8) {
        if (fputc(writer->buffer, writer->file) == EOF) {
            return 0;
        }
        writer->total_bytes_written++;
        writer->buffer = 0;
        writer->bit_count = 0;
    }
    return 1;
}

int bit_writer_write_code(BitWriter *writer, const char *bit_str) {
    if (!writer || !bit_str) {
        return 0;
    }
    for (size_t i = 0; bit_str[i] != '\0'; i++) {
        int bit = (bit_str[i] == '1') ? 1 : 0;
        if (!bit_writer_write_bit(writer, bit)) {
            return 0;
        }
    }
    return 1;
}

int bit_writer_flush(BitWriter *writer) {
    if (!writer || !writer->file) {
        return 0;
    }
    if (writer->bit_count > 0) {
        /* Shift remaining bits to the most significant positions and pad with 0s */
        writer->buffer = (uint8_t)(writer->buffer << (8 - writer->bit_count));
        if (fputc(writer->buffer, writer->file) == EOF) {
            return 0;
        }
        writer->total_bytes_written++;
        writer->buffer = 0;
        writer->bit_count = 0;
    }
    return 1;
}

void bit_writer_destroy(BitWriter *writer) {
    if (writer) {
        free(writer);
    }
}

BitReader* bit_reader_create(FILE *file) {
    if (!file) {
        return NULL;
    }
    BitReader *reader = (BitReader*)malloc(sizeof(BitReader));
    if (!reader) {
        return NULL;
    }
    reader->file = file;
    reader->buffer = 0;
    reader->bit_count = 0;
    reader->total_bytes_read = 0;
    reader->eof_reached = 0;
    return reader;
}

int bit_reader_read_bit(BitReader *reader) {
    if (!reader || !reader->file) {
        return -1;
    }

    /* If all bits in the current byte have been consumed, read the next byte */
    if (reader->bit_count == 0) {
        int byte_read = fgetc(reader->file);
        if (byte_read == EOF) {
            reader->eof_reached = 1;
            return -1;
        }
        reader->buffer = (uint8_t)byte_read;
        reader->bit_count = 8;
        reader->total_bytes_read++;
    }

    /* Extract the most significant unread bit */
    int bit = (reader->buffer >> (reader->bit_count - 1)) & 1;
    reader->bit_count--;
    return bit;
}

void bit_reader_destroy(BitReader *reader) {
    if (reader) {
        free(reader);
    }
}
