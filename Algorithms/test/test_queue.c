#include <stdlib.h>
#include <stdio.h>
#include "./algorithms.h"
#include "benchmark.h"

#define MAX_LENGTH 1048576

static void queue_benchmark() {
    srand(314);
    for (int i = 0; i < 50; i++) {
        queue_t queue = queue_new(1);
        for (int j = 0; j < 50; j++) {
            int r = rand();
            int s = (double)MAX_LENGTH * r / RAND_MAX;
            while (queue.length < s) queue_addFirst(&queue, r + queue.length);
            while (queue.length > s) queue_popFirst(&queue);
            for (int k = 0; k < s; k++) {
                queue_addFirst(&queue, r + k);
                queue_popLast(&queue);
            }
            r = rand();
            s = (double)MAX_LENGTH * r / RAND_MAX;
            while (queue.length < s) queue_addLast(&queue, r + queue.length);
            while (queue.length > s) queue_popLast(&queue);
            for (int k = 0; k < s; k++) {
                queue_addLast(&queue, r + k);
                queue_popFirst(&queue);
            }
        }
        queue_free(&queue);
    }
}

static void queue_test() {
    long h = 0;
    srand(314);
    for (int i = 0; i < 50; i++) {
        queue_t queue = queue_new(1);
        for (int j = 0; j < 50; j++) {
            int r = rand();
            int s = (double)MAX_LENGTH * r / RAND_MAX;
            while (queue.length < s) queue_addFirst(&queue, r + queue.length);
            while (queue.length > s) h = h * 31 + queue_popFirst(&queue);
            for (int k = 0; k < s; k++) {
                queue_addFirst(&queue, r + k);
                h = h * 31 + queue_popLast(&queue);
            }
            r = rand();
            s = (double)MAX_LENGTH * r / RAND_MAX;
            while (queue.length < s) queue_addLast(&queue, r + queue.length);
            while (queue.length > s) h = h * 31 + queue_popLast(&queue);
            for (int k = 0; k < s; k++) {
                queue_addLast(&queue, r + k);
                h = h * 31 + queue_popFirst(&queue);
            }
        }
        queue_free(&queue);
    }
    printf("%ld\n", h);
}

int main() {
    TIME("Queue benchmark", 
        queue_benchmark();
    )
    queue_test();
}