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
} GEN_ALGO;


/**
 * @brief Initialize a queue
 * @param queue The queue
 * @param capacity Initial capacity (must be a power of 2)
**/
inline void GEN_NAME(init)(GEN_ALGO* queue, GEN_SIZE capacity) {
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
inline GEN_ALGO GEN_NAME(new)(GEN_SIZE capacity) {
    GEN_ALGO queue;
    GEN_NAME(init)(&queue, capacity);
    return queue;
}


/**
 * @brief Free a queue
 * @param list The queue
**/
inline void GEN_NAME(free)(GEN_ALGO* queue) {
    free(queue->values);
}


/**
 * @brief Grow a queue
 * @param queue The queue
**/
void GEN_NAME_(grow)(GEN_ALGO* queue);


/**
 * @brief Access a value in a queue
 * @param queue The queue
 * @param index Index of the value
 * @return Pointer to the value
**/
inline GEN_TYPE* GEN_NAME(ref)(GEN_ALGO* queue, GEN_SIZE index) {
    return queue->values + ((queue->first + index) & queue->mask);
}


/**
 * @brief Get a value in a queue
 * @param queue The queue
 * @param index Index of the value
 * @return The value
**/
inline GEN_TYPE GEN_NAME(get)(GEN_ALGO* queue, GEN_SIZE index) {
    return *GEN_NAME(ref)(queue, index);
}


/**
 * @brief Set a value in a queue
 * @param queue The queue
 * @param index Index of the value
 * @return The value
**/
inline void GEN_NAME(set)(GEN_ALGO* queue, GEN_SIZE index, GEN_TYPE value) {
    *GEN_NAME(ref)(queue, index) = value;
}


/**
 * @brief Add a value at the end of a queue after growing it if necessary
 * @param list The queue
 * @param value The value
**/
inline void GEN_NAME(addLast)(GEN_ALGO* queue, GEN_TYPE value) {
    if (queue->length > queue->mask) GEN_NAME_(grow)(queue);
    queue->values[(queue->first + queue->length++) & queue->mask] = value;
}


/**
 * @brief Add a value at the start of a queue after growing it if necessary
 * @param list The queue
 * @param value The value
**/
inline void GEN_NAME(addFirst)(GEN_ALGO* queue, GEN_TYPE value) {
    if (queue->length > queue->mask) GEN_NAME_(grow)(queue);
    queue->first = (queue->first - 1) & queue->mask;
    queue->length++;
    queue->values[queue->first] = value;
}


/**
 * @brief Remove and return the first value of a queue
 * @param queue The queue
 * @return The value
**/
inline GEN_TYPE GEN_NAME(popFirst)(GEN_ALGO* queue) {
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
inline GEN_TYPE GEN_NAME(popLast)(GEN_ALGO* queue) {
    return queue->values[(queue->first + --queue->length) & queue->mask];
}


#ifdef GEN_SOURCE


void GEN_NAME(init)(GEN_ALGO* queue, GEN_SIZE capacity);
GEN_ALGO GEN_NAME(new)(GEN_SIZE capacity);
void GEN_NAME(free)(GEN_ALGO* queue);
GEN_TYPE* GEN_NAME(ref)(GEN_ALGO* queue, GEN_SIZE index);
GEN_TYPE GEN_NAME(get)(GEN_ALGO* queue, GEN_SIZE index);
void GEN_NAME(set)(GEN_ALGO* queue, GEN_SIZE index, GEN_TYPE value);
void GEN_NAME(addLast)(GEN_ALGO* queue, GEN_TYPE value);
void GEN_NAME(addFirst)(GEN_ALGO* queue, GEN_TYPE value);
GEN_TYPE GEN_NAME(popFirst)(GEN_ALGO* queue);
GEN_TYPE GEN_NAME(popLast)(GEN_ALGO* queue);


void GEN_NAME_(grow)(GEN_ALGO* queue) {
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