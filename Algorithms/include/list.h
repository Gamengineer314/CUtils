#define GEN_PREFIX list
#include "generic_start.h"

#include <stdlib.h>
#include "throw.h"


/**
 * @brief Resizable list
 * @param values Values in the list
 * @param length Size of the list
**/
typedef struct {
    GEN_TYPE* values;
    GEN_SIZE length;
    GEN_SIZE capacity; // Size of [values]
} GEN_COL;


/**
 * @brief Initialize a list
 * @param list The list
 * @param capacity Initial capacity
**/
inline void GEN_FUNC(init)(GEN_COL* list, GEN_SIZE capacity) {
    list->values = THROW_PN(malloc(sizeof(GEN_TYPE) * capacity), list->values);
    list->length = 0;
    list->capacity = capacity;
}


/**
 * @brief Create a list
 * @param capacity Initial capacity
 * @return The list
**/
inline GEN_COL GEN_FUNC(new)(GEN_SIZE capacity) {
    GEN_COL list;
    GEN_FUNC(init)(&list, capacity);
    return list;
}


/**
 * @brief Free a list
 * @param list The list
**/
inline void GEN_FUNC(free)(GEN_COL* list) {
    free(list->values);
}


/**
 * @brief Grow a list if it is full
 * @param list The list
**/
inline void GEN_FUNC_(grow)(GEN_COL* list) {
    if (list->length >= list->capacity) {
        list->capacity <<= 1;
        list->values = THROW_PN(realloc(list->values, sizeof(GEN_TYPE) * list->capacity), list->values);
    }
}


/**
 * @brief Add a value to a list after growing it if necessary
 * @param list The list
 * @param value The value
**/
inline void GEN_FUNC(add)(GEN_COL* list, GEN_TYPE value) {
    GEN_FUNC_(grow)(list);
    list->values[list->length++] = value;
}


/**
 * @brief Remove and return the last value of a list
 * @param list The list
 * @return The value
**/
inline GEN_TYPE GEN_FUNC(pop)(GEN_COL* list) {
    return list->values[--list->length];
}


#ifdef GEN_SOURCE


void GEN_FUNC(init)(GEN_COL* list, GEN_SIZE capacity);
GEN_COL GEN_FUNC(new)(GEN_SIZE capacity);
void GEN_FUNC(free)(GEN_COL* list);
void GEN_FUNC_(grow)(GEN_COL* list);
void GEN_FUNC(add)(GEN_COL* list, GEN_TYPE value);
GEN_TYPE GEN_FUNC(pop)(GEN_COL* list);


#endif

#include "generic_end.h"