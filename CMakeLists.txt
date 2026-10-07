cmake_minimum_required(VERSION 3.16)

project(LowLatencyBS LANGUAGES CXX)

add_executable(main
    main.cpp
    benchmark.cpp
    workload.cpp
    pricing1.cpp
    pricing2.cpp
)

target_compile_features(main PRIVATE cxx_std_17)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(main PRIVATE
        $<$<CONFIG:Release>:-O3>
        -march=native
    )
elseif(MSVC)
    target_compile_options(main PRIVATE
        $<$<CONFIG:Release>:/O2>
    )
endif()
