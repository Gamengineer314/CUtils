#include <cstdlib>
#include <cstdio>
#include <map>
#include "benchmark.h"

using namespace std;

#define N 10000

static void tree_benchmark() {
    srand(314);
    int keys[N];
    for (int i = 0; i < N; i++) keys[i] = rand();
    for (int i = 0; i < 2000; i++) {
        map<int, int> map;
        for (int j = 0; j < N; j++) {
            map[keys[j]] = j;
        }
        for (int j = 0; j < N; j++) {
            map[keys[j]];
        }
        for (int j = 0; j < N; j++) {
            map.erase(keys[j]);
        }
    }
}

int main() {
    TIME("CPP tree benchmark",
        tree_benchmark();
    )
}