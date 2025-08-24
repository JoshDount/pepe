#pragma once

#include <vector>
#include <functional>
#include <string>
#include <fstream>
#include <queue>
#include <filesystem>
#include <chrono>
#include <algorithm>
#include <memory>
#include <iostream>
#include <random>

namespace rts::core::algorithms {

/**
 * @brief External sorting implementation for large datasets
 * 
 * Handles datasets that don't fit in memory using divide-and-conquer approach:
 * 1. Split large data into chunks that fit in memory
 * 2. Sort each chunk internally and write to temporary files
 * 3. Merge sorted chunks using k-way merge with priority queue
 * 
 * Time Complexity: O(n log n) where n is total data size
 * Space Complexity: O(M + k) where M is memory limit, k is number of chunks
 */
template<typename T>
class ExternalSort {
public:
    struct SortResult {
        bool success;
        std::string algorithm_name;
        size_t total_elements;
        size_t chunks_created;
        size_t memory_limit_bytes;
        double phase1_time_ms;  // Chunking and internal sorting
        double phase2_time_ms;  // K-way merging
        double total_time_ms;
        size_t temp_files_created;
        size_t disk_io_operations;
        std::string output_file;
        std::string error_message;
        
        SortResult() : success(false), algorithm_name("ExternalSort"), total_elements(0),
                      chunks_created(0), memory_limit_bytes(0), phase1_time_ms(0.0),
                      phase2_time_ms(0.0), total_time_ms(0.0), temp_files_created(0),
                      disk_io_operations(0) {}
    };
    
    struct Config {
        size_t memory_limit_bytes;     // Maximum memory to use
        std::string temp_directory;    // Directory for temporary files
        std::string temp_prefix;       // Prefix for temporary file names
        bool cleanup_temp_files;       // Whether to delete temp files after sorting
        size_t io_buffer_size;         // Buffer size for file I/O
        
        Config() : memory_limit_bytes(64 * 1024 * 1024),  // 64MB default
                  temp_directory("./temp"), temp_prefix("esort_"),
                  cleanup_temp_files(true), io_buffer_size(8192) {}
    };

private:
    Config config_;
    mutable size_t disk_io_count_;
    
    /**
     * @brief Structure for k-way merge priority queue
     */
    struct MergeNode {
        T value;
        size_t file_index;
        
        MergeNode(const T& val, size_t idx) : value(val), file_index(idx) {}
    };
    
    /**
     * @brief Comparator for k-way merge (min-heap behavior)
     */
    struct MergeComparator {
        std::function<bool(const T&, const T&)> comp;
        
        MergeComparator(std::function<bool(const T&, const T&)> c) : comp(c) {}
        
        bool operator()(const MergeNode& a, const MergeNode& b) const {
            return comp(b.value, a.value); // Reverse for min-heap
        }
    };
    
public:
    ExternalSort(const Config& cfg = Config()) : config_(cfg), disk_io_count_(0) {
        // Ensure temp directory exists
        std::filesystem::create_directories(config_.temp_directory);
    }
    
    /**
     * @brief Sort data from input file to output file
     * @param input_file Path to input file containing unsorted data
     * @param output_file Path to output file for sorted data
     * @param comp Comparison function for sorting
     * @return Sort result with performance metrics
     */
    SortResult sort_file(const std::string& input_file, 
                        const std::string& output_file,
                        std::function<bool(const T&, const T&)> comp = std::less<T>()) const {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        SortResult result;
        result.algorithm_name = "ExternalSort";
        result.output_file = output_file;
        result.memory_limit_bytes = config_.memory_limit_bytes;
        disk_io_count_ = 0;
        
        try {
            // Phase 1: Split into sorted chunks
            auto phase1_start = std::chrono::high_resolution_clock::now();
            auto chunk_files = create_sorted_chunks(input_file, comp, result);
            auto phase1_end = std::chrono::high_resolution_clock::now();
            
            result.phase1_time_ms = std::chrono::duration<double, std::milli>(phase1_end - phase1_start).count();
            result.chunks_created = chunk_files.size();
            result.temp_files_created = chunk_files.size();
            
            if (chunk_files.empty()) {
                result.error_message = "No data found in input file";
                return result;
            }
            
            // Phase 2: K-way merge
            auto phase2_start = std::chrono::high_resolution_clock::now();
            bool merge_success = merge_sorted_chunks(chunk_files, output_file, comp, result);
            auto phase2_end = std::chrono::high_resolution_clock::now();
            
            result.phase2_time_ms = std::chrono::duration<double, std::milli>(phase2_end - phase2_start).count();
            
            // Cleanup temporary files
            if (config_.cleanup_temp_files) {
                for (const auto& file : chunk_files) {
                    std::filesystem::remove(file);
                }
            }
            
            result.success = merge_success;
            result.disk_io_operations = disk_io_count_;
            
        } catch (const std::exception& e) {
            result.success = false;
            result.error_message = std::string("Exception: ") + e.what();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.total_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        
        return result;
    }
    
    /**
     * @brief Sort vector data using external sorting approach
     * @param data Vector to sort (will be sorted in-place if fits in memory)
     * @param comp Comparison function
     * @return Sort result with performance metrics
     */
    SortResult sort_vector(std::vector<T>& data,
                          std::function<bool(const T&, const T&)> comp = std::less<T>()) const {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        SortResult result;
        result.algorithm_name = "ExternalSort";
        result.total_elements = data.size();
        result.memory_limit_bytes = config_.memory_limit_bytes;
        disk_io_count_ = 0;
        
        try {
            size_t data_size_bytes = data.size() * sizeof(T);
            
            // If data fits in memory, use internal sorting
            if (data_size_bytes <= config_.memory_limit_bytes) {
                auto phase1_start = std::chrono::high_resolution_clock::now();
                std::sort(data.begin(), data.end(), comp);
                auto phase1_end = std::chrono::high_resolution_clock::now();
                
                result.phase1_time_ms = std::chrono::duration<double, std::milli>(phase1_end - phase1_start).count();
                result.phase2_time_ms = 0.0;
                result.chunks_created = 1;
                result.success = true;
            } else {
                // Use external sorting approach
                std::string temp_input = config_.temp_directory + "/" + config_.temp_prefix + "input.dat";
                std::string temp_output = config_.temp_directory + "/" + config_.temp_prefix + "output.dat";
                
                // Write data to temporary file
                write_vector_to_file(data, temp_input);
                
                // Sort using external method
                auto ext_result = sort_file(temp_input, temp_output, comp);
                result = ext_result;
                
                // Read sorted data back
                read_vector_from_file(data, temp_output);
                
                // Cleanup
                if (config_.cleanup_temp_files) {
                    std::filesystem::remove(temp_input);
                    std::filesystem::remove(temp_output);
                }
            }
            
        } catch (const std::exception& e) {
            result.success = false;
            result.error_message = std::string("Exception: ") + e.what();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.total_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        
        return result;
    }

private:
    /**
     * @brief Create sorted chunks from input file
     */
    std::vector<std::string> create_sorted_chunks(const std::string& input_file,
                                                  std::function<bool(const T&, const T&)> comp,
                                                  SortResult& result) const {
        std::vector<std::string> chunk_files;
        std::ifstream input(input_file, std::ios::binary);
        
        if (!input.is_open()) {
            throw std::runtime_error("Cannot open input file: " + input_file);
        }
        
        size_t chunk_index = 0;
        size_t elements_per_chunk = config_.memory_limit_bytes / sizeof(T);
        
        std::vector<T> chunk_buffer;
        chunk_buffer.reserve(elements_per_chunk);
        
        T element;
        while (input.read(reinterpret_cast<char*>(&element), sizeof(T))) {
            chunk_buffer.push_back(element);
            result.total_elements++;
            disk_io_count_++;
            
            if (chunk_buffer.size() >= elements_per_chunk) {
                // Sort and write chunk
                std::string chunk_file = create_chunk_file(chunk_buffer, chunk_index++, comp);
                chunk_files.push_back(chunk_file);
                chunk_buffer.clear();
            }
        }
        
        // Handle remaining elements
        if (!chunk_buffer.empty()) {
            std::string chunk_file = create_chunk_file(chunk_buffer, chunk_index++, comp);
            chunk_files.push_back(chunk_file);
        }
        
        input.close();
        return chunk_files;
    }
    
    /**
     * @brief Create a sorted chunk file
     */
    std::string create_chunk_file(std::vector<T>& chunk_data, size_t chunk_index,
                                 std::function<bool(const T&, const T&)> comp) const {
        // Sort chunk in memory
        std::sort(chunk_data.begin(), chunk_data.end(), comp);
        
        // Write to temporary file
        std::string chunk_file = config_.temp_directory + "/" + config_.temp_prefix + 
                                "chunk_" + std::to_string(chunk_index) + ".dat";
        
        std::ofstream output(chunk_file, std::ios::binary);
        if (!output.is_open()) {
            throw std::runtime_error("Cannot create chunk file: " + chunk_file);
        }
        
        for (const auto& element : chunk_data) {
            output.write(reinterpret_cast<const char*>(&element), sizeof(T));
            disk_io_count_++;
        }
        
        output.close();
        return chunk_file;
    }
    
    /**
     * @brief Merge sorted chunk files using k-way merge
     */
    bool merge_sorted_chunks(const std::vector<std::string>& chunk_files,
                           const std::string& output_file,
                           std::function<bool(const T&, const T&)> comp,
                           SortResult& result) const {
        std::ofstream output(output_file, std::ios::binary);
        if (!output.is_open()) {
            result.error_message = "Cannot create output file: " + output_file;
            return false;
        }
        
        // Open all chunk files
        std::vector<std::unique_ptr<std::ifstream>> input_streams;
        for (const auto& file : chunk_files) {
            auto stream = std::make_unique<std::ifstream>(file, std::ios::binary);
            if (!stream->is_open()) {
                result.error_message = "Cannot open chunk file: " + file;
                return false;
            }
            input_streams.push_back(std::move(stream));
        }
        
        // Priority queue for k-way merge
        MergeComparator merge_comp(comp);
        std::priority_queue<MergeNode, std::vector<MergeNode>, MergeComparator> pq(merge_comp);
        
        // Initialize priority queue with first element from each file
        for (size_t i = 0; i < input_streams.size(); ++i) {
            T element;
            if (input_streams[i]->read(reinterpret_cast<char*>(&element), sizeof(T))) {
                pq.push(MergeNode(element, i));
                disk_io_count_++;
            }
        }
        
        // K-way merge
        while (!pq.empty()) {
            MergeNode min_node = pq.top();
            pq.pop();
            
            // Write minimum element to output
            output.write(reinterpret_cast<const char*>(&min_node.value), sizeof(T));
            disk_io_count_++;
            
            // Read next element from the same file
            T next_element;
            if (input_streams[min_node.file_index]->read(reinterpret_cast<char*>(&next_element), sizeof(T))) {
                pq.push(MergeNode(next_element, min_node.file_index));
                disk_io_count_++;
            }
        }
        
        // Close all files
        for (auto& stream : input_streams) {
            stream->close();
        }
        output.close();
        
        return true;
    }
    
    /**
     * @brief Write vector to binary file
     */
    void write_vector_to_file(const std::vector<T>& data, const std::string& filename) const {
        std::ofstream output(filename, std::ios::binary);
        if (!output.is_open()) {
            throw std::runtime_error("Cannot create file: " + filename);
        }
        
        for (const auto& element : data) {
            output.write(reinterpret_cast<const char*>(&element), sizeof(T));
            disk_io_count_++;
        }
        
        output.close();
    }
    
    /**
     * @brief Read vector from binary file
     */
    void read_vector_from_file(std::vector<T>& data, const std::string& filename) const {
        std::ifstream input(filename, std::ios::binary);
        if (!input.is_open()) {
            throw std::runtime_error("Cannot open file: " + filename);
        }
        
        data.clear();
        T element;
        while (input.read(reinterpret_cast<char*>(&element), sizeof(T))) {
            data.push_back(element);
            disk_io_count_++;
        }
        
        input.close();
    }

public:
    /**
     * @brief Generate large test dataset
     */
    static void generate_large_dataset(const std::string& filename, size_t num_elements,
                                      int min_val = 0, int max_val = 1000000) {
        std::ofstream output(filename, std::ios::binary);
        if (!output.is_open()) {
            throw std::runtime_error("Cannot create dataset file: " + filename);
        }
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(min_val, max_val);
        
        for (size_t i = 0; i < num_elements; ++i) {
            int value = dist(gen);
            output.write(reinterpret_cast<const char*>(&value), sizeof(int));
        }
        
        output.close();
    }
    
    /**
     * @brief Verify that file is sorted
     */
    static bool verify_sorted_file(const std::string& filename,
                                  std::function<bool(const T&, const T&)> comp = std::less<T>()) {
        std::ifstream input(filename, std::ios::binary);
        if (!input.is_open()) {
            return false;
        }
        
        T prev_element, current_element;
        bool first = true;
        
        while (input.read(reinterpret_cast<char*>(&current_element), sizeof(T))) {
            if (!first && comp(current_element, prev_element)) {
                input.close();
                return false; // Not sorted
            }
            prev_element = current_element;
            first = false;
        }
        
        input.close();
        return true;
    }
    
    /**
     * @brief Get configuration
     */
    const Config& get_config() const { return config_; }
    
    /**
     * @brief Set configuration
     */
    void set_config(const Config& cfg) { 
        config_ = cfg;
        std::filesystem::create_directories(config_.temp_directory);
    }
    
    /**
     * @brief Estimate memory usage for given data size
     */
    static size_t estimate_memory_usage(size_t num_elements) {
        return num_elements * sizeof(T) + 1024; // Add overhead
    }
    
    /**
     * @brief Calculate optimal chunk size for memory limit
     */
    static size_t calculate_chunk_size(size_t memory_limit_bytes) {
        return (memory_limit_bytes * 0.8) / sizeof(T); // Use 80% of memory limit
    }
};

} // namespace rts::core::algorithms