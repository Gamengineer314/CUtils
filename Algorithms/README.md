# Algorithms : generic data structures and algorithms

## Usage

Create an `algorithms.h` file and include the data structures and algorithms you need.
Parameters are provided with `#define` directives above the corresponding `#include`.
The same file can be included multiple times with different parameters if needed.
For example, this file includes a list of `int`, a list of `float` and a queue of `int` :
```c
#ifndef ALGORITHMS_H
#define ALGORITHMS_H

// List of float
#define GEN_TYPE float
#include "list.h"

// List of int
#define GEN_TYPE int
#include "list.h"

// Queue of int
#define GEN_TYPE int
#include "queue.h"

#endif
```

Create an `algorithm.c` file with the following content, and compile it with your program :
```c
#define GEN_SOURCE
#include "algorithms.h"
```

The `algorithm.h` file can now be included anywhere in your program to use the requested algorithms.


## Algorithms

- `list` : simple list that can also be used as a stack.
- `queue` : double-ended queue implemented using a circular buffer
- `heap` : priority queue implemented using a min-heap
- `map` : hash map using separate chaining
- `tree` : red-black binary search tree
- `sort` : insertion sort, quick sort, radix sort