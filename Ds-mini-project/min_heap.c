/**
 * ============================================================================
 * Project: Huffman Coding-Based File Compression System
 * Course:  Computer Engineering — Data Structures and Algorithms
 * Student: Shalom Dabre
 * Roll No: 11070
 * 
 * File:    src/min_heap.c
 * Purpose: Manual implementation of a 0-indexed Min-Heap Priority Queue
 *          using an array of HuffmanNode pointers.
 * ============================================================================
 */

#include "min_heap.h"
#include <stdio.h>
#include <stdlib.h>

MinHeap* create_min_heap(size_t capacity) {
    if (capacity == 0) {
        capacity = 16;
    }
    MinHeap *heap = (MinHeap*)malloc(sizeof(MinHeap));
    if (!heap) {
        return NULL;
    }
    heap->size = 0;
    heap->capacity = capacity;
    heap->array = (HuffmanNode**)malloc(capacity * sizeof(HuffmanNode*));
    if (!heap->array) {
        free(heap);
        return NULL;
    }
    return heap;
}

void destroy_min_heap(MinHeap *heap) {
    if (!heap) {
        return;
    }
    if (heap->array) {
        free(heap->array);
        heap->array = NULL;
    }
    free(heap);
}

void swap_nodes(HuffmanNode **a, HuffmanNode **b) {
    HuffmanNode *temp = *a;
    *a = *b;
    *b = temp;
}

int compare_nodes(const HuffmanNode *a, const HuffmanNode *b) {
    if (!a && !b) return 0;
    if (!a) return 1;
    if (!b) return -1;

    /* Primary comparison: lower frequency has higher priority */
    if (a->freq < b->freq) {
        return -1;
    }
    if (a->freq > b->freq) {
        return 1;
    }

    /* Secondary tie-breaker: deterministic insertion order */
    if (a->order < b->order) {
        return -1;
    }
    if (a->order > b->order) {
        return 1;
    }

    /* Tertiary tie-breaker: byte value if both are leaf nodes */
    if (a->is_leaf && b->is_leaf) {
        if (a->data < b->data) return -1;
        if (a->data > b->data) return 1;
    }

    return 0;
}

void heapify_up(MinHeap *heap, size_t index) {
    if (!heap || index == 0 || index >= heap->size) {
        return;
    }

    while (index > 0) {
        size_t parent = (index - 1) / 2;
        /* If current node is smaller than its parent, swap and continue upwards */
        if (compare_nodes(heap->array[index], heap->array[parent]) < 0) {
            swap_nodes(&heap->array[index], &heap->array[parent]);
            index = parent;
        } else {
            break;
        }
    }
}

void heapify_down(MinHeap *heap, size_t index) {
    if (!heap || heap->size <= 1) {
        return;
    }

    while (1) {
        size_t smallest = index;
        size_t left = 2 * index + 1;
        size_t right = 2 * index + 2;

        if (left < heap->size && compare_nodes(heap->array[left], heap->array[smallest]) < 0) {
            smallest = left;
        }
        if (right < heap->size && compare_nodes(heap->array[right], heap->array[smallest]) < 0) {
            smallest = right;
        }

        if (smallest != index) {
            swap_nodes(&heap->array[index], &heap->array[smallest]);
            index = smallest;
        } else {
            break;
        }
    }
}

int insert_min_heap(MinHeap *heap, HuffmanNode *node) {
    if (!heap || !node) {
        return 0;
    }

    /* Dynamic resize if current capacity is reached */
    if (heap->size >= heap->capacity) {
        size_t new_cap = heap->capacity * 2;
        HuffmanNode **new_array = (HuffmanNode**)realloc(heap->array, new_cap * sizeof(HuffmanNode*));
        if (!new_array) {
            return 0;
        }
        heap->array = new_array;
        heap->capacity = new_cap;
    }

    /* Place node at the end of the heap and bubble up */
    size_t index = heap->size;
    heap->array[index] = node;
    heap->size++;
    heapify_up(heap, index);
    return 1;
}

HuffmanNode* extract_min(MinHeap *heap) {
    if (!heap || heap->size == 0) {
        return NULL;
    }

    HuffmanNode *min_node = heap->array[0];
    heap->size--;

    if (heap->size > 0) {
        /* Move the last element to the root and bubble down */
        heap->array[0] = heap->array[heap->size];
        heapify_down(heap, 0);
    }

    return min_node;
}

int is_heap_size_one(const MinHeap *heap) {
    return (heap && heap->size == 1);
}

int is_heap_empty(const MinHeap *heap) {
    return (!heap || heap->size == 0);
}
