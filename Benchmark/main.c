#include "benchmark.h"
#include <stdio.h>
#include <stdlib.h>


void* test(void* arg) {
    printf("%zu\n", (size_t)arg);
    system("sleep 0.1");
    return (void*)314;
}


int main() {
    printf("%zu\n", (size_t)benchmark(test, (void*)42, "Test", 1));
    print_results();
}