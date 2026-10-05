cmake_minimum_required(VERSION 3.16)

project(LowLatencyBS LANGUAGES CXX)

add_executable(benchmark
    main.cpp
    benchmark.cpp
    workload.cpp
    pricing.cpp
)

target_compile_features(benchmark PRIVATE cxx_std_17)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(benchmark PRIVATE
        $<$<CONFIG:Release>:-O3>
        -march=native
    )
elseif(MSVC)
    target_compile_options(benchmark PRIVATE
        $<$<CONFIG:Release>:/O2>
    )
endif()
