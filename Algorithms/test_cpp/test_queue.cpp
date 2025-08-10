#include <cstdlib>
#include <cstdio>
#include <deque>
#include "benchmark.h"

using namespace std;

#define MAX_LENGTH 1048576

static void queue_benchmark() {
    srand(314);
    for (int i = 0; i < 50; i++) {
        deque<int> queue;
        for (int j = 0; j < 50; j++) {
            int r = rand();
            size_t s = (double)MAX_LENGTH * r / RAND_MAX;
            while (queue.size() < s) queue.push_front(r + queue.size());
            while (queue.size() > s) queue.pop_front();
            for (size_t k = 0; k < s; k++) {
                queue.push_front(r + k);
                queue.pop_back();
            }
            r = rand();
            s = (double)MAX_LENGTH * r / RAND_MAX;
            while (queue.size() < s) queue.push_back(r + queue.size());
            while (queue.size() > s) queue.pop_back();
            for (size_t k = 0; k < s; k++) {
                queue.push_back(r + k);
                queue.pop_front();
            }
        }
    }
}

static void queue_test() {
    long h = 0;
    srand(314);
    for (int i = 0; i < 50; i++) {
        deque<int> queue;
        for (int j = 0; j < 50; j++) {
            int r = rand();
            size_t s = (double)MAX_LENGTH * r / RAND_MAX;
            while (queue.size() < s) queue.push_front(r + queue.size());
            while (queue.size() > s) {
                h = h * 31 + queue[0];
                queue.pop_front();
            }
            for (size_t k = 0; k < s; k++) {
                queue.push_front(r + k);
                h = h * 31 + queue[queue.size() - 1];
                queue.pop_back();
            }
            r = rand();
            s = (double)MAX_LENGTH * r / RAND_MAX;
            while (queue.size() < s) queue.push_back(r + queue.size());
            while (queue.size() > s) {
                h = h * 31 + queue[queue.size() - 1];
                queue.pop_back();
            }
            for (size_t k = 0; k < s; k++) {
                queue.push_back(r + k);
                h = h * 31 + queue[0];
                queue.pop_front();
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