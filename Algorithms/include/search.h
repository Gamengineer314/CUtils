#define GEN_PREFIX search
#include "generic_start.h"


/**
 * @brief Search the index of an item in an ordered array using binary search
 * @param items The items
 * @param item The item to search
 * @param n Number of items
 * @return Index of the item if found or index where it would be inserted otherwise
**/
GEN_SIZE GEN_ALGO(GEN_TYPE* items, GEN_TYPE item, GEN_SIZE n);


#ifdef GEN_SOURCE


GEN_SIZE GEN_ALGO(GEN_TYPE* items, GEN_TYPE item, GEN_SIZE n) {
    GEN_SIZE start = 0, len = n;
    while (len > 0) {
        GEN_SIZE half = len >> 1;
        GEN_SIZE middle = start + half;
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(items[middle], item);
        if (cmp < 0) {
            start = middle + 1;
            len -= half + 1;
        }
        else len = half;
    }
    return start;
}


#endif

#include "generic_end.h"