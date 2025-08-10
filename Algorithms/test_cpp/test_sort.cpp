#include <cstdlib>
#include <algorithm>
#include "benchmark.h"

using namespace std;

#define ITER 10
#define N 5000000

static void sort_benchmark() {
    srand(314);
    int* items = new int[N];
    for (int i = 0; i < ITER; i++) {
        for (int j = 0; j < N; j++) {
            items[j] = rand() - rand();
        }
        sort(items, items + N);
    }
    delete[] items;
}

int main() {
    TIME("CPP sort benchmark", 
        sort_benchmark();
    )
}