#include "benchmark.h"
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include "throw.h"


static const char** names = NULL;
static double* means = NULL;
static double* stdevs = NULL;
static int count = 0;
static int capacity = 0;

struct timeval _t1, _t2;


void* benchmark(void* (*func)(void*), void* arg, const char* name, double time) {
    // Get result
    THROW(gettimeofday(&_t1, NULL));
    void* result = func(arg);
    THROW(gettimeofday(&_t2, NULL));
    double batchTime = (_t2.tv_sec - _t1.tv_sec) + (_t2.tv_usec - _t1.tv_usec) / 1e6;

    double mean;
    double stdev;
    if (batchTime < time * 0.75) {
        // Find best batch size
        long batchSize = 5;
        while (batchTime < time / 20) {
            THROW(gettimeofday(&_t1, NULL));
            for (int i = 0; i < batchSize; i++) {
                func(arg);
            }
            THROW(gettimeofday(&_t2, NULL));
            batchTime = (_t2.tv_sec - _t1.tv_sec) + (_t2.tv_usec - _t1.tv_usec) / 1e6;
            batchSize *= 5;
        }
        batchSize /= 5;

        // Run batches
        int n = 1 + (int)(time / batchTime);
        double times[n];
        times[0] = batchTime * 1e6 / batchSize;
        for (int i = 1; i < n; i++) {
            THROW(gettimeofday(&_t1, NULL));
            for (int j = 0; j < batchSize; j++) {
                func(arg);
            }
            THROW(gettimeofday(&_t2, NULL));
            times[i] = ((_t2.tv_sec - _t1.tv_sec) * 1e6 + (_t2.tv_usec - _t1.tv_usec)) / batchSize;
        }

        // Compute results
        mean = 0;
        for (int i = 0; i < n; i++) {
            mean += times[i];
        }
        mean /= n;
        stdev = 0;
        for (int i = 0; i < n; i++) {
            stdev += (times[i] - mean) * (times[i] - mean);
        }
        stdev = sqrt(stdev / (n - 1));
    }
    else {
        mean = batchTime * 1e6;
        stdev = -1;
    }

    // Add results
    if (++count > capacity) {
        capacity *= 2;
        if (capacity == 0) capacity = 1;
        names = THROW_PN(realloc(names, capacity * sizeof(char*)), names);
        means = THROW_PN(realloc(means, capacity * sizeof(double)), means);
        stdevs = THROW_PN(realloc(stdevs, capacity * sizeof(double)), stdevs);
    }
    names[count-1] = name;
    means[count-1] = mean;
    stdevs[count - 1] = stdev;

    return result;
}


void print_results() {
    // Print results
    fprintf(stderr, "Benchmark results:\n");
    for (int i = 0; i < count; i++) {
        if (stdevs[i] == -1) {
            fprintf(stderr, "%s: %lf us (std-dev: ??? µs)\n", names[i], means[i]);
        }
        else {
            fprintf(stderr, "%s: %lf us (std-dev: %lf µs)\n", names[i], means[i], stdevs[i]);
        }
    }

    // Clear results
    free(names);
    names = NULL;
    free(means);
    means = NULL;
    free(stdevs);
    stdevs = NULL;
    count = 0;
    capacity = 0;
}