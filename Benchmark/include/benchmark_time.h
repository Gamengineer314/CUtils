#ifndef BENCHMARK_TIME_H
#define BENCHMARK_TIME_H

#include <stdio.h>
#include <sys/time.h>

extern struct timeval _t1, _t2;

/**
 * @brief Run some code once and print the time it took
 * @param name The name of the code
 * @param code The code to run
**/
#define TIME(name, code) \
    gettimeofday(&_t1, NULL); \
    code \
    gettimeofday(&_t2, NULL); \
    printf(name" : %ld us\n", (_t2.tv_sec - _t1.tv_sec) * 1000000 + (_t2.tv_usec - _t1.tv_usec));

#endif