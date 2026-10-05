#include "workload.hpp"

#include <cmath>
#include <vector>

std::size_t Workload::calculationCount() const {
    return options.size() * spots.size();
}

Workload createWorkload() {
    Workload workload;

    workload.r = 0.04;

    const std::vector<double> expirations = {
        0.05,
        0.10,
        0.20,
        0.30,
        0.50,
        0.75,
        1.00,
        2.00
    };

    const int strikesPerExpiry = 64;

    for (double T : expirations) {
        for (int i = 0; i < strikesPerExpiry; ++i) {
            double K =
                70.0
                + 60.0 * i / (strikesPerExpiry - 1);

            double moneyness =
                (K - 100.0) / 100.0;

            double sigma =
                0.20
                + 0.15 * moneyness * moneyness
                + 0.02 * T;

            workload.options.push_back(Option{K, T, sigma});
        }
    }

    const int spotUpdates = 2000;

    workload.spots.reserve(spotUpdates);

    // "random" spot price movements through addition of two sine waves
    // every run should have same output but the data is made this way
    // to mimic real stock updates.
    for (int i = 0; i < spotUpdates; ++i) {
        double S =
            100.0
            + 3.0 * std::sin(i * 0.013)
            + 1.5 * std::sin(i * 0.031);

        workload.spots.push_back(S);
    }

    return workload;
}
