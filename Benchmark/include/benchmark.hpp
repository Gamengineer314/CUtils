#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include "benchmark_time.h"

#include <functional>
#include <tuple>
#include <type_traits>


extern "C" {
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
}


template<typename TResult, typename... TArgs>
struct _BenchmarkFunction {
    std::function<TResult(TArgs...)> func;
    std::tuple<TArgs...> args;
};

template<typename TResult, typename... TArgs>
void* _benchmark_wrapper(void* arg) {
    auto* function = static_cast<_BenchmarkFunction<TResult, TArgs...>*>(arg);
    if constexpr (std::is_void_v<TResult>) {
        std::apply(function->func, function->args);
        return nullptr;
    }
    else return new TResult(std::apply(function->func, function->args));
}


/**
 * @brief Run a benchmark for a given function
 * @param func The function to benchmark
 * @param arg The argument to pass to the function
 * @param name The name of the benchmark
 * @param time Approximate time the function should be run (in seconds)
 * @return The value returned by the function
**/
template<typename TFunc, typename... TArgs>
auto benchmark(TFunc&& func, const char* name, double time, TArgs... args) {
    using TResult = std::invoke_result_t<TFunc, TArgs...>;
    _BenchmarkFunction<TResult, TArgs...> function{func, {args...}};
    void* vpResult = benchmark(_benchmark_wrapper<TResult, TArgs...>, name, time, (void*)&function);
    if constexpr (!std::is_void_v<TResult>) {
        TResult* pResult = static_cast<TResult*>(vpResult);
        TResult result = *pResult;
        delete pResult;
        return result;
    }
}


#endif