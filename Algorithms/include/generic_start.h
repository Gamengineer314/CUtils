#include <stdint.h>

// Common parameters : (not all parameters are applicable for every algorithm)
// GEN_TYPE : data type (default: void*)
// GEN_KEY : key type (default: int32_t)
// GEN_NAME : name of the type (added as a suffix to every function and struct name) (default: "")
// GEN_SIZE : type used for sizes and indices (must be able to contain the number of items) (default: int32_t)
// GEN_COMPARE : comparator for keys or items (default: a - b)
// GEN_EQUALS : equality function for keys or items (default: a == b)
// GEN_COMPARE_TYPE : type used for comparisons (returned by GEN_COMPARE) (default: int)

#ifndef GEN_TYPE
#define GEN_TYPE void*
#endif

#ifdef GEN_NAME
#define GEN_SUFFIX GEN_CAT2(_, GEN_NAME)
#else
#define GEN_SUFFIX
#endif

#ifndef GEN_SIZE
#define GEN_SIZE int32_t
#endif

#ifndef GEN_KEY
#define GEN_KEY int32_t
#endif

#ifndef GEN_COMPARE
#define GEN_COMPARE(a, b) (((a) > (b)) - ((a) < (b)))
#endif

#ifndef GEN_EQUALS
#define GEN_EQUALS(a, b) ((a) == (b))
#endif

#ifndef GEN_COMPARE_TYPE
#define GEN_COMPARE_TYPE int
#endif


#ifndef GEN_H
#define GEN_H

// Concatenation
#define _GEN_CAT2(A, B) A ## B
#define GEN_CAT2(A, B) _GEN_CAT2(A, B)
#define _GEN_CAT3(A, B, C) A ## B ## C
#define GEN_CAT3(A, B, C) _GEN_CAT3(A, B, C)
#define _GEN_CAT4(A, B, C, D) A ## B ## C ## D
#define GEN_CAT4(A, B, C, D) _GEN_CAT4(A, B, C, D)
#define _GEN_CAT5(A, B, C, D, E) A ## B ## C ## D ## E
#define GEN_CAT5(A, B, C, D, E) _GEN_CAT5(A, B, C, D, E)

// Struct and function names
#define GEN_COL GEN_CAT3(GEN_PREFIX, GEN_SUFFIX, _t)
#define GEN_ALGO GEN_CAT2(GEN_PREFIX, GEN_SUFFIX)
#define GEN_STRUCT(name) GEN_CAT5(GEN_PREFIX, _, name, GEN_SUFFIX, _t)
#define GEN_STRUCT_(name) GEN_CAT2(_, GEN_STRUCT(name))
#define GEN_FUNC(name) GEN_CAT4(GEN_PREFIX, _, name, GEN_SUFFIX)
#define GEN_FUNC_(name) GEN_CAT2(_, GEN_FUNC(name))

#endif