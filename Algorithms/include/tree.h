#define GEN_FUNC_PREFIX tree
#define GEN_STRUCT_PREFIX Tree
#define GEN_KV
#include "generic_start.h"

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include "throw.h"

// Additional parameters :
// TREE_SIZE : Whether subtrees contain their size (required to count the size of a range without iterating) (default: false)

// Stack size
#define TREE_STACK (2 * (sizeof(GEN_SIZE) * CHAR_BIT - 1))


/**
 * @brief Tree item
 * @param key Key of the item
 * @param value Value of the item
 * @param children Left and right child, highest bit used for link color
 * children[0] can also be the index of the next reusable item if the item is reusable
 * @param size Number of items in the subtree starting at this node if [TREE_SIZE]
**/
typedef struct {
    GEN_SIZE children[2];
    GEN_KV_TYPE kv;
#ifdef TREE_SIZE
    GEN_SIZE size;
#endif
} GEN_STRUCT_(Item);

#define TREE_RED ((GEN_SIZE)1 << (sizeof(GEN_SIZE) * CHAR_BIT - 1))

// Stack frame containing the index of an item and the direction to the next item
#define TREE_FRAME(index, dir) ((index) | (dir) << (sizeof(GEN_SIZE) * CHAR_BIT - 1))
#define TREE_INDEX(frame) ((frame) & ~TREE_RED)
#define TREE_DIR(frame) ((frame) >> (sizeof(GEN_SIZE) * CHAR_BIT - 1) & 1)


/**
 * @brief Red-black binary search tree
 * @param items Items in the tree, first item points to the root
 * @param root Index of the root
 * @param length Number of items in the tree
 * @param capacity Size of [items]
 * @param reusable Index of the first reusable item in [items]
**/
typedef struct {
    GEN_STRUCT_(Item)* items;
    GEN_SIZE length;
    GEN_SIZE capacity;
    GEN_SIZE reusable;
} GEN_ALGO;


/**
 * @brief Tree iterator
**/
typedef struct {
    GEN_SIZE stack[TREE_STACK];
    int top;
} GEN_STRUCT(Iter);


/**
 * @brief Initialize a tree
 * @param tree The tree
 * @param capacity Initial capacity (must be a power of 2)
**/
inline void GEN_FUNC(init)(GEN_ALGO* tree, GEN_SIZE capacity) {
    tree->items = THROW_PN(malloc(sizeof(GEN_STRUCT_(Item)) * capacity), tree->items);
    tree->items[0] = (GEN_STRUCT_(Item)) {
        .children[1] = 0,
#ifdef TREE_SIZE
        .size = 0
#endif
    };
    tree->length = 0;
    tree->capacity = capacity;
    tree->reusable = -1;
}


/**
 * @brief Create a tree
 * @param capacity Initial capacity (must be a power of 2)
 * @return The tree
**/
inline GEN_ALGO GEN_FUNC(new)(GEN_SIZE capacity) {
    GEN_ALGO tree;
    GEN_FUNC(init)(&tree, capacity);
    return tree;
}


/**
 * @brief Free a tree
 * @param tree The tree
**/
inline void GEN_FUNC(free)(GEN_ALGO* tree) {
    free(tree->items);
}


#ifndef GEN_NO_VALUE
/**
 * @brief Get an item in a tree
 * @param tree The tree
 * @param key Key of the item
 * @return Pointer to the value of the item (usable until next added item), NULL if not found
**/
GEN_TYPE* GEN_FUNC(ref)(GEN_ALGO* tree, GEN_KEY key);


/**
 * @brief Get an item in a tree or create it if not found
 * @param tree The tree
 * @param key Key of the item
 * @param added Whether a new item was added (NULL to ignore)
 * @return Pointer to the value of the item (usable until next added item)
**/
GEN_TYPE* GEN_FUNC(refOrEmpty)(GEN_ALGO* tree, GEN_KEY key, bool* added);


/**
 * @brief Get an item in a tree or create it with a default value if not found
 * @param tree The tree
 * @param key Key of the item
 * @param value Default value
 * @param added Whether a new item was added (NULL to ignore)
 * @return Pointer to the value of the item (usable until next added item)
**/
inline GEN_TYPE* GEN_FUNC(refOrDefault)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value, bool* added) {
    bool _added;
    GEN_TYPE* pValue = GEN_FUNC(refOrEmpty)(tree, key, &_added);
    if (_added) *pValue = value;
    if (added) *added = _added;
    return pValue;
}


/**
 * @brief Get the value associated with a key in a tree if found (undefined otherwise)
 * @param tree The tree
 * @param key The key
 * @return The value
**/
inline GEN_TYPE GEN_FUNC(get)(GEN_ALGO* tree, GEN_KEY key) {
    return *GEN_FUNC(ref)(tree, key);
}


/**
 * @brief Get the value associated with a key in a tree if found or a default value otherwise
 * @param tree The tree
 * @param key The key
 * @param value Default value
 * @return The value
**/
inline GEN_TYPE GEN_FUNC(getOrDefault)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value) {
    GEN_TYPE* pValue = GEN_FUNC(ref)(tree, key);
    return pValue == NULL ? value : *pValue;
}


/**
 * @brief Replace the value associated with a key in a tree if found or create a new item with that value otherwise
 * @param tree The tree
 * @param key Key of the item
 * @param value Value of the item
 * @return Whether a new item was added
**/
inline bool GEN_FUNC(setOrAdd)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value) {
    bool added;
    *GEN_FUNC(refOrEmpty)(tree, key, &added) = value;
    return added;
}
#endif


/**
 * @brief Check whether a tree contains a key
 * @param tree The tree
 * @param key The key
 * @return Whether the key was found
**/
#ifdef GEN_NO_VALUE
bool GEN_FUNC(contains)(GEN_ALGO* tree, GEN_KEY key);
#else
inline bool GEN_FUNC(contains)(GEN_ALGO* tree, GEN_KEY key) {
    return GEN_FUNC(ref)(tree, key) != NULL;
}
#endif


/**
 * @brief Add an item in a tree if it does not already exist
 * @param tree The tree
 * @param key Key of the item
 * @param value Value of the item
 * @return Whether a new item was added
**/
#ifdef GEN_NO_VALUE
bool GEN_FUNC(tryAdd)(GEN_ALGO* tree, GEN_KEY key);
#else
inline bool GEN_FUNC(tryAdd)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value) {
    bool added;
    GEN_FUNC(refOrDefault)(tree, key, value, &added);
    return added;
}
#endif


/**
 * @brief Remove an item in a tree if found
 * @param tree The tree
 * @param key Key of the item
**/
GEN_IF_VALUE(GEN_TYPE*, bool) GEN_FUNC(remove)(GEN_ALGO* tree, GEN_KEY key);


/**
 * @brief Start iterating on a tree
 * @return The iterator
**/
inline GEN_STRUCT(Iter) GEN_FUNC(iterAll)(void) {
    GEN_STRUCT(Iter) iter;
    iter.top = 0;
    iter.stack[0] = 0;
    return iter;
}


/**
 * @brief Start iterating on a sub-tree
 * @param tree The tree
 * @param start Start key of the sub-tree (inclusive)
 * @return The iterator
**/
inline GEN_STRUCT(Iter) GEN_FUNC(iterAfter)(GEN_ALGO* tree, GEN_KEY start) {
    GEN_STRUCT(Iter) iter;
    iter.top = 0;
    int top = -1;
    GEN_SIZE prevIndex = 0;
    GEN_SIZE index = tree->items[0].children[1];
    while (index != 0) {
        index &= ~TREE_RED;
        GEN_STRUCT_(Item)* item = tree->items + index;
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(start, GEN_KV_KEY(item->kv));
        if (cmp <= 0) iter.stack[++top] = index;
        if (cmp > 0) {
            iter.top = top + 1;
            prevIndex = index;
        }
        index = item->children[cmp > 0];
    }
    iter.stack[iter.top] = prevIndex;
    return iter;
}


/**
 * @brief Start iterating on a sub-tree
 * @param tree The tree
 * @param end End key of the sub-tree (exclusive)
 * @return The iterator
**/
inline GEN_STRUCT(Iter) GEN_FUNC(iterBefore)(GEN_ALGO* tree, GEN_KEY end) {
    return GEN_FUNC(iterAll)();
}


/**
 * @brief Start iterating on a sub-tree
 * @param tree The tree
 * @param start Start key of the sub-tree (inclusive)
 * @param end End key of the sub-tree (exclusive)
 * @return The iterator
**/
inline GEN_STRUCT(Iter) GEN_FUNC(iterBetween)(GEN_ALGO* tree, GEN_KEY start, GEN_KEY end) {
    return GEN_FUNC(iterAfter)(tree, start);
}


/**
 * @brief Get the next item while iterating on a tree
 * @param tree The tree
 * @param iter The iterator
 * @return Next item, NULL if no more items
**/
inline GEN_KV_TYPE* GEN_FUNC(nextAll)(GEN_ALGO* tree, GEN_STRUCT(Iter)* iter) {
    GEN_STRUCT_(Item)* item = tree->items + iter->stack[iter->top--];
    if (item->children[1] != 0) {
        GEN_SIZE index = item->children[1];
        do {
            index &= ~TREE_RED;
            item = tree->items + index;
            iter->stack[++iter->top] = index;
            index = item->children[0];
        } while (index != 0);
        return &item->kv;
    }
    else if (iter->top >= 0) return &tree->items[iter->stack[iter->top]].kv;
    return NULL;
}


/**
 * @brief Get the next item while iterating on a sub-tree
 * @param tree The tree
 * @param start Start key of the sub-tree (inclusive)
 * @param iter The iterator
 * @return Next item, NULL if no more items
**/
inline GEN_KV_TYPE* GEN_FUNC(nextAfter)(GEN_ALGO* tree, GEN_KEY start, GEN_STRUCT(Iter)* iter) {
    return GEN_FUNC(nextAll)(tree, iter);
}


/**
 * @brief Get the next item while iterating on a sub-tree
 * @param tree The tree
 * @param end End key of the sub-tree (exclusive)
 * @param iter The iterator
 * @return Next item, NULL if no more items
**/
inline GEN_KV_TYPE* GEN_FUNC(nextBefore)(GEN_ALGO* tree, GEN_KEY end, GEN_STRUCT(Iter)* iter) {
    GEN_KV_TYPE* kv = GEN_FUNC(nextAll)(tree, iter);
    if (kv == NULL || GEN_COMPARE(GEN_KV_KEY(*kv), end) >= 0) return NULL;
    return kv;
}


/**
 * @brief Get the next item while iterating on a sub-tree
 * @param tree The tree
 * @param start Start key of the sub-tree (inclusive)
 * @param end End key of the sub-tree (exclusive)
 * @param iter The iterator
 * @return Next item, NULL if no more items
**/
inline GEN_KV_TYPE* GEN_FUNC(nextBetween)(GEN_ALGO* tree, GEN_KEY start, GEN_KEY end, GEN_STRUCT(Iter)* iter) {
    return GEN_FUNC(nextBefore)(tree, end, iter);
}


/**
 * @brief Get the closest item with a key smaller or equal to a given key in a tree
 * @param tree The tree
 * @param key The key
 * @return Pointer to the item (usable until next added item), NULL if none
**/
GEN_KV_TYPE* GEN_FUNC(floor)(GEN_ALGO* tree, GEN_KEY key);


/**
 * @brief Get the closest item with a key greater than a given key in a tree
 * @param tree The tree
 * @param key The key
 * @return Pointer to the item (usable until next added item), NULL if none
**/
GEN_KV_TYPE* GEN_FUNC(ceil)(GEN_ALGO* tree, GEN_KEY key);


#ifdef TREE_SIZE

GEN_SIZE GEN_FUNC_(countAfter)(GEN_ALGO* tree, GEN_KEY start, GEN_SIZE index);
GEN_SIZE GEN_FUNC_(countBefore)(GEN_ALGO* tree, GEN_KEY end, GEN_SIZE index);


/**
 * @brief Count the number of items in a sub-tree
 * @param tree The tree
 * @param start Start key of the sub-tree (inclusive)
 * @return Number of items in the sub-tree
**/
inline GEN_SIZE GEN_FUNC(countAfter)(GEN_ALGO* tree, GEN_KEY start) {
    return GEN_FUNC_(countAfter)(tree, start, tree->items[0].children[1]);
}


/**
 * @brief Count the number of items in a sub-tree
 * @param tree The tree
 * @param end End key of the sub-tree (exclusive)
 * @return Number of items in the sub-tree
**/
inline GEN_SIZE GEN_FUNC(countBefore)(GEN_ALGO* tree, GEN_KEY end) {
    return GEN_FUNC_(countBefore)(tree, end, tree->items[0].children[1]);
}


/**
 * @brief Count the number of items in a sub-tree
 * @param tree The tree
 * @param start Start key of the sub-tree (inclusive)
 * @param end End key of the sub-tree (exclusive)
 * @return Number of items in the sub-tree
**/
GEN_SIZE GEN_FUNC(countBetween)(GEN_ALGO* tree, GEN_KEY start, GEN_KEY end);

#endif


#ifdef GEN_SOURCE


void GEN_FUNC(init)(GEN_ALGO* tree, GEN_SIZE capacity);
GEN_ALGO GEN_FUNC(new)(GEN_SIZE capacity);
void GEN_FUNC(free)(GEN_ALGO* tree);
void GEN_FUNC_(grow)(GEN_ALGO* tree);
#ifndef GEN_NO_VALUE
GEN_TYPE* GEN_FUNC(refOrDefault)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value, bool* added);
GEN_TYPE GEN_FUNC(get)(GEN_ALGO* tree, GEN_KEY key);
GEN_TYPE GEN_FUNC(getOrDefault)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value);
bool GEN_FUNC(setOrAdd)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value);
bool GEN_FUNC(contains)(GEN_ALGO* tree, GEN_KEY key);
bool GEN_FUNC(tryAdd)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value);
#endif
GEN_STRUCT(Iter) GEN_FUNC(iterAll)();
GEN_STRUCT(Iter) GEN_FUNC(iterAfter)(GEN_ALGO* tree, GEN_KEY start);
GEN_STRUCT(Iter) GEN_FUNC(iterBefore)(GEN_ALGO* tree, GEN_KEY end);
GEN_STRUCT(Iter) GEN_FUNC(iterBetween)(GEN_ALGO* tree, GEN_KEY start, GEN_KEY end);
GEN_KV_TYPE* GEN_FUNC(nextAll)(GEN_ALGO* tree, GEN_STRUCT(Iter)* iter);
GEN_KV_TYPE* GEN_FUNC(nextAfter)(GEN_ALGO* tree, GEN_KEY start, GEN_STRUCT(Iter)* iter);
GEN_KV_TYPE* GEN_FUNC(nextBefore)(GEN_ALGO* tree, GEN_KEY end, GEN_STRUCT(Iter)* iter);
GEN_KV_TYPE* GEN_FUNC(nextBetween)(GEN_ALGO* tree, GEN_KEY start, GEN_KEY end, GEN_STRUCT(Iter)* iter);
#ifdef TREE_SIZE
GEN_SIZE GEN_FUNC(countAfter)(GEN_ALGO* tree, GEN_KEY start);
GEN_SIZE GEN_FUNC(countBefore)(GEN_ALGO* tree, GEN_KEY end);
#endif


inline void GEN_FUNC_(grow)(GEN_ALGO* tree) {
    if (tree->length + 2 >= tree->capacity) {
        tree->capacity <<= 1;
        tree->items = THROW_PN(realloc(tree->items, sizeof(GEN_STRUCT_(Item)) * tree->capacity), tree->items);
    }
}


// Maintain tree invariant after adding an item
static inline void GEN_FUNC_(maintainAdd)(GEN_ALGO* tree, GEN_SIZE* stack, GEN_SIZE* top) {
    while (top - 2 > stack) {
        GEN_SIZE parentIndex = TREE_INDEX(top[-2]);
        GEN_SIZE dir = TREE_DIR(top[-2]);
        GEN_SIZE invDir = dir ^ 1;
        GEN_STRUCT_(Item)* parent = tree->items + parentIndex;
        if (!(parent->children[dir] & TREE_RED)) break;
        top -= 2;
        GEN_SIZE nodeIndex = TREE_INDEX(top[1]);
        GEN_STRUCT_(Item)* node = tree->items + nodeIndex;
        GEN_SIZE* pParentIndex = &tree->items[TREE_INDEX(top[-1])].children[TREE_DIR(top[-1])];

        if (parent->children[invDir] & TREE_RED) { // Swap colors and continue
            *pParentIndex |= TREE_RED;
            parent->children[0] &= ~TREE_RED;
            parent->children[1] &= ~TREE_RED;
#ifdef TREE_SIZE
            node->size++;
            parent->size++;
#endif
            continue;
        }
        
        // Rotate
        if (dir == TREE_DIR(top[1])) {
            *pParentIndex = nodeIndex & ~TREE_RED;
            parent->children[dir] = node->children[invDir];
            node->children[invDir] = parentIndex | TREE_RED;
#ifdef TREE_SIZE
            node->size = 1 + parent->size;
            parent->size -= tree->items[node->children[dir] & ~TREE_RED].size;
#endif
        }
        else {
            GEN_SIZE childIndex = node->children[invDir] & ~TREE_RED;
            GEN_STRUCT_(Item)* child = tree->items + childIndex;
            *pParentIndex = childIndex;
            parent->children[dir] = child->children[invDir];
            child->children[invDir] = parentIndex | TREE_RED;
            node->children[invDir] = child->children[dir];
            child->children[dir] = nodeIndex | TREE_RED;
#ifdef TREE_SIZE
            child->size = 1 + parent->size;
            node->size -= tree->items[parent->children[dir]].size;
            parent->size -= node->size;
#endif
        }
        break;
    }
#ifdef TREE_SIZE
    while (--top > stack) {
        tree->items[TREE_INDEX(*top)].size++;
    }
#endif
}


// Maintain tree invariant after removing an item
static inline GEN_SIZE* GEN_FUNC_(maintainRemove)(GEN_ALGO* tree, GEN_SIZE* stack, GEN_SIZE* top) {
    while (--top > stack) {
        GEN_SIZE parentIndex = TREE_INDEX(*top);
        GEN_SIZE dir = TREE_DIR(*top);
        GEN_SIZE invDir = dir ^ 1;
        GEN_STRUCT_(Item)* parent = tree->items + parentIndex;
        GEN_SIZE siblingIndex = parent->children[invDir] & ~TREE_RED;
        GEN_STRUCT_(Item)* sibling = tree->items + siblingIndex;
        GEN_SIZE* pParentIndex = &tree->items[TREE_INDEX(top[-1])].children[TREE_DIR(top[-1])];
        
        if (parent->children[invDir] & TREE_RED) { // Rotate and continue
            *pParentIndex = siblingIndex;
            parent->children[invDir] = sibling->children[dir];
            sibling->children[dir] = parentIndex | TREE_RED;
#ifdef TREE_SIZE
            sibling->size = parent->size;
            parent->size -= 1 + tree->items[sibling->children[invDir]].size;
#endif
            pParentIndex = &sibling->children[dir];
            siblingIndex = parent->children[invDir];
            sibling = tree->items + siblingIndex;
        }
        
        if (sibling->children[invDir] & TREE_RED) { // Rotate
            *pParentIndex = siblingIndex | (*pParentIndex & TREE_RED);
            parent->children[invDir] = sibling->children[dir];
            sibling->children[dir] = parentIndex;
            sibling->children[invDir] &= ~TREE_RED;
#ifdef TREE_SIZE
            sibling->size = parent->size - 1;
            parent->size -= 2 + tree->items[sibling->children[invDir]].size;
#endif
            break;
        }
        
        if (sibling->children[dir] & TREE_RED) { // Rotate
            GEN_SIZE siblingChildIndex = sibling->children[dir] & ~TREE_RED;
            GEN_STRUCT_(Item)* siblingChild = tree->items + siblingChildIndex;
            *pParentIndex = siblingChildIndex | (*pParentIndex & TREE_RED);
            parent->children[invDir] = siblingChild->children[dir];
            siblingChild->children[dir] = parentIndex;
            sibling->children[dir] = siblingChild->children[invDir];
            siblingChild->children[invDir] = siblingIndex;
#ifdef TREE_SIZE
            siblingChild->size = parent->size - 1;
            sibling->size -= 1 + tree->items[parent->children[invDir]].size;
            parent->size -= 2 + sibling->size;
#endif
            break;
        }
        
        // Swap colors and continue if parent is black
        parent->children[invDir] |= TREE_RED;
#ifdef TREE_SIZE
        parent->size--;
#endif
        if (*pParentIndex & TREE_RED) {
            *pParentIndex &= ~TREE_RED;
            break;
        }
    }
    return top;
}


#ifdef GEN_NO_VALUE
bool GEN_FUNC(contains)(GEN_ALGO* tree, GEN_KEY key) {
#else
GEN_TYPE* GEN_FUNC(ref)(GEN_ALGO* tree, GEN_KEY key) {
#endif
    GEN_SIZE index = tree->items[0].children[1];
    while (index != 0) {
        GEN_STRUCT_(Item)* item = tree->items + (index & ~TREE_RED);
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(key, GEN_KV_KEY(item->kv));
        if (cmp == 0) {
#ifdef GEN_NO_VALUE
            return true;
#else
            return &item->kv.value;
#endif
        }
        index = item->children[cmp > 0];
    }
    return 0;
}


#ifdef GEN_NO_VALUE
bool GEN_FUNC(tryAdd)(GEN_ALGO* tree, GEN_KEY key) {
#else
GEN_TYPE* GEN_FUNC(refOrEmpty)(GEN_ALGO* tree, GEN_KEY key, bool* added) {
#endif
    GEN_FUNC_(grow)(tree);
    GEN_SIZE stack[TREE_STACK];
    stack[0] = TREE_FRAME(0, 1);
    GEN_SIZE* top = stack + 1;

    // Find item
    GEN_STRUCT_(Item)* item = tree->items;
    GEN_SIZE dir = 1;
    GEN_SIZE index = item->children[1];
    while (index != 0) {
        index &= ~TREE_RED;
        item = tree->items + index;
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(key, GEN_KV_KEY(item->kv));
        if (cmp == 0) {
#ifdef GEN_NO_VALUE
            return false;
#else
            if (added) *added = false;
            return &item->kv.value;
#endif
        }
        dir = cmp > 0;
        *top++ = TREE_FRAME(index, dir);
        index = item->children[dir];
    }
    
    // Add item
    tree->length++;
    GEN_SIZE i;
    if (tree->reusable != -1) {
        i = tree->reusable;
        tree->reusable = tree->items[i].children[0];
    }
    else i = tree->length;
    GEN_STRUCT_(Item)* child = tree->items + i;
    GEN_KV_KEY(child->kv) = key;
    child->children[0] = 0;
    child->children[1] = 0;
#ifdef TREE_SIZE
    child->size = 1;
#endif
    item->children[dir] = i | TREE_RED;
    GEN_FUNC_(maintainAdd)(tree, stack, top);
#ifdef GEN_NO_VALUE
    return true;
#else
    if (added) *added = true;
    return &child->kv.value;
#endif
}


GEN_IF_VALUE(GEN_TYPE*, bool) GEN_FUNC(remove)(GEN_ALGO* tree, GEN_KEY key) {
    GEN_SIZE stack[TREE_STACK];
    GEN_SIZE* top = stack;

    // Find item
    GEN_SIZE index = 0, nextIndex;
    GEN_STRUCT_(Item)* item = tree->items;
    GEN_COMPARE_TYPE cmp = 0;
    GEN_SIZE dir = 1;
    do {
        *top++ = TREE_FRAME(index, dir);
        nextIndex = item->children[dir];
        if (nextIndex == 0) return 0;
        index = nextIndex & ~TREE_RED;
        item = tree->items + index;
        cmp = GEN_COMPARE(key, GEN_KV_KEY(item->kv));
        dir = cmp > 0;
    } while (cmp != 0);
    
    // Replace item by successor if more than one child
    GEN_SIZE removedIndex = index;
    GEN_SIZE* removedParent = &tree->items[TREE_INDEX(top[-1])].children[TREE_DIR(top[-1])];
    bool mustFix;
    if (item->children[0] == 0) {
        mustFix = !(*removedParent & TREE_RED) && !(item->children[1] & TREE_RED);
        *removedParent = item->children[1] & ~TREE_RED;
    }
    else if (item->children[1] == 0) {
        mustFix = !(*removedParent & TREE_RED) && !(item->children[0] & TREE_RED);
        *removedParent = item->children[0] & ~TREE_RED;
    }
    else {
        GEN_STRUCT_(Item)* removedItem = item;
        GEN_SIZE* removedFrame = top;
        nextIndex = item->children[1];
        do {
            *top++ = TREE_FRAME(index, 0);
            index = nextIndex & ~TREE_RED;
            item = tree->items + index;
            nextIndex = item->children[0];
        } while (nextIndex != 0);
        *removedFrame = TREE_FRAME(TREE_INDEX(*removedFrame), 1);
        GEN_SIZE* pIndex = &tree->items[TREE_INDEX(top[-1])].children[TREE_DIR(top[-1])];
        *removedFrame = TREE_FRAME(index, 1);
        mustFix = !(*pIndex & TREE_RED) && !(item->children[1] & TREE_RED);
        *pIndex = item->children[1] & ~TREE_RED;
        item->children[0] = removedItem->children[0];
        item->children[1] = removedItem->children[1];
#ifdef TREE_SIZE
        item->size = removedItem->size;
#endif
        *removedParent = index | (*removedParent & TREE_RED);
    }

    // Remove and maintain
    tree->items[removedIndex].children[0] = tree->reusable;
    tree->reusable = removedIndex;
    tree->length--;
    if (mustFix) top = GEN_FUNC_(maintainRemove)(tree, stack, top);
#ifdef TREE_SIZE
    while (--top > stack) {
        tree->items[TREE_INDEX(*top)].size--;
    }
#endif

    return GEN_IF_VALUE(&tree->items[removedIndex].kv.value, true);
}


GEN_KV_TYPE* GEN_FUNC(floor)(GEN_ALGO* tree, GEN_KEY key) {
    GEN_STRUCT_(Item)* floor = NULL;
    GEN_SIZE index = tree->items[0].children[1];
    while (index != 0) {
        GEN_STRUCT_(Item)* item = tree->items + (index & ~TREE_RED);
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(key, GEN_KV_KEY(item->kv));
        if (cmp < 0) index = item->children[0];
        else {
            floor = item;
            index = item->children[1];
        }
    }
    return floor ? &floor->kv : NULL;
}


GEN_KV_TYPE* GEN_FUNC(ceil)(GEN_ALGO* tree, GEN_KEY key) {
    GEN_STRUCT_(Item)* ceil = NULL;
    GEN_SIZE index = tree->items[0].children[1];
    while (index != 0) {
        GEN_STRUCT_(Item)* item = tree->items + (index & ~TREE_RED);
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(key, GEN_KV_KEY(item->kv));
        if (cmp > 0) index = item->children[1];
        else {
            ceil = item;
            index = item->children[0];
        }
    }
    return ceil ? &ceil->kv : NULL;
}


#ifdef TREE_SIZE

GEN_SIZE GEN_FUNC_(countAfter)(GEN_ALGO* tree, GEN_KEY start, GEN_SIZE index) {
    GEN_SIZE count = 0;
    while (index != 0) {
        GEN_STRUCT_(Item)* item = tree->items + (index & ~TREE_RED);
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(start, GEN_KV_KEY(item->kv));
        if (cmp > 0) index = item->children[1];
        else {
            count += 1 + tree->items[item->children[1] & ~TREE_RED].size;
            index = item->children[0];
        }
    }
    return count;
}


GEN_SIZE GEN_FUNC_(countBefore)(GEN_ALGO* tree, GEN_KEY end, GEN_SIZE index) {
    GEN_SIZE count = 0;
    while (index != 0) {
        GEN_STRUCT_(Item)* item = tree->items + (index & ~TREE_RED);
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(end, GEN_KV_KEY(item->kv));
        if (cmp <= 0) index = item->children[0];
        else {
            count += 1 + tree->items[item->children[0] & ~TREE_RED].size;
            index = item->children[1];
        }
    }
    return count;
}


GEN_SIZE GEN_FUNC(countBetween)(GEN_ALGO* tree, GEN_KEY start, GEN_KEY end) {
    GEN_SIZE index = tree->items[0].children[1];
    while (index != 0) {
        GEN_STRUCT_(Item)* item = tree->items + (index & ~TREE_RED);
        GEN_COMPARE_TYPE cmpStart = GEN_COMPARE(start, GEN_KV_KEY(item->kv));
        GEN_COMPARE_TYPE cmpEnd = GEN_COMPARE(end, GEN_KV_KEY(item->kv));
        if (cmpStart > 0) index = item->children[1];
        else if (cmpEnd < 0) index = item->children[0];
        else return 1 + 
            GEN_FUNC_(countAfter)(tree, start, item->children[0]) +
            GEN_FUNC_(countBefore)(tree, end, item->children[1]);
    }
    return 0;
}

#endif


#endif

// Undef parameters for later use
#undef TREE_SIZE
#undef TREE_STACK
#undef TREE_RED
#undef TREE_FRAME
#undef TREE_INDEX
#undef TREE_DIR

#include "generic_end.h"