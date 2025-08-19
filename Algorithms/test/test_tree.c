#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "./algorithms.h"
#include "benchmark.h"
#include "throw.h"

#define N 10000

static void treeest() {
    srand(314);
    int keys[N];
    tree test = tree_new(2);
    for (int i = 0; i < N; i++) {
        keys[i] = rand();
        if (tree_contains(&test, keys[i]) || tree_ref(&test, keys[i]) != NULL) THROW_ERR("Key already in tree");
        tree_setOrAdd(&test, keys[i], i);
        keys[++i] = rand();
        tree_tryAdd(&test, keys[i], i);
        keys[++i] = rand();
        *tree_refOrEmpty(&test, keys[i]) = i;
        keys[++i] = rand();
        tree_refOrDefault(&test, keys[i], i);
    }
    if (test.length != N) THROW_ERR("Incorrect length");

    int sortedKeys[N];
    memcpy(sortedKeys, keys, sizeof(int) * N);
    sort(sortedKeys, N);
    tree_iter iter = tree_iterAll();
    tree_item* item;
    int i = 0;
    while ((item = tree_nextAll(&test, &iter))) {
        if (item->key != sortedKeys[i++]) THROW_ERR("Incorrect key");
    }
    if (i != N) THROW_ERR("Missing keys");

    for (int i = 0; i < 1000; i++) {
        int start = rand(), end = rand();
        int startIndex = search(sortedKeys, start, N);
        int endIndex = search(sortedKeys, end, N);
        tree_iter iter = tree_iterAfter(&test, start);
        tree_item* item;
        int j = 0;
        while ((item = tree_nextAfter(&test, start, &iter))) {
            if (item->key != sortedKeys[startIndex + j++]) THROW_ERR("Incorrect key");
        }
        if (j != N - startIndex) THROW_ERR("Missing keys");
        if (j != tree_countAfter(&test, start)) THROW_ERR("Incorrect count");
        iter = tree_iterBefore(&test, end);
        j = 0;
        while ((item = tree_nextBefore(&test, end, &iter))) {
            if (item->key != sortedKeys[j++]) THROW_ERR("Incorrect key");
        }
        if (j != endIndex) THROW_ERR("Missing keys");
        if (j != tree_countBefore(&test, end)) THROW_ERR("Incorrect count");
        if (endIndex >= startIndex) {
            iter = tree_iterBetween(&test, start, end);
            j = 0;
            while ((item = tree_nextBetween(&test, start, end, &iter))) {
                if (item->key != sortedKeys[startIndex + j++]) THROW_ERR("Incorrect key");
            }
            if (j != endIndex - startIndex) THROW_ERR("Missing keys");
            if (j != tree_countBetween(&test, start, end)) THROW_ERR("Incorrect count");
        }
        item = tree_floor(&test, start);
        if (item == NULL) {
            if (startIndex > 0 || sortedKeys[0] == start) THROW_ERR("Floor not found");
        }
        else {
            if (sortedKeys[startIndex] == start) {
                if (item->key != start) THROW_ERR("Incorrect floor");
            }
            else {
                if (startIndex == 0 || item->key != sortedKeys[startIndex - 1]) THROW_ERR("Incorrect floor");
            }
        }
        item = tree_ceil(&test, start);
        if (item == NULL) {
            if (startIndex < N) THROW_ERR("Ceil not found");
        }
        else {
            if (startIndex == N || item->key != sortedKeys[startIndex]) THROW_ERR("Incorrect ceil");
        }
    }

    for (int i = 0; i < N; i++) {
        if (!tree_contains(&test, keys[i])) THROW_ERR("Key not found");
        if (tree_get(&test, keys[i]) != i) THROW_ERR("Incorrect value");
        if (tree_getOrDefault(&test, keys[i], 314) != i) THROW_ERR("Incorrect value");
        if (*tree_ref(&test, keys[i]) != i) THROW_ERR("Incorrect ref");
        if (*tree_refOrEmpty(&test, keys[i]) != i) THROW_ERR("Incorrect ref");
        if (*tree_refOrDefault(&test, keys[i], 314) != i) THROW_ERR("Incorrect ref");
        tree_remove(&test, keys[i]);
        tree_remove(&test, keys[i]);
    }
    if (test.length != 0) THROW_ERR("Incorrect length");

    tree_free(&test);
}

static void tree_benchmark() {
    srand(314);
    int keys[N];
    for (int i = 0; i < N; i++) keys[i] = rand();
    for (int i = 0; i < 2000; i++) {
        tree test = tree_new(1);
        for (int j = 0; j < N; j++) {
            tree_tryAdd(&test, keys[j], j);
        }
        for (int j = 0; j < N; j++) {
            tree_get(&test, keys[j]);
        }
        for (int j = 0; j < N; j++) {
            tree_remove(&test, keys[j]);
        }
        tree_free(&test);
    }
}

int main() {
    TIME("Tree benchmark",
        tree_benchmark();
    )
    treeest();
}