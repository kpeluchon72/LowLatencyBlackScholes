#pragma once

#include "workload.hpp"

#include <vector>

struct PreparedOption {
    double K;
    double logK;

    double sigmaSqrtT;
    double invSigmaSqrtT;

    double drift;
    double discountedK;
};

PreparedOption prepareOption(const Option& option, double r);

void priceV2(
    const Workload& workload,
    const std::vector<PreparedOption>& options,
    std::vector<double>& prices
);
