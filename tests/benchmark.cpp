#include <benchmark/benchmark.h>

#include <vector>
#include <list>
#include <map>

#include <cstdlib>
#include <ctime>
#include <numeric>

#include "any_iterator.hpp"
#include "any_iterator_virtual.hpp"

template< class IterT, class ContainerT >
void benchmark_iteration(benchmark::State &state)
{
    using Iter = IterT;

    for (auto _ : state)
    {
        state.PauseTiming();
        ContainerT container(state.range(0));
        srand(static_cast<unsigned>(time(NULL)));
        for (auto& v : container)
            v = rand();
        volatile int result = 0;

        Iter it{ container.begin() };
        const Iter it_end{ container.end() };

        state.ResumeTiming();
        int intm = 0;
        for (; it != it_end; ++it)
            intm += *it;
        result = intm;
    }


    state.SetBytesProcessed(int64_t(state.iterations()) *
        int64_t(state.range(0))*sizeof(int));
}

template< class IterT, class ContainerT >
void benchmark_iteration_accumulate(benchmark::State &state)
{
    using Iter = IterT;

    for (auto _ : state)
    {
        state.PauseTiming();
        ContainerT container(state.range(0));
        srand(static_cast<unsigned>(time(NULL)));
        for (auto& v : container)
            v = rand();
        volatile int result = 0;

        Iter it{ container.begin() };
        const Iter it_end{ container.end() };

        state.ResumeTiming();
        result = std::accumulate(it, it_end, 0);
    }


    state.SetBytesProcessed(int64_t(state.iterations()) *
        int64_t(state.range(0)) * sizeof(int));
}

template< class IterT >
void benchmark_iteration_map(benchmark::State &state)
{
    using Iter = IterT;

    for (auto _ : state)
    {
        state.PauseTiming();
        std::map<int,int> container;
        srand(static_cast<unsigned>(time(NULL)));
        for (size_t i = 0; i < state.range(0); ++i)
            container.emplace(static_cast<int>(i), rand());
        volatile int result = 0;

        Iter it{ container.begin() };
        const Iter it_end{ container.end() };

        state.ResumeTiming();
        int intm = 0;
        for (; it != it_end; ++it)
            intm += it->second;
        result = intm;
    }


    state.SetBytesProcessed(int64_t(state.iterations()) *
        int64_t(state.range(0)) * sizeof(int));
}

constexpr size_t MY_RANGE_START = 8 << 5;
constexpr size_t MY_RANGE_END = 8 << 20;
constexpr benchmark::TimeUnit tu = benchmark::kMicrosecond;

//BENCHMARK_TEMPLATE(benchmark_iteration, std::vector<int>::iterator, std::vector<int>)->Range(MY_RANGE_START, MY_RANGE_END);
//BENCHMARK_TEMPLATE(benchmark_iteration, tyti::any_iterator<int>, std::vector<int>)->Range(MY_RANGE_START, MY_RANGE_END);
BENCHMARK_TEMPLATE(benchmark_iteration, std::list<int>::iterator, std::list<int>)->Range(MY_RANGE_START, MY_RANGE_END)->Unit(tu);
BENCHMARK_TEMPLATE(benchmark_iteration, tyti::any_iterator<int>, std::list<int>)->Range(MY_RANGE_START, MY_RANGE_END)->Unit(tu);
BENCHMARK_TEMPLATE(benchmark_iteration, tyti::any_iterator_virtual<int>, std::list<int>)->Range(MY_RANGE_START, MY_RANGE_END)->Unit(tu);

//BENCHMARK_TEMPLATE(benchmark_iteration_accumulate, std::vector<int>::iterator, std::vector<int>)->Range(MY_RANGE_START, MY_RANGE_END);
////BENCHMARK_TEMPLATE(benchmark_iteration_accumulate, tyti::any_iterator<int>, std::vector<int>)->Range(MY_RANGE_START, MY_RANGE_END);
//BENCHMARK_TEMPLATE(benchmark_iteration_accumulate, std::list<int>::iterator, std::list<int>)->Range(MY_RANGE_START, MY_RANGE_END)->Unit(tu);
//BENCHMARK_TEMPLATE(benchmark_iteration_accumulate, tyti::any_iterator<int>, std::list<int>)->Range(MY_RANGE_START, MY_RANGE_END)->Unit(tu);
//BENCHMARK_TEMPLATE(benchmark_iteration_accumulate, tyti2::any_iterator<int>, std::list<int>)->Range(MY_RANGE_START, MY_RANGE_END)->Unit(tu);

BENCHMARK_TEMPLATE(benchmark_iteration_map, std::map<int, int>::iterator)->Range(MY_RANGE_START, MY_RANGE_END)->Unit(tu);
BENCHMARK_TEMPLATE(benchmark_iteration_map, tyti::any_iterator<std::pair<const int, int>>)->Range(MY_RANGE_START, MY_RANGE_END)->Unit(tu);
BENCHMARK_TEMPLATE(benchmark_iteration_map, tyti::any_iterator_virtual<std::pair<const int,int>>)->Range(MY_RANGE_START, MY_RANGE_END)->Unit(tu);

BENCHMARK_MAIN();