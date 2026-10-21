/**
 * ============================================================================
 * Project: Huffman Coding-Based File Compression System
 * Course:  Computer Engineering — Data Structures and Algorithms
 * Student: Shalom Dabre
 * Roll No: 11070
 * 
 * File:    src/huffman.c
 * Purpose: Huffman Tree construction, traversal, and code generation.
 * ============================================================================
 */

#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void count_frequencies(const unsigned char *data, size_t size, uint64_t frequencies[256]) {
    if (!data || !frequencies) {
        return;
    }
    for (size_t i = 0; i < size; i++) {
        frequencies[data[i]]++;
    }
}

HuffmanNode* create_leaf_node(unsigned char data, uint64_t freq, unsigned int order) {
    HuffmanNode *node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    if (!node) {
        return NULL;
    }
    node->data = data;
    node->freq = freq;
    node->is_leaf = 1;
    node->order = order;
    node->left = NULL;
    node->right = NULL;
    return node;
}

HuffmanNode* create_internal_node(HuffmanNode *left, HuffmanNode *right, unsigned int order) {
    HuffmanNode *node = (HuffmanNode*)malloc(sizeof(HuffmanNode));
    if (!node) {
        return NULL;
    }
    node->data = 0;
    node->freq = (left ? left->freq : 0) + (right ? right->freq : 0);
    node->is_leaf = 0;
    node->order = order;
    node->left = left;
    node->right = right;
    return node;
}

void free_huffman_tree(HuffmanNode *root) {
    if (!root) {
        return;
    }
    free_huffman_tree(root->left);
    free_huffman_tree(root->right);
    free(root);
}

HuffmanNode* build_huffman_tree(const uint64_t frequencies[256]) {
    if (!frequencies) {
        return NULL;
    }

    /* Count number of distinct characters present in the input */
    size_t distinct_count = 0;
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            distinct_count++;
        }
    }

    if (distinct_count == 0) {
        return NULL; /* Empty input file */
    }

    /* Edge Case: exactly one distinct byte */
    if (distinct_count == 1) {
        for (int i = 0; i < 256; i++) {
            if (frequencies[i] > 0) {
                HuffmanNode *leaf = create_leaf_node((unsigned char)i, frequencies[i], 0);
                HuffmanNode *root = create_internal_node(leaf, NULL, 1);
                return root;
            }
        }
    }

    /* Standard Case: multiple distinct symbols */
    MinHeap *heap = create_min_heap(distinct_count);
    if (!heap) {
        return NULL;
    }

    unsigned int order = 0;
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            HuffmanNode *node = create_leaf_node((unsigned char)i, frequencies[i], order++);
            if (!node || !insert_min_heap(heap, node)) {
                destroy_min_heap(heap);
                return NULL;
            }
        }
    }

    /* Iteratively combine the two nodes with the lowest frequencies */
    while (heap->size > 1) {
        HuffmanNode *left = extract_min(heap);
        HuffmanNode *right = extract_min(heap);

        HuffmanNode *parent = create_internal_node(left, right, order++);
        if (!parent || !insert_min_heap(heap, parent)) {
            if (left) free_huffman_tree(left);
            if (right) free_huffman_tree(right);
            destroy_min_heap(heap);
            return NULL;
        }
    }

    HuffmanNode *root = extract_min(heap);
    destroy_min_heap(heap);
    return root;
}

static void generate_codes_recursive(const HuffmanNode *node, char *buffer, int depth, HuffmanCodeTable *table) {
    if (!node || !table) {
        return;
    }

    /* Leaf node reached: record the code */
    if (node->is_leaf) {
        if (depth == 0) {
            /* Single node edge case */
            buffer[0] = '0';
            buffer[1] = '\0';
            depth = 1;
        } else {
            buffer[depth] = '\0';
        }
        unsigned char c = node->data;
        strncpy(table->codes[c].bits, buffer, sizeof(table->codes[c].bits) - 1);
        table->codes[c].bits[sizeof(table->codes[c].bits) - 1] = '\0';
        table->codes[c].length = (size_t)depth;
        table->exists[c] = 1;
        table->distinct_count++;
        return;
    }

    /* Traverse left (bit '0') */
    if (node->left) {
        buffer[depth] = '0';
        generate_codes_recursive(node->left, buffer, depth + 1, table);
    }

    /* Traverse right (bit '1') */
    if (node->right) {
        buffer[depth] = '1';
        generate_codes_recursive(node->right, buffer, depth + 1, table);
    }
}

void generate_huffman_codes(const HuffmanNode *root, HuffmanCodeTable *table) {
    if (!table) {
        return;
    }
    memset(table, 0, sizeof(HuffmanCodeTable));
    if (!root) {
        return;
    }

    char buffer[MAX_TREE_DEPTH + 1];
    generate_codes_recursive(root, buffer, 0, table);
}

void format_byte_symbol(unsigned char byte_val, char *out_str) {
    if (!out_str) return;

    if (byte_val == ' ') {
        snprintf(out_str, 16, "' ' (space)");
    } else if (byte_val == '\n') {
        snprintf(out_str, 16, "'\\n' (newline)");
    } else if (byte_val == '\t') {
        snprintf(out_str, 16, "'\\t' (tab)");
    } else if (byte_val == '\r') {
        snprintf(out_str, 16, "'\\r' (CR)");
    } else if (byte_val == '\0') {
        snprintf(out_str, 16, "'\\0' (null)");
    } else if (isprint(byte_val)) {
        snprintf(out_str, 16, "'%c'", byte_val);
    } else {
        snprintf(out_str, 16, "0x%02X", byte_val);
    }
}

void display_frequency_table(const uint64_t frequencies[256], uint64_t total_bytes) {
    if (!frequencies) {
        printf("No frequency data available.\n");
        return;
    }

    printf("\n+-----+--------+-----------------+---------------+------------+\n");
    printf("| No. | Byte   | Symbol          | Frequency     | Percentage |\n");
    printf("+-----+--------+-----------------+---------------+------------+\n");

    int count = 0;
    for (int i = 0; i < 256; i++) {
        if (frequencies[i] > 0) {
            count++;
            char sym[16];
            format_byte_symbol((unsigned char)i, sym);
            double pct = total_bytes > 0 ? ((double)frequencies[i] / (double)total_bytes) * 100.0 : 0.0;
            printf("| %-3d | 0x%02X   | %-15s | %-13llu | %9.2f%% |\n",
                   count, i, sym, (unsigned long long)frequencies[i], pct);
        }
    }

    if (count == 0) {
        printf("|              (File is completely empty: 0 bytes)             |\n");
    }

    printf("+-----+--------+-----------------+---------------+------------+\n");
    printf("Total distinct byte values: %d | Total byte count: %llu\n\n",
           count, (unsigned long long)total_bytes);
}

void display_huffman_codes(const HuffmanCodeTable *table, const uint64_t frequencies[256]) {
    if (!table) {
        printf("No code table available.\n");
        return;
    }

    printf("\n+-----+--------+-----------------+--------+-----------------------------------+\n");
    printf("| No. | Byte   | Symbol          | Bits   | Huffman Code                      |\n");
    printf("+-----+--------+-----------------+--------+-----------------------------------+\n");

    int count = 0;
    for (int i = 0; i < 256; i++) {
        if (table->exists[i]) {
            count++;
            char sym[16];
            format_byte_symbol((unsigned char)i, sym);
            printf("| %-3d | 0x%02X   | %-15s | %-6zu | %-33s |\n",
                   count, i, sym, table->codes[i].length, table->codes[i].bits);
        }
    }

    if (count == 0) {
        printf("|              (No Huffman codes generated: empty data)              |\n");
    }

    printf("+-----+--------+-----------------+--------+-----------------------------------+\n");
    printf("Total encoded symbols: %d\n\n", count);
    (void)frequencies; /* Reserved for theoretical entropy/weighted length displays */
}

void print_tree_structure(const HuffmanNode *root, int depth, const char *prefix) {
    if (!root) {
        return;
    }

    for (int i = 0; i < depth; i++) {
        printf("    ");
    }

    if (root->is_leaf) {
        char sym[16];
        format_byte_symbol(root->data, sym);
        printf("%s[LEAF] Symbol: %s (0x%02X), Freq: %llu\n",
               prefix, sym, root->data, (unsigned long long)root->freq);
    } else {
        printf("%s[INTERNAL] Combined Freq: %llu\n", prefix, (unsigned long long)root->freq);
        if (root->left) {
            print_tree_structure(root->left, depth + 1, "0-> ");
        }
        if (root->right) {
            print_tree_structure(root->right, depth + 1, "1-> ");
        }
    }
}
