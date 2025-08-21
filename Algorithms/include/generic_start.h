#include <stdint.h>

// Common parameters : (not all parameters are applicable for every algorithm)
// GEN_TYPE : data type (default: void*)
// GEN_KEY : key type (default: int32_t)
// GEN_SUFFIX : suffix added after every function and struct name (default: "")
// GEN_SIZE : type used for sizes and indices (must be able to contain the number of items) (default: int32_t)
// GEN_COMPARE : comparator for keys or items (default: a > b ? 1 : a < b ? -1 : 0)
// GEN_EQUALS : equality function for keys or items (default: a == b)
// GEN_COMPARE_TYPE : type used for comparisons (returned by GEN_COMPARE) (default: int)
// GEN_NO_VALUE : whether items contain only a key (otherwise they contain a key and a value) (default: false)


#ifndef GEN_H
#define GEN_H

// Concatenation
#define __GEN_CAT2(A, B) A ## B
#define _GEN_CAT2(A, B) __GEN_CAT2(A, B)
#define __GEN_CAT3(A, B, C) A ## B ## C
#define _GEN_CAT3(A, B, C) __GEN_CAT3(A, B, C)
#define __GEN_CAT4(A, B, C, D) A ## B ## C ## D
#define _GEN_CAT4(A, B, C, D) __GEN_CAT4(A, B, C, D)
#define __GEN_CAT5(A, B, C, D, E) A ## B ## C ## D ## E
#define _GEN_CAT5(A, B, C, D, E) __GEN_CAT5(A, B, C, D, E)

#endif


// Default parameters
#ifndef GEN_TYPE
#define GEN_TYPE void*
#endif

#ifdef GEN_SUFFIX
#define GEN_SUFFIX_ _GEN_CAT2(_, GEN_SUFFIX)
#else
#define GEN_SUFFIX_
#endif

#ifndef GEN_SIZE
#define GEN_SIZE int32_t
#endif

#ifndef GEN_KEY
#define GEN_KEY int32_t
#endif

#ifndef GEN_COMPARE
#define GEN_COMPARE(a, b) ((a) > (b) ? 1 : (a) < (b) ? -1 : 0)
#endif

#ifndef GEN_EQUALS
#define GEN_EQUALS(a, b) ((a) == (b))
#endif

#ifndef GEN_COMPARE_TYPE
#define GEN_COMPARE_TYPE int
#endif


// Struct and function names
#define GEN_ALGO _GEN_CAT2(GEN_PREFIX, GEN_SUFFIX_)
#define GEN_NAME(name) _GEN_CAT4(GEN_PREFIX, _, name, GEN_SUFFIX_)
#define GEN_NAME_(name) _GEN_CAT5(_, GEN_PREFIX, _, name, GEN_SUFFIX_)


// Key-value or only key
#ifdef GEN_KV
#ifdef GEN_NO_VALUE
#define GEN_KV_TYPE GEN_KEY
#define GEN_KV_KEY(kv) (kv)
#define GEN_ITER_TYPE GEN_KEY
#define GEN_ITER_ITEM(kv) (kv)
#else
/**
 * @brief Key-value pair
 * @param key The key
 * @param value The value
**/
typedef struct {
    GEN_KEY key;
    GEN_TYPE value;
} GEN_NAME(kv);

#define GEN_KV_TYPE GEN_NAME(kv)
#define GEN_KV_KEY(kv) ((kv).key)
#endif
#endif