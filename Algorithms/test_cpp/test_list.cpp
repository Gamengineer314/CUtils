#include <cstdlib>
#include <cstdio>
#include <vector>
#include "benchmark.h"

using namespace std;

#define MAX_LENGTH 1048576

static void list_benchmark() {
    srand(314);
    for (int i = 0; i < 50; i++) {
        vector<int> list;
        for (int j = 0; j < 100; j++) {
            int r = rand();
            size_t s = (double)MAX_LENGTH * r / RAND_MAX;
            while (list.size() < s) list.push_back(r + list.size());
            while (list.size() > s) list.pop_back();
        }
    }
}

static void list_test() {
    long h = 0;
    srand(314);
    for (int i = 0; i < 50; i++) {
        vector<int> list;
        for (int j = 0; j < 100; j++) {
            int r = rand();
            size_t s = (double)MAX_LENGTH * r / RAND_MAX;
            while (list.size() < s) list.push_back(r + list.size());
            while (list.size() > s) {
                h = h * 31 + list[list.size() - 1];
                list.pop_back();
            }
        }
    }
    printf("%ld\n", h);
}

int main() {
    TIME("CPP list benchmark", 
        list_benchmark();
    )
    list_test();
}