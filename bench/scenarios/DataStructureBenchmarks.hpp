#pragma once

#include "../BenchmarkFramework.hpp"
#include "../ComplexityAnalysis.hpp"
#include "../../core/containers/MinHeap.hpp"
#include "../../core/containers/HashMap.hpp"
#include "../../core/containers/LinkedList.hpp"
#include "../../core/containers/Queue.hpp"
#include "../../core/containers/Stack.hpp"
#include "../../core/containers/LRUCache.hpp"
#include "../../core/trees/AVLTree.hpp"
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>

namespace rts::bench::scenarios {

/**
 * @brief Data structure performance benchmarks
 */
class DataStructureBenchmarks {
private:
    std::mt19937 rng_;
    
    /**
     * @brief Operation patterns for testing
     */
    enum class OperationPattern {
        SEQUENTIAL,      // Sequential access pattern
        RANDOM,          // Random access pattern
        MIXED,           // Mixed operations
        WORST_CASE,      // Worst-case scenarios
        REALISTIC        // Realistic usage patterns
    };
    
    std::string pattern_name(OperationPattern pattern) const {
        switch (pattern) {
            case OperationPattern::SEQUENTIAL: return "Sequential";
            case OperationPattern::RANDOM: return "Random";
            case OperationPattern::MIXED: return "Mixed";
            case OperationPattern::WORST_CASE: return "WorstCase";
            case OperationPattern::REALISTIC: return "Realistic";
            default: return "Unknown";
        }
    }
    
public:
    DataStructureBenchmarks() : rng_(std::random_device{}()) {}
    
    /**
     * @brief Run MinHeap benchmarks
     */
    void run_minheap_benchmarks(BenchmarkSuite& suite) {
        std::cout << "Running MinHeap Benchmarks...\n";
        
        std::vector<size_t> sizes = {1000, 5000, 10000, 25000, 50000};
        
        for (size_t size : sizes) {
            // Insertion benchmark
            suite.add_benchmark("MinHeap_Insert_n" + std::to_string(size),
                [this, size]() {
                    rts::core::containers::MinHeap<int> heap;
                    std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                    
                    for (size_t i = 0; i < size; ++i) {
                        heap.push(dist(rng_));
                    }
                });
            
            // Extract-min benchmark
            suite.add_benchmark("MinHeap_ExtractMin_n" + std::to_string(size),
                [this, size]() {
                    rts::core::containers::MinHeap<int> heap;
                    std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                    
                    // Fill heap
                    for (size_t i = 0; i < size; ++i) {
                        heap.push(dist(rng_));
                    }
                    
                    // Extract all elements
                    while (!heap.empty()) {
                        heap.pop();
                    }
                });
            
            // Decrease-key benchmark
            suite.add_benchmark("MinHeap_DecreaseKey_n" + std::to_string(size),
                [this, size]() {
                    rts::core::containers::MinHeap<int> heap;
                    std::vector<size_t> element_ids;
                    std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                    
                    // Fill heap and store element IDs
                    for (size_t i = 0; i < size; ++i) {
                        auto id = heap.push(dist(rng_));
                        element_ids.push_back(id);
                    }
                    
                    // Perform decrease-key operations
                    std::uniform_int_distribution<size_t> id_dist(0, element_ids.size() - 1);
                    size_t operations = size / 10; // 10% of elements
                    for (size_t i = 0; i < operations; ++i) {
                        size_t idx = id_dist(rng_);
                        heap.decrease_key(element_ids[idx], 0); // Decrease to minimum
                    }
                });
        }
    }
    
    /**
     * @brief Run HashMap benchmarks
     */
    void run_hashmap_benchmarks(BenchmarkSuite& suite) {
        std::cout << "Running HashMap Benchmarks...\n";
        
        std::vector<size_t> sizes = {1000, 5000, 10000, 25000, 50000};
        
        for (size_t size : sizes) {
            // Insertion benchmark
            suite.add_benchmark("HashMap_Insert_n" + std::to_string(size),
                [this, size]() {
                    rts::core::containers::HashMap<int, int> map;
                    std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                    
                    for (size_t i = 0; i < size; ++i) {
                        map.put(dist(rng_), dist(rng_));
                    }
                });
            
            // Lookup benchmark
            suite.add_benchmark("HashMap_Lookup_n" + std::to_string(size),
                [this, size]() {
                    rts::core::containers::HashMap<int, int> map;
                    std::vector<int> keys;
                    std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                    
                    // Fill map
                    for (size_t i = 0; i < size; ++i) {
                        int key = dist(rng_);
                        keys.push_back(key);
                        map.put(key, dist(rng_));
                    }
                    
                    // Perform lookups
                    std::uniform_int_distribution<size_t> key_dist(0, keys.size() - 1);
                    for (size_t i = 0; i < size; ++i) {
                        int key = keys[key_dist(rng_)];
                        map.get(key);
                    }
                });
            
            // Mixed operations benchmark
            suite.add_benchmark("HashMap_Mixed_n" + std::to_string(size),
                [this, size]() {
                    rts::core::containers::HashMap<int, int> map;
                    std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                    std::uniform_int_distribution<int> op_dist(0, 2); // 0=insert, 1=lookup, 2=erase
                    
                    std::vector<int> inserted_keys;
                    
                    for (size_t i = 0; i < size; ++i) {
                        int operation = op_dist(rng_);
                        
                        if (operation == 0 || inserted_keys.empty()) {
                            // Insert
                            int key = dist(rng_);
                            map.put(key, dist(rng_));
                            inserted_keys.push_back(key);
                        } else if (operation == 1) {
                            // Lookup
                            std::uniform_int_distribution<size_t> key_dist(0, inserted_keys.size() - 1);
                            int key = inserted_keys[key_dist(rng_)];
                            map.get(key);
                        } else {
                            // Erase
                            std::uniform_int_distribution<size_t> key_dist(0, inserted_keys.size() - 1);
                            size_t key_idx = key_dist(rng_);
                            int key = inserted_keys[key_idx];
                            map.erase(key);
                            inserted_keys.erase(inserted_keys.begin() + key_idx);
                        }
                    }
                });
        }
    }
    
    /**
     * @brief Run LinkedList benchmarks
     */
    void run_linkedlist_benchmarks(BenchmarkSuite& suite) {
        std::cout << "Running LinkedList Benchmarks...\n";
        
        std::vector<size_t> sizes = {1000, 5000, 10000, 25000, 50000};
        
        for (size_t size : sizes) {
            // Front insertion benchmark
            suite.add_benchmark("LinkedList_PushFront_n" + std::to_string(size),
                [this, size]() {
                    rts::core::containers::LinkedList<int> list;
                    std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                    
                    for (size_t i = 0; i < size; ++i) {
                        list.push_front(dist(rng_));
                    }
                });
            
            // Back insertion benchmark
            suite.add_benchmark("LinkedList_PushBack_n" + std::to_string(size),
                [this, size]() {
                    rts::core::containers::LinkedList<int> list;
                    std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                    
                    for (size_t i = 0; i < size; ++i) {
                        list.push_back(dist(rng_));
                    }
                });
            
            // Search benchmark
            suite.add_benchmark("LinkedList_Search_n" + std::to_string(size),
                [this, size]() {
                    rts::core::containers::LinkedList<int> list;
                    std::vector<int> values;
                    std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                    
                    // Fill list
                    for (size_t i = 0; i < size; ++i) {
                        int value = dist(rng_);
                        values.push_back(value);
                        list.push_back(value);
                    }
                    
                    // Perform searches
                    std::uniform_int_distribution<size_t> val_dist(0, values.size() - 1);
                    size_t searches = size / 10; // 10% of elements
                    for (size_t i = 0; i < searches; ++i) {
                        int value = values[val_dist(rng_)];
                        list.find(value);
                    }
                });
        }
    }
    
    /**
     * @brief Run AVL Tree benchmarks
     */
    void run_avltree_benchmarks(BenchmarkSuite& suite) {
        std::cout << "Running AVL Tree Benchmarks...\n";
        
        std::vector<size_t> sizes = {1000, 5000, 10000, 25000, 50000};
        
        for (size_t size : sizes) {
            // Insertion benchmark
            suite.add_benchmark("AVLTree_Insert_n" + std::to_string(size),
                [this, size]() {
                    rts::core::trees::AVLTree<int, int> tree;
                    std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                    
                    for (size_t i = 0; i < size; ++i) {
                        int key = dist(rng_);
                        tree.insert(key, key);
                    }
                });
            
            // Search benchmark
            suite.add_benchmark("AVLTree_Search_n" + std::to_string(size),
                [this, size]() {
                    rts::core::trees::AVLTree<int, int> tree;
                    std::vector<int> values;
                    std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                    
                    // Fill tree
                    for (size_t i = 0; i < size; ++i) {
                        int value = dist(rng_);
                        values.push_back(value);
                        tree.insert(value, value);
                    }
                    
                    // Perform searches
                    std::uniform_int_distribution<size_t> val_dist(0, values.size() - 1);
                    for (size_t i = 0; i < size; ++i) {
                        int value = values[val_dist(rng_)];
                        tree.find(value);
                    }
                });
            
            // Mixed operations benchmark
            suite.add_benchmark("AVLTree_Mixed_n" + std::to_string(size),
                [this, size]() {
                    rts::core::trees::AVLTree<int, int> tree;
                    std::uniform_int_distribution<int> dist(1, static_cast<int>(size * 10));
                    std::uniform_int_distribution<int> op_dist(0, 1); // 0=insert, 1=search (removed erase)
                    
                    std::vector<int> inserted_values;
                    
                    for (size_t i = 0; i < size; ++i) {
                        int operation = op_dist(rng_);
                        
                        if (operation == 0 || inserted_values.empty()) {
                            // Insert
                            int value = dist(rng_);
                            tree.insert(value, value);
                            inserted_values.push_back(value);
                        } else {
                            // Search
                            std::uniform_int_distribution<size_t> val_dist(0, inserted_values.size() - 1);
                            int value = inserted_values[val_dist(rng_)];
                            tree.find(value);
                        }
                    }
                });
        }
    }
    
    /**
     * @brief Run LRU Cache benchmarks
     */
    void run_lru_cache_benchmarks(BenchmarkSuite& suite) {
        std::cout << "Running LRU Cache Benchmarks...\n";
        
        std::vector<size_t> cache_sizes = {100, 500, 1000, 2500, 5000};
        
        for (size_t cache_size : cache_sizes) {
            size_t num_operations = cache_size * 10; // 10x cache size operations
            
            // Cache with high hit rate
            suite.add_benchmark("LRUCache_HighHitRate_c" + std::to_string(cache_size),
                [this, cache_size, num_operations]() {
                    rts::core::containers::LRUCache<int, int> cache(cache_size);
                    std::uniform_int_distribution<int> key_dist(1, static_cast<int>(cache_size / 2)); // High locality
                    std::uniform_int_distribution<int> val_dist(1, 1000);
                    
                    for (size_t i = 0; i < num_operations; ++i) {
                        int key = key_dist(rng_);
                        auto value = cache.get(key);
                        if (!value) {
                            cache.put(key, val_dist(rng_));
                        }
                    }
                });
            
            // Cache with low hit rate
            suite.add_benchmark("LRUCache_LowHitRate_c" + std::to_string(cache_size),
                [this, cache_size, num_operations]() {
                    rts::core::containers::LRUCache<int, int> cache(cache_size);
                    std::uniform_int_distribution<int> key_dist(1, static_cast<int>(cache_size * 5)); // Low locality
                    std::uniform_int_distribution<int> val_dist(1, 1000);
                    
                    for (size_t i = 0; i < num_operations; ++i) {
                        int key = key_dist(rng_);
                        auto value = cache.get(key);
                        if (!value) {
                            cache.put(key, val_dist(rng_));
                        }
                    }
                });
        }
    }
    
    /**
     * @brief Run comprehensive data structure benchmarks
     */
    void run_comprehensive_benchmarks(BenchmarkSuite& suite) {
        run_minheap_benchmarks(suite);
        run_hashmap_benchmarks(suite);
        run_linkedlist_benchmarks(suite);
        run_avltree_benchmarks(suite);
        run_lru_cache_benchmarks(suite);
    }
    
    /**
     * @brief Analyze data structure complexity
     */
    void analyze_datastructure_complexity(const std::vector<std::unique_ptr<BenchmarkResult>>& results) {
        std::cout << "\nAnalyzing Data Structure Complexity...\n";
        
        // Group results by data structure and operation
        std::map<std::string, ComplexityAnalyzer> analyzers;
        
        for (const auto& result : results) {
            // Extract data structure and operation from benchmark name
            std::string name = result->name;
            size_t first_underscore = name.find('_');
            if (first_underscore != std::string::npos) {
                std::string ds_op = name.substr(0, name.find("_n"));
                
                auto& analyzer = analyzers[ds_op];
                if (result->input_size > 0) {
                    analyzer.add_data_point(result->input_size, 
                                          result->execution_time_ms.mean,
                                          result->memory_usage_mb.mean);
                }
            }
        }
        
        // Analyze each data structure operation
        for (const auto& [ds_op, analyzer] : analyzers) {
            std::cout << "\n" << ds_op << " Complexity Analysis:\n";
            std::cout << std::string(50, '-') << "\n";
            
            auto time_fit = analyzer.analyze_time_complexity();
            if (time_fit.complexity_class != ComplexityAnalyzer::ComplexityClass::UNKNOWN) {
                std::cout << "Time Complexity: " << time_fit.formula 
                          << " (confidence: " << std::fixed << std::setprecision(1) 
                          << time_fit.confidence_score * 100 << "%)\n";
                
                // Compare with expected complexity
                std::string expected = get_expected_ds_complexity(ds_op);
                if (!expected.empty()) {
                    std::cout << "Expected: " << expected << "\n";
                    std::cout << "Match: " << (time_fit.formula == expected ? "✓" : "✗") << "\n";
                }
            }
            
            auto space_fit = analyzer.analyze_space_complexity();
            if (space_fit.complexity_class != ComplexityAnalyzer::ComplexityClass::UNKNOWN) {
                std::cout << "Space Complexity: " << space_fit.formula 
                          << " (confidence: " << std::fixed << std::setprecision(1) 
                          << space_fit.confidence_score * 100 << "%)\n";
            }
        }
    }
    
private:
    std::string get_expected_ds_complexity(const std::string& ds_operation) const {
        // Expected time complexities for common data structure operations
        if (ds_operation.find("MinHeap_Insert") != std::string::npos) return "O(log n)";
        if (ds_operation.find("MinHeap_ExtractMin") != std::string::npos) return "O(log n)";
        if (ds_operation.find("HashMap_Insert") != std::string::npos) return "O(1)";
        if (ds_operation.find("HashMap_Lookup") != std::string::npos) return "O(1)";
        if (ds_operation.find("LinkedList_PushFront") != std::string::npos) return "O(1)";
        if (ds_operation.find("LinkedList_Search") != std::string::npos) return "O(n)";
        if (ds_operation.find("AVLTree_Insert") != std::string::npos) return "O(log n)";
        if (ds_operation.find("AVLTree_Search") != std::string::npos) return "O(log n)";
        return "";
    }
};

} // namespace rts::bench::scenarios