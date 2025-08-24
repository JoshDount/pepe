#include "../../core/algorithms/ExternalSort.hpp"
#include "../TestFramework.hpp"
#include <fstream>
#include <filesystem>
#include <random>

void test_external_sorting() {
    using namespace rts::core::algorithms;
    
    // Test external sorting with integer data
    {
        ExternalSort<int>::Config config;
        config.memory_limit_bytes = 1024; // Small memory limit to force chunking
        config.temp_directory = "./test_temp";
        config.cleanup_temp_files = true;
        
        ExternalSort<int> sorter(config);
        
        // Test 1: Small dataset that fits in memory
        {
            std::vector<int> small_data = {64, 34, 25, 12, 22, 11, 90};
            std::vector<int> expected = {11, 12, 22, 25, 34, 64, 90};
            
            auto result = sorter.sort_vector(small_data);
            
            TestFramework::assert_true(result.success, "ExternalSort: Small dataset should succeed");
            TestFramework::assert_equal_string("ExternalSort", result.algorithm_name, "ExternalSort: Should have correct name");
            TestFramework::assert_equal_size_t(size_t(7), result.total_elements, "ExternalSort: Should track total elements");
            TestFramework::assert_equal_size_t(size_t(1), result.chunks_created, "ExternalSort: Small data should create 1 chunk");
            TestFramework::assert_true(result.total_time_ms >= 0, "ExternalSort: Should track execution time");
            
            for (size_t i = 0; i < small_data.size(); ++i) {
                TestFramework::assert_equal_int(expected[i], small_data[i], "ExternalSort: Should match expected result");
            }
        }
        
        // Test 2: Larger dataset that requires chunking
        {
            // Create test data larger than memory limit
            std::vector<int> large_data;
            size_t num_elements = config.memory_limit_bytes / sizeof(int) + 100; // Force multiple chunks
            
            std::random_device rd;
            std::mt19937 gen(42); // Fixed seed for reproducible results
            std::uniform_int_distribution<int> dist(1, 1000);
            
            for (size_t i = 0; i < num_elements; ++i) {
                large_data.push_back(dist(gen));
            }
            
            std::vector<int> original_data = large_data; // Keep copy for verification
            auto result = sorter.sort_vector(large_data);
            
            TestFramework::assert_true(result.success, "ExternalSort: Large dataset should succeed");
            TestFramework::assert_equal_size_t(num_elements, result.total_elements, "ExternalSort: Should track all elements");
            TestFramework::assert_true(result.chunks_created > 1, "ExternalSort: Should create multiple chunks");
            TestFramework::assert_true(result.phase1_time_ms > 0, "ExternalSort: Should track phase 1 time");
            TestFramework::assert_true(result.phase2_time_ms >= 0, "ExternalSort: Should track phase 2 time");
            TestFramework::assert_true(result.disk_io_operations > 0, "ExternalSort: Should have disk I/O operations");
            
            // Verify data is sorted
            for (size_t i = 1; i < large_data.size(); ++i) {
                TestFramework::assert_true(large_data[i-1] <= large_data[i], "ExternalSort: Data should be sorted");
            }
            
            // Verify all original elements are present
            std::sort(original_data.begin(), original_data.end());
            TestFramework::assert_equal_size_t(original_data.size(), large_data.size(), "ExternalSort: Should preserve all elements");
            for (size_t i = 0; i < original_data.size(); ++i) {
                TestFramework::assert_equal_int(original_data[i], large_data[i], "ExternalSort: Should preserve all original elements");
            }
        }
        
        // Test 3: File-based sorting
        {
            std::string input_file = config.temp_directory + "/test_input.dat";
            std::string output_file = config.temp_directory + "/test_output.dat";
            
            // Create input file with test data
            std::filesystem::create_directories(config.temp_directory);
            
            std::vector<int> test_data = {50, 20, 80, 10, 30, 70, 40, 60, 90, 15};
            std::ofstream input(input_file, std::ios::binary);
            TestFramework::assert_true(input.is_open(), "ExternalSort: Should create input file");
            
            for (int value : test_data) {
                input.write(reinterpret_cast<const char*>(&value), sizeof(int));
            }
            input.close();
            
            // Sort file
            auto result = sorter.sort_file(input_file, output_file);
            
            TestFramework::assert_true(result.success, "ExternalSort: File sorting should succeed");
            TestFramework::assert_equal_size_t(test_data.size(), result.total_elements, "ExternalSort: Should process all elements");
            TestFramework::assert_true(result.chunks_created >= 1, "ExternalSort: Should create at least 1 chunk");
            TestFramework::assert_equal_string(output_file, result.output_file, "ExternalSort: Should track output file");
            
            // Verify output file is sorted
            TestFramework::assert_true(ExternalSort<int>::verify_sorted_file(output_file), "ExternalSort: Output file should be sorted");
            
            // Read and verify output
            std::ifstream output(output_file, std::ios::binary);
            TestFramework::assert_true(output.is_open(), "ExternalSort: Should read output file");
            
            std::vector<int> sorted_data;
            int value;
            while (output.read(reinterpret_cast<char*>(&value), sizeof(int))) {
                sorted_data.push_back(value);
            }
            output.close();
            
            std::sort(test_data.begin(), test_data.end());
            TestFramework::assert_equal_size_t(test_data.size(), sorted_data.size(), "ExternalSort: Should have same number of elements");
            for (size_t i = 0; i < test_data.size(); ++i) {
                TestFramework::assert_equal_int(test_data[i], sorted_data[i], "ExternalSort: Should match sorted data");
            }
            
            // Cleanup
            std::filesystem::remove(input_file);
            std::filesystem::remove(output_file);
        }
        
        // Test 4: Custom comparator (descending order)
        {
            std::vector<int> data = {1, 5, 3, 9, 2, 8, 4, 7, 6};
            std::vector<int> expected = {9, 8, 7, 6, 5, 4, 3, 2, 1};
            
            auto result = sorter.sort_vector(data, std::greater<int>());
            
            TestFramework::assert_true(result.success, "ExternalSort: Custom comparator should work");
            for (size_t i = 0; i < data.size(); ++i) {
                TestFramework::assert_equal_int(expected[i], data[i], "ExternalSort: Should sort in descending order");
            }
        }
        
        // Test 5: Edge cases
        {
            // Empty vector
            std::vector<int> empty_data;
            auto empty_result = sorter.sort_vector(empty_data);
            TestFramework::assert_true(empty_result.success, "ExternalSort: Should handle empty data");
            TestFramework::assert_equal_size_t(size_t(0), empty_result.total_elements, "ExternalSort: Empty data should have 0 elements");
            
            // Single element
            std::vector<int> single_data = {42};
            auto single_result = sorter.sort_vector(single_data);
            TestFramework::assert_true(single_result.success, "ExternalSort: Should handle single element");
            TestFramework::assert_equal_int(42, single_data[0], "ExternalSort: Single element should remain unchanged");
            
            // Already sorted
            std::vector<int> sorted_data = {1, 2, 3, 4, 5};
            auto sorted_result = sorter.sort_vector(sorted_data);
            TestFramework::assert_true(sorted_result.success, "ExternalSort: Should handle already sorted data");
            
            // All duplicates
            std::vector<int> duplicate_data = {5, 5, 5, 5, 5};
            auto duplicate_result = sorter.sort_vector(duplicate_data);
            TestFramework::assert_true(duplicate_result.success, "ExternalSort: Should handle all duplicates");
            for (int val : duplicate_data) {
                TestFramework::assert_equal_int(5, val, "ExternalSort: All duplicates should remain unchanged");
            }
        }
        
        // Test 6: Configuration and utilities
        {
            // Test memory usage estimation
            size_t estimated = ExternalSort<int>::estimate_memory_usage(1000);
            TestFramework::assert_true(estimated > 1000 * sizeof(int), "ExternalSort: Should estimate memory with overhead");
            
            // Test chunk size calculation
            size_t chunk_size = ExternalSort<int>::calculate_chunk_size(1024);
            TestFramework::assert_true(chunk_size > 0, "ExternalSort: Should calculate reasonable chunk size");
            TestFramework::assert_true(chunk_size * sizeof(int) <= 1024, "ExternalSort: Chunk size should fit in memory limit");
            
            // Test configuration
            auto current_config = sorter.get_config();
            TestFramework::assert_equal_size_t(config.memory_limit_bytes, current_config.memory_limit_bytes, "ExternalSort: Should preserve config");
            TestFramework::assert_equal_string(config.temp_directory, current_config.temp_directory, "ExternalSort: Should preserve temp directory");
        }
        
        // Test 7: Performance characteristics
        {
            // Test with different sizes to verify performance scaling
            std::vector<size_t> test_sizes = {100, 500, 1000};
            std::vector<double> times;
            
            for (size_t size : test_sizes) {
                std::vector<int> perf_data;
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<int> dist(1, 10000);
                
                for (size_t i = 0; i < size; ++i) {
                    perf_data.push_back(dist(gen));
                }
                
                auto result = sorter.sort_vector(perf_data);
                TestFramework::assert_true(result.success, "ExternalSort: Performance test should succeed");
                times.push_back(result.total_time_ms);
                
                // Verify sorting is correct
                for (size_t i = 1; i < perf_data.size(); ++i) {
                    TestFramework::assert_true(perf_data[i-1] <= perf_data[i], "ExternalSort: Performance test data should be sorted");
                }
            }
            
            TestFramework::assert_true(times.size() == test_sizes.size(), "ExternalSort: Should have timing data for all test sizes");
            for (double time : times) {
                TestFramework::assert_true(time >= 0, "ExternalSort: All timing measurements should be non-negative");
            }
        }
        
        // Test 8: Error handling
        {
            // Test with invalid input file
            auto error_result = sorter.sort_file("nonexistent_file.dat", "output.dat");
            TestFramework::assert_true(!error_result.success, "ExternalSort: Should fail with invalid input file");
            TestFramework::assert_true(!error_result.error_message.empty(), "ExternalSort: Should provide error message");
        }
        
        // Cleanup test directory
        if (std::filesystem::exists(config.temp_directory)) {
            std::filesystem::remove_all(config.temp_directory);
        }
    }
    
    // Test 9: Large dataset generation and verification
    {
        ExternalSort<int>::Config config;
        config.temp_directory = "./test_large";
        config.memory_limit_bytes = 2048; // 2KB limit for testing
        
        ExternalSort<int> sorter(config);
        
        std::string large_file = config.temp_directory + "/large_dataset.dat";
        std::string sorted_file = config.temp_directory + "/large_sorted.dat";
        
        std::filesystem::create_directories(config.temp_directory);
        
        // Generate large dataset
        size_t num_elements = 1000;
        ExternalSort<int>::generate_large_dataset(large_file, num_elements, 1, 10000);
        
        // Verify file was created
        TestFramework::assert_true(std::filesystem::exists(large_file), "ExternalSort: Should create large dataset file");
        
        // Sort the large dataset
        auto result = sorter.sort_file(large_file, sorted_file);
        
        TestFramework::assert_true(result.success, "ExternalSort: Should sort large dataset");
        TestFramework::assert_equal_size_t(num_elements, result.total_elements, "ExternalSort: Should process all elements in large dataset");
        TestFramework::assert_true(result.chunks_created > 1, "ExternalSort: Large dataset should require multiple chunks");
        
        // Verify sorted file
        TestFramework::assert_true(ExternalSort<int>::verify_sorted_file(sorted_file), "ExternalSort: Large dataset should be properly sorted");
        
        // Cleanup
        std::filesystem::remove_all(config.temp_directory);
    }
    
    // Test 10: Memory limit enforcement
    {
        ExternalSort<int>::Config small_config;
        small_config.memory_limit_bytes = 64; // Very small memory limit
        small_config.temp_directory = "./test_memory";
        
        ExternalSort<int> memory_sorter(small_config);
        
        // Create data that definitely exceeds memory limit
        std::vector<int> memory_test_data;
        for (int i = 0; i < 100; ++i) {
            memory_test_data.push_back(100 - i); // Reverse order
        }
        
        auto result = memory_sorter.sort_vector(memory_test_data);
        
        TestFramework::assert_true(result.success, "ExternalSort: Should handle memory constraints");
        TestFramework::assert_true(result.chunks_created > 1, "ExternalSort: Small memory should force multiple chunks");
        
        // Verify data is sorted
        for (size_t i = 1; i < memory_test_data.size(); ++i) {
            TestFramework::assert_true(memory_test_data[i-1] <= memory_test_data[i], "ExternalSort: Memory-constrained sort should be correct");
        }
        
        // Cleanup
        if (std::filesystem::exists(small_config.temp_directory)) {
            std::filesystem::remove_all(small_config.temp_directory);
        }
    }
}