#include <stdlib.h>
#include "benchmark.h"

void* example_func(void* arg) {
    return NULL;
}

int main() {
    void* arg = NULL;
    void* result = benchmark(example_func, arg, "Example benchmark", 2);
    print_results();
}