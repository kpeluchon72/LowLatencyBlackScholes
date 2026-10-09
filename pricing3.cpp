#include "pricing3.hpp"
#include "errorfunc.hpp"
#include <cmath>

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
