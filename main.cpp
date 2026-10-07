#include "benchmark.hpp"
#include "pricing1.hpp"
#include "pricing2.hpp"
#include "workload.hpp"

#include <vector>

int main() {
    Workload workload1 = createWorkload();
    BenchmarkResult result1 =
        runBenchmark(workload1, priceNaive);

    Workload workload2 = createWorkload();
    std::vector<PreparedOption> preparedOptions;
    preparedOptions.reserve(workload2.options.size());

    for (const Option& option : workload2.options) {
        preparedOptions.push_back(
            prepareOption(option, workload2.r)
        );
    }

    BenchmarkResult result2 =
        runBenchmark(
            workload2,
            [&preparedOptions](
                const Workload& workload,
                std::vector<double>& prices
            ) {
                priceV2(workload, preparedOptions, prices);
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

    return 0;
}
