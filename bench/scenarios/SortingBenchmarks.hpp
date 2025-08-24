#pragma once

#include "../BenchmarkFramework.hpp"
#include "../ComplexityAnalysis.hpp"
#include "../../core/algorithms/InternalSort.hpp"
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

namespace rts::bench::scenarios {

/**
 * @brief Sorting algorithm benchmark scenarios
 */
class SortingBenchmarks {
private:
    std::mt19937 rng_;
    
    /**
     * @brief Generate different data patterns for testing
     */
    enum class DataPattern {
        RANDOM,           // Completely random
        SORTED,           // Already sorted
        REVERSE_SORTED,   // Reverse sorted
        NEARLY_SORTED,    // 90% sorted with some out-of-place elements
        FEW_UNIQUE,       // Many duplicates
        MANY_DUPLICATES,  // Extreme duplicates
        SAWTOOTH         // Sawtooth pattern
    };
    
    std::vector<int> generate_data(size_t size, DataPattern pattern) {
        std::vector<int> data;
        data.reserve(size);
        
        switch (pattern) {
            case DataPattern::RANDOM: {
                std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                for (size_t i = 0; i < size; ++i) {
                    data.push_back(dist(rng_));
                }
                break;
            }
            
            case DataPattern::SORTED: {
                for (size_t i = 0; i < size; ++i) {
                    data.push_back(static_cast<int>(i + 1));
                }
                break;
            }
            
            case DataPattern::REVERSE_SORTED: {
                for (size_t i = 0; i < size; ++i) {
                    data.push_back(static_cast<int>(size - i));
                }
                break;
            }
            
            case DataPattern::NEARLY_SORTED: {
                // Start with sorted data
                for (size_t i = 0; i < size; ++i) {
                    data.push_back(static_cast<int>(i + 1));
                }
                // Randomly swap 10% of elements
                std::uniform_int_distribution<size_t> index_dist(0, size - 1);
                size_t swaps = size / 10;
                for (size_t i = 0; i < swaps; ++i) {
                    size_t idx1 = index_dist(rng_);
                    size_t idx2 = index_dist(rng_);
                    std::swap(data[idx1], data[idx2]);
                }
                break;
            }
            
            case DataPattern::FEW_UNIQUE: {
                std::uniform_int_distribution<int> dist(1, static_cast<int>(size / 100 + 1));
                for (size_t i = 0; i < size; ++i) {
                    data.push_back(dist(rng_));
                }
                break;
            }
            
            case DataPattern::MANY_DUPLICATES: {
                std::uniform_int_distribution<int> dist(1, 5);
                for (size_t i = 0; i < size; ++i) {
                    data.push_back(dist(rng_));
                }
                break;
            }
            
            case DataPattern::SAWTOOTH: {
                for (size_t i = 0; i < size; ++i) {
                    data.push_back(static_cast<int>(i % 100));
                }
                break;
            }
        }
        
        return data;
    }
    
    std::string pattern_name(DataPattern pattern) const {
        switch (pattern) {
            case DataPattern::RANDOM: return "Random";
            case DataPattern::SORTED: return "Sorted";
            case DataPattern::REVERSE_SORTED: return "ReverseSorted";
            case DataPattern::NEARLY_SORTED: return "NearlySorted";
            case DataPattern::FEW_UNIQUE: return "FewUnique";
            case DataPattern::MANY_DUPLICATES: return "ManyDuplicates";
            case DataPattern::SAWTOOTH: return "Sawtooth";
            default: return "Unknown";
        }
    }
    
public:
    SortingBenchmarks() : rng_(std::random_device{}()) {}
    
    /**
     * @brief Run comprehensive sorting benchmarks
     */
    void run_comprehensive_benchmarks(BenchmarkSuite& suite) {
        std::cout << "Running Sorting Algorithm Benchmarks...\n";
        
        // Test different input sizes
        std::vector<size_t> sizes = {100, 500, 1000, 5000, 10000, 50000, 100000};
        
        // Test different data patterns
        std::vector<DataPattern> patterns = {
            DataPattern::RANDOM,
            DataPattern::SORTED,
            DataPattern::REVERSE_SORTED,
            DataPattern::NEARLY_SORTED,
            DataPattern::FEW_UNIQUE,
            DataPattern::MANY_DUPLICATES
        };
        
        // Benchmark each algorithm with each pattern and size
        for (DataPattern pattern : patterns) {
            std::string pattern_str = pattern_name(pattern);
            
            // QuickSort benchmarks
            for (size_t size : sizes) {
                std::string bench_name = "QuickSort_" + pattern_str + "_n" + std::to_string(size);
                
                suite.add_benchmark(bench_name, 
                    [this, size, pattern]() {
                        auto data = generate_data(size, pattern);
                        rts::core::algorithms::InternalSort<int> local_sorter;
                        local_sorter.quicksort(data);
                    });
            }
            
            // HeapSort benchmarks
            for (size_t size : sizes) {
                std::string bench_name = "HeapSort_" + pattern_str + "_n" + std::to_string(size);
                
                suite.add_benchmark(bench_name,
                    [this, size, pattern]() {
                        auto data = generate_data(size, pattern);
                        rts::core::algorithms::InternalSort<int> local_sorter;
                        local_sorter.heapsort(data);
                    });
            }
        }
    }
    
    /**
     * @brief Run algorithm comparison benchmarks
     */
    void run_algorithm_comparison(BenchmarkSuite& suite) {
        std::cout << "Running Algorithm Comparison Benchmarks...\n";
        
        std::vector<size_t> sizes = {1000, 5000, 10000, 25000, 50000};
        
        // Compare algorithms on random data
        for (size_t size : sizes) {
            // QuickSort
            suite.add_benchmark("QuickSort_Random_n" + std::to_string(size),
                [this, size]() {
                    auto data = generate_data(size, DataPattern::RANDOM);
                    rts::core::algorithms::InternalSort<int> local_sorter;
                    local_sorter.quicksort(data);
                });
            
            // HeapSort
            suite.add_benchmark("HeapSort_Random_n" + std::to_string(size),
                [this, size]() {
                    auto data = generate_data(size, DataPattern::RANDOM);
                    rts::core::algorithms::InternalSort<int> local_sorter;
                    local_sorter.heapsort(data);
                });
        }
    }
    
    /**
     * @brief Analyze sorting complexity
     */
    void analyze_sorting_complexity(const std::vector<std::unique_ptr<BenchmarkResult>>& results) {
        std::cout << "\nAnalyzing Sorting Algorithm Complexity...\n";
        
        // Separate results by algorithm
        std::map<std::string, std::vector<const BenchmarkResult*>> algorithm_results;
        
        for (const auto& result : results) {
            // Extract algorithm name from benchmark name
            std::string algo_name = result->name;
            size_t underscore_pos = algo_name.find('_');
            if (underscore_pos != std::string::npos) {
                algo_name = algo_name.substr(0, underscore_pos);
            }
            
            algorithm_results[algo_name].push_back(result.get());
        }
        
        // Analyze each algorithm
        ComparativeAnalyzer comp_analyzer;
        
        for (const auto& [algo_name, algo_results] : algorithm_results) {
            ComplexityAnalyzer analyzer;
            
            for (const auto* result : algo_results) {
                if (result->input_size > 0) {
                    analyzer.add_data_point(result->input_size, 
                                          result->execution_time_ms.mean,
                                          result->memory_usage_mb.mean);
                }
            }
            
            std::cout << "\n" << algo_name << " Complexity Analysis:\n";
            std::cout << std::string(40, '-') << "\n";
            
            auto time_fit = analyzer.analyze_time_complexity();
            if (time_fit.complexity_class != ComplexityAnalyzer::ComplexityClass::UNKNOWN) {
                std::cout << "Time Complexity: " << time_fit.formula 
                          << " (confidence: " << std::fixed << std::setprecision(1) 
                          << time_fit.confidence_score * 100 << "%)\n";
                std::cout << "Constant Factor: " << std::scientific << std::setprecision(3) 
                          << time_fit.constant_factor << "\n";
            }
            
            auto space_fit = analyzer.analyze_space_complexity();
            if (space_fit.complexity_class != ComplexityAnalyzer::ComplexityClass::UNKNOWN) {
                std::cout << "Space Complexity: " << space_fit.formula 
                          << " (confidence: " << std::fixed << std::setprecision(1) 
                          << space_fit.confidence_score * 100 << "%)\n";
            }
        }
    }
    
    /**
     * @brief Generate sorting performance report
     */
    void generate_sorting_report(const std::vector<std::unique_ptr<BenchmarkResult>>& results) {
        std::cout << "\n" << std::string(80, '=') << "\n";
        std::cout << "SORTING ALGORITHM PERFORMANCE REPORT\n";
        std::cout << std::string(80, '=') << "\n";
        
        // Group results by data pattern and size
        std::map<std::string, std::map<size_t, std::map<std::string, const BenchmarkResult*>>> grouped_results;
        
        for (const auto& result : results) {
            // Parse benchmark name: Algorithm_Pattern_nSize
            std::string name = result->name;
            std::vector<std::string> parts;
            
            size_t pos = 0;
            size_t found = 0;
            while ((found = name.find('_', pos)) != std::string::npos) {
                parts.push_back(name.substr(pos, found - pos));
                pos = found + 1;
            }
            parts.push_back(name.substr(pos));
            
            if (parts.size() >= 3) {
                std::string algorithm = parts[0];
                std::string pattern = parts[1];
                size_t size = result->input_size;
                
                grouped_results[pattern][size][algorithm] = result.get();
            }
        }
        
        // Generate report for each pattern
        for (const auto& [pattern, pattern_results] : grouped_results) {
            std::cout << "\n" << pattern << " Data Pattern:\n";
            std::cout << std::string(50, '-') << "\n";
            std::cout << std::left << std::setw(10) << "Size";
            
            // Get algorithm names from first size group
            std::vector<std::string> algorithms;
            if (!pattern_results.empty()) {
                for (const auto& [algo, _] : pattern_results.begin()->second) {
                    algorithms.push_back(algo);
                    std::cout << std::setw(15) << algo;
                }
            }
            std::cout << "\n" << std::string(50, '-') << "\n";
            
            // Print results for each size
            for (const auto& [size, size_results] : pattern_results) {
                std::cout << std::left << std::setw(10) << size;
                
                for (const auto& algo : algorithms) {
                    auto it = size_results.find(algo);
                    if (it != size_results.end()) {
                        std::cout << std::setw(15) << std::fixed << std::setprecision(2) 
                                  << it->second->execution_time_ms.mean;
                    } else {
                        std::cout << std::setw(15) << "N/A";
                    }
                }
                std::cout << "\n";
            }
        }
        
        // Find best and worst performers
        std::cout << "\nPERFORMANCE SUMMARY:\n";
        std::cout << std::string(30, '-') << "\n";
        
        // Find fastest algorithm for each input size
        std::map<size_t, std::pair<std::string, double>> fastest_per_size;
        
        for (const auto& result : results) {
            size_t size = result->input_size;
            if (size > 0) {
                auto it = fastest_per_size.find(size);
                if (it == fastest_per_size.end() || 
                    result->execution_time_ms.mean < it->second.second) {
                    
                    // Extract algorithm name
                    std::string algo_name = result->name;
                    size_t underscore_pos = algo_name.find('_');
                    if (underscore_pos != std::string::npos) {
                        algo_name = algo_name.substr(0, underscore_pos);
                    }
                    
                    fastest_per_size[size] = {algo_name, result->execution_time_ms.mean};
                }
            }
        }
        
        std::cout << "Fastest Algorithm by Input Size:\n";
        for (const auto& [size, best] : fastest_per_size) {
            std::cout << "  Size " << size << ": " << best.first 
                      << " (" << std::fixed << std::setprecision(2) << best.second << " ms)\n";
        }
    }
};

} // namespace rts::bench::scenarios