#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

// ============================================================
// DATA
// ============================================================

struct Option {
    double K;
    double T;
    double sigma;
};

struct Workload {
    std::vector<Option> options;
    std::vector<double> spots;

    double r;

    std::size_t calculationCount() const {
        return options.size() * spots.size();
    }
};

struct BenchmarkResult {
    double medianMs;
    double meanMs;
    double minMs;
    double maxMs;

    std::size_t calculations;
    int runs;

    double checksum;
};


// ============================================================
// WORKLOAD GENERATION
// Not included in benchmark timing
// ============================================================

Workload createWorkload() {
    Workload workload;

    workload.r = 0.04;

    // 8 expiration dates
    const std::vector<double> expirations = {
        0.05,
        0.10,
        0.20,
        0.30,
        0.50,
        0.75,
        1.00,
        2.00
    };

    const int strikesPerExpiry = 64;

    // Build option chain
    for (double T : expirations) {

        for (int i = 0; i < strikesPerExpiry; ++i) {

            double K =
                70.0
                + 60.0 * i / (strikesPerExpiry - 1);

            // Simple deterministic volatility surface
            double moneyness =
                (K - 100.0) / 100.0;

            double sigma =
                0.20
                + 0.15 * moneyness * moneyness
                + 0.02 * T;

            workload.options.push_back({
                K,
                T,
                sigma
            });
        }
    }

    // Simulate 2,000 underlying price updates
    const int spotUpdates = 2000;

    workload.spots.reserve(spotUpdates);

    for (int i = 0; i < spotUpdates; ++i) {

        double S =
            100.0
            + 3.0 * std::sin(i * 0.013)
            + 1.5 * std::sin(i * 0.031);

        workload.spots.push_back(S);
    }

    return workload;
}


// ============================================================
// BLACK-SCHOLES MATH
// ============================================================

double normalCDF(double x) {
    return 0.5 * std::erfc(
        -x / std::sqrt(2.0)
    );
}

double blackScholesCall(
    double S,
    double K,
    double T,
    double r,
    double sigma
) {
    double d1 =
        (
            std::log(S / K)
            + (r + 0.5 * sigma * sigma) * T
        )
        /
        (sigma * std::sqrt(T));

    double d2 =
        d1 - sigma * std::sqrt(T);

    return
        S * normalCDF(d1)
        -
        K * std::exp(-r * T) * normalCDF(d2);
}


// ============================================================
// VERSION 0: NAIVE IMPLEMENTATION
//
// THIS IS THE PART YOU OPTIMIZE.
// ============================================================

void priceNaive(
    const Workload& workload,
    std::vector<double>& prices
) {
    std::size_t index = 0;

    for (double S : workload.spots) {

        for (const Option& option : workload.options) {

            prices[index] = blackScholesCall(
                S,
                option.K,
                option.T,
                workload.r,
                option.sigma
            );

            ++index;
        }
    }
}


// ============================================================
// BENCHMARK UTILITIES
// ============================================================

double calculateChecksum(
    const std::vector<double>& prices
) {
    double sum = 0.0;

    for (double price : prices) {
        sum += price;
    }

    return sum;
}


// Pricing function is chosen at compile time.
// This avoids adding function-pointer overhead to the benchmark.
template <
    void (*PricingFunction)(
        const Workload&,
        std::vector<double>&
    )
>
BenchmarkResult runBenchmark(
    const Workload& workload,
    int warmupRuns = 3,
    int measuredRuns = 15
) {
    using Clock = std::chrono::steady_clock;

    const std::size_t totalCalculations =
        workload.calculationCount();

    // Allocate before timing
    std::vector<double> prices(totalCalculations);

    double checksum = 0.0;


    // --------------------------------------------------------
    // Warm-up
    // --------------------------------------------------------

    for (int i = 0; i < warmupRuns; ++i) {

        PricingFunction(workload, prices);

        // Make sure results are actually used
        checksum += calculateChecksum(prices);
    }


    // --------------------------------------------------------
    // Measured runs
    // --------------------------------------------------------

    std::vector<double> durations;

    durations.reserve(measuredRuns);

    for (int run = 0; run < measuredRuns; ++run) {

        auto start = Clock::now();

        PricingFunction(workload, prices);

        auto end = Clock::now();


        std::chrono::duration<double, std::milli>
            elapsed = end - start;

        durations.push_back(elapsed.count());


        // Outside timed section
        checksum += calculateChecksum(prices);
    }


    // --------------------------------------------------------
    // Statistics
    // --------------------------------------------------------

    std::vector<double> sorted = durations;

    std::sort(
        sorted.begin(),
        sorted.end()
    );

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


// ============================================================
// OUTPUT
// ============================================================

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


// ============================================================
// MAIN
// ============================================================

int main() {

    Workload workload =
        createWorkload();

    BenchmarkResult result =
        runBenchmark<priceNaive>(workload);

    printBenchmark(
        "V0 - Naive",
        workload,
        result
    );

    return 0;
}