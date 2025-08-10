#include <stdio.h>
#include "algorithms.h"

int main() {
    list_t list = list_new(1);
    list_ptr_t ptrs = list_new_ptr(1);
    
    for (int i = 0; i < 10; i++) {
        list_add(&list, i);
        list_add_ptr(&ptrs, list.values + i);
    }
    for (int i = 0; i < 10; i++) {
        printf("%d\n", list.values[i]);
    }
    for (int i = 0; i < 10; i++) {
        printf("%p\n", list_pop_ptr(&ptrs));
        printf("%d\n", list_pop(&list));
    }

    list_free(&list);
    list_free_ptr(&ptrs);
}