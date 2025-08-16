#include <cstdlib>
#include <cstdio>
#include <unordered_map>
#include "benchmark.h"

using namespace std;

#define N 10000

static void map_benchmark() {
    srand(314);
    int keys[N];
    for (int i = 0; i < N; i++) keys[i] = rand();
    for (int i = 0; i < 4000; i++) {
        unordered_map<int, int> test;
        for (int j = 0; j < N; j++) {
            test[keys[j]] = j;
        }
        for (int j = 0; j < N; j++) {
            test[keys[j]];
        }
        for (int j = 0; j < N; j++) {
            test.erase(keys[j]);
        }
    }
}

int main() {
    TIME("CPP map benchmark",
        map_benchmark();
    )
}