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
    map_t map = map_new(2);
    for (int i = 0; i < N; i++) {
        keys[i] = rand();
        if (map_contains(&map, keys[i]) || map_ref(&map, keys[i]) != NULL) THROW_ERR("Key already in map");
        map_add(&map, keys[i], i);
        keys[++i] = rand();
        map_setOrAdd(&map, keys[i], i);
        keys[++i] = rand();
        map_tryAdd(&map, keys[i], i);
        keys[++i] = rand();
        map_item_t* ref = map_refOrEmpty(&map, keys[i]);
        if (ref->key != keys[i]) THROW_ERR("Incorrect ref");
        ref->value = i;
        keys[++i] = rand();
        ref = map_refOrDefault(&map, keys[i], i);
        if (ref->key != keys[i] || ref->value != i) THROW_ERR("Incorrect ref");
    }
    if (map.length != N) THROW_ERR("Incorrect length");

    bool found[N] = {};
    map_iter_t iter = map_iter();
    map_item_t* item;
    while ((item = map_next(&map, &iter))) {
        if (item->key != keys[item->value]) THROW_ERR("Incorrect key");
        found[item->value] = true;
    }
    for (int i = 0; i < N; i++) {
        if (!found[i]) THROW_ERR("Value not found");
    }

    for (int i = 0; i < N; i++) {
        if (!map_contains(&map, keys[i])) THROW_ERR("Key not found %d", keys[i]);
        if (map_get(&map, keys[i]) != i) THROW_ERR("Incorrect value");
        if (map_getOrDefault(&map, keys[i], 314) != i) THROW_ERR("Incorrect value");
        map_item_t* ref = map_ref(&map, keys[i]);
        if (ref->key != keys[i] || ref->value != i) THROW_ERR("Incorrect ref");
        ref = map_refOrEmpty(&map, keys[i]);
        if (ref->key != keys[i] || ref->value != i) THROW_ERR("Incorrect ref");
        ref = map_refOrDefault(&map, keys[i], 314);
        if (ref->key != keys[i] || ref->value != i) THROW_ERR("Incorrect ref");
        map_remove(&map, keys[i]);
        map_remove(&map, keys[i]);
    }
    if (map.length != 0) THROW_ERR("Incorrect length %d");

    map_free(&map);
}

static void map_benchmark() {
    srand(314);
    int keys[N];
    for (int i = 0; i < N; i++) keys[i] = rand();
    for (int i = 0; i < 4000; i++) {
        map_t map = map_new(1);
        for (int j = 0; j < N; j++) {
            map_add(&map, keys[j], j);
        }
        for (int j = 0; j < N; j++) {
            map_get(&map, keys[j]);
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