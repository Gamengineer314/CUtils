#define GEN_PREFIX tree
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
 * @param child1 Left child, highest bit used for link color
 * @param child2 Right child, highest bit used for link color
 * @param size Number of items in the subtree starting at this node if [TREE_SIZE]
**/
typedef struct {
    GEN_SIZE children[2];
    GEN_KEY key;
    GEN_TYPE value;
#ifdef TREE_SIZE
    GEN_SIZE size;
#endif
} GEN_NAME(item);

#define TREE_RED ((GEN_SIZE)1 << (sizeof(GEN_SIZE) * CHAR_BIT - 1))


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
 * @brief Red-black binary search tree
 * @param items Items in the tree, first item points to the root
 * @param root Index of the root
 * @param length Number of items in the tree
 * @param capacity Size of [items]
 * @param reusable Index of the first reusable item in [items]
**/
typedef struct {
    GEN_NAME(reusable)* items;
    GEN_SIZE length;
    GEN_SIZE capacity;
    GEN_SIZE reusable;
} GEN_ALGO;


/**
 * @brief Index and direction
 * @param index Index of the item
 * @param dir dir to the next item
**/
typedef struct {
    GEN_SIZE index;
    GEN_SIZE dir;
} GEN_NAME_(dir);


/**
 * @brief Tree iterator
**/
typedef struct {
    GEN_SIZE stack[TREE_STACK];
    int top;
} GEN_NAME(iter);


/**
 * @brief Initialize a tree
 * @param tree The tree
 * @param capacity Initial capacity (must be a power of 2)
**/
inline void GEN_NAME(init)(GEN_ALGO* tree, GEN_SIZE capacity) {
    tree->items = THROW_PN(malloc(sizeof(GEN_NAME(reusable)) * capacity), tree->items);
    tree->items[0].item = (GEN_NAME(item)) {
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
inline GEN_ALGO GEN_NAME(new)(GEN_SIZE capacity) {
    GEN_ALGO tree;
    GEN_NAME(init)(&tree, capacity);
    return tree;
}


/**
 * @brief Free a tree
 * @param tree The tree
**/
inline void GEN_NAME(free)(GEN_ALGO* tree) {
    free(tree->items);
}


/**
 * @brief Get an item in a tree
 * @param tree The tree
 * @param key Key of the item
 * @return Pointer to the item (usable until next added item), NULL if not found
**/
GEN_NAME(item)* GEN_NAME(ref)(GEN_ALGO* tree, GEN_KEY key);


/**
 * @brief Get an item in a tree or create it if not found
 * @param tree The tree
 * @param key Key of the item
 * @return Pointer to the item (usable until next added item)
**/
GEN_NAME(item)* GEN_NAME(refOrEmpty)(GEN_ALGO* tree, GEN_KEY key);


/**
 * @brief Get an item in a tree or create it with a default value if not found
 * @param tree The tree
 * @param key Key of the item
 * @param value Default value
 * @return Pointer to the item (usable until next added item)
**/
inline GEN_NAME(item)* GEN_NAME(refOrDefault)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value) {
    GEN_SIZE length = tree->length;
    GEN_NAME(item)* item = GEN_NAME(refOrEmpty)(tree, key);
    if (length != tree->length) item->value = value;
    return item;
}


/**
 * @brief Check whether a tree contains a key
 * @param tree The tree
 * @param key The key
 * @return true if the key was found, false otherwise
**/
inline bool GEN_NAME(contains)(GEN_ALGO* tree, GEN_KEY key) {
    return GEN_NAME(ref)(tree, key) != NULL;
}


/**
 * @brief Get the value associated with a key in a tree if found (undefined otherwise)
 * @param tree The tree
 * @param key The key
 * @return The value
**/
inline GEN_TYPE GEN_NAME(get)(GEN_ALGO* tree, GEN_KEY key) {
    return GEN_NAME(ref)(tree, key)->value;
}


/**
 * @brief Get the value associated with a key in a tree if found or a default value otherwise
 * @param tree The tree
 * @param key The key
 * @param value Default value
 * @return The value
**/
inline GEN_TYPE GEN_NAME(getOrDefault)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value) {
    GEN_NAME(item)* item = GEN_NAME(ref)(tree, key);
    return item == NULL ? value : item->value;
}


/**
 * @brief Replace the value associated with a key in a tree if found or create a new item with that value otherwise
 * @param tree The tree
 * @param key Key of the item
 * @param value Value of the item
**/
inline void GEN_NAME(setOrAdd)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value) {
    GEN_NAME(refOrEmpty)(tree, key)->value = value;
}


/**
 * @brief Add an item in a tree if it does not already exist
 * @param tree The tree
 * @param key Key of the item
 * @param value Value of the item
**/
inline void GEN_NAME(tryAdd)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value) {
    GEN_NAME(refOrDefault)(tree, key, value);
}


/**
 * @brief Remove an item in a tree if found
 * @param tree The tree
 * @param key Key of the item
**/
void GEN_NAME(remove)(GEN_ALGO* tree, GEN_KEY key);


/**
 * @brief Start iterating on a tree
 * @return The iterator
**/
inline GEN_NAME(iter) GEN_NAME(iterAll)(void) {
    GEN_NAME(iter) iter;
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
inline GEN_NAME(iter) GEN_NAME(iterAfter)(GEN_ALGO* tree, GEN_KEY start) {
    GEN_NAME(iter) iter;
    iter.top = 0;
    int top = -1;
    GEN_SIZE prevIndex = 0;
    GEN_SIZE index = tree->items[0].item.children[1];
    while (index != 0) {
        index &= ~TREE_RED;
        GEN_NAME(item)* item = &tree->items[index].item;
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(start, item->key);
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
 * @param end End key of the sub-tree (inclusive)
 * @return The iterator
**/
inline GEN_NAME(iter) GEN_NAME(iterBefore)(GEN_ALGO* tree, GEN_KEY end) {
    return GEN_NAME(iterAll)();
}


/**
 * @brief Start iterating on a sub-tree
 * @param tree The tree
 * @param start Start key of the sub-tree (inclusive)
 * @param end End key of the sub-tree (exclusive)
 * @return The iterator
**/
inline GEN_NAME(iter) GEN_NAME(iterBetween)(GEN_ALGO* tree, GEN_KEY start, GEN_KEY end) {
    return GEN_NAME(iterAfter)(tree, start);
}


/**
 * @brief Get the next item while iterating on a tree
 * @param tree The tree
 * @param iter The iterator
 * @return Pointer to the item, NULL if no more items
**/
inline GEN_NAME(item)* GEN_NAME(nextAll)(GEN_ALGO* tree, GEN_NAME(iter)* iter) {
    GEN_NAME(item)* item = &tree->items[iter->stack[iter->top]].item;
    iter->top--;
    if (item->children[1] != 0) {
        GEN_SIZE index = item->children[1];
        do {
            index &= ~TREE_RED;
            item = &tree->items[index].item;
            iter->stack[++iter->top] = index;
            index = item->children[0];
        } while (index != 0);
        return item;
    }
    else if (iter->top >= 0) return &tree->items[iter->stack[iter->top]].item;
    return NULL;
}


/**
 * @brief Get the next item while iterating on a sub-tree
 * @param tree The tree
 * @param start Start key of the sub-tree (inclusive)
 * @param iter The iterator
 * @return Pointer to the item, NULL if no more items
**/
inline GEN_NAME(item)* GEN_NAME(nextAfter)(GEN_ALGO* tree, GEN_KEY start, GEN_NAME(iter)* iter) {
    return GEN_NAME(nextAll)(tree, iter);
}


/**
 * @brief Get the next item while iterating on a sub-tree
 * @param tree The tree
 * @param end End key of the sub-tree (exclusive)
 * @param iter The iterator
 * @return Pointer to the item, NULL if no more items
**/
inline GEN_NAME(item)* GEN_NAME(nextBefore)(GEN_ALGO* tree, GEN_KEY end, GEN_NAME(iter)* iter) {
    GEN_NAME(item)* item = GEN_NAME(nextAll)(tree, iter);
    if (item == NULL || GEN_COMPARE(item->key, end) >= 0) return NULL;
    return item;
}


/**
 * @brief Get the next item while iterating on a sub-tree
 * @param tree The tree
 * @param start Start key of the sub-tree (inclusive)
 * @param end End key of the sub-tree (exclusive)
 * @param iter The iterator
 * @return Pointer to the item, NULL if no more items
**/
inline GEN_NAME(item)* GEN_NAME(nextBetween)(GEN_ALGO* tree, GEN_KEY start, GEN_KEY end, GEN_NAME(iter)* iter) {
    return GEN_NAME(nextBefore)(tree, end, iter);
}


/**
 * @brief Get the closest item with a key smaller or equal to a given key in a tree
 * @param tree The tree
 * @param key The key
**/
GEN_NAME(item)* GEN_NAME(floor)(GEN_ALGO* tree, GEN_KEY key);


/**
 * @brief Get the closest item with a key greater than a given key in a tree
 * @param tree The tree
 * @param key The key
**/
GEN_NAME(item)* GEN_NAME(ceil)(GEN_ALGO* tree, GEN_KEY key);


#ifdef TREE_SIZE

GEN_SIZE GEN_NAME_(countAfter)(GEN_ALGO* tree, GEN_KEY start, GEN_SIZE index);
GEN_SIZE GEN_NAME_(countBefore)(GEN_ALGO* tree, GEN_KEY end, GEN_SIZE index);


/**
 * @brief Count the number of items in a sub-tree
 * @param tree The tree
 * @param start Start key of the sub-tree (inclusive)
 * @return Number of items in the sub-tree
**/
inline GEN_SIZE GEN_NAME(countAfter)(GEN_ALGO* tree, GEN_KEY start) {
    return GEN_NAME_(countAfter)(tree, start, tree->items[0].item.children[1]);
}


/**
 * @brief Count the number of items in a sub-tree
 * @param tree The tree
 * @param end End key of the sub-tree (exclusive)
 * @return Number of items in the sub-tree
**/
inline GEN_SIZE GEN_NAME(countBefore)(GEN_ALGO* tree, GEN_KEY end) {
    return GEN_NAME_(countBefore)(tree, end, tree->items[0].item.children[1]);
}


/**
 * @brief Count the number of items in a sub-tree
 * @param tree The tree
 * @param start Start key of the sub-tree (inclusive)
 * @param end End key of the sub-tree (exclusive)
 * @return Number of items in the sub-tree
**/
GEN_SIZE GEN_NAME(countBetween)(GEN_ALGO* tree, GEN_KEY start, GEN_KEY end);

#endif


#ifdef GEN_SOURCE


void GEN_NAME(init)(GEN_ALGO* tree, GEN_SIZE capacity);
GEN_ALGO GEN_NAME(new)(GEN_SIZE capacity);
void GEN_NAME(free)(GEN_ALGO* tree);
void GEN_NAME_(grow)(GEN_ALGO* tree);
GEN_NAME(item)* GEN_NAME(refOrDefault)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value);
bool GEN_NAME(contains)(GEN_ALGO* tree, GEN_KEY key);
GEN_TYPE GEN_NAME(get)(GEN_ALGO* tree, GEN_KEY key);
GEN_TYPE GEN_NAME(getOrDefault)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value);
void GEN_NAME(tryAdd)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value);
void GEN_NAME(setOrAdd)(GEN_ALGO* tree, GEN_KEY key, GEN_TYPE value);
GEN_NAME(iter) GEN_NAME(iterAll)();
GEN_NAME(iter) GEN_NAME(iterAfter)(GEN_ALGO* tree, GEN_KEY start);
GEN_NAME(iter) GEN_NAME(iterBefore)(GEN_ALGO* tree, GEN_KEY end);
GEN_NAME(iter) GEN_NAME(iterBetween)(GEN_ALGO* tree, GEN_KEY start, GEN_KEY end);
GEN_NAME(item)* GEN_NAME(nextAll)(GEN_ALGO* tree, GEN_NAME(iter)* iter);
GEN_NAME(item)* GEN_NAME(nextAfter)(GEN_ALGO* tree, GEN_KEY start, GEN_NAME(iter)* iter);
GEN_NAME(item)* GEN_NAME(nextBefore)(GEN_ALGO* tree, GEN_KEY end, GEN_NAME(iter)* iter);
GEN_NAME(item)* GEN_NAME(nextBetween)(GEN_ALGO* tree, GEN_KEY start, GEN_KEY end, GEN_NAME(iter)* iter);
void GEN_NAME_(maintainAdd)(GEN_ALGO* tree, GEN_NAME_(dir)* stack, GEN_NAME_(dir)* top);
GEN_NAME_(dir)* GEN_NAME_(maintainRemove)(GEN_ALGO* tree, GEN_NAME_(dir)* stack, GEN_NAME_(dir)* top);
#ifdef TREE_SIZE
GEN_SIZE GEN_NAME(countAfter)(GEN_ALGO* tree, GEN_KEY start);
GEN_SIZE GEN_NAME(countBefore)(GEN_ALGO* tree, GEN_KEY end);
#endif


inline void GEN_NAME_(grow)(GEN_ALGO* tree) {
    if (tree->length + 2 >= tree->capacity) {
        tree->capacity <<= 1;
        tree->items = THROW_PN(realloc(tree->items, sizeof(GEN_NAME(reusable)) * tree->capacity), tree->items);
    }
}


// Maintain tree invariant after adding an item
inline void GEN_NAME_(maintainAdd)(GEN_ALGO* tree, GEN_NAME_(dir)* stack, GEN_NAME_(dir)* top) {
    while (top - 2 > stack) {
        GEN_SIZE parentIndex = top[-2].index;
        GEN_SIZE dir = top[-2].dir;
        GEN_SIZE invDir = dir ^ 1;
        GEN_NAME(item)* parent = &tree->items[parentIndex].item;
        if (!(parent->children[dir] & TREE_RED)) break;
        top -= 2;
        GEN_SIZE nodeIndex = top[1].index;
        GEN_NAME(item)* node = &tree->items[nodeIndex].item;
        GEN_SIZE* pParentIndex = &tree->items[top[-1].index].item.children[top[-1].dir];

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
        if (dir == top[1].dir) {
            *pParentIndex = nodeIndex & ~TREE_RED;
            parent->children[dir] = node->children[invDir];
            node->children[invDir] = parentIndex | TREE_RED;
#ifdef TREE_SIZE
            node->size = 1 + parent->size;
            parent->size -= tree->items[node->children[dir] & ~TREE_RED].item.size;
#endif
        }
        else {
            GEN_SIZE childIndex = node->children[invDir] & ~TREE_RED;
            GEN_NAME(item)* child = &tree->items[childIndex].item;
            *pParentIndex = childIndex;
            parent->children[dir] = child->children[invDir];
            child->children[invDir] = parentIndex | TREE_RED;
            node->children[invDir] = child->children[dir];
            child->children[dir] = nodeIndex | TREE_RED;
#ifdef TREE_SIZE
            child->size = 1 + parent->size;
            node->size -= tree->items[parent->children[dir]].item.size;
            parent->size -= node->size;
#endif
        }
        break;
    }
#ifdef TREE_SIZE
    while (--top > stack) {
        tree->items[top->index].item.size++;
    }
#endif
}


// Maintain tree invariant after removing an item
inline GEN_NAME_(dir)* GEN_NAME_(maintainRemove)(GEN_ALGO* tree, GEN_NAME_(dir)* stack, GEN_NAME_(dir)* top) {
    while (--top > stack) {
        GEN_SIZE parentIndex = top->index;
        GEN_SIZE dir = top->dir;
        GEN_SIZE invDir = dir ^ 1;
        GEN_NAME(item)* parent = &tree->items[parentIndex].item;
        GEN_SIZE siblingIndex = parent->children[invDir] & ~TREE_RED;
        GEN_NAME(item)* sibling = &tree->items[siblingIndex].item;
        GEN_SIZE* pParentIndex = &tree->items[top[-1].index].item.children[top[-1].dir];

        if (parent->children[invDir] & TREE_RED) { // Rotate and continue
            *pParentIndex = siblingIndex;
            parent->children[invDir] = sibling->children[dir];
            sibling->children[dir] = parentIndex | TREE_RED;
#ifdef TREE_SIZE
            sibling->size = parent->size;
            parent->size -= 1 + tree->items[sibling->children[invDir]].item.size;
#endif
            top++->index = siblingIndex;
            pParentIndex = &sibling->children[dir];
            siblingIndex = parent->children[invDir];
            sibling = &tree->items[siblingIndex].item;
        }
        
        if (sibling->children[invDir] & TREE_RED) { // Rotate
            *pParentIndex = siblingIndex | (*pParentIndex & TREE_RED);
            parent->children[invDir] = sibling->children[dir];
            sibling->children[dir] = parentIndex;
            sibling->children[invDir] &= ~TREE_RED;
#ifdef TREE_SIZE
            sibling->size = parent->size - 1;
            parent->size -= 2 + tree->items[sibling->children[invDir]].item.size;
#endif
            break;
        }
        else if (sibling->children[dir] & TREE_RED) { // Rotate
            GEN_SIZE siblingChildIndex = sibling->children[dir] & ~TREE_RED;
            GEN_NAME(item)* siblingChild = &tree->items[siblingChildIndex].item;
            *pParentIndex = siblingChildIndex | (*pParentIndex & TREE_RED);
            parent->children[invDir] = siblingChild->children[dir];
            siblingChild->children[dir] = parentIndex;
            sibling->children[dir] = siblingChild->children[invDir];
            siblingChild->children[invDir] = siblingIndex;
#ifdef TREE_SIZE
            siblingChild->size = parent->size - 1;
            sibling->size -= 1 + tree->items[parent->children[invDir]].item.size;
            parent->size -= 2 + sibling->size;
#endif
            break;
        }
        
        else { // Swap colors and continue if parent is black
            parent->children[invDir] |= TREE_RED;
#ifdef TREE_SIZE
            parent->size--;
#endif
            if (*pParentIndex & TREE_RED) {
                *pParentIndex &= ~TREE_RED;
                break;
            }
        }
    }
    return top;
}


GEN_NAME(item)* GEN_NAME(ref)(GEN_ALGO* tree, GEN_KEY key) {
    GEN_SIZE index = tree->items[0].item.children[1];
    while (index != 0) {
        GEN_NAME(item)* item = &tree->items[index & ~TREE_RED].item;
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(key, item->key);
        if (cmp == 0) return item;
        index = item->children[cmp > 0];
    }
    return NULL;
}


GEN_NAME(item)* GEN_NAME(refOrEmpty)(GEN_ALGO* tree, GEN_KEY key) {
    GEN_NAME_(grow)(tree);
    GEN_NAME_(dir) stack[TREE_STACK];
    stack->index = 0;
    stack->dir = 1;
    GEN_NAME_(dir)* top = stack + 1;

    // Find item
    GEN_NAME(item)* item = &tree->items[0].item;
    GEN_SIZE dir = 1;
    GEN_SIZE index = item->children[1];
    while (index != 0) {
        index &= ~TREE_RED;
        item = &tree->items[index].item;
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(key, item->key);
        if (cmp == 0) return item;
        dir = cmp > 0;
        top->index = index;
        top->dir = dir;
        top++;
        index = item->children[dir];
    }
    
    // Add item
    tree->length++;
    GEN_SIZE i;
    if (tree->reusable != -1) {
        i = tree->reusable;
        tree->reusable = tree->items[i].reusable;
    }
    else i = tree->length;
    GEN_NAME(item)* child = &tree->items[i].item;
    child->key = key;
    child->children[0] = 0;
    child->children[1] = 0;
#ifdef TREE_SIZE
    child->size = 1;
#endif
    item->children[dir] = i | TREE_RED;
    GEN_NAME_(maintainAdd)(tree, stack, top);
    return child;
}


void GEN_NAME(remove)(GEN_ALGO* tree, GEN_KEY key) {
    GEN_NAME_(dir) stack[TREE_STACK];
    GEN_NAME_(dir)* top = stack;

    // Find item
    GEN_SIZE index = 0, nextIndex;
    GEN_NAME(item)* item = &tree->items[0].item;
    GEN_COMPARE_TYPE cmp = 0;
    GEN_SIZE dir = 1;
    do {
        top->index = index;
        top->dir = dir;
        top++;
        nextIndex = item->children[dir];
        if (nextIndex == 0) return;
        index = nextIndex & ~TREE_RED;
        item = &tree->items[index].item;
        cmp = GEN_COMPARE(key, item->key);
        dir = cmp > 0;
    } while (cmp != 0);
    
    // Replace item by successor if more than one child
    GEN_SIZE removedIndex = index;
    GEN_SIZE* removedParent = &tree->items[top[-1].index].item.children[top[-1].dir];
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
        GEN_NAME(item)* removedItem = item;
        GEN_NAME_(dir)* removedDir = top;
        nextIndex = item->children[1];
        do {
            top->index = index;
            top->dir = 0;
            top++;
            index = nextIndex & ~TREE_RED;
            item = &tree->items[index].item;
            nextIndex = item->children[0];
        } while (nextIndex != 0);
        removedDir->dir = 1;
        GEN_SIZE* pIndex = &tree->items[top[-1].index].item.children[top[-1].dir];
        removedDir->index = index;
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
    tree->items[removedIndex].reusable = tree->reusable;
    tree->reusable = removedIndex;
    tree->length--;
    if (mustFix) top = GEN_NAME_(maintainRemove)(tree, stack, top);
#ifdef TREE_SIZE
    while (--top > stack) {
        tree->items[top->index].item.size--;
    }
#endif
}


GEN_NAME(item)* GEN_NAME(floor)(GEN_ALGO* tree, GEN_KEY key) {
    GEN_NAME(item)* floor = NULL;
    GEN_SIZE index = tree->items[0].item.children[1];
    while (index != 0) {
        GEN_NAME(item)* item = &tree->items[index & ~TREE_RED].item;
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(key, item->key);
        if (cmp < 0) index = item->children[0];
        else {
            floor = item;
            index = item->children[1];
        }
    }
    return floor;
}


GEN_NAME(item)* GEN_NAME(ceil)(GEN_ALGO* tree, GEN_KEY key) {
    GEN_NAME(item)* ceil = NULL;
    GEN_SIZE index = tree->items[0].item.children[1];
    while (index != 0) {
        GEN_NAME(item)* item = &tree->items[index & ~TREE_RED].item;
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(key, item->key);
        if (cmp > 0) index = item->children[1];
        else {
            ceil = item;
            index = item->children[0];
        }
    }
    return ceil;
}


#ifdef TREE_SIZE

GEN_SIZE GEN_NAME_(countAfter)(GEN_ALGO* tree, GEN_KEY start, GEN_SIZE index) {
    GEN_SIZE count = 0;
    while (index != 0) {
        GEN_NAME(item)* item = &tree->items[index & ~TREE_RED].item;
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(start, item->key);
        if (cmp > 0) index = item->children[1];
        else {
            count += 1 + tree->items[item->children[1] & ~TREE_RED].item.size;
            index = item->children[0];
        }
    }
    return count;
}


GEN_SIZE GEN_NAME_(countBefore)(GEN_ALGO* tree, GEN_KEY end, GEN_SIZE index) {
    GEN_SIZE count = 0;
    while (index != 0) {
        GEN_NAME(item)* item = &tree->items[index & ~TREE_RED].item;
        GEN_COMPARE_TYPE cmp = GEN_COMPARE(end, item->key);
        if (cmp <= 0) index = item->children[0];
        else {
            count += 1 + tree->items[item->children[0] & ~TREE_RED].item.size;
            index = item->children[1];
        }
    }
    return count;
}


GEN_SIZE GEN_NAME(countBetween)(GEN_ALGO* tree, GEN_KEY start, GEN_KEY end) {
    GEN_SIZE index = tree->items[0].item.children[1];
    while (index != 0) {
        GEN_NAME(item)* item = &tree->items[index & ~TREE_RED].item;
        GEN_COMPARE_TYPE cmpStart = GEN_COMPARE(start, item->key);
        GEN_COMPARE_TYPE cmpEnd = GEN_COMPARE(end, item->key);
        if (cmpStart > 0) index = item->children[1];
        else if (cmpEnd < 0) index = item->children[0];
        else return 1 + 
            GEN_NAME_(countAfter)(tree, start, item->children[0]) +
            GEN_NAME_(countBefore)(tree, end, item->children[1]);
    }
    return 0;
}

#endif


#endif

// Undef parameters for later use
#undef TREE_SIZE
#undef TREE_STACK
#undef TREE_RED

#include "generic_end.h"