#include "benchmark.hpp"
#include "pricing.hpp"
#include "workload.hpp"

int main() {
    Workload workload = createWorkload();

    BenchmarkResult result =
        runBenchmark<priceNaive>(workload);

    printBenchmark(
        "V0 - Naive",
        workload,
        result
    );

    return 0;
}
