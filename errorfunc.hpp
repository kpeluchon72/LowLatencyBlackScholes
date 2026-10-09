#pragma once

#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

double normalCDF(double x);

double normalCDFfast(double x);

inline double polynomialRationalCDF(double x) {
    constexpr double INV_SQRT_2PI = 0.3989422804014327;
    constexpr double p = 0.2316419;
    constexpr double b1 = 0.319381530;
    constexpr double b2 = -0.356563782;
    constexpr double b3 = 1.781477937;
    constexpr double b4 = -1.821255978;
    constexpr double b5 = 1.330274429;

    if (x >= 6.0) {
        return 1.0;
    }

    if (x <= -6.0) {
        return 0.0;
    }

    const bool negative = x < 0.0;
    const double z = std::abs(x);
    const double t = 1.0 / (1.0 + p * z);
    const double poly =
        t * (b1 + t * (b2 + t * (b3 + t * (b4 + b5 * t))));
    const double pdf = INV_SQRT_2PI * std::exp(-0.5 * z * z);
    const double cdf = 1.0 - pdf * poly;

    return negative ? 1.0 - cdf : cdf;
}

double logisticCubicCDFAppoximation(double x);

struct CDFBenchmarkResult {
    double medianMs;
    double meanMs;
    double minMs;
    double maxMs;
    double nsPerCall;
    double checksum;
};

std::vector<double> createInputs(std::size_t count);

void printBenchmark(
    const std::string& name,
    const CDFBenchmarkResult& result
);
