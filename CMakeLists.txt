cmake_minimum_required(VERSION 3.16)

project(LowLatencyBS LANGUAGES CXX)

add_executable(main
    main.cpp
    benchmark.cpp
    workload.cpp
    pricing1.cpp
    pricing2.cpp
    pricing3.cpp
)

add_executable(errorfunc
    errorfunc.cpp
)

foreach(target main errorfunc)
    target_compile_features(${target} PRIVATE cxx_std_17)
endforeach()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    foreach(target main errorfunc)
        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Release>:-O3>
            -march=native
        )
    endforeach()
elseif(MSVC)
    foreach(target main errorfunc)
        target_compile_options(${target} PRIVATE
            $<$<CONFIG:Release>:/O2>
        )
    endforeach()
endif()
