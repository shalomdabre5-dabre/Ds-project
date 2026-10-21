/**
 * ============================================================================
 * Project: Huffman Coding-Based File Compression System
 * Course:  Computer Engineering — Data Structures and Algorithms
 * Student: Shalom Dabre
 * Roll No: 11070
 * 
 * File:    src/min_heap.h
 * Purpose: Header for manual Min-Heap Priority Queue and Huffman Node definition.
 * ============================================================================
 */

#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Structure representing a node in the Huffman Tree.
 * Each node is either:
 *  - A Leaf Node: holds a distinct input byte value and its frequency.
 *  - An Internal Node: holds the sum of its children's frequencies.
 */
typedef struct HuffmanNode {
    unsigned char data;          /* Byte value (valid for leaf nodes) */
    uint64_t freq;               /* Frequency of byte or combined subtree frequency */
    int is_leaf;                 /* Flag: 1 for leaf node, 0 for internal node */
    unsigned int order;          /* Deterministic sequence order / tie-breaker */
    struct HuffmanNode *left;    /* Left child pointer (represents bit '0') */
    struct HuffmanNode *right;   /* Right child pointer (represents bit '1') */
} HuffmanNode;

/**
 * Structure representing the Min-Heap Priority Queue.
 * Dynamically allocated array of pointers to HuffmanNode.
 */
typedef struct MinHeap {
    size_t size;                 /* Current number of elements in the heap */
    size_t capacity;             /* Maximum allocated capacity */
    HuffmanNode **array;         /* Dynamic array of node pointers */
} MinHeap;

/**
 * Allocates and initializes an empty Min-Heap with the given initial capacity.
 * @param capacity Maximum number of nodes the heap can initially hold.
 * @return Pointer to newly created MinHeap, or NULL on allocation failure.
 */
MinHeap* create_min_heap(size_t capacity);

/**
 * Deallocates the min-heap structure and its pointer array.
 * Note: Does not free the individual HuffmanNode elements pointed to by the array.
 * @param heap Pointer to the MinHeap to destroy.
 */
void destroy_min_heap(MinHeap *heap);

/**
 * Swaps two HuffmanNode pointers.
 * @param a Pointer to first node pointer.
 * @param b Pointer to second node pointer.
 */
void swap_nodes(HuffmanNode **a, HuffmanNode **b);

/**
 * Compares two Huffman nodes to maintain the min-heap invariant.
 * Compares by frequency first; if equal, tie-breaks deterministically by sequence order.
 * @param a First node.
 * @param b Second node.
 * @return Negative if a < b, positive if a > b, 0 if equal.
 */
int compare_nodes(const HuffmanNode *a, const HuffmanNode *b);

/**
 * Restores the min-heap property upward from the given index to the root.
 * @param heap Pointer to the MinHeap.
 * @param index Index of the element to percolate up.
 */
void heapify_up(MinHeap *heap, size_t index);

/**
 * Restores the min-heap property downward from the given index to the leaves.
 * @param heap Pointer to the MinHeap.
 * @param index Index of the element to percolate down.
 */
void heapify_down(MinHeap *heap, size_t index);

/**
 * Inserts a new HuffmanNode into the min-heap.
 * Automatically doubles heap capacity if full.
 * @param heap Pointer to the MinHeap.
 * @param node Pointer to the HuffmanNode to insert.
 * @return 1 on success, 0 on memory allocation failure.
 */
int insert_min_heap(MinHeap *heap, HuffmanNode *node);

/**
 * Extracts and returns the minimum node (root) from the min-heap.
 * Rebalances the heap using heapify_down.
 * @param heap Pointer to the MinHeap.
 * @return Pointer to the minimum HuffmanNode, or NULL if heap is empty.
 */
HuffmanNode* extract_min(MinHeap *heap);

/**
 * Checks if the min-heap currently contains exactly one element.
 * @param heap Pointer to the MinHeap.
 * @return 1 if size is 1, 0 otherwise.
 */
int is_heap_size_one(const MinHeap *heap);

/**
 * Checks if the min-heap is currently empty.
 * @param heap Pointer to the MinHeap.
 * @return 1 if empty, 0 otherwise.
 */
int is_heap_empty(const MinHeap *heap);

#ifdef __cplusplus
}
#endif

#endif /* MIN_HEAP_H */
