#ifndef BENCHMARK_H
#define BENCHMARK_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <sys/time.h>


/**
 * @brief Run a benchmark for a given function
 * @param func The function to benchmark
 * @param arg The argument to pass to the function
 * @param name The name of the benchmark
 * @param time Approximate time the function should be run (in seconds)
 * @return The value returned by the function
**/
void* benchmark(void* (*func)(void*), void* arg, const char* name, double time);

/**
 * @brief Print the results of all previously run benchmarks
**/
void print_results();


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

    
#ifdef __cplusplus
}
#endif
#endif