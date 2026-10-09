#include "pricing2.hpp"

#include <cmath>

namespace {

double normalCDF(double x) {
    return 0.5 * std::erfc(
        -x / std::sqrt(2.0)
    );
}

} // namespace


PreparedOption prepareOption(const Option& option, double r)
{
    double sqrtT = std::sqrt(option.T);
    double sigmaSqrtT = option.sigma * sqrtT;

    return {
        option.K,
        std::log(option.K),

        sigmaSqrtT,
        1.0 / sigmaSqrtT,

        (r + 0.5 * option.sigma * option.sigma) * option.T,
        option.K * std::exp(-r * option.T)
    };
}

void priceV2(
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

            prices[index++] = S * normalCDF(d1) - option.discountedK * normalCDF(d2);
            
        }
    }
}
