#include "benchmark.hpp"

#include <iomanip>
#include <iostream>
#include <vector>

double calculateChecksum(const std::vector<double>& prices) {
    double sum = 0.0;

    for (double price : prices) {
        sum += price;
    }

    return sum;
}

void printBenchmark(
    const char* version,
    const Workload& workload,
    const BenchmarkResult& result
) {
    double calculationsPerSecond =
        result.calculations
        / (result.medianMs / 1000.0);

    double nsPerCalculation =
        (result.medianMs * 1'000'000.0)
        / result.calculations;

    std::cout
        << "\n========================================\n";

    std::cout
        << " Black-Scholes Benchmark\n";

    std::cout
        << "========================================\n";

    std::cout << std::left;

    std::cout << std::setw(22)
              << "Version:"
              << version
              << '\n';

    std::cout << std::setw(22)
              << "Options / update:"
              << workload.options.size()
              << '\n';

    std::cout << std::setw(22)
              << "Spot updates:"
              << workload.spots.size()
              << '\n';

    std::cout << std::setw(22)
              << "Calculations:"
              << result.calculations
              << '\n';

    std::cout << std::setw(22)
              << "Measured runs:"
              << result.runs
              << '\n';

    std::cout
        << "----------------------------------------\n";

    std::cout << std::fixed
              << std::setprecision(3);

    std::cout << std::setw(22)
              << "Median:"
              << result.medianMs
              << " ms\n";

    std::cout << std::setw(22)
              << "Mean:"
              << result.meanMs
              << " ms\n";

    std::cout << std::setw(22)
              << "Minimum:"
              << result.minMs
              << " ms\n";

    std::cout << std::setw(22)
              << "Maximum:"
              << result.maxMs
              << " ms\n";

    std::cout
        << "----------------------------------------\n";

    std::cout << std::setw(22)
              << "ns / calculation:"
              << nsPerCalculation
              << '\n';

    std::cout << std::setw(22)
              << "Million calc / sec:"
              << calculationsPerSecond / 1'000'000.0
              << '\n';

    std::cout << std::setw(22)
              << "Checksum:"
              << result.checksum
              << '\n';

    std::cout
        << "========================================\n";
}
