#define GEN_PREFIX map
#include "generic_start.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "throw.h"

// Additional parameters :
// MAP_HASH : Hash function for keys (default: the key)
// MAP_INDEX : Conversion from a hash to an index in a map with a given mask (default: (hash ^ (hash >> 16)) & mask)

#ifndef MAP_HASH
#define MAP_HASH(key) (key)
#endif
#ifndef MAP_INDEX
#define MAP_INDEX(hash, mask) (((hash) ^ (hash >> 16)) & (mask))
#endif


/**
 * @brief Map item
 * @param key Key of the item
 * @param value Value of the item
 * @param next Index of the next item in the bucket
**/
typedef struct {
    GEN_SIZE next;
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
 * @brief Resizable hash map
 * @param items Items in the map
 * @param buckets Linked list of items for each hash value
 * @param length Number of items in the map
 * @param mask Size of [buckets] - 1 = size of [items] * 2 - 1
 * @param reusable Index of the first reusable item in [items]
**/
typedef struct {
    GEN_NAME(reusable)* items;
    GEN_SIZE* buckets;
    GEN_SIZE length;
    GEN_SIZE mask;
    GEN_SIZE reusable;
} GEN_ALGO;


/**
 * @brief Map iterator
 * @param bucket Bucket index
 * @param index Index of the item
**/
typedef struct {
    GEN_SIZE bucket;
    GEN_SIZE index;
} GEN_NAME(iter);


/**
 * @brief Initialize a map
 * @param map The map
 * @param capacity Initial capacity (must be a power of 2)
**/
inline void GEN_NAME(init)(GEN_ALGO* map, GEN_SIZE capacity) {
    map->items = THROW_PN(malloc(sizeof(GEN_NAME(reusable)) * capacity), map->items);
    map->buckets = THROW_PN(malloc(sizeof(GEN_SIZE) * capacity << 1), map->buckets);
    memset(map->buckets, -1, sizeof(GEN_SIZE) * capacity << 1);
    map->length = 0;
    map->mask = (capacity << 1) - 1;
    map->reusable = -1;
}


/**
 * @brief Create a map
 * @param capacity Initial capacity (must be a power of 2)
 * @return The map
**/
inline GEN_ALGO GEN_NAME(new)(GEN_SIZE capacity) {
    GEN_ALGO map;
    GEN_NAME(init)(&map, capacity);
    return map;
}


/**
 * @brief Free a map
 * @param map The map
**/
inline void GEN_NAME(free)(GEN_ALGO* map) {
    free(map->items);
    free(map->buckets);
}


/**
 * @brief Search an item in a bucket (undefined if not found)
 * @param map The map
 * @param index Start index
 * @param key Key of the item
 * @return Pointer to the value of the item (usable until next added item)
**/
GEN_TYPE* GEN_NAME_(search)(GEN_ALGO* map, GEN_SIZE index, GEN_KEY key);


/**
 * @brief Grow a map if it is full
 * @param map The map
**/
void GEN_NAME_(grow)(GEN_ALGO* map);


/**
 * @brief Add an item with no value in a bucket
 * @param map The map
 * @param bucket Index of the bucket
 * @return Pointer to the item (usable until next added item)
**/
GEN_NAME(item)* GEN_NAME_(addEmpty)(GEN_ALGO* map, GEN_SIZE bucket);


/**
 * @brief Get an item in a map
 * @param map The map
 * @param key Key of the item
 * @return Pointer to the value of the item (usable until next added item), NULL if not found
**/
inline GEN_TYPE* GEN_NAME(ref)(GEN_ALGO* map, GEN_KEY key) {
    GEN_SIZE index = map->buckets[MAP_INDEX(MAP_HASH(key), map->mask)];
    return GEN_NAME_(search)(map, index, key);
}


/**
 * @brief Get an item in a map or create it if not found
 * @param map The map
 * @param key Key of the item
 * @return Pointer to the value of the item (usable until next added item)
**/
inline GEN_TYPE* GEN_NAME(refOrEmpty)(GEN_ALGO* map, GEN_KEY key) {
    if (map->length >= map->mask >> 1) GEN_NAME_(grow)(map);
    GEN_SIZE bucket = MAP_INDEX(MAP_HASH(key), map->mask);
    GEN_TYPE* pValue = GEN_NAME_(search)(map, map->buckets[bucket], key);
    if (pValue == NULL) {
        GEN_NAME(item)* item = GEN_NAME_(addEmpty)(map, bucket);
        item->key = key;
        return &item->value;
    }
    return pValue;
}


/**
 * @brief Get an item in a map or create it with a default value if not found
 * @param map The map
 * @param key Key of the item
 * @param value Default value
 * @return Pointer to the value of the item (usable until next added item)
**/
inline GEN_TYPE* GEN_NAME(refOrDefault)(GEN_ALGO* map, GEN_KEY key, GEN_TYPE value) {
    if (map->length >= map->mask >> 1) GEN_NAME_(grow)(map);
    GEN_SIZE bucket = MAP_INDEX(MAP_HASH(key), map->mask);
    GEN_TYPE* pValue = GEN_NAME_(search)(map, map->buckets[bucket], key);
    if (pValue == NULL) {
        GEN_NAME(item)* item = GEN_NAME_(addEmpty)(map, bucket);
        item->key = key;
        item->value = value;
    }
    return pValue;
}


/**
 * @brief Check whether a map contains a key
 * @param map The map
 * @param key The key
 * @return true if the key was found, false otherwise
**/
inline bool GEN_NAME(contains)(GEN_ALGO* map, GEN_KEY key) {
    return GEN_NAME(ref)(map, key) != NULL;
}


/**
 * @brief Get the value associated with a key in a map if found (undefined otherwise)
 * @param map The map
 * @param key The key
 * @return The value
**/
inline GEN_TYPE GEN_NAME(get)(GEN_ALGO* map, GEN_KEY key) {
    return *GEN_NAME(ref)(map, key);
}


/**
 * @brief Get the value associated with a key in a map if found or a default value otherwise
 * @param map The map
 * @param key The key
 * @param value Default value
 * @return The value
**/
inline GEN_TYPE GEN_NAME(getOrDefault)(GEN_ALGO* map, GEN_KEY key, GEN_TYPE value) {
    GEN_TYPE* pValue = GEN_NAME(ref)(map, key);
    return pValue == NULL ? value : *pValue;
}


/**
 * @brief Replace the value associated with a key in a map (undefined if not found)
 * @param map The map
 * @param key The key
 * @param value New value
**/
inline void GEN_NAME(set)(GEN_ALGO* map, GEN_KEY key, GEN_TYPE value) {
    *GEN_NAME(ref)(map, key) = value;
}


/**
 * @brief Add an item in a map (undefined if it already exists)
 * @param map The map
 * @param key Key of the item
 * @param value Value of the item
**/
inline void GEN_NAME(add)(GEN_ALGO* map, GEN_KEY key, GEN_TYPE value) {
    if (map->length >= map->mask >> 1) GEN_NAME_(grow)(map);
    GEN_SIZE bucket = MAP_INDEX(MAP_HASH(key), map->mask);
    GEN_NAME(item)* item = GEN_NAME_(addEmpty)(map, bucket);
    item->key = key;
    item->value = value;
}


/**
 * @brief Replace the value associated with a key in a map if found or create a new item with that value otherwise
 * @param map The map
 * @param key The key
 * @param value New value
**/
inline void GEN_NAME(setOrAdd)(GEN_ALGO* map, GEN_KEY key, GEN_TYPE value) {
    *GEN_NAME(refOrEmpty)(map, key) = value;
}


/**
 * @brief Add an item in a map if it does not already exist
 * @param map The map
 * @param key Key of the item
 * @param value Value of the item
**/
inline void GEN_NAME(tryAdd)(GEN_ALGO* map, GEN_KEY key, GEN_TYPE value) {
    GEN_NAME(refOrDefault)(map, key, value);
}


/**
 * @brief Remove an item in a map if found
 * @param map The map
 * @param key Key of the item
**/
void GEN_NAME(remove)(GEN_ALGO* map, GEN_KEY key);


/**
 * @brief Start iterating on a map
 * @return The iterator
**/
inline GEN_NAME(iter) GEN_NAME(iterStart)(void) {
    return (GEN_NAME(iter)) { .bucket = -1, .index = -1 };
}


/**
 * @brief Get the next item while iterating on a map
 * @param map The map
 * @param iter The iterator
 * @return Pointer to the item, NULL if no more items
**/
inline GEN_NAME(item)* GEN_NAME(iterNext)(GEN_ALGO* map, GEN_NAME(iter)* iter) {
    while (iter->index == -1) {
        iter->bucket++;
        if (iter->bucket > map->mask) return NULL;
        iter->index = map->buckets[iter->bucket];
    }
    GEN_NAME(item)* item = &map->items[iter->index].item;
    iter->index = item->next;
    return item;
}


#ifdef GEN_SOURCE


void GEN_NAME(init)(GEN_ALGO* map, GEN_SIZE capacity);
GEN_ALGO GEN_NAME(new)(GEN_SIZE capacity);
void GEN_NAME(free)(GEN_ALGO* map);
GEN_TYPE* GEN_NAME(ref)(GEN_ALGO* map, GEN_KEY key);
GEN_TYPE* GEN_NAME(refOrEmpty)(GEN_ALGO* map, GEN_KEY key);
GEN_TYPE* GEN_NAME(refOrDefault)(GEN_ALGO* map, GEN_KEY key, GEN_TYPE value);
bool GEN_NAME(contains)(GEN_ALGO* map, GEN_KEY key);
GEN_TYPE GEN_NAME(get)(GEN_ALGO* map, GEN_KEY key);
GEN_TYPE GEN_NAME(getOrDefault)(GEN_ALGO* map, GEN_KEY key, GEN_TYPE value);
void GEN_NAME(set)(GEN_ALGO* map, GEN_KEY key, GEN_TYPE value);
void GEN_NAME(add)(GEN_ALGO* map, GEN_KEY key, GEN_TYPE value);
void GEN_NAME(setOrAdd)(GEN_ALGO* map, GEN_KEY key, GEN_TYPE value);
void GEN_NAME(tryAdd)(GEN_ALGO* map, GEN_KEY key, GEN_TYPE value);
GEN_NAME(iter) GEN_NAME(iterStart)();
GEN_NAME(item)* GEN_NAME(iterNext)(GEN_ALGO* map, GEN_NAME(iter)* iter);


void GEN_NAME_(grow)(GEN_ALGO* map) {
    GEN_SIZE oldCapacity = map->mask + 1;
    GEN_SIZE capacity = oldCapacity << 1;
    map->mask = capacity - 1;
    map->items = THROW_PN(realloc(map->items, sizeof(GEN_NAME(reusable)) * oldCapacity), map->items);
    GEN_SIZE* newBuckets = THROW_PN(malloc(sizeof(GEN_SIZE) * capacity), newBuckets);
    memset(newBuckets, -1, sizeof(GEN_SIZE) * capacity);
    for (GEN_SIZE i = 0; i < oldCapacity; i++) { // Rebuild linked lists in newBuckets
        GEN_SIZE index = map->buckets[i];
        while (index != -1) {
            GEN_NAME(item)* item = &map->items[index].item;
            GEN_SIZE newBucket = MAP_INDEX(MAP_HASH(item->key), map->mask);
            if (newBuckets[newBucket] == -1) newBuckets[newBucket] = index;
            else { // Add at the end
                GEN_NAME(item)* prevItem = &map->items[newBuckets[newBucket]].item;
                while (prevItem->next != -1) prevItem = &map->items[prevItem->next].item;
                prevItem->next = index;
            }
            index = item->next;
            item->next = -1;
        }
    }
    free(map->buckets);
    map->buckets = newBuckets;
}


GEN_NAME(item)* GEN_NAME_(addEmpty)(GEN_ALGO* map, GEN_SIZE bucket) {
    GEN_SIZE i;
    if (map->reusable != -1) {
        i = map->reusable;
        map->reusable = map->items[i].reusable;
    }
    else i = map->length;
    map->length++;
    GEN_NAME(item)* item = &map->items[i].item;
    item->next = map->buckets[bucket];
    map->buckets[bucket] = i;
    return item;
}


GEN_TYPE* GEN_NAME_(search)(GEN_ALGO* map, GEN_SIZE index, GEN_KEY key) {
    while (index != -1) { // Search in linked list
        GEN_NAME(item)* item = &map->items[index].item;
        if (GEN_EQUALS(key, item->key)) return &item->value;
        index = item->next;
    }
    return NULL;
}


void GEN_NAME(remove)(GEN_ALGO* map, GEN_KEY key) {
    GEN_SIZE bucket = MAP_INDEX(MAP_HASH(key), map->mask);
    GEN_SIZE* index = map->buckets + bucket;
    while (*index != -1) {
        GEN_NAME(reusable)* reusable = map->items + *index;
        if (GEN_EQUALS(key, reusable->item.key)) {
            GEN_SIZE next = reusable->item.next;
            reusable->reusable = map->reusable;
            map->reusable = *index;
            map->length--;
            *index = next;
            break;
        }
        index = &reusable->item.next;
    }
}

#endif


// Undef parameters for later use
#undef MAP_HASH
#undef MAP_INDEX

#include "generic_end.h"