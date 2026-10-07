#pragma once

#include "workload.hpp"

#include <vector>
#include <cmath>
#include <cstddef>

double normalCDF(double x);

double blackScholesCall(
    double S,
    double K,
    double T,
    double r,
    double sigma
);

void priceNaive(
    const Workload& workload,
    std::vector<double>& prices
);
