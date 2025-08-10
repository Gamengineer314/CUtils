#include <cstdlib>
#include <cstdio>
#include <queue>
#include "benchmark.h"

using namespace std;

class Item {
public:
    int key, value;

    Item(int key, int value) : key(key), value(value) {}

    bool operator<(const Item& other) const {
        return key >= other.key;
    }
};

#define MAX_LENGTH 1048576

static void heap_benchmark() {
    srand(314);
    for (int i = 0; i < 10; i++) {
        priority_queue<Item> heap;
        for (int j = 0; j < 10; j++) {
            int r = rand();
            size_t s = (double)MAX_LENGTH * r / RAND_MAX;
            while (heap.size() < s) heap.push(Item(rand(), rand()));
            while (heap.size() > s) heap.pop();
        }
    }
}

static void heap_test() {
    long h = 0;
    srand(314);
    for (int i = 0; i < 10; i++) {
        priority_queue<Item> heap;
        for (int j = 0; j < 10; j++) {
            int r = rand();
            size_t s = (double)MAX_LENGTH * r / RAND_MAX;
            while (heap.size() < s) heap.push(Item(rand(), rand()));
            while (heap.size() > s) {
                Item item = heap.top();
                h = h * 31 + item.key;
                h = h * 31 + item.value;
                heap.pop();
            }
        }
    }
    printf("%ld\n", h);
}

static void heap_mod_test() {
    long h = 0;
    srand(314);
    priority_queue<Item> heap;
    int32_t values[5000];
    for (int i = 0; i < 5000; i++) {
        rand();
        values[i] = rand();
    }
    for (int i = 0; i < 10000; i++) rand();
    for (int i = 0; i < 5000; i++) {
        heap.push(Item(rand(), values[i]));
    }
    for (int i = 0; i < 5000; i++) {
        heap.push(Item(rand(), rand()));
        Item item = heap.top();
        h = h * 31 + item.key;
        h = h * 31 + item.value;
        heap.pop();
    }
    printf("%ld\n", h);
}

int main() {
    TIME("Heap benchmark", 
        heap_benchmark();
    )
    heap_test();
    heap_mod_test();
}