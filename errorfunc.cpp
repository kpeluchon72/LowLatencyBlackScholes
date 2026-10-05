#include <iostream>
#include <chrono>
#include <cmath>
#include <iomanip>

double normalCDF(double x) {
    return 0.5 * std::erfc(
        -x / std::sqrt(2.0)
    );
}

//taylor expansion
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


// int main() {
//     using Clock = std::chrono::steady_clock;

//     auto start1 = Clock::now();
//     double normalVersion = normalCDF(3);
//     auto end1 = Clock::now();

//     std::cout << std::setprecision(17);
//     std::cout << "\n----------------------------------------" << "\n";
//     std::cout << "Normal Version Value:     " << normalVersion << "\n";
//     std::chrono::duration<double, std::milli> elapsed1 =
//         end1 - start1;
//     std::cout << std::setprecision(4);
//     std::cout << "Duration:                 " << elapsed1.count() << "ms" << "\n";
//     std::cout << "----------------------------------------" << "\n";
    
//     auto start2 = Clock::now();
//     double polynomialVersion = normalCDFfast(3);
//     auto end2 = Clock::now();

//     std::cout << std::setprecision(17);
//     std::cout << "Polynomial Version Value: " << polynomialVersion << "\n";
//     std::chrono::duration<double, std::milli> elapsed2 =
//         end2 - start2;
//     std::cout << std::setprecision(4);
//     std::cout << "Duration:                 " << elapsed2.count() << "ms" << "\n";
//     std::cout << "----------------------------------------" << "\n";
//     std::cout << "Speed up:                 " << (elapsed1.count())/(elapsed2.count()) << "x";
    

    
// }

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

    std::chrono::duration<double, std::milli> elapsed1 =
        end1 - start1;

    std::chrono::duration<double, std::milli> elapsed2 =
        end2 - start2;

    std::cout << std::setprecision(17);

    std::cout << "std::erfc: " << elapsed1.count() << " ms\n";
    std::cout << "Taylor:    " << elapsed2.count() << " ms\n";

    std::cout << "Speedup:   "
              << elapsed1.count() / elapsed2.count()
              << "x\n";

    // Prevent compiler from deleting calculations
    std::cout << "Checksums: " << sum1 << " " << sum2 << '\n';
}
