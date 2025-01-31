#ifndef BENCHMARK_H
#define BENCHMARK_H
#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif
#endif