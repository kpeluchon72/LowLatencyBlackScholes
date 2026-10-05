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

// Horners method implementation
inline double normalCDFfast2(double x)
{
    constexpr double INV_SQRT_2PI = 0.39894228040143267794;

    constexpr double p  =  0.2316419;
    constexpr double b1 =  0.319381530;
    constexpr double b2 = -0.356563782;
    constexpr double b3 =  1.781477937;
    constexpr double b4 = -1.821255978;
    constexpr double b5 =  1.330274429;

    // These tails are already irrelevant for normal
    // Black-Scholes pricing precision.
    if (x <= -8.0)
        return 0.0;

    if (x >= 8.0)
        return 1.0;

    double ax = std::abs(x);

    double t = 1.0 / (1.0 + p * ax);

    // Horner's method
    double poly =
        (((((b5 * t + b4)
             * t + b3)
             * t + b2)
             * t + b1)
             * t);

    double tail =
        INV_SQRT_2PI
        * std::exp(-0.5 * ax * ax)
        * poly;

    if (x >= 0.0)
        return 1.0 - tail;

    return tail;
}

inline __m256d normalCDFfast2_SIMD(__m256d x)
{
    const __m256d INV_SQRT_2PI = _mm256_set1_pd(0.39894228040143267794);

    const __m256d p  = _mm256_set1_pd( 0.2316419);
    const __m256d b1 = _mm256_set1_pd( 0.319381530);
    const __m256d b2 = _mm256_set1_pd(-0.356563782);
    const __m256d b3 = _mm256_set1_pd( 1.781477937);
    const __m256d b4 = _mm256_set1_pd(-1.821255978);
    const __m256d b5 = _mm256_set1_pd( 1.330274429);

    const __m256d one  = _mm256_set1_pd(1.0);
    const __m256d zero = _mm256_setzero_pd();

    // -----------------------------------
    // ax = abs(x)
    // -----------------------------------

    // -0.0 only has the sign bit set.
    const __m256d signBit = _mm256_set1_pd(-0.0);

    __m256d ax =
        _mm256_andnot_pd(signBit, x);

    // -----------------------------------
    // t = 1 / (1 + p * abs(x))
    // -----------------------------------

    __m256d denominator =
        _mm256_add_pd(
            one,
            _mm256_mul_pd(p, ax)
        );

    __m256d t =
        _mm256_div_pd(one, denominator);

    // -----------------------------------
    // Horner polynomial
    // -----------------------------------

    __m256d poly = b5;

    poly = _mm256_add_pd(
        _mm256_mul_pd(poly, t),
        b4
    );

    poly = _mm256_add_pd(
        _mm256_mul_pd(poly, t),
        b3
    );

    poly = _mm256_add_pd(
        _mm256_mul_pd(poly, t),
        b2
    );

    poly = _mm256_add_pd(
        _mm256_mul_pd(poly, t),
        b1
    );

    poly = _mm256_mul_pd(poly, t);

    // -----------------------------------
    // exp(-0.5 * x*x)
    //
    // AVX2 has NO exp instruction, so
    // temporarily calculate each lane.
    // -----------------------------------

    __m256d exponent =
        _mm256_mul_pd(
            _mm256_set1_pd(-0.5),
            _mm256_mul_pd(ax, ax)
        );

    alignas(32) double expInput[4];

    _mm256_store_pd(expInput, exponent);

    expInput[0] = std::exp(expInput[0]);
    expInput[1] = std::exp(expInput[1]);
    expInput[2] = std::exp(expInput[2]);
    expInput[3] = std::exp(expInput[3]);

    __m256d expResult =
        _mm256_load_pd(expInput);

    // -----------------------------------
    // tail = INV_SQRT_2PI * exp * poly
    // -----------------------------------

    __m256d tail =
        _mm256_mul_pd(
            INV_SQRT_2PI,
            _mm256_mul_pd(expResult, poly)
        );

    // positive result = 1 - tail
    __m256d positiveResult =
        _mm256_sub_pd(one, tail);

    // mask:
    // true wherever x >= 0
    __m256d positiveMask =
        _mm256_cmp_pd(
            x,
            zero,
            _CMP_GE_OQ
        );

    // If x >= 0 -> 1-tail
    // otherwise -> tail
    return _mm256_blendv_pd(
        tail,
        positiveResult,
        positiveMask
    );
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
        sum2 += normalCDFfast2(x);
    }

    auto end2 = Clock::now();


    double sum3 = 0.0;


    auto start3 = Clock::now();

    __m256d vectorSum = _mm256_setzero_pd();

    for (int i = 0; i < N; i += 4) {

        // Make:
        //
        // x0 = -3 + 6*i/N
        // x1 = -3 + 6*(i+1)/N
        // x2 = ...
        // x3 = ...

        __m256d indices = _mm256_set_pd(
            static_cast<double>(i + 3),
            static_cast<double>(i + 2),
            static_cast<double>(i + 1),
            static_cast<double>(i)
        );

        __m256d x =
            _mm256_add_pd(
                _mm256_set1_pd(-3.0),
                _mm256_mul_pd(
                    _mm256_set1_pd(6.0 / N),
                    indices
                )
            );

        __m256d result =
            normalCDFfast2_SIMD(x);

        vectorSum =
            _mm256_add_pd(vectorSum, result);
    }

    // Convert the 4 SIMD sums back into scalar doubles
    alignas(32) double sums[4];

    _mm256_store_pd(sums, vectorSum);

    sum3 =
        sums[0]
        + sums[1]
        + sums[2]
        + sums[3];

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
    std::cout << "SIMD:      " << elapsed3.count() << " ms\n";

    std::cout << "Speedup1:  "
              << elapsed1.count() / elapsed2.count()
              << "x\n";
    
    std::cout << "Speedup2:  "
              << elapsed1.count() / elapsed3.count()
              << "x\n";

    // Prevent compiler from deleting calculations
    std::cout << "Checksums: " << sum1 << " " << sum2 << " " << sum3 << '\n';
}
