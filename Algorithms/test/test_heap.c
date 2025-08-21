#include <stdlib.h>
#include <stdio.h>
#include "./algorithms.h"
#include "benchmark.h"

#define MAX_LENGTH 1048576

static void heap_benchmark() {
    srand(314);
    for (int i = 0; i < 10; i++) {
        heap test = heap_new(1);
        for (int j = 0; j < 10; j++) {
            int r = rand();
            int s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.length < s) heap_add(&test, rand(), rand());
            while (test.length > s) heap_pop(&test);
        }
        heap_free(&test);
    }
}

static void heap_test() {
    long h = 0;
    srand(314);
    for (int i = 0; i < 10; i++) {
        heap test = heap_new(1);
        for (int j = 0; j < 10; j++) {
            int r = rand();
            int s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.length < s) heap_add(&test, rand(), rand());
            while (test.length > s) {
                heap_kv item = heap_pop(&test);
                h = h * 31 + item.key;
                h = h * 31 + item.value;
            }
        }
        heap_free(&test);
    }
    printf("%ld\n", h);
}

static void heap_mod_test() {
    long h = 0;
    srand(314);
    heap_mod test = heap_new_mod(1);
    int32_t indices[10000];
    for (int i = 0; i < 10000; i++) {
        int key = rand();
        int value = rand();
        indices[i] = heap_add_mod(&test, key, value);
    }
    for (int i = 0; i < 5000; i++) {
        heap_changeKey_mod(&test, indices[i], rand());
        heap_remove_mod(&test, indices[5000+i]);
    }
    for (int i = 0; i < 5000; i++) {
        heap_add_mod(&test, rand(), rand());
        heap_kv_mod item = heap_pop_mod(&test);
        h = h * 31 + item.key;
        h = h * 31 + item.value;
    }
    heap_free_mod(&test);
    printf("%ld\n", h);
}

int main() {
    TIME("Heap benchmark", 
        heap_benchmark();
    )
    heap_test();
    heap_mod_test();
}