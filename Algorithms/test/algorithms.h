#ifndef TEST_COLLECTIONS_H
#define TEST_COLLECTIONS_H

#define GEN_TYPE int
#include "list.h"

#define GEN_TYPE int
#include "queue.h"

#define GEN_KEY int
#define GEN_TYPE int
#include "heap.h"

#define GEN_NAME mod
#define GEN_KEY int
#define GEN_TYPE int
#define HEAP_MODIFIABLE
#include "heap.h"

#define GEN_KEY int
#define GEN_TYPE int
#include "map.h"

#define GEN_KEY int
#define GEN_TYPE int
#define TREE_SIZE
#include "tree.h"

#define GEN_TYPE int
#define GEN_KEY unsigned int
#define SORT_KEY SORT_SIGNED
#include "sort.h"

#define GEN_TYPE int
#include "search.h"

#endif