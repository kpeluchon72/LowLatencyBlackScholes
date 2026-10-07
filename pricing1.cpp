#include "pricing1.hpp"

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
