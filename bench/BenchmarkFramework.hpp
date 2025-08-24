#pragma once

#include <chrono>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>
#include <memory>

namespace rts::bench {

/**
 * @brief High-precision timer for benchmarking
 */
class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time_;
    std::chrono::high_resolution_clock::time_point end_time_;
    bool is_running_;

public:
    Timer() : is_running_(false) {}
    
    void start() {
        start_time_ = std::chrono::high_resolution_clock::now();
        is_running_ = true;
    }
    
    void stop() {
        end_time_ = std::chrono::high_resolution_clock::now();
        is_running_ = false;
    }
    
    double elapsed_microseconds() const {
        if (is_running_) {
            auto current = std::chrono::high_resolution_clock::now();
            return std::chrono::duration<double, std::micro>(current - start_time_).count();
        }
        return std::chrono::duration<double, std::micro>(end_time_ - start_time_).count();
    }
    
    double elapsed_milliseconds() const {
        return elapsed_microseconds() / 1000.0;
    }
    
    double elapsed_seconds() const {
        return elapsed_microseconds() / 1000000.0;
    }
};

/**
 * @brief Memory usage tracker
 */
class MemoryTracker {
private:
    size_t peak_memory_;
    size_t current_memory_;
    bool tracking_;

public:
    MemoryTracker() : peak_memory_(0), current_memory_(0), tracking_(false) {}
    
    void start() {
        peak_memory_ = 0;
        current_memory_ = 0;
        tracking_ = true;
    }
    
    void allocate(size_t bytes) {
        if (tracking_) {
            current_memory_ += bytes;
            peak_memory_ = std::max(peak_memory_, current_memory_);
        }
    }
    
    void deallocate(size_t bytes) {
        if (tracking_) {
            current_memory_ = (current_memory_ > bytes) ? current_memory_ - bytes : 0;
        }
    }
    
    void stop() {
        tracking_ = false;
    }
    
    size_t peak_memory_bytes() const { return peak_memory_; }
    size_t current_memory_bytes() const { return current_memory_; }
    
    double peak_memory_mb() const { return peak_memory_ / 1024.0 / 1024.0; }
    double current_memory_mb() const { return current_memory_ / 1024.0 / 1024.0; }
};

/**
 * @brief Statistical analysis for benchmark results
 */
struct Statistics {
    double mean;
    double median;
    double min;
    double max;
    double std_dev;
    double variance;
    size_t sample_count;
    
    Statistics() : mean(0), median(0), min(0), max(0), std_dev(0), variance(0), sample_count(0) {}
    
    static Statistics calculate(const std::vector<double>& values) {
        Statistics stats;
        if (values.empty()) return stats;
        
        stats.sample_count = values.size();
        
        // Sort for median calculation
        std::vector<double> sorted_values = values;
        std::sort(sorted_values.begin(), sorted_values.end());
        
        // Min/Max
        stats.min = sorted_values.front();
        stats.max = sorted_values.back();
        
        // Mean
        stats.mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        
        // Median
        if (values.size() % 2 == 0) {
            stats.median = (sorted_values[values.size() / 2 - 1] + sorted_values[values.size() / 2]) / 2.0;
        } else {
            stats.median = sorted_values[values.size() / 2];
        }
        
        // Variance and Standard Deviation
        double sum_squared_diff = 0.0;
        for (double value : values) {
            double diff = value - stats.mean;
            sum_squared_diff += diff * diff;
        }
        stats.variance = sum_squared_diff / values.size();
        stats.std_dev = std::sqrt(stats.variance);
        
        return stats;
    }
};

/**
 * @brief Individual benchmark result
 */
struct BenchmarkResult {
    std::string name;
    std::string algorithm;
    std::string dataset;
    size_t input_size;
    
    Statistics execution_time_ms;
    Statistics memory_usage_mb;
    Statistics operations_per_second;
    
    size_t total_operations;
    size_t iterations;
    double complexity_factor; // For complexity analysis
    
    std::map<std::string, double> custom_metrics;
    
    BenchmarkResult(const std::string& n) 
        : name(n), input_size(0), total_operations(0), iterations(0), complexity_factor(0.0) {}
};

/**
 * @brief Benchmark configuration
 */
struct BenchmarkConfig {
    size_t warm_up_iterations = 3;
    size_t measurement_iterations = 10;
    size_t min_iterations = 5;
    size_t max_iterations = 100;
    double min_time_seconds = 1.0;
    double max_time_seconds = 30.0;
    bool enable_memory_tracking = true;
    bool enable_statistical_analysis = true;
    bool save_raw_data = false;
    std::string output_format = "console"; // console, csv, json
    std::string output_file = "";
};

/**
 * @brief Main benchmarking framework
 */
class BenchmarkSuite {
private:
    std::vector<std::unique_ptr<BenchmarkResult>> results_;
    BenchmarkConfig config_;
    Timer timer_;
    MemoryTracker memory_tracker_;
    
    /**
     * @brief Run a single benchmark with automatic iteration counting
     */
    void run_adaptive_benchmark(const std::string& name,
                               std::function<void()> benchmark_func,
                               std::function<void()> setup_func = nullptr,
                               std::function<void()> teardown_func = nullptr) {
        
        auto result = std::make_unique<BenchmarkResult>(name);
        std::vector<double> execution_times;
        std::vector<double> memory_usages;
        
        // Warm-up
        for (size_t i = 0; i < config_.warm_up_iterations; ++i) {
            if (setup_func) setup_func();
            benchmark_func();
            if (teardown_func) teardown_func();
        }
        
        // Adaptive iteration counting
        size_t iterations = config_.min_iterations;
        double total_time = 0.0;
        
        while (iterations <= config_.max_iterations && total_time < config_.min_time_seconds) {
            if (setup_func) setup_func();
            
            if (config_.enable_memory_tracking) {
                memory_tracker_.start();
            }
            
            timer_.start();
            benchmark_func();
            timer_.stop();
            
            if (config_.enable_memory_tracking) {
                memory_tracker_.stop();
                memory_usages.push_back(memory_tracker_.peak_memory_mb());
            }
            
            double elapsed = timer_.elapsed_milliseconds();
            execution_times.push_back(elapsed);
            total_time += timer_.elapsed_seconds();
            
            if (teardown_func) teardown_func();
            
            iterations++;
            
            // Early termination if we've exceeded max time
            if (total_time > config_.max_time_seconds) break;
        }
        
        // Calculate statistics
        result->execution_time_ms = Statistics::calculate(execution_times);
        if (config_.enable_memory_tracking) {
            result->memory_usage_mb = Statistics::calculate(memory_usages);
        }
        result->iterations = iterations - config_.min_iterations;
        
        results_.push_back(std::move(result));
    }
    
public:
    BenchmarkSuite() = default;
    
    void set_config(const BenchmarkConfig& config) {
        config_ = config;
    }
    
    const BenchmarkConfig& get_config() const {
        return config_;
    }
    
    /**
     * @brief Add a benchmark test
     */
    template<typename Func>
    void add_benchmark(const std::string& name, Func benchmark_func) {
        run_adaptive_benchmark(name, benchmark_func);
    }
    
    /**
     * @brief Add a benchmark test with setup and teardown
     */
    template<typename BenchFunc, typename SetupFunc, typename TeardownFunc>
    void add_benchmark(const std::string& name, 
                      BenchFunc benchmark_func,
                      SetupFunc setup_func,
                      TeardownFunc teardown_func) {
        run_adaptive_benchmark(name, benchmark_func, setup_func, teardown_func);
    }
    
    /**
     * @brief Add a parameterized benchmark (for complexity analysis)
     */
    template<typename Func>
    void add_parameterized_benchmark(const std::string& base_name,
                                   const std::vector<size_t>& input_sizes,
                                   Func benchmark_func) {
        for (size_t size : input_sizes) {
            std::string name = base_name + "_n" + std::to_string(size);
            auto wrapped_func = [&, size]() { benchmark_func(size); };
            
            run_adaptive_benchmark(name, wrapped_func);
            
            // Set additional metadata
            if (!results_.empty()) {
                results_.back()->input_size = size;
                results_.back()->algorithm = base_name;
            }
        }
    }
    
    /**
     * @brief Run all benchmarks
     */
    void run_all() {
        std::cout << "Running Benchmark Suite...\n";
        std::cout << "Configuration:\n";
        std::cout << "  Warm-up iterations: " << config_.warm_up_iterations << "\n";
        std::cout << "  Min iterations: " << config_.min_iterations << "\n";
        std::cout << "  Max iterations: " << config_.max_iterations << "\n";
        std::cout << "  Min time: " << config_.min_time_seconds << "s\n";
        std::cout << "  Max time: " << config_.max_time_seconds << "s\n\n";
    }
    
    /**
     * @brief Generate performance report
     */
    void generate_report() const {
        if (config_.output_format == "csv") {
            generate_csv_report();
        } else {
            generate_console_report();
        }
    }
    
    /**
     * @brief Get all benchmark results
     */
    const std::vector<std::unique_ptr<BenchmarkResult>>& get_results() const {
        return results_;
    }
    
    /**
     * @brief Clear all results
     */
    void clear() {
        results_.clear();
    }

private:
    void generate_console_report() const {
        std::cout << "\n" << std::string(80, '=') << "\n";
        std::cout << "BENCHMARK RESULTS\n";
        std::cout << std::string(80, '=') << "\n\n";
        
        for (const auto& result : results_) {
            std::cout << "Benchmark: " << result->name << "\n";
            std::cout << std::string(40, '-') << "\n";
            
            if (result->input_size > 0) {
                std::cout << "Input Size: " << result->input_size << "\n";
            }
            
            std::cout << "Iterations: " << result->iterations << "\n";
            std::cout << "Execution Time (ms):\n";
            std::cout << "  Mean:   " << std::fixed << std::setprecision(3) << result->execution_time_ms.mean << "\n";
            std::cout << "  Median: " << std::fixed << std::setprecision(3) << result->execution_time_ms.median << "\n";
            std::cout << "  Min:    " << std::fixed << std::setprecision(3) << result->execution_time_ms.min << "\n";
            std::cout << "  Max:    " << std::fixed << std::setprecision(3) << result->execution_time_ms.max << "\n";
            std::cout << "  StdDev: " << std::fixed << std::setprecision(3) << result->execution_time_ms.std_dev << "\n";
            
            if (config_.enable_memory_tracking) {
                std::cout << "Memory Usage (MB):\n";
                std::cout << "  Peak:   " << std::fixed << std::setprecision(3) << result->memory_usage_mb.mean << "\n";
            }
            
            for (const auto& [metric_name, value] : result->custom_metrics) {
                std::cout << metric_name << ": " << std::fixed << std::setprecision(3) << value << "\n";
            }
            
            std::cout << "\n";
        }
    }
    
    void generate_csv_report() const {
        std::ofstream file(config_.output_file.empty() ? "benchmark_results.csv" : config_.output_file);
        
        // Header
        file << "Name,Algorithm,InputSize,Iterations,";
        file << "TimeMean,TimeMedian,TimeMin,TimeMax,TimeStdDev,";
        if (config_.enable_memory_tracking) {
            file << "MemoryMean,MemoryMax,";
        }
        file << "ComplexityFactor\n";
        
        // Data
        for (const auto& result : results_) {
            file << result->name << "," << result->algorithm << "," << result->input_size << ","
                 << result->iterations << ","
                 << result->execution_time_ms.mean << "," << result->execution_time_ms.median << ","
                 << result->execution_time_ms.min << "," << result->execution_time_ms.max << ","
                 << result->execution_time_ms.std_dev << ",";
            
            if (config_.enable_memory_tracking) {
                file << result->memory_usage_mb.mean << "," << result->memory_usage_mb.max << ",";
            }
            
            file << result->complexity_factor << "\n";
        }
    }
};

} // namespace rts::bench