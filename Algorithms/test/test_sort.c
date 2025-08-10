#include <stdlib.h>
#include <stdio.h>
#include "./algorithms.h"
#include "benchmark.h"
#include "throw.h"

#define ITER 10
#define N 5000000

static void sort_benchmark(void (*sort)(int* items, int n)) {
    srand(314);
    int* items = malloc(sizeof(int) * N);
    for (int i = 0; i < ITER; i++) {
        for (int j = 0; j < N; j++) {
            items[j] = rand() - rand();
        }
        sort(items, N);
    }
    free(items);
}

int cmp(const void* p1, const void* p2) {
    int i1 = *(int*)p1, i2 = *(int*)p2;
    return (i1 > i2) - (i1 < i2);
}

static void sort_test(void (*sort)(int* items, int n)) {
    srand(314);
    int* items = malloc(sizeof(int) * N);
    for (int i = 0; i < N; i++) {
        items[i] = rand() - rand();
    }
    int* copy = malloc(sizeof(int) * N);
    memcpy(copy, items, sizeof(int) * N);

    sort(items, N);
    qsort(copy, N, sizeof(int), cmp);

    for (int i = 0; i < N; i++) {
        if (items[i] != copy[i]) THROW_ERR("Different items at index %d : %f %f", i, items[i], copy[i]);
    }

    free(items);
    free(copy);
}

int main() {
    TIME("Sort benchmark",
        sort_benchmark(sort);
    )
    sort_test(sort);
}