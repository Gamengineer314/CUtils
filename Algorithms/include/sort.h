#define GEN_PREFIX sort
#include "generic_start.h"

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <limits.h>

// Additional parameters :
// SORT_COMPARABLE : Whether to use a comparator to compare keys (not compatible with radix sort)
//                   or the key itself (must be an unsigned integer type for radix sort) (default: false)
// SORT_KEY : Get the key from an item (default: the item)
//            SORT_SIGNED, SORT_FLOAT and SORT_DOUBLE can be used for signed integer or floating point key types

#ifndef SORT_KEY
#define SORT_KEY(item) (item)
#endif

// Not standard but most likely works
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
static inline int32_t floatToInt(float f) { return *(int32_t*)&f; }
static inline int64_t doubleToInt(double d) { return *(int64_t*)&d; }
#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

// Keys for signed integer and floating point types (GEN_TYPE must be the unsigned integer type with the same size)
#define SORT_SIGNED(item) ((GEN_KEY)(item) ^ ((GEN_KEY)1 << (sizeof(GEN_KEY) * CHAR_BIT - 1)))
#define SORT_FLOAT(item) ((uint32_t)floatToInt(item) ^ (uint32_t)((floatToInt(item) >> 31) | ((int32_t)1 << 31)))
#define SORT_DOUBLE(item) ((uint64_t)doubleToInt(item) ^ (uint64_t)((doubleToInt(item) >> 63) | ((int64_t)1 << 63)))

// Thresholds for sort functions
#define SORT_QUICK_THRESH 32
#define SORT_RADIX_THRESH (64 * sizeof(GEN_KEY))

// Comparisons
#ifdef SORT_COMPARABLE
#define SORT_GT(item1, item2) GEN_COMPARE(SORT_KEY(item1), SORT_KEY(item2)) > 0
#define SORT_LT(item1, item2) GEN_COMPARE(SORT_KEY(item1), SORT_KEY(item2)) < 0
#else
#define SORT_GT(item1, item2) (SORT_KEY(item1) > SORT_KEY(item2))
#define SORT_LT(item1, item2) (SORT_KEY(item1) < SORT_KEY(item2))
#endif


/**
 * @brief Find the smallest item in an array of items and move it at index 0
 * @param items The items
 * @param n Number of items
**/
void GEN_FUNC_(findFirst)(GEN_TYPE* items, GEN_SIZE n);


/**
 * @brief Sort an array of items using insertion sort assuming the first item is the smallest
 * @param items The items
 * @param n Number of items
**/
void GEN_FUNC_(unguardedInsertion)(GEN_TYPE* items, GEN_SIZE n);


/**
 * @brief Sort an array of items using insertion sort
 * @param items The items
 * @param n Number of items
**/
inline void GEN_FUNC(insertion)(GEN_TYPE* items, GEN_SIZE n) {
    GEN_FUNC_(findFirst)(items, n);
    GEN_FUNC_(unguardedInsertion)(items, n);
}


#ifndef SORT_COMPARABLE

/**
 * @brief Sort an array of items using radix sort
 * @param items The items
 * @param n Number of items
**/
void GEN_FUNC(radix)(GEN_TYPE* items, GEN_SIZE n);

#endif


/**
 * @brief Quicksort recursion
 * @param items Items to sort
 * @param start Start index (inclusive)
 * @param end End index (exclusive)
**/
void GEN_FUNC_(quickRecur)(GEN_TYPE* items, GEN_SIZE start, GEN_SIZE end);


/**
 * @brief Sort an array of items using quick sort
 * @param items The items
 * @param n Number of items
**/
inline void GEN_FUNC(quick)(GEN_TYPE* items, GEN_SIZE n) {
    if (n > SORT_QUICK_THRESH) {
        GEN_FUNC_(quickRecur)(items, 0, n);
        GEN_FUNC_(findFirst)(items, SORT_QUICK_THRESH);
    }
    else GEN_FUNC_(findFirst)(items, n);
    GEN_FUNC_(unguardedInsertion)(items, n);
}


/**
 * @brief Sort an array of items
 * @param items The items
 * @param n Number of items
**/
inline void GEN_ALGO(GEN_TYPE* items, GEN_SIZE n) {
#ifdef SORT_COMPARABLE
    GEN_FUNC(quick)(items, n);
#else
    if (n < SORT_RADIX_THRESH) GEN_FUNC(quick)(items, n);
    else GEN_FUNC(radix)(items, n);
#endif
}


#ifdef GEN_SOURCE


void GEN_FUNC(quick)(GEN_TYPE* items, GEN_SIZE n);
void GEN_FUNC(insertion)(GEN_TYPE* items, GEN_SIZE n);
void GEN_ALGO(GEN_TYPE* items, GEN_SIZE n);


void GEN_FUNC_(findFirst)(GEN_TYPE* items, GEN_SIZE n) {
    GEN_SIZE minIndex = 0;
    GEN_TYPE minItem = items[0];
    for (GEN_SIZE i = 1; i < n; i++) {
        if (SORT_LT(items[i], minItem)) {
            minIndex = i;
            minItem = items[i];
        }
    }
    items[minIndex] = items[0];
    items[0] = minItem;
}


void GEN_FUNC_(unguardedInsertion)(GEN_TYPE* items, GEN_SIZE n) {
    for (GEN_SIZE i = 2; i < n; i++) {
        GEN_TYPE item = items[i];
        GEN_SIZE j = i - 1;
        while (SORT_GT(items[j], item)) {
            items[j + 1] = items[j];
            j--;
        }
        items[j + 1] = item;
    }
}


void GEN_FUNC_(quickRecur)(GEN_TYPE* items, GEN_SIZE start, GEN_SIZE end) {
    // Use median of 3 items as pivot
    GEN_SIZE mid = start + ((end - start) >> 1);
    GEN_TYPE item = items[mid];
    items[mid] = items[start + 1];
    items[start + 1] = item;
    if (SORT_GT(items[start + 1], items[start])) {
        item = items[start + 1];
        items[start + 1] = items[start];
        items[start] = item;
    }
    if (SORT_GT(items[start + 1], items[end - 1])) {
        item = items[start + 1];
        items[start + 1] = items[end - 1];
        items[end - 1] = item;
    }
    if (SORT_GT(items[start], items[end - 1])) {
        item = items[start];
        items[start] = items[end - 1];
        items[end - 1] = item;
    }
    GEN_TYPE pivot = items[start];

    // Partition
    GEN_SIZE i = start + 1, j = end - 1;
    while (1) {
        do {
            item = items[++i];
        } while (SORT_LT(item, pivot));
        do {
            item = items[--j];
        } while (SORT_GT(item, pivot));
        if (i >= j) break;
        item = items[i];
        items[i] = items[j];
        items[j] = item;
    }
    items[start] = items[i];
    items[i] = pivot;

    // Recursion
    if (i - start > SORT_QUICK_THRESH) GEN_FUNC_(quickRecur)(items, start, i);
    if (end - i - 1 > SORT_QUICK_THRESH) GEN_FUNC_(quickRecur)(items, i + 1, end);
}


#ifndef SORT_COMPARABLE

void GEN_FUNC(radix)(GEN_TYPE* items, GEN_SIZE n) {
    GEN_TYPE* items2 = THROW_PN(malloc(sizeof(GEN_TYPE) * n), items2);
    for (int i = 0; i < sizeof(GEN_KEY); i++) {
        GEN_SIZE count[257] = {};
        for (GEN_SIZE j = 0; j < n; j++) {
            count[((SORT_KEY(items[j]) >> (8 * i)) & 0xFF) + 1]++;
        }
        for (int j = 2; j < 256; j++) {
            count[j] += count[j - 1];
        }
        for (GEN_SIZE j = 0; j < n; j++) {
            items2[count[((SORT_KEY(items[j]) >> (8 * i)) & 0xFF)]++] = items[j];
        }
        GEN_TYPE* tmp = items;
        items = items2;
        items2 = tmp;
    }
    if (sizeof(GEN_KEY) & 1) memcpy(items, items2, sizeof(GEN_TYPE) * n);
    free(items2);
}

#endif


#endif

#undef SORT_COMPARABLE
#undef SORT_KEY

#include "generic_end.h"