#include "pricing3.hpp"

#include <cmath>

// Polynomial rational approximation, fast and fairly accurate, 
// more than enough precision for black scholes computations
inline double polynomialRationalCDF(double x) {
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

void priceV3(
    const Workload& workload,
    const std::vector<PreparedOption>& options,
    std::vector<double>& prices
) {
    std::size_t index = 0;

    for (double S : workload.spots) {

        double logSpot = std::log(S);

        for (const PreparedOption& option : options) {
            
            double d1 = (logSpot - option.logK + option.drift) * option.invSigmaSqrtT;

            double d2 = d1 - option.sigmaSqrtT;

            prices[index++] = S * polynomialRationalCDF(d1) - option.discountedK * polynomialRationalCDF(d2);
            
        }
    }
}
