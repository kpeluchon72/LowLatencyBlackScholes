#pragma once

#include <cstddef>
#include <vector>

struct Option {
    double K;
    double T;
    double sigma;
};

struct Workload {
    std::vector<Option> options;
    std::vector<double> spots;
    double r;

    std::size_t calculationCount() const;
};

Workload createWorkload();
