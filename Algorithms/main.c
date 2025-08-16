#include <stdio.h>
#include "algorithms.h"

int main() {
    list ints = list_new(1);
    list_ptr ptrs = list_new_ptr(1);
    
    for (int i = 0; i < 10; i++) {
        list_add(&ints, i);
        list_add_ptr(&ptrs, ints.values + i);
    }
    for (int i = 0; i < 10; i++) {
        printf("%d\n", ints.values[i]);
    }
    for (int i = 0; i < 10; i++) {
        printf("%p\n", list_pop_ptr(&ptrs));
        printf("%d\n", list_pop(&ints));
    }

    list_free(&ints);
    list_free_ptr(&ptrs);
}