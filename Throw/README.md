# Throw : error checking for C functions


## Basic usage

Include `throw.h` and enclose functions that can fail with the `THROW` macro.
If the function doesn't return 0, the program will exit after printing the error, its description and the stack trace (compile the program with `-g` for a detailed stack trace) to `stderr`. `THROW` uses `errno` to get the error code.
For example :
```c
THROW(gettimeofday(&time, NULL));
```

A message can also be added :
```c
THROW_MSG(gettimeofday(&time, NULL), "Failed to get time (using %p)", &time);
```


## Other cases

- If the function returns the error code instead of setting `errno`, use `THROW_R`.
```c
THROW_R(pthread_create(&thread, NULL, func, NULL));
```

- If the function returns a useful value, use `THROW_P` or `THROW_PR` and give the value to check as a second argument. `THROW_P` uses `errno` to get the error code and `THROW_PR` uses the given value.
```c
int result = THROW_PR(example_func(&err), err);
```

- If the function returns `0` in case of error, use `THROW_N` or `THROW_PN`. Both use `errno` to get the error code.
```c
int* p = THROW_PN(malloc(sizeof(int)), p);
```

- If the error code is non-standard, use `THROW_C` or `THROW_PC` to not print the unknown error description.
```c
cl_mem buffer = THROW_PC(clCreateBuffer(context, 0, sizeof(int), NULL, &err), err);
```

- To manually throw an error without an error code, use `THROW_ERR`.
```c
THROW_ERR("Error");
```