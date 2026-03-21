#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "benchmark_time.h"

/**
 * @brief Run a benchmark for a given function
 * @param func The function to benchmark
 * @param name The name of the benchmark
 * @param time Approximate time the function should be run (in seconds)
 * @param arg The argument to pass to the function
 * @return The value returned by the function
**/
void* benchmark(void* (*func)(void*), const char* name, double time, void* arg);

/**
 * @brief Print the results of all previously run benchmarks
**/
void print_results();

#endif