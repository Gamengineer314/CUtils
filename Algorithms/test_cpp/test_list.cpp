#include <cstdlib>
#include <cstdio>
#include <vector>
#include "benchmark.h"

using namespace std;

#define MAX_LENGTH 1048576

static void list_benchmark() {
    srand(314);
    for (int i = 0; i < 50; i++) {
        vector<int> test;
        for (int j = 0; j < 100; j++) {
            int r = rand();
            size_t s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.size() < s) test.push_back(r + test.size());
            while (test.size() > s) test.pop_back();
        }
    }
}

static void list_test() {
    long h = 0;
    srand(314);
    for (int i = 0; i < 50; i++) {
        vector<int> test;
        for (int j = 0; j < 100; j++) {
            int r = rand();
            size_t s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.size() < s) test.push_back(r + test.size());
            while (test.size() > s) {
                h = h * 31 + test[test.size() - 1];
                test.pop_back();
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