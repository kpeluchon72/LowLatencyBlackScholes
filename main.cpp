#include "benchmark.hpp"
#include "pricing1.hpp"
#include "workload.hpp"

int main() {
    Workload workload = createWorkload();

    BenchmarkResult result1 =
        runBenchmark<priceNaive>(workload);

    printBenchmark(
        "V0 - Naive",
        workload,
        result1
    );

    return 0;
}
