#include <iostream>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <immintrin.h>
#include <algorithm>
#include <chrono>
#include <numeric>
#include <string>
#include <vector>


constexpr double INVERSE_SQRT2 = 0.70710678118654746;

double normalCDF(double x) {
    return 0.5 * std::erfc(
        -x * INVERSE_SQRT2
    );
}

//taylor expansion - slow under lots of calls
double normalCDFfast(double x) {
    constexpr double twoOverSqrtPi = 1.1283791670955126;

    double z = x / std::sqrt(2.0);

    double term = z;
    double sum = term;

    // erf calculation with reccurance formula
    for (int i = 0; i < 31; i++) {
        term *= (-1 * z * z * (2*i + 1))/((i+1) * (2*i + 3));
        sum += term;
    }

    return 0.5 * (1.0 + twoOverSqrtPi * sum);

}

// Polynomial rational approximation, fast and fairly accurate, 
// more than enough precision for black scholes computations
inline double polynomialRational(double x) {
    constexpr double INV_SQRT_2PI =
        0.3989422804014327;

    constexpr double p  = 0.2316419;
    constexpr double b1 = 0.319381530;
    constexpr double b2 = -0.356563782;
    constexpr double b3 = 1.781477937;
    constexpr double b4 = -1.821255978;
    constexpr double b5 = 1.330274429;

    if (x >= 6.0)
        return 1.0;

    if (x <= -6.0)
        return 0.0;

    bool negative = x < 0.0;

    double z = std::abs(x);

    double t = 1.0 / (1 + p * z);

    double poly = t*(b1 + t*(b2 + t*(b3 + t*(b4 + b5*t))));
    double pdf = INV_SQRT_2PI * std::exp(-0.5 * z * z);

    double cdf = 1 - pdf*poly;

    return negative ? 1.0 - cdf : cdf;

}



// logistic cubic cdf approximation but loses a lot more accuracy
inline double logisticCubicCDFAppoximation(double x) {
    constexpr double A = 1.59760287;
    constexpr double B = 0.07056410;

    double ax = std::abs(x);
    double z = ax * (A + B * ax * ax);

    double p = 1.0 / (1.0 + std::exp(-z));

    return x >= 0.0 ? p : 1.0 - p;
}


struct BenchmarkResult {
    double medianMs;
    double meanMs;
    double minMs;
    double maxMs;
    double nsPerCall;
    double checksum;
};


template <double (*CDF)(double)>
BenchmarkResult benchmarkCDF(
    const std::vector<double>& inputs,
    int warmupRuns = 3,
    int measuredRuns = 15
) {
    using Clock = std::chrono::steady_clock;

    const std::size_t N = inputs.size();

    std::vector<double> outputs(N);

    for (int run = 0; run < warmupRuns; ++run) {

        for (std::size_t i = 0; i < N; ++i) {
            outputs[i] = CDF(inputs[i]);
        }
    }

    std::vector<double> times;
    times.reserve(measuredRuns);

    for (int run = 0; run < measuredRuns; ++run) {

        auto start = Clock::now();

        for (std::size_t i = 0; i < N; ++i) {
            outputs[i] = CDF(inputs[i]);
        }

        auto end = Clock::now();

        std::chrono::duration<double, std::milli> elapsed =
            end - start;

        times.push_back(elapsed.count());
    }


    double checksum =
        std::accumulate(
            outputs.begin(),
            outputs.end(),
            0.0
        );

    // stats
    std::vector<double> sortedTimes = times;

    std::sort(
        sortedTimes.begin(),
        sortedTimes.end()
    );

    double median;

    if (measuredRuns % 2 == 0) {
        median =
            (
                sortedTimes[measuredRuns / 2 - 1]
                +
                sortedTimes[measuredRuns / 2]
            )
            / 2.0;
    }
    else {
        median =
            sortedTimes[measuredRuns / 2];
    }

    double mean =
        std::accumulate(
            times.begin(),
            times.end(),
            0.0
        )
        / measuredRuns;

    double minimum =
        *std::min_element(
            times.begin(),
            times.end()
        );

    double maximum =
        *std::max_element(
            times.begin(),
            times.end()
        );

    double nsPerCall =
        median * 1'000'000.0 / N;

    return {
        median,
        mean,
        minimum,
        maximum,
        nsPerCall,
        checksum
    };
}

std::vector<double> createInputs(std::size_t N) {

    std::vector<double> inputs(N);

    for (std::size_t i = 0; i < N; ++i) {

        // Deterministic, mixed values roughly in [-6, 6]
        double x =
            3.0 * std::sin(i * 0.017)
            +
            2.0 * std::sin(i * 0.031)
            +
            1.0 * std::sin(i * 0.071);

        inputs[i] = x;
    }

    return inputs;
}


void printBenchmark(
    const std::string& name,
    const BenchmarkResult& result
) {
    std::cout << "\n"
              << name
              << '\n';

    std::cout << "-----------------------------\n";

    std::cout << std::fixed
              << std::setprecision(3);

    std::cout << std::left
              << std::setw(18)
              << "Median:"
              << result.medianMs
              << " ms\n";

    std::cout << std::setw(18)
              << "Mean:"
              << result.meanMs
              << " ms\n";

    std::cout << std::setw(18)
              << "Minimum:"
              << result.minMs
              << " ms\n";

    std::cout << std::setw(18)
              << "Maximum:"
              << result.maxMs
              << " ms\n";

    std::cout << std::setw(18)
              << "ns / call:"
              << result.nsPerCall
              << '\n';

    std::cout << std::setprecision(10);

    std::cout << std::setw(18)
              << "Checksum:"
              << result.checksum
              << '\n';
}


int main() {

    constexpr std::size_t N = 10'000'000;

    std::vector<double> inputs =
        createInputs(N);


    auto stdResult =
    benchmarkCDF<normalCDF>(inputs);

    auto polyResult =
        benchmarkCDF<polynomialRational>(inputs);

    auto logisticResult =
        benchmarkCDF<logisticCubicCDFAppoximation>(inputs);


    printBenchmark(
        "std::erfc",
        stdResult
    );

    printBenchmark(
        "Polynomial rational",
        polyResult
    );

    printBenchmark(
        "Logistic cubic",
        logisticResult
    );


    std::cout
        << "\nPolynomial speedup: "
        << stdResult.medianMs
           / polyResult.medianMs
        << "x\n";

    return 0;
}
    