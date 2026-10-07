#pragma once

#include "workload.hpp"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <numeric>
#include <vector>

struct BenchmarkResult {
    double medianMs;
    double meanMs;
    double minMs;
    double maxMs;
    std::size_t calculations;
    int runs;
    double checksum;
};

double calculateChecksum(const std::vector<double>& prices);

template <typename PricingFunction>
BenchmarkResult runBenchmark(
    const Workload& workload,
    PricingFunction pricingFunction,
    int warmupRuns = 3,
    int measuredRuns = 15
) {
    using Clock = std::chrono::steady_clock;

    const std::size_t totalCalculations =
        workload.calculationCount();

    std::vector<double> prices(totalCalculations);

    double checksum = 0.0;

    for (int i = 0; i < warmupRuns; ++i) {
        pricingFunction(workload, prices);

        checksum += calculateChecksum(prices);
    }

    std::vector<double> durations;
    durations.reserve(measuredRuns);

    for (int run = 0; run < measuredRuns; ++run) {
        auto start = Clock::now();

        pricingFunction(workload, prices);

        auto end = Clock::now();

        std::chrono::duration<double, std::milli> elapsed =
            end - start;

        durations.push_back(elapsed.count());

        checksum += calculateChecksum(prices);
    }

    std::vector<double> sorted = durations;

    std::sort(sorted.begin(), sorted.end());

    double median =
        sorted[sorted.size() / 2];

    double mean =
        std::accumulate(
            durations.begin(),
            durations.end(),
            0.0
        )
        / durations.size();

    double min =
        *std::min_element(
            durations.begin(),
            durations.end()
        );

    double max =
        *std::max_element(
            durations.begin(),
            durations.end()
        );

    return {
        median,
        mean,
        min,
        max,
        totalCalculations,
        measuredRuns,
        checksum
    };
}

void printBenchmark(
    const char* version,
    const Workload& workload,
    const BenchmarkResult& result
);
