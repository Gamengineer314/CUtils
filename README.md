# Throw : error checking for C functions


## Basic usage

Include `throw.h` and enclose functions that can fail with the `THROW` macro.
If the function doesn't return 0, the program will exit after printing the error, its description and the stack trace (compile the program with `-g` for a detailed stack trace) to `stderr`.
For example :
```c
THROW(gettimeofday(&time, NULL));
```

A message can also be added :
```c
THROW_MSG(gettimeofday(&time, NULL), "Failed to get time (using %p)", &time);
```


## Other cases

- If the function returns `0` in case of error, use `THROW_N` :
```c
THROW_N(malloc(sizeof(int)));
```

- If the function returns the error code instead of setting `errno`, use `THROW_R` :
```c
THROW_R(pthread_create(&thread, NULL, func, NULL));
```

- If the function returns the error in a given pointer, use `THROW_P` :
```c
int result = THROW_P(example_func(&err), err);
```

- If the function returns (as its return value or in a given pointer) a non-standard error code, use `THROW_RC` or `THROW_PC` to not print the unknown description :
```c
cl_mem buffer = THROW_PC(clCreateBuffer(context, 0, sizeof(int), NULL, &err), err);
```

- To manually throw an error without an error code, use `THROW_ERR` :
```c
THROW_ERR("Error");
```



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