#define GEN_PREFIX queue
#include "generic_start.h"

#include <stdlib.h>
#include <string.h>


/**
 * @brief Resizable queue
 * @param values Values in the queue
 * @param length Size of the queue
 * @param first Index of the first value
 * @param mask Size of [values] - 1
**/
typedef struct {
    GEN_TYPE* values;
    GEN_SIZE length;
    GEN_SIZE first;
    GEN_SIZE mask;
} GEN_COL;


/**
 * @brief Initialize a queue
 * @param queue The queue
 * @param capacity Initial capacity (must be a power of 2)
**/
inline void GEN_FUNC(init)(GEN_COL* queue, GEN_SIZE capacity) {
    queue->values = THROW_PN(malloc(sizeof(GEN_TYPE) * capacity), queue->values);
    queue->length = 0;
    queue->first = 0;
    queue->mask = capacity - 1;
}


/**
 * @brief Create a queue
 * @param capacity Initial capacity (must be a power of 2)
 * @return The queue
**/
inline GEN_COL GEN_FUNC(new)(GEN_SIZE capacity) {
    GEN_COL queue;
    GEN_FUNC(init)(&queue, capacity);
    return queue;
}


/**
 * @brief Free a queue
 * @param list The queue
**/
inline void GEN_FUNC(free)(GEN_COL* queue) {
    free(queue->values);
}


/**
 * @brief Grow a queue
 * @param queue The queue
**/
void GEN_FUNC_(grow)(GEN_COL* queue);


/**
 * @brief Access a value in a queue
 * @param queue The queue
 * @param index Index of the value
 * @return Pointer to the value
**/
inline GEN_TYPE* GEN_FUNC(ref)(GEN_COL* queue, GEN_SIZE index) {
    return queue->values + ((queue->first + index) & queue->mask);
}


/**
 * @brief Get a value in a queue
 * @param queue The queue
 * @param index Index of the value
 * @return The value
**/
inline GEN_TYPE GEN_FUNC(get)(GEN_COL* queue, GEN_SIZE index) {
    return *GEN_FUNC(ref)(queue, index);
}


/**
 * @brief Set a value in a queue
 * @param queue The queue
 * @param index Index of the value
 * @return The value
**/
inline void GEN_FUNC(set)(GEN_COL* queue, GEN_SIZE index, GEN_TYPE value) {
    *GEN_FUNC(ref)(queue, index) = value;
}


/**
 * @brief Add a value at the end of a queue after growing it if necessary
 * @param list The queue
 * @param value The value
**/
inline void GEN_FUNC(addLast)(GEN_COL* queue, GEN_TYPE value) {
    if (queue->length > queue->mask) GEN_FUNC_(grow)(queue);
    queue->values[(queue->first + queue->length++) & queue->mask] = value;
}


/**
 * @brief Add a value at the start of a queue after growing it if necessary
 * @param list The queue
 * @param value The value
**/
inline void GEN_FUNC(addFirst)(GEN_COL* queue, GEN_TYPE value) {
    if (queue->length > queue->mask) GEN_FUNC_(grow)(queue);
    queue->first = (queue->first - 1) & queue->mask;
    queue->length++;
    queue->values[queue->first] = value;
}


/**
 * @brief Remove and return the first value of a queue
 * @param queue The queue
 * @return The value
**/
inline GEN_TYPE GEN_FUNC(popFirst)(GEN_COL* queue) {
    GEN_TYPE value = queue->values[queue->first];
    queue->first = (queue->first + 1) & queue->mask;
    queue->length--;
    return value;
}


/**
 * @brief Remove and return the last value of a queue
 * @param queue The queue
 * @return The value
**/
inline GEN_TYPE GEN_FUNC(popLast)(GEN_COL* queue) {
    return queue->values[(queue->first + --queue->length) & queue->mask];
}


#ifdef GEN_SOURCE


void GEN_FUNC(init)(GEN_COL* queue, GEN_SIZE capacity);
GEN_COL GEN_FUNC(new)(GEN_SIZE capacity);
void GEN_FUNC(free)(GEN_COL* queue);
GEN_TYPE* GEN_FUNC(ref)(GEN_COL* queue, GEN_SIZE index);
GEN_TYPE GEN_FUNC(get)(GEN_COL* queue, GEN_SIZE index);
void GEN_FUNC(set)(GEN_COL* queue, GEN_SIZE index, GEN_TYPE value);
void GEN_FUNC(addLast)(GEN_COL* queue, GEN_TYPE value);
void GEN_FUNC(addFirst)(GEN_COL* queue, GEN_TYPE value);
GEN_TYPE GEN_FUNC(popFirst)(GEN_COL* queue);
GEN_TYPE GEN_FUNC(popLast)(GEN_COL* queue);


void GEN_FUNC_(grow)(GEN_COL* queue) {
    GEN_SIZE capacity = (queue->mask + 1) << 1;
    queue->mask = capacity - 1;
    queue->values = THROW_PN(realloc(queue->values, sizeof(GEN_TYPE) * capacity), queue->values);
    if (queue->first < queue->length >> 1) {
        memcpy(queue->values + queue->length, queue->values, sizeof(GEN_TYPE) * queue->first);
    }
    else {
        GEN_SIZE newFirst = capacity - (queue->length - queue->first);
        memcpy(queue->values + newFirst, queue->values + queue->first, sizeof(GEN_TYPE) * (queue->length - queue->first));
        queue->first = newFirst;
    }
}


#endif

#include "generic_end.h"