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
    Map map = map_new(2);
    for (int i = 0; i < N; i++) {
        keys[i] = rand();
        if (map_contains(&map, keys[i]) || map_ref(&map, keys[i]) != NULL) THROW_ERR("Key already in map");
        map_add(&map, keys[i], i);
        keys[++i] = rand();
        map_setOrAdd(&map, keys[i], i);
        keys[++i] = rand();
        if (!map_tryAdd(&map, keys[i], i)) THROW_ERR("Key not added");
        keys[++i] = rand();
        bool added;
        *map_refOrEmpty(&map, keys[i], &added) = i;
        if (!added) THROW_ERR("Key not added");
        keys[++i] = rand();
        map_refOrDefault(&map, keys[i], i, &added);
        if (!added) THROW_ERR("Key not added");
    }
    if (map.length != N) THROW_ERR("Incorrect length");

    bool found[N] = {};
    MapIter iter = map_iterStart();
    MapKV* item;
    while ((item = map_iterNext(&map, &iter))) {
        if (item->key != keys[item->value]) THROW_ERR("Incorrect key");
        found[item->value] = true;
    }
    for (int i = 0; i < N; i++) {
        if (!found[i]) THROW_ERR("Value not found");
    }

    for (int i = 0; i < N; i++) {
        if (!map_contains(&map, keys[i])) THROW_ERR("Key not found");
        if (map_get(&map, keys[i]) != i) THROW_ERR("Incorrect value");
        if (map_getOrDefault(&map, keys[i], 314) != i) THROW_ERR("Incorrect value");
        if (*map_ref(&map, keys[i]) != i) THROW_ERR("Incorrect ref");
        bool added;
        if (*map_refOrEmpty(&map, keys[i], &added) != i) THROW_ERR("Incorrect ref");
        if (added) THROW_ERR("Key added");
        if (*map_refOrDefault(&map, keys[i], 314, &added) != i) THROW_ERR("Incorrect ref");
        if (added) THROW_ERR("Key added");
        int* ref = map_remove(&map, keys[i]);
        if (!ref) THROW_ERR("Key not found");
        if (*ref != i) THROW_ERR("Incorrect ref");
        ref = map_remove(&map, keys[i]);
        if (ref) THROW_ERR("Key not removed");
    }
    if (map.length != 0) THROW_ERR("Incorrect length");

    map_free(&map);
}

static void map_benchmark() {
    srand(314);
    int keys[N];
    for (int i = 0; i < N; i++) keys[i] = rand();
    for (int i = 0; i < 4000; i++) {
        Map map = map_new(1);
        for (int j = 0; j < N; j++) {
            map_add(&map, keys[j], j);
        }
        for (int j = 0; j < N; j++) {
            map_contains(&map, keys[j]);
        }
        for (int j = 0; j < N; j++) {
            map_remove(&map, keys[j]);
        }
        map_free(&map);
    }
}

int main() {
    TIME("Map benchmark",
        map_benchmark();
    )
    map_test();
}