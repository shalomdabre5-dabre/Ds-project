/**
 * ============================================================================
 * Project: Huffman Coding-Based File Compression System
 * Course:  Computer Engineering — Data Structures and Algorithms
 * Student: Shalom Dabre
 * Roll No: 11070
 * 
 * File:    src/huffman.h
 * Purpose: Core Huffman Coding algorithm, Binary Tree construction,
 *          code generation, and frequency analysis.
 * ============================================================================
 */

#ifndef HUFFMAN_H
#define HUFFMAN_H

#include "min_heap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Maximum depth of Huffman tree (256 distinct bytes) */
#define MAX_TREE_DEPTH 256

/**
 * Representation of a single Huffman variable-length bit code.
 */
typedef struct {
    char bits[MAX_TREE_DEPTH + 1]; /* Null-terminated string of '0' and '1' chars */
    size_t length;                 /* Length of the code in bits */
} HuffmanCode;

/**
 * Lookup table mapping each byte value (0-255) to its Huffman code.
 */
typedef struct {
    HuffmanCode codes[256];
    int exists[256];               /* 1 if byte exists in input, 0 otherwise */
    size_t distinct_count;         /* Number of distinct characters */
} HuffmanCodeTable;

/**
 * Counts the frequency of each byte value (0-255) in a buffer.
 * @param data Pointer to input data buffer.
 * @param size Number of bytes in the buffer.
 * @param frequencies Array of 256 uint64_t counters to populate.
 */
void count_frequencies(const unsigned char *data, size_t size, uint64_t frequencies[256]);

/**
 * Creates a new leaf node representing a specific byte symbol.
 * @param data The byte symbol (0-255).
 * @param freq Frequency count of the symbol.
 * @param order Sequence order for deterministic tie-breaking.
 * @return Pointer to newly allocated HuffmanNode.
 */
HuffmanNode* create_leaf_node(unsigned char data, uint64_t freq, unsigned int order);

/**
 * Creates a new internal node combining two child nodes.
 * @param left Pointer to left child (bit 0).
 * @param right Pointer to right child (bit 1).
 * @param order Sequence order for deterministic tie-breaking.
 * @return Pointer to newly allocated HuffmanNode.
 */
HuffmanNode* create_internal_node(HuffmanNode *left, HuffmanNode *right, unsigned int order);

/**
 * Frees all memory associated with a Huffman tree using post-order traversal.
 * @param root Pointer to root of the tree to free.
 */
void free_huffman_tree(HuffmanNode *root);

/**
 * Builds a Huffman tree from an array of 256 character frequencies
 * using a manually implemented Min-Heap priority queue.
 * @param frequencies Array of 256 character frequencies.
 * @return Pointer to the root of the constructed Huffman tree, or NULL if all frequencies are 0.
 */
HuffmanNode* build_huffman_tree(const uint64_t frequencies[256]);

/**
 * Traverses the Huffman tree to generate binary prefix codes for all symbols.
 * Left branch represents '0', right branch represents '1'.
 * Handles single-symbol edge cases gracefully.
 * @param root Pointer to root of the Huffman tree.
 * @param table Pointer to HuffmanCodeTable to populate.
 */
void generate_huffman_codes(const HuffmanNode *root, HuffmanCodeTable *table);

/**
 * Helper function to format a byte value into human-readable representation
 * (e.g., 'A', '\n', '\t', ' ', or '0x00').
 * @param byte_val The byte value (0-255).
 * @param out_str Output buffer (minimum 16 bytes).
 */
void format_byte_symbol(unsigned char byte_val, char *out_str);

/**
 * Displays the character frequency table in a clean, tabular format.
 * @param frequencies Array of 256 frequencies.
 * @param total_bytes Total size of original data for percentage calculation.
 */
void display_frequency_table(const uint64_t frequencies[256], uint64_t total_bytes);

/**
 * Displays the generated Huffman codes alongside their symbol and bit length.
 * @param table Pointer to HuffmanCodeTable.
 * @param frequencies Optional array of frequencies (can be NULL).
 */
void display_huffman_codes(const HuffmanCodeTable *table, const uint64_t frequencies[256]);

/**
 * Prints an ASCII visual representation of the Huffman tree.
 * @param root Pointer to tree root.
 * @param depth Current recursion depth (pass 0).
 * @param prefix Prefix string for formatting (pass "").
 */
void print_tree_structure(const HuffmanNode *root, int depth, const char *prefix);

#ifdef __cplusplus
}
#endif

#endif /* HUFFMAN_H */
