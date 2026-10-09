#pragma once

#include "pricing2.hpp"

void priceV3(
    const Workload& workload,
    const std::vector<PreparedOption>& options,
    std::vector<double>& prices
);
