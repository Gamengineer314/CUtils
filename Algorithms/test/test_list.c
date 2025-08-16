#include <stdlib.h>
#include <stdio.h>
#include "./algorithms.h"
#include "benchmark.h"

#define MAX_LENGTH 1048576

static void list_benchmark() {
    srand(314);
    for (int i = 0; i < 50; i++) {
        list test = list_new(1);
        for (int j = 0; j < 100; j++) {
            int r = rand();
            int s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.length < s) list_add(&test, r + test.length);
            while (test.length > s) list_pop(&test);
        }
        list_free(&test);
    }
}

static void listest() {
    long h = 0;
    srand(314);
    for (int i = 0; i < 50; i++) {
        list test = list_new(1);
        for (int j = 0; j < 100; j++) {
            int r = rand();
            int s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.length < s) list_add(&test, r + test.length);
            while (test.length > s) h = h * 31 + list_pop(&test);
        }
        list_free(&test);
    }
    printf("%ld\n", h);
}

int main() {
    TIME("List benchmark", 
        list_benchmark();
    )
    listest();
}