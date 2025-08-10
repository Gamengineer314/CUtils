#include <stdlib.h>
#include <stdio.h>
#include "./algorithms.h"
#include "benchmark.h"

#define ITER 10000000
#define N 1000000

static void search_benchmark() {
    srand(314);
    int items[N];
    for (int i = 0; i < N; i++) items[i] = rand();
    sort(items, N);

    for (int i = 0; i < ITER; i++) {
        search(items, rand(), N);
    }
}

static void search_test() {
    srand(314);
    int items[N];
    for (int i = 0; i < N; i++) items[i] = rand();
    sort(items, N);

    for (int i = 0; i < ITER; i++) {
        int item = rand();
        int index = search(items, item, N);
        if ((index < N && items[index] < item) || (index > 0 && items[index-1] > item)) THROW_ERR("Incorrect index");
    }
}

int main() {
    TIME("Search benchmark",
        search_benchmark();
    )
    search_test();
}