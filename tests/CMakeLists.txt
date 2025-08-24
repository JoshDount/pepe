# Tests module
enable_testing()

# Find or create a simple testing framework (we'll use a simple assert-based approach)
add_executable(unit_tests
    TestFramework.cpp
    unit/test_main.cpp
    unit/test_simple.cpp
    unit/test_graph_algorithms.cpp
    unit/test_sorting_algorithms.cpp
    unit/test_external_sorting.cpp
    unit/test_event_simulation.cpp
    unit/test_traffic_simulation.cpp
    unit/test_gtfs_data_loading.cpp
)

# Link with core library
target_link_libraries(unit_tests rts_core)

# Add test discovery
add_test(NAME CoreDataStructures COMMAND unit_tests)