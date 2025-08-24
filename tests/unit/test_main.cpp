#include "../TestFramework.hpp"

// Test function declarations
void test_simple();
void test_graph_algorithms();
void test_sorting_algorithms();
void test_external_sorting();
void test_discrete_event_simulation();
void test_traffic_simulation();
void test_gtfs_data_loading();

int main() {
    std::cout << "Route Transit Simulator - Unit Tests" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    try {
        TestFramework::run_test("All Data Structures - Simple Test", test_simple);
        TestFramework::run_test("Graph Algorithms - BFS and DFS", test_graph_algorithms);
        TestFramework::run_test("Sorting Algorithms - Internal Sorting", test_sorting_algorithms);
        TestFramework::run_test("External Sorting - Large Dataset Sorting", test_external_sorting);
        TestFramework::run_test("Discrete Event Simulation - EventQueue and Processing", test_discrete_event_simulation);
        TestFramework::run_test("Traffic Simulation - Congestion and Incident Modeling", test_traffic_simulation);
        TestFramework::run_test("GTFS Data Loading - Parser and Graph Conversion", test_gtfs_data_loading);
    } catch (const std::exception& e) {
        std::cout << "Critical test failure: " << e.what() << std::endl;
    }
    
    TestFramework::print_summary();
    return TestFramework::all_passed() ? 0 : 1;
}