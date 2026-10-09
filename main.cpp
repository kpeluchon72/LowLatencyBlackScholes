#include "benchmark.hpp"
#include "pricing1.hpp"
#include "pricing2.hpp"
#include "pricing3.hpp"
#include "workload.hpp"

#include <vector>

int main() {
    Workload workload1 = createWorkload();
    BenchmarkResult result1 =
        runBenchmark(workload1, priceNaive);

    Workload workload2 = createWorkload();
    std::vector<PreparedOption> preparedOptions2;
    preparedOptions2.reserve(workload2.options.size());

    for (const Option& option : workload2.options) {
        preparedOptions2.push_back(
            prepareOption(option, workload2.r)
        );
    }

    Workload workload3 = createWorkload();
    std::vector<PreparedOption> preparedOptions3;
    preparedOptions3.reserve(workload3.options.size());

    for (const Option& option : workload3.options) {
        preparedOptions3.push_back(
            prepareOption(option, workload3.r)
        );
    }

    BenchmarkResult result2 =
        runBenchmark(
            workload2,
            [&preparedOptions2](
                const Workload& workload,
                std::vector<double>& prices
            ) {
                priceV2(workload, preparedOptions2, prices);
            }
        );

    BenchmarkResult result3 =
        runBenchmark(
            workload3,
            [&preparedOptions3](
                const Workload& workload,
                std::vector<double>& prices
            ) {
                priceV3(workload, preparedOptions3, prices);
            }
        );

    printBenchmark(
        "v1 - Naive",
        workload1,
        result1
    );

    printBenchmark(
        "v2 - Precomputed Values",
        workload2,
        result2
    );

    printBenchmark(
        "v3 - Precomputed Values + Optimized CDF",
        workload3,
        result3
    );

    return 0;
}
