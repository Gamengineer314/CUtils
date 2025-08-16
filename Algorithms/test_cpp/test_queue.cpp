#include <cstdlib>
#include <cstdio>
#include <deque>
#include "benchmark.h"

using namespace std;

#define MAX_LENGTH 1048576

static void queue_benchmark() {
    srand(314);
    for (int i = 0; i < 50; i++) {
        deque<int> test;
        for (int j = 0; j < 50; j++) {
            int r = rand();
            size_t s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.size() < s) test.push_front(r + test.size());
            while (test.size() > s) test.pop_front();
            for (size_t k = 0; k < s; k++) {
                test.push_front(r + k);
                test.pop_back();
            }
            r = rand();
            s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.size() < s) test.push_back(r + test.size());
            while (test.size() > s) test.pop_back();
            for (size_t k = 0; k < s; k++) {
                test.push_back(r + k);
                test.pop_front();
            }
        }
    }
}

static void queue_test() {
    long h = 0;
    srand(314);
    for (int i = 0; i < 50; i++) {
        deque<int> test;
        for (int j = 0; j < 50; j++) {
            int r = rand();
            size_t s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.size() < s) test.push_front(r + test.size());
            while (test.size() > s) {
                h = h * 31 + test[0];
                test.pop_front();
            }
            for (size_t k = 0; k < s; k++) {
                test.push_front(r + k);
                h = h * 31 + test[test.size() - 1];
                test.pop_back();
            }
            r = rand();
            s = (double)MAX_LENGTH * r / RAND_MAX;
            while (test.size() < s) test.push_back(r + test.size());
            while (test.size() > s) {
                h = h * 31 + test[test.size() - 1];
                test.pop_back();
            }
            for (size_t k = 0; k < s; k++) {
                test.push_back(r + k);
                h = h * 31 + test[0];
                test.pop_front();
            }
        }
    }
    printf("%ld\n", h);
}

int main() {
    TIME("CPP queue benchmark", 
        queue_benchmark();
    )
    queue_test();
}