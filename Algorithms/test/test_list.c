#include <stdlib.h>
#include <stdio.h>
#include "./algorithms.h"
#include "benchmark.h"

#define MAX_LENGTH 1048576

static void list_benchmark() {
    srand(314);
    for (int i = 0; i < 50; i++) {
        list_t list = list_new(1);
        for (int j = 0; j < 100; j++) {
            int r = rand();
            int s = (double)MAX_LENGTH * r / RAND_MAX;
            while (list.length < s) list_add(&list, r + list.length);
            while (list.length > s) list_pop(&list);
        }
        list_free(&list);
    }
}

static void list_test() {
    long h = 0;
    srand(314);
    for (int i = 0; i < 50; i++) {
        list_t list = list_new(1);
        for (int j = 0; j < 100; j++) {
            int r = rand();
            int s = (double)MAX_LENGTH * r / RAND_MAX;
            while (list.length < s) list_add(&list, r + list.length);
            while (list.length > s) h = h * 31 + list_pop(&list);
        }
        list_free(&list);
    }
    printf("%ld\n", h);
}

int main() {
    TIME("List benchmark", 
        list_benchmark();
    )
    list_test();
}