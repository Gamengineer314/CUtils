#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "./algorithms.h"
#include "benchmark.h"
#include "throw.h"

#define N 10000

static void tree_test() {
    srand(314);
    int keys[N];
    tree_t tree = tree_new(2);
    for (int i = 0; i < N; i++) {
        keys[i] = rand();
        if (tree_contains(&tree, keys[i]) || tree_ref(&tree, keys[i]) != NULL) THROW_ERR("Key already in tree");
        tree_setOrAdd(&tree, keys[i], i);
        keys[++i] = rand();
        tree_tryAdd(&tree, keys[i], i);
        keys[++i] = rand();
        tree_item_t* ref = tree_refOrEmpty(&tree, keys[i]);
        if (ref->key != keys[i]) THROW_ERR("Incorrect ref");
        ref->value = i;
        keys[++i] = rand();
        ref = tree_refOrDefault(&tree, keys[i], i);
        if (ref->key != keys[i] || ref->value != i) THROW_ERR("Incorrect ref");
    }
    if (tree.length != N) THROW_ERR("Incorrect length");

    int sortedKeys[N];
    memcpy(sortedKeys, keys, sizeof(int) * N);
    sort(sortedKeys, N);
    tree_iter_t iter = tree_iter();
    tree_item_t* item;
    int i = 0;
    while ((item = tree_next(&tree, &iter))) {
        if (item->key != sortedKeys[i++]) THROW_ERR("Incorrect key");
    }
    if (i != N) THROW_ERR("Missing keys");

    for (int i = 0; i < 1000; i++) {
        int start = rand(), end = rand();
        int startIndex = search(sortedKeys, start, N);
        int endIndex = search(sortedKeys, end, N);
        tree_iter_t iter = tree_iterAfter(&tree, start);
        tree_item_t* item;
        int j = 0;
        while ((item = tree_nextAfter(&tree, start, &iter))) {
            if (item->key != sortedKeys[startIndex + j++]) THROW_ERR("Incorrect key");
        }
        if (j != N - startIndex) THROW_ERR("Missing keys");
        if (j != tree_countAfter(&tree, start)) THROW_ERR("Incorrect count");
        iter = tree_iterBefore(&tree, end);
        j = 0;
        while ((item = tree_nextBefore(&tree, end, &iter))) {
            if (item->key != sortedKeys[j++]) THROW_ERR("Incorrect key");
        }
        if (j != endIndex) THROW_ERR("Missing keys");
        if (j != tree_countBefore(&tree, end)) THROW_ERR("Incorrect count");
        if (endIndex >= startIndex) {
            iter = tree_iterBetween(&tree, start, end);
            j = 0;
            while ((item = tree_nextBetween(&tree, start, end, &iter))) {
                if (item->key != sortedKeys[startIndex + j++]) THROW_ERR("Incorrect key");
            }
            if (j != endIndex - startIndex) THROW_ERR("Missing keys");
            if (j != tree_countBetween(&tree, start, end)) THROW_ERR("Incorrect count");
        }
    }

    for (int i = 0; i < N; i++) {
        if (!tree_contains(&tree, keys[i])) THROW_ERR("Key not found %d", keys[i]);
        if (tree_get(&tree, keys[i]) != i) THROW_ERR("Incorrect value");
        if (tree_getOrDefault(&tree, keys[i], 314) != i) THROW_ERR("Incorrect value");
        tree_item_t* ref = tree_ref(&tree, keys[i]);
        if (ref->key != keys[i] || ref->value != i) THROW_ERR("Incorrect ref");
        ref = tree_refOrEmpty(&tree, keys[i]);
        if (ref->key != keys[i] || ref->value != i) THROW_ERR("Incorrect ref");
        ref = tree_refOrDefault(&tree, keys[i], 314);
        if (ref->key != keys[i] || ref->value != i) THROW_ERR("Incorrect ref");
        tree_remove(&tree, keys[i]);
        tree_remove(&tree, keys[i]);
    }
    if (tree.length != 0) THROW_ERR("Incorrect length %d");

    tree_free(&tree);
}

static void tree_benchmark() {
    srand(314);
    int keys[N];
    for (int i = 0; i < N; i++) keys[i] = rand();
    for (int i = 0; i < 2000; i++) {
        tree_t tree = tree_new(1);
        for (int j = 0; j < N; j++) {
            tree_tryAdd(&tree, keys[j], j);
        }
        for (int j = 0; j < N; j++) {
            tree_get(&tree, keys[j]);
        }
        for (int j = 0; j < N; j++) {
            tree_remove(&tree, keys[j]);
        }
        tree_free(&tree);
    }
}

int main() {
    TIME("Tree benchmark",
        //tree_benchmark();
    )
    tree_test();
}