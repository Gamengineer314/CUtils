#include <cstdlib>
#include <algorithm>
#include "benchmark.h"

using namespace std;

#define ITER 10000000
#define N 1000000

static void search_benchmark() {
    srand(314);
    int items[N];
    for (int i = 0; i < N; i++) items[i] = rand();
    sort(items, items + N);

    for (int i = 0; i < ITER; i++) {
        lower_bound(items, items + N, rand());
    }
}

int main() {
    TIME("CPP search benchmark",
        search_benchmark();
    )
}