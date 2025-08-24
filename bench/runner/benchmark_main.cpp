#include "BenchmarkRunner.hpp"
#include <iostream>
#include <string>
#include <vector>

/**
 * @brief Main benchmark application
 */
int main(int argc, char* argv[]) {
    try {
        rts::bench::BenchmarkRunner runner;
        
        // Parse command line arguments
        std::vector<std::string> args(argv, argv + argc);
        
        if (argc == 1) {
            // Default: run quick check
            std::cout << "Route Transit Simulator - Performance Benchmark Suite\n";
            std::cout << "Usage: " << argv[0] << " [option]\n";
            std::cout << "Options:\n";
            std::cout << "  quick       - Quick performance check\n";
            std::cout << "  sorting     - Sorting algorithm benchmarks\n";
            std::cout << "  graph       - Graph algorithm benchmarks\n";
            std::cout << "  datastr     - Data structure benchmarks\n";
            std::cout << "  compare     - Algorithm comparison benchmarks\n";
            std::cout << "  regression  - Performance regression tests\n";
            std::cout << "  full        - Comprehensive benchmark suite\n";
            std::cout << "  help        - Show this help message\n\n";
            
            std::cout << "Running quick performance check...\n";
            runner.run_quick_check();
            return 0;
        }
        
        std::string option = args[1];
        
        if (option == "help" || option == "-h" || option == "--help") {
            std::cout << "Route Transit Simulator - Performance Benchmark Suite\n\n";
            std::cout << "This tool provides comprehensive performance analysis of all\n";
            std::cout << "data structures and algorithms implemented in the RTS project.\n\n";
            std::cout << "Available benchmark categories:\n\n";
            std::cout << "  quick       - Quick sanity check (1-2 minutes)\n";
            std::cout << "              Tests key algorithms with small datasets\n\n";
            std::cout << "  sorting     - Sorting algorithm analysis (5-10 minutes)\n";
            std::cout << "              Tests QuickSort, HeapSort with various data patterns\n\n";
            std::cout << "  graph       - Graph algorithm analysis (10-15 minutes)\n";
            std::cout << "              Tests BFS, DFS, Dijkstra, A*, MST algorithms\n\n";
            std::cout << "  datastr     - Data structure analysis (5-10 minutes)\n";
            std::cout << "              Tests MinHeap, HashMap, LinkedList, AVL Tree, LRU Cache\n\n";
            std::cout << "  compare     - Algorithm comparison (10-15 minutes)\n";
            std::cout << "              Direct performance comparison between similar algorithms\n\n";
            std::cout << "  regression  - Performance regression testing (2-5 minutes)\n";
            std::cout << "              Validates that performance hasn't degraded\n\n";
            std::cout << "  full        - Complete benchmark suite (30-60 minutes)\n";
            std::cout << "              Runs all benchmarks with comprehensive analysis\n\n";
            std::cout << "Output includes:\n";
            std::cout << "• Execution time statistics (mean, median, min, max, stddev)\n";
            std::cout << "• Memory usage analysis\n";
            std::cout << "• Complexity analysis and curve fitting\n";
            std::cout << "• Performance predictions for larger datasets\n";
            std::cout << "• Comparative analysis and recommendations\n\n";
            return 0;
        }
        
        // Configure for CSV output if requested
        if (argc > 2 && std::string(argv[2]) == "--csv") {
            rts::bench::BenchmarkConfig config;
            config.output_format = "csv";
            config.output_file = "benchmark_results.csv";
            runner.set_config(config);
            std::cout << "Results will be saved to benchmark_results.csv\n";
        }
        
        // Run requested benchmark category
        if (option == "quick") {
            runner.run_quick_check();
        }
        else if (option == "sorting") {
            runner.run_sorting_benchmarks();
        }
        else if (option == "graph") {
            runner.run_graph_benchmarks();
        }
        else if (option == "datastr" || option == "datastructure") {
            runner.run_datastructure_benchmarks();
        }
        else if (option == "compare" || option == "comparison") {
            runner.run_algorithm_comparison();
        }
        else if (option == "regression") {
            runner.run_performance_regression();
        }
        else if (option == "full" || option == "comprehensive") {
            std::cout << "Warning: Comprehensive benchmark suite will take 30-60 minutes.\n";
            std::cout << "Continue? (y/N): ";
            std::string confirm;
            std::getline(std::cin, confirm);
            if (confirm == "y" || confirm == "Y" || confirm == "yes") {
                runner.run_comprehensive_suite();
            } else {
                std::cout << "Benchmark cancelled.\n";
                return 0;
            }
        }
        else {
            std::cerr << "Error: Unknown option '" << option << "'\n";
            std::cerr << "Use '" << argv[0] << " help' for usage information.\n";
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Benchmark failed with error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Benchmark failed with unknown error" << std::endl;
        return 1;
    }
    
    return 0;
}