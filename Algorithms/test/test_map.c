#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "./algorithms.h"
#include "benchmark.h"
#include "throw.h"

#define N 10000

static void map_test() {
    srand(314);
    int keys[N];
    map test = map_new(2);
    for (int i = 0; i < N; i++) {
        keys[i] = rand();
        if (map_contains(&test, keys[i]) || map_ref(&test, keys[i]) != NULL) THROW_ERR("Key already in map");
        map_add(&test, keys[i], i);
        keys[++i] = rand();
        map_setOrAdd(&test, keys[i], i);
        keys[++i] = rand();
        map_tryAdd(&test, keys[i], i);
        keys[++i] = rand();
        *map_refOrEmpty(&test, keys[i]) = i;
        keys[++i] = rand();
        map_refOrDefault(&test, keys[i], i);
    }
    if (test.length != N) THROW_ERR("Incorrect length");

    bool found[N] = {};
    map_iter iter = map_iterStart();
    map_kv* item;
    while ((item = map_iterNext(&test, &iter))) {
        if (item->key != keys[item->value]) THROW_ERR("Incorrect key");
        found[item->value] = true;
    }
    for (int i = 0; i < N; i++) {
        if (!found[i]) THROW_ERR("Value not found");
    }

    for (int i = 0; i < N; i++) {
        if (!map_contains(&test, keys[i])) THROW_ERR("Key not found %d", keys[i]);
        if (map_get(&test, keys[i]) != i) THROW_ERR("Incorrect value");
        if (map_getOrDefault(&test, keys[i], 314) != i) THROW_ERR("Incorrect value");
        if (*map_ref(&test, keys[i]) != i) THROW_ERR("Incorrect ref");
        if (*map_refOrEmpty(&test, keys[i]) != i) THROW_ERR("Incorrect ref");
        if (*map_refOrDefault(&test, keys[i], 314) != i) THROW_ERR("Incorrect ref");
        map_remove(&test, keys[i]);
        map_remove(&test, keys[i]);
    }
    if (test.length != 0) THROW_ERR("Incorrect length %d");

    map_free(&test);
}

static void map_benchmark() {
    srand(314);
    int keys[N];
    for (int i = 0; i < N; i++) keys[i] = rand();
    for (int i = 0; i < 4000; i++) {
        map test = map_new(1);
        for (int j = 0; j < N; j++) {
            map_add(&test, keys[j], j);
        }
        for (int j = 0; j < N; j++) {
            map_contains(&test, keys[j]);
        }
        for (int j = 0; j < N; j++) {
            map_remove(&test, keys[j]);
        }
        map_free(&test);
    }
}

int main() {
    TIME("Map benchmark",
        map_benchmark();
    )
    map_test();
}