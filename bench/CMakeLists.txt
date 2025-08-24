# Benchmarking module
add_library(rts_bench INTERFACE)

target_include_directories(rts_bench INTERFACE 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rts_bench INTERFACE rts_core)

# Add all benchmark headers
target_sources(rts_bench INTERFACE
    BenchmarkFramework.hpp
    ComplexityAnalysis.hpp
    BenchmarkRunner.hpp
    scenarios/SortingBenchmarks.hpp
    scenarios/GraphAlgorithmBenchmarks.hpp
    scenarios/DataStructureBenchmarks.hpp
)

# Benchmark runner executable
add_executable(benchmark_runner
    runner/benchmark_main.cpp
)

target_link_libraries(benchmark_runner 
    rts_bench
    rts_core
)