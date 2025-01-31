# Benchmark : measure execution time of a function

Include `benchmark.h` and use the `benchmark` function to run a benchmark on a given function for a given time.
The function must take a `void*` argument (given to `benchmark`) and return a `void*` (returned by `benchmark`).
It will be run multiple times while measuring elapsed time for approximately the requested time.
At the end of the program, use the `print_result` function to print the results (mean and standard deviation for each function) to `stderr`.
For example :
```c
#include "benchmark.h"

void* example_func(void* arg) {
    return NULL;
}

int main() {
    void* arg = NULL;
    void* result = benchmark(example_func, arg, "Example benchmark", 2);
    // ...
    print_results();
}
```