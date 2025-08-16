#define GEN_PREFIX heap
#include "generic_start.h"

// Additional parameters :
// HEAP_INDEXED : Whether the heap contains the items or indices to fixed items in a speparate array (default: false)
// HEAP_MODIFIABLE : Whether or not item keys can be modified or removed after adding them (default: false)

// Indexed if modifiable
#if defined(HEAP_MODIFIABLE) && !defined(HEAP_INDEXED)
#define HEAP_INDEXED
#endif

// Types and macros
#ifdef HEAP_INDEXED
#define HEAP_NODE GEN_SIZE
#define HEAP_KEY(heap, node) ((heap)->items[node].item.key)
#else
#define HEAP_NODE GEN_NAME(item)
#define HEAP_KEY(heap, node) ((node).key)
#endif

#include <stdlib.h>
#include "throw.h"


/**
 * @brief Heap item
 * @param key Key of the item
 * @param value Value of the item
 * @param index Index of the item in the heap if [HEAP_MODIFIABLE]
**/
typedef struct {
#ifdef HEAP_MODIFIABLE
    GEN_SIZE index;
#endif
    GEN_KEY key;
    GEN_TYPE value;
} GEN_NAME(item);


/**
 * @brief Reusable item
 * @param item The item if it is used
 * @param reusable Index of the next reusable item if the item is not used
**/
typedef union {
    GEN_NAME(item) item;
    GEN_SIZE reusable;
} GEN_NAME(reusable);


/**
 * @brief Resizable min-heap
 * @param heap Items in the heap or indices of the items in [items] if [HEAP_INDEXED], item at index 1 has minimum key
 * @param items Items in the heap if [HEAP_INDEXED]
 * @param length Size of the heap
 * @param reusable Index of the first reusable item in [items] if [HEAP_INDEXED]
**/
typedef struct {
    HEAP_NODE* heap;
#ifdef HEAP_INDEXED
    GEN_NAME(reusable)* items; // Reusable items
    GEN_SIZE reusable; // Index of the first reusable item in [items]
#endif
    GEN_SIZE length;
    GEN_SIZE capacity; // Size of [heap] and [items]
} GEN_ALGO;


/**
 * @brief Initialize a heap
 * @param heap The heap
 * @param capacity Initial capacity
**/
inline void GEN_NAME(init)(GEN_ALGO* heap, GEN_SIZE capacity) {
    heap->heap = THROW_PN(malloc(sizeof(HEAP_NODE) * capacity), heap->heap);
#ifdef HEAP_INDEXED
    heap->items = THROW_PN(malloc(sizeof(GEN_NAME(reusable)) * capacity), heap->heap);
    heap->reusable = -1;
#endif
    heap->length = 0;
    heap->capacity = capacity;
}


/**
 * @brief Create a heap
 * @param capacity Initial capacity
 * @return The heap
**/
inline GEN_ALGO GEN_NAME(new)(GEN_SIZE capacity) {
    GEN_ALGO heap;
    GEN_NAME(init)(&heap, capacity);
    return heap;
}


/**
 * @brief Free a heap
 * @param heap The heap
**/
inline void GEN_NAME(free)(GEN_ALGO* heap) {
    free(heap->heap);
#ifdef HEAP_INDEXED
    free(heap->items);
#endif
}


/**
 * @brief Grow a heap if it is full
**/
void GEN_NAME_(grow)(GEN_ALGO* heap);


/**
 * @brief Fix a heap above an inserted node
 * @param heap The heap
 * @param index Index of the node
 * @param node The node
**/
void GEN_NAME_(heapifyUp)(GEN_ALGO* heap, GEN_SIZE index, HEAP_NODE node);


/**
 * @brief Fix a heap under an inserted node
 * @param heap The heap
 * @param index Index of the node
 * @param node The node
**/
void GEN_NAME_(heapifyDown)(GEN_ALGO* heap, GEN_SIZE index, HEAP_NODE node);


#ifdef HEAP_INDEXED

/**
 * @brief Add an item with no value in a heap
 * @param heap The heap
 * @param key Key of the item
 * @return Index of the added item
**/
GEN_SIZE GEN_NAME_(addEmpty)(GEN_ALGO* heap, GEN_KEY key);


/**
 * @brief Remove the first item in a heap
 * @param heap The heap
**/
void GEN_NAME_(removeFirst)(GEN_ALGO* heap);

#endif


/**
 * @brief Add an item in a heap
 * @param heap The heap
 * @param key Key of the item
 * @param value Value of the item
 * @return Index of the added item if [HEAP_MODIFIABLE]
**/
#ifdef HEAP_MODIFIABLE
inline GEN_SIZE GEN_NAME(add)(GEN_ALGO* heap, GEN_KEY key, GEN_TYPE value) {
#else
inline void GEN_NAME(add)(GEN_ALGO* heap, GEN_KEY key, GEN_TYPE value) {
#endif
#ifdef HEAP_INDEXED
#ifdef HEAP_MODIFIABLE
    GEN_SIZE i = GEN_NAME_(addEmpty)(heap, key);
    heap->items[i].item.value = value;
    return i;
#else
    heap->items[GEN_NAME_(addEmpty)(heap, key)].value = value;
#endif
#else
    GEN_NAME_(grow)(heap);
    GEN_NAME_(heapifyUp)(heap, ++heap->length, (GEN_NAME(item)) { .key = key, .value = value });
#endif
}


/**
 * @brief Remove and return the first item in a heap
 * @param heap The heap
 * @return The item
**/
inline GEN_NAME(item) GEN_NAME(pop)(GEN_ALGO* heap) {
#ifdef HEAP_INDEXED
    GEN_NAME(item) item = heap->items[heap->heap[1]].item;
    GEN_NAME_(removeFirst)(heap);
#else
    GEN_NAME(item) item = heap->heap[1];
    GEN_NAME_(heapifyDown)(heap, 1, heap->heap[heap->length--]);
#endif
    return item;
}


#ifdef HEAP_MODIFIABLE

/**
 * @brief Change the key of an item in a heap
 * @param heap The heap
 * @param index Index of the item
 * @param newKey New key of the item
**/
inline void GEN_NAME(changeKey)(GEN_ALGO* heap, GEN_SIZE index, GEN_KEY newKey) {
    heap->items[index].item.key = newKey;
    GEN_SIZE heapIndex = heap->items[index].item.index;
    GEN_SIZE parent = heap->heap[heapIndex >> 1];
    if (heapIndex != 1 && GEN_COMPARE(heap->items[parent].item.key, newKey) > 0) {
        GEN_NAME_(heapifyUp)(heap, heapIndex, heap->heap[heapIndex]);
    }
    else {
        GEN_NAME_(heapifyDown)(heap, heapIndex, heap->heap[heapIndex]);
    }
}


/**
 * @brief Remove an item in a heap
 * @param heap The heap
 * @param index Index of the item
**/
inline void GEN_NAME(remove)(GEN_ALGO* heap, GEN_SIZE index) {
    GEN_SIZE heapIndex = heap->items[index].item.index;
    GEN_SIZE parent = heap->heap[heapIndex >> 1];
    GEN_SIZE last = heap->heap[heap->length--];
    if (heapIndex != 1 && GEN_COMPARE(heap->items[parent].item.key, heap->items[last].item.key) > 0) {
        GEN_NAME_(heapifyUp)(heap, heapIndex, last);
    }
    else {
        GEN_NAME_(heapifyDown)(heap, heapIndex, last);
    }
    heap->items[index].reusable = heap->reusable;
    heap->reusable = index;
}

#endif


#ifdef GEN_SOURCE


void GEN_NAME(init)(GEN_ALGO* heap, GEN_SIZE capacity);
GEN_ALGO GEN_NAME(new)(GEN_SIZE capacity);
void GEN_NAME(free)(GEN_ALGO* heap);
void GEN_NAME_(grow)(GEN_ALGO* heap);
#ifdef HEAP_MODIFIABLE
GEN_SIZE GEN_NAME(add)(GEN_ALGO* heap, GEN_KEY key, GEN_TYPE value);
#else
void GEN_NAME(add)(GEN_ALGO* heap, GEN_KEY key, GEN_TYPE value);
#endif
GEN_NAME(item) GEN_NAME(pop)(GEN_ALGO* heap);
#ifdef HEAP_MODIFIABLE
void GEN_NAME(changeKey)(GEN_ALGO* heap, GEN_SIZE index, GEN_KEY newKey);
void GEN_NAME(remove)(GEN_ALGO* heap, GEN_SIZE index);
#endif


inline void GEN_NAME_(grow)(GEN_ALGO* heap) {
    if (heap->length >= heap->capacity - 1) {
        heap->capacity <<= 1;
        heap->heap = THROW_PN(realloc(heap->heap, sizeof(HEAP_NODE) * heap->capacity), heap->heap);
#ifdef HEAP_INDEXED
        heap->items = THROW_PN(realloc(heap->items, sizeof(GEN_NAME(reusable)) * heap->capacity), heap->items);
#endif
    }
}


#ifdef HEAP_INDEXED

GEN_SIZE GEN_NAME_(addEmpty)(GEN_ALGO* heap, GEN_KEY key) {
    GEN_NAME_(grow)(heap);
    GEN_SIZE i;
    if (heap->reusable != -1) {
        i = heap->reusable;
        heap->reusable = heap->items[i].reusable;
    }
    else i = heap->length;
    heap->items[i].item.key = key;
    GEN_NAME_(heapifyUp)(heap, ++heap->length, i);
    return i;
}


void GEN_NAME_(removeFirst)(GEN_ALGO* heap) {
    GEN_SIZE i = heap->heap[1];
    heap->items[i].reusable = heap->reusable;
    heap->reusable = i;
    GEN_NAME_(heapifyDown)(heap, 1, heap->heap[heap->length--]);
}

#endif


void GEN_NAME_(heapifyUp)(GEN_ALGO* heap, GEN_SIZE index, HEAP_NODE node) {
    GEN_KEY key = HEAP_KEY(heap, node);
    while (index > 1) {
        GEN_SIZE parentIndex = index >> 1;
        HEAP_NODE parent = heap->heap[parentIndex];
        if (GEN_COMPARE(key, HEAP_KEY(heap, parent)) >= 0) break;
        heap->heap[index] = parent;
#ifdef HEAP_MODIFIABLE
        heap->items[parent].item.index = index;
#endif
        index = parentIndex;
    }
    heap->heap[index] = node;
#ifdef HEAP_MODIFIABLE
    heap->items[node].item.index = index;
#endif
}


void GEN_NAME_(heapifyDown)(GEN_ALGO* heap, GEN_SIZE index, HEAP_NODE node) {
    GEN_KEY key = HEAP_KEY(heap, node);
    GEN_SIZE childIndex = index << 1;
    while (childIndex <= heap->length) {
        HEAP_NODE child = heap->heap[childIndex];
        if (childIndex + 1 <= heap->length) {
            HEAP_NODE child2 = heap->heap[childIndex + 1];
            if (GEN_COMPARE(HEAP_KEY(heap, child), HEAP_KEY(heap, child2)) > 0) {
                childIndex++;
                child = child2;
            }
        }
        if (GEN_COMPARE(key, HEAP_KEY(heap, child)) <= 0) break;
        heap->heap[index] = child;
#ifdef HEAP_MODIFIABLE
        heap->items[child].item.index = index;
#endif
        index = childIndex;
        childIndex <<= 1;
    }
    heap->heap[index] = node;
#ifdef HEAP_MODIFIABLE
    heap->items[node].item.index = index;
#endif
}


#endif

// Undef parameters for later use
#undef HEAP_MODIFIABLE
#undef HEAP_INDEXED
#undef HEAP_NODE
#undef HEAP_KEY

#include "generic_end.h"