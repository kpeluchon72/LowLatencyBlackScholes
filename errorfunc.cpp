#include <iostream>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <immintrin.h>

double normalCDF(double x) {
    return 0.5 * std::erfc(
        -x / std::sqrt(2.0)
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


inline double logisticCubicCDFAppoximation(double x) {
    constexpr double A = 1.59760287;
    constexpr double B = 0.07056410;

    double ax = std::abs(x);
    double z = ax * (A + B * ax * ax);

    double p = 1.0 / (1.0 + std::exp(-z));

    return x >= 0.0 ? p : 1.0 - p;
}


constexpr int N = 10'000'000;

int main() {
    using Clock = std::chrono::steady_clock;

    double sum1 = 0.0;

    auto start1 = Clock::now();

    for (int i = 0; i < N; ++i) {
        double x = -3.0 + 6.0 * i / N;
        sum1 += normalCDF(x);
    }

    auto end1 = Clock::now();

    double sum2 = 0.0;

    auto start2 = Clock::now();

    for (int i = 0; i < N; ++i) {
        double x = -3.0 + 6.0 * i / N;
        sum2 += normalCDFfast(x);
    }

    auto end2 = Clock::now();


    double sum3 = 0.0;

    auto start3 = Clock::now();

    for (int i = 0; i < N; ++i) {
        double x = -3.0 + 6.0 * i / N;
        sum3 += logisticCubicCDFAppoximation(x);
    }

    auto end3 = Clock::now();


    std::chrono::duration<double, std::milli> elapsed1 =
        end1 - start1;

    std::chrono::duration<double, std::milli> elapsed2 =
        end2 - start2;

    std::chrono::duration<double, std::milli> elapsed3 =
        end3 - start3;

    std::cout << std::setprecision(17);

    std::cout << "std::erfc: " << elapsed1.count() << " ms\n";
    std::cout << "Taylor:    " << elapsed2.count() << " ms\n";
    std::cout << "LogCubic:  " << elapsed3.count() << "ms\n";
    std::cout << "std:       " << normalCDF(5) << "\n";

    std::cout << "Speedup1:  "
              << elapsed1.count() / elapsed2.count()
              << "x\n";
    

    // Prevent compiler from deleting calculations
    std::cout << "Checksums: " << sum1 << " " << sum2 << '\n';

    return 0;
}

