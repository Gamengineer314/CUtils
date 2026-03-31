#define GEN_FUNC_PREFIX heap
#define GEN_STRUCT_PREFIX Heap
#define GEN_KV
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
#define HEAP_NODE_TYPE GEN_SIZE
#define HEAP_KV(heap, node) ((heap)->items[node].kv)
#else
#define HEAP_NODE_TYPE GEN_KV_TYPE
#define HEAP_KV(heap, node) (node)
#endif
#define HEAP_KEY(heap, node) GEN_KV_KEY(HEAP_KV(heap, node))

#include <stdlib.h>
#include "throw.h"


#ifdef HEAP_INDEXED
/**
 * @brief Heap item
 * @param kv Key-value pair
 * @param index Index of the item in the heap or index of the next reusable item if the item is reusable
**/
#ifdef HEAP_MODIFIABLE
typedef struct {
    GEN_SIZE index;
    GEN_KV_TYPE kv;
} GEN_STRUCT_(Item);
#else
typedef union {
    GEN_SIZE index;
    GEN_KV_TYPE kv;
} GEN_STRUCT_(Item);
#endif
#endif


/**
 * @brief Resizable min-heap
 * @param heap Items in the heap or indices of the items in [items] if [HEAP_INDEXED], item at index 1 has minimum key
 * @param items Items in the heap if [HEAP_INDEXED]
 * @param length Size of the heap
 * @param reusable Index of the first reusable item in [items] if [HEAP_INDEXED]
**/
typedef struct {
    HEAP_NODE_TYPE* heap;
#ifdef HEAP_INDEXED
    GEN_STRUCT_(Item)* items; // Reusable items
    GEN_SIZE reusable; // Index of the first reusable item in [items]
#endif
    GEN_SIZE length;
    GEN_SIZE capacity; // Size of [heap] and [items]
} GEN_ALGO;


/**
 * @brief Heap iterator
 * @param index Node index
**/
typedef struct {
    GEN_SIZE index;
} GEN_STRUCT(Iter);


/**
 * @brief Initialize a heap
 * @param heap The heap
 * @param capacity Initial capacity
**/
inline void GEN_FUNC(init)(GEN_ALGO* heap, GEN_SIZE capacity) {
    heap->heap = THROW_PN(malloc(sizeof(HEAP_NODE_TYPE) * capacity), heap->heap);
#ifdef HEAP_INDEXED
    heap->items = THROW_PN(malloc(sizeof(GEN_STRUCT_(Item)) * capacity), heap->heap);
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
inline GEN_ALGO GEN_FUNC(new)(GEN_SIZE capacity) {
    GEN_ALGO heap;
    GEN_FUNC(init)(&heap, capacity);
    return heap;
}


/**
 * @brief Free a heap
 * @param heap The heap
**/
inline void GEN_FUNC(free)(GEN_ALGO* heap) {
    free(heap->heap);
#ifdef HEAP_INDEXED
    free(heap->items);
#endif
}


/**
 * @brief Grow a heap if it is full
**/
void GEN_FUNC_(grow)(GEN_ALGO* heap);


/**
 * @brief Fix a heap above an inserted node
 * @param heap The heap
 * @param index Index of the node
 * @param node The node
**/
void GEN_FUNC_(heapifyUp)(GEN_ALGO* heap, GEN_SIZE index, HEAP_NODE_TYPE node);


/**
 * @brief Fix a heap under an inserted node
 * @param heap The heap
 * @param index Index of the node
 * @param node The node
**/
void GEN_FUNC_(heapifyDown)(GEN_ALGO* heap, GEN_SIZE index, HEAP_NODE_TYPE node);


#ifdef HEAP_INDEXED

/**
 * @brief Add an item with no value in a heap
 * @param heap The heap
 * @param key Key of the item
 * @return Index of the added item
**/
GEN_SIZE GEN_FUNC_(addEmpty)(GEN_ALGO* heap, GEN_KEY key);


/**
 * @brief Remove the first item in a heap
 * @param heap The heap
**/
void GEN_FUNC_(removeFirst)(GEN_ALGO* heap);

#endif


/**
 * @brief Add an item in a heap
 * @param heap The heap
 * @param key Key of the item
 * @param value Value of the item
 * @return Index of the added item if [HEAP_MODIFIABLE]
**/
inline
#ifdef HEAP_MODIFIABLE
GEN_SIZE
#else
void
#endif
GEN_FUNC(add)(GEN_ALGO* heap, GEN_KEY key
#ifndef GEN_NO_VALUE
, GEN_TYPE value
#endif
) {
#ifdef HEAP_INDEXED
    GEN_SIZE i = GEN_FUNC_(addEmpty)(heap, key);
#ifndef GEN_NO_VALUE
    heap->items[i].kv.value = value;
#endif
#ifdef HEAP_MODIFIABLE
    return i;
#endif
#else
    GEN_FUNC_(grow)(heap);
    GEN_FUNC_(heapifyUp)(heap, ++heap->length, GEN_IF_VALUE(((GEN_STRUCT(KV)) { .key = key, .value = value }), key));
#endif
}


/**
 * @brief Remove and return the first item in a heap
 * @param heap The heap
 * @return The item
**/
inline GEN_KV_TYPE GEN_FUNC(pop)(GEN_ALGO* heap) {
    GEN_KV_TYPE kv = HEAP_KV(heap, heap->heap[1]);
#ifdef HEAP_INDEXED
    GEN_FUNC_(removeFirst)(heap);
#else
    GEN_FUNC_(heapifyDown)(heap, 1, heap->heap[heap->length--]);
#endif
    return kv;
}


/**
 * @brief Get the first item in a heap
 * @param heap The heap
 * @return The item
**/
inline GEN_KV_TYPE GEN_FUNC(peek)(GEN_ALGO* heap) {
    return HEAP_KV(heap, heap->heap[1]);
}


#ifdef HEAP_MODIFIABLE

/**
 * @brief Change the key of an item in a heap
 * @param heap The heap
 * @param index Index of the item
 * @param newKey New key of the item
**/
inline void GEN_FUNC(changeKey)(GEN_ALGO* heap, GEN_SIZE index, GEN_KEY newKey) {
    HEAP_KEY(heap, index) = newKey;
    GEN_SIZE heapIndex = heap->items[index].index;
    GEN_SIZE parent = heap->heap[heapIndex >> 1];
    if (heapIndex != 1 && GEN_COMPARE(HEAP_KEY(heap, parent), newKey) > 0) {
        GEN_FUNC_(heapifyUp)(heap, heapIndex, heap->heap[heapIndex]);
    }
    else {
        GEN_FUNC_(heapifyDown)(heap, heapIndex, heap->heap[heapIndex]);
    }
}


/**
 * @brief Remove an item in a heap
 * @param heap The heap
 * @param index Index of the item
**/
inline void GEN_FUNC(remove)(GEN_ALGO* heap, GEN_SIZE index) {
    GEN_SIZE heapIndex = heap->items[index].index;
    GEN_SIZE parent = heap->heap[heapIndex >> 1];
    GEN_SIZE last = heap->heap[heap->length--];
    if (heapIndex != 1 && GEN_COMPARE(HEAP_KEY(heap, parent), HEAP_KEY(heap, last)) > 0) {
        GEN_FUNC_(heapifyUp)(heap, heapIndex, last);
    }
    else {
        GEN_FUNC_(heapifyDown)(heap, heapIndex, last);
    }
    heap->items[index].index = heap->reusable;
    heap->reusable = index;
}

#endif


/**
 * @brief Start iterating on a heap
 * @return The iterator
**/
inline GEN_STRUCT(Iter) GEN_FUNC(iterStart)(GEN_ALGO* heap) {
    return (GEN_STRUCT(Iter)) { 0 };
}


/**
 * @brief Get the next item while iterating on a heap
 * @param heap The heap
 * @param iter The iterator
 * @return Next item, NULL if no more items
**/
inline GEN_KV_TYPE* GEN_FUNC(iterNext)(GEN_ALGO* heap, GEN_STRUCT(Iter)* iter) {
    if (iter->index < heap->length) {
        return &HEAP_KV(heap, heap->heap[++iter->index]);
    }
    return NULL;
}


#ifdef GEN_SOURCE


void GEN_FUNC(init)(GEN_ALGO* heap, GEN_SIZE capacity);
GEN_ALGO GEN_FUNC(new)(GEN_SIZE capacity);
void GEN_FUNC(free)(GEN_ALGO* heap);
void GEN_FUNC_(grow)(GEN_ALGO* heap);
#ifdef HEAP_MODIFIABLE
GEN_SIZE
#else
void
#endif
GEN_FUNC(add)(GEN_ALGO* heap, GEN_KEY key
#ifndef GEN_NO_VALUE
, GEN_TYPE value
#endif
);
GEN_KV_TYPE GEN_FUNC(pop)(GEN_ALGO* heap);
GEN_KV_TYPE GEN_FUNC(peek)(GEN_ALGO* heap);
#ifdef HEAP_MODIFIABLE
void GEN_FUNC(changeKey)(GEN_ALGO* heap, GEN_SIZE index, GEN_KEY newKey);
void GEN_FUNC(remove)(GEN_ALGO* heap, GEN_SIZE index);
#endif
GEN_STRUCT(Iter) GEN_FUNC(iterStart)(GEN_ALGO* heap);
GEN_KV_TYPE* GEN_FUNC(iterNext)(GEN_ALGO* heap, GEN_STRUCT(Iter)* iter);


inline void GEN_FUNC_(grow)(GEN_ALGO* heap) {
    if (heap->length >= heap->capacity - 1) {
        heap->capacity <<= 1;
        heap->heap = THROW_PN(realloc(heap->heap, sizeof(HEAP_NODE_TYPE) * heap->capacity), heap->heap);
#ifdef HEAP_INDEXED
        heap->items = THROW_PN(realloc(heap->items, sizeof(GEN_STRUCT_(Item)) * heap->capacity), heap->items);
#endif
    }
}


#ifdef HEAP_INDEXED

GEN_SIZE GEN_FUNC_(addEmpty)(GEN_ALGO* heap, GEN_KEY key) {
    GEN_FUNC_(grow)(heap);
    GEN_SIZE i;
    if (heap->reusable != -1) {
        i = heap->reusable;
        heap->reusable = heap->items[i].index;
    }
    else i = heap->length;
    HEAP_KEY(heap, i) = key;
    GEN_FUNC_(heapifyUp)(heap, ++heap->length, i);
    return i;
}


void GEN_FUNC_(removeFirst)(GEN_ALGO* heap) {
    GEN_SIZE i = heap->heap[1];
    heap->items[i].index = heap->reusable;
    heap->reusable = i;
    GEN_FUNC_(heapifyDown)(heap, 1, heap->heap[heap->length--]);
}

#endif


void GEN_FUNC_(heapifyUp)(GEN_ALGO* heap, GEN_SIZE index, HEAP_NODE_TYPE node) {
    GEN_KEY key = HEAP_KEY(heap, node);
    while (index > 1) {
        GEN_SIZE parentIndex = index >> 1;
        HEAP_NODE_TYPE parent = heap->heap[parentIndex];
        if (GEN_COMPARE(key, HEAP_KEY(heap, parent)) >= 0) break;
        heap->heap[index] = parent;
#ifdef HEAP_MODIFIABLE
        heap->items[parent].index = index;
#endif
        index = parentIndex;
    }
    heap->heap[index] = node;
#ifdef HEAP_MODIFIABLE
    heap->items[node].index = index;
#endif
}


void GEN_FUNC_(heapifyDown)(GEN_ALGO* heap, GEN_SIZE index, HEAP_NODE_TYPE node) {
    GEN_KEY key = HEAP_KEY(heap, node);
    GEN_SIZE childIndex = index << 1;
    while (childIndex <= heap->length) {
        HEAP_NODE_TYPE child = heap->heap[childIndex];
        if (childIndex + 1 <= heap->length) {
            HEAP_NODE_TYPE child2 = heap->heap[childIndex + 1];
            if (GEN_COMPARE(HEAP_KEY(heap, child), HEAP_KEY(heap, child2)) > 0) {
                childIndex++;
                child = child2;
            }
        }
        if (GEN_COMPARE(key, HEAP_KEY(heap, child)) <= 0) break;
        heap->heap[index] = child;
#ifdef HEAP_MODIFIABLE
        heap->items[child].index = index;
#endif
        index = childIndex;
        childIndex <<= 1;
    }
    heap->heap[index] = node;
#ifdef HEAP_MODIFIABLE
    heap->items[node].index = index;
#endif
}


#endif

// Undef parameters for later use
#undef HEAP_MODIFIABLE
#undef HEAP_INDEXED
#undef GEN_KV_KEY
#undef HEAP_KV
#undef HEAP_KEY
#undef HEAP_NODE_TYPE
#undef GEN_KV_TYPE

#include "generic_end.h"