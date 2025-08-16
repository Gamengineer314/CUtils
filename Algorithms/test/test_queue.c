#include <stdlib.h>
#include <stdio.h>
#include "./algorithms.h"
#include "benchmark.h"

#define MAX_LENGTH 1048576

static void queue_benchmark() {
    srand(314);
    for (int i = 0; i < 50; i++) {
        queue test = queue_new(1);
        for (int j = 0; j < 50; j++) {
            int r = rand();
            int s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.length < s) queue_addFirst(&test, r + test.length);
            while (test.length > s) queue_popFirst(&test);
            for (int k = 0; k < s; k++) {
                queue_addFirst(&test, r + k);
                queue_popLast(&test);
            }
            r = rand();
            s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.length < s) queue_addLast(&test, r + test.length);
            while (test.length > s) queue_popLast(&test);
            for (int k = 0; k < s; k++) {
                queue_addLast(&test, r + k);
                queue_popFirst(&test);
            }
        }
        queue_free(&test);
    }
}

static void queueest() {
    long h = 0;
    srand(314);
    for (int i = 0; i < 50; i++) {
        queue test = queue_new(1);
        for (int j = 0; j < 50; j++) {
            int r = rand();
            int s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.length < s) queue_addFirst(&test, r + test.length);
            while (test.length > s) h = h * 31 + queue_popFirst(&test);
            for (int k = 0; k < s; k++) {
                queue_addFirst(&test, r + k);
                h = h * 31 + queue_popLast(&test);
            }
            r = rand();
            s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.length < s) queue_addLast(&test, r + test.length);
            while (test.length > s) h = h * 31 + queue_popLast(&test);
            for (int k = 0; k < s; k++) {
                queue_addLast(&test, r + k);
                h = h * 31 + queue_popFirst(&test);
            }
        }
        queue_free(&test);
    }
    printf("%ld\n", h);
}

int main() {
    TIME("Queue benchmark", 
        queue_benchmark();
    )
    queueest();
}