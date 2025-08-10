#include <stdlib.h>
#include <stdio.h>
#include "./algorithms.h"
#include "benchmark.h"

#define MAX_LENGTH 1048576

static void heap_benchmark() {
    srand(314);
    for (int i = 0; i < 10; i++) {
        heap_t heap = heap_new(1);
        for (int j = 0; j < 10; j++) {
            int r = rand();
            int s = (double)MAX_LENGTH * r / RAND_MAX;
            while (heap.length < s) heap_add(&heap, rand(), rand());
            while (heap.length > s) heap_pop(&heap);
        }
        heap_free(&heap);
    }
}

static void heap_test() {
    long h = 0;
    srand(314);
    for (int i = 0; i < 10; i++) {
        heap_t heap = heap_new(1);
        for (int j = 0; j < 10; j++) {
            int r = rand();
            int s = (double)MAX_LENGTH * r / RAND_MAX;
            while (heap.length < s) heap_add(&heap, rand(), rand());
            while (heap.length > s) {
                heap_item_t item = heap_pop(&heap);
                h = h * 31 + item.key;
                h = h * 31 + item.value;
            }
        }
        heap_free(&heap);
    }
    printf("%ld\n", h);
}

static void heap_mod_test() {
    long h = 0;
    srand(314);
    heap_mod_t heap = heap_new_mod(1);
    int32_t indices[10000];
    for (int i = 0; i < 10000; i++) {
        int key = rand();
        int value = rand();
        indices[i] = heap_add_mod(&heap, key, value);
    }
    for (int i = 0; i < 5000; i++) {
        heap_changeKey_mod(&heap, indices[i], rand());
        heap_remove_mod(&heap, indices[5000+i]);
    }
    for (int i = 0; i < 5000; i++) {
        heap_add_mod(&heap, rand(), rand());
        heap_item_mod_t item = heap_pop_mod(&heap);
        h = h * 31 + item.key;
        h = h * 31 + item.value;
    }
    heap_free_mod(&heap);
    printf("%ld\n", h);
}

int main() {
    TIME("Heap benchmark", 
        heap_benchmark();
    )
    heap_test();
    heap_mod_test();
}