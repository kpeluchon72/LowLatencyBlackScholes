#include "pricing2.hpp"
#include "pricing1.hpp"

#include <cmath>

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
