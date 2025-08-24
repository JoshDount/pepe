#pragma once

#include "BenchmarkFramework.hpp"
#include <cmath>
#include <vector>
#include <algorithm>
#include <map>
#include <string>

namespace rts::bench {

/**
 * @brief Complexity analysis framework
 */
class ComplexityAnalyzer {
public:
    /**
     * @brief Known complexity classes
     */
    enum class ComplexityClass {
        CONSTANT,       // O(1)
        LOGARITHMIC,    // O(log n)
        LINEAR,         // O(n)
        LINEARITHMIC,   // O(n log n)
        QUADRATIC,      // O(n²)
        CUBIC,          // O(n³)
        EXPONENTIAL,    // O(2^n)
        FACTORIAL,      // O(n!)
        UNKNOWN
    };
    
    /**
     * @brief Complexity fit result
     */
    struct ComplexityFit {
        ComplexityClass complexity_class;
        double correlation_coefficient;
        double constant_factor;
        std::string formula;
        double confidence_score;
        
        ComplexityFit() : complexity_class(ComplexityClass::UNKNOWN), 
                         correlation_coefficient(0.0), constant_factor(0.0), confidence_score(0.0) {}
    };
    
    /**
     * @brief Data point for complexity analysis
     */
    struct DataPoint {
        size_t input_size;
        double execution_time;
        double memory_usage;
        size_t operations_count;
        
        DataPoint(size_t n, double time, double memory = 0.0, size_t ops = 0)
            : input_size(n), execution_time(time), memory_usage(memory), operations_count(ops) {}
    };
    
private:
    std::vector<DataPoint> data_points_;
    
    /**
     * @brief Calculate correlation coefficient
     */
    double calculate_correlation(const std::vector<double>& x, const std::vector<double>& y) const {
        if (x.size() != y.size() || x.empty()) return 0.0;
        
        double mean_x = std::accumulate(x.begin(), x.end(), 0.0) / x.size();
        double mean_y = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
        
        double numerator = 0.0;
        double sum_x_sq = 0.0;
        double sum_y_sq = 0.0;
        
        for (size_t i = 0; i < x.size(); ++i) {
            double dx = x[i] - mean_x;
            double dy = y[i] - mean_y;
            numerator += dx * dy;
            sum_x_sq += dx * dx;
            sum_y_sq += dy * dy;
        }
        
        double denominator = std::sqrt(sum_x_sq * sum_y_sq);
        return (denominator == 0.0) ? 0.0 : numerator / denominator;
    }
    
    /**
     * @brief Generate theoretical complexity values
     */
    std::vector<double> generate_complexity_values(ComplexityClass complexity, 
                                                 const std::vector<size_t>& input_sizes) const {
        std::vector<double> values;
        values.reserve(input_sizes.size());
        
        for (size_t n : input_sizes) {
            double value = 0.0;
            switch (complexity) {
                case ComplexityClass::CONSTANT:
                    value = 1.0;
                    break;
                case ComplexityClass::LOGARITHMIC:
                    value = std::log2(static_cast<double>(n));
                    break;
                case ComplexityClass::LINEAR:
                    value = static_cast<double>(n);
                    break;
                case ComplexityClass::LINEARITHMIC:
                    value = static_cast<double>(n) * std::log2(static_cast<double>(n));
                    break;
                case ComplexityClass::QUADRATIC:
                    value = static_cast<double>(n * n);
                    break;
                case ComplexityClass::CUBIC:
                    value = static_cast<double>(n * n * n);
                    break;
                case ComplexityClass::EXPONENTIAL:
                    value = std::pow(2.0, static_cast<double>(n));
                    break;
                case ComplexityClass::FACTORIAL:
                    value = 1.0;
                    for (size_t i = 1; i <= n; ++i) {
                        value *= i;
                    }
                    break;
                default:
                    value = 1.0;
            }
            values.push_back(value);
        }
        
        return values;
    }
    
    /**
     * @brief Get complexity class name
     */
    std::string get_complexity_name(ComplexityClass complexity) const {
        switch (complexity) {
            case ComplexityClass::CONSTANT: return "O(1)";
            case ComplexityClass::LOGARITHMIC: return "O(log n)";
            case ComplexityClass::LINEAR: return "O(n)";
            case ComplexityClass::LINEARITHMIC: return "O(n log n)";
            case ComplexityClass::QUADRATIC: return "O(n²)";
            case ComplexityClass::CUBIC: return "O(n³)";
            case ComplexityClass::EXPONENTIAL: return "O(2^n)";
            case ComplexityClass::FACTORIAL: return "O(n!)";
            default: return "O(?)";
        }
    }
    
public:
    /**
     * @brief Add measurement data point
     */
    void add_data_point(size_t input_size, double execution_time, 
                       double memory_usage = 0.0, size_t operations = 0) {
        data_points_.emplace_back(input_size, execution_time, memory_usage, operations);
    }
    
    /**
     * @brief Add data points from benchmark results
     */
    void add_benchmark_results(const std::vector<std::unique_ptr<BenchmarkResult>>& results) {
        for (const auto& result : results) {
            if (result->input_size > 0) {
                add_data_point(result->input_size, 
                             result->execution_time_ms.mean,
                             result->memory_usage_mb.mean,
                             result->total_operations);
            }
        }
    }
    
    /**
     * @brief Analyze complexity and find best fit
     */
    ComplexityFit analyze_time_complexity() const {
        if (data_points_.size() < 3) {
            return ComplexityFit(); // Not enough data
        }
        
        // Extract input sizes and execution times
        std::vector<size_t> input_sizes;
        std::vector<double> execution_times;
        
        for (const auto& point : data_points_) {
            input_sizes.push_back(point.input_size);
            execution_times.push_back(point.execution_time);
        }
        
        // Test different complexity classes
        std::vector<ComplexityClass> complexity_classes = {
            ComplexityClass::CONSTANT,
            ComplexityClass::LOGARITHMIC,
            ComplexityClass::LINEAR,
            ComplexityClass::LINEARITHMIC,
            ComplexityClass::QUADRATIC,
            ComplexityClass::CUBIC
        };
        
        ComplexityFit best_fit;
        double best_correlation = -1.0;
        
        for (ComplexityClass complexity : complexity_classes) {
            auto theoretical_values = generate_complexity_values(complexity, input_sizes);
            double correlation = calculate_correlation(theoretical_values, execution_times);
            
            if (std::abs(correlation) > std::abs(best_correlation)) {
                best_correlation = correlation;
                best_fit.complexity_class = complexity;
                best_fit.correlation_coefficient = correlation;
                best_fit.formula = get_complexity_name(complexity);
                best_fit.confidence_score = std::abs(correlation);
            }
        }
        
        // Calculate constant factor (average ratio of actual time to theoretical time)
        if (best_fit.complexity_class != ComplexityClass::UNKNOWN) {
            auto theoretical_values = generate_complexity_values(best_fit.complexity_class, input_sizes);
            double sum_ratios = 0.0;
            size_t valid_ratios = 0;
            
            for (size_t i = 0; i < execution_times.size() && i < theoretical_values.size(); ++i) {
                if (theoretical_values[i] > 0.0) {
                    sum_ratios += execution_times[i] / theoretical_values[i];
                    valid_ratios++;
                }
            }
            
            if (valid_ratios > 0) {
                best_fit.constant_factor = sum_ratios / valid_ratios;
            }
        }
        
        return best_fit;
    }
    
    /**
     * @brief Analyze space complexity
     */
    ComplexityFit analyze_space_complexity() const {
        if (data_points_.size() < 3) {
            return ComplexityFit(); // Not enough data
        }
        
        // Extract input sizes and memory usage
        std::vector<size_t> input_sizes;
        std::vector<double> memory_usage;
        
        for (const auto& point : data_points_) {
            input_sizes.push_back(point.input_size);
            memory_usage.push_back(point.memory_usage);
        }
        
        // Similar analysis as time complexity but for memory
        std::vector<ComplexityClass> complexity_classes = {
            ComplexityClass::CONSTANT,
            ComplexityClass::LOGARITHMIC,
            ComplexityClass::LINEAR,
            ComplexityClass::QUADRATIC
        };
        
        ComplexityFit best_fit;
        double best_correlation = -1.0;
        
        for (ComplexityClass complexity : complexity_classes) {
            auto theoretical_values = generate_complexity_values(complexity, input_sizes);
            double correlation = calculate_correlation(theoretical_values, memory_usage);
            
            if (std::abs(correlation) > std::abs(best_correlation)) {
                best_correlation = correlation;
                best_fit.complexity_class = complexity;
                best_fit.correlation_coefficient = correlation;
                best_fit.formula = get_complexity_name(complexity);
                best_fit.confidence_score = std::abs(correlation);
            }
        }
        
        return best_fit;
    }
    
    /**
     * @brief Generate performance prediction for larger input sizes
     */
    std::vector<std::pair<size_t, double>> predict_performance(const ComplexityFit& fit,
                                                             const std::vector<size_t>& prediction_sizes) const {
        std::vector<std::pair<size_t, double>> predictions;
        
        if (fit.complexity_class == ComplexityClass::UNKNOWN || fit.constant_factor == 0.0) {
            return predictions;
        }
        
        auto theoretical_values = generate_complexity_values(fit.complexity_class, prediction_sizes);
        
        for (size_t i = 0; i < prediction_sizes.size(); ++i) {
            double predicted_time = theoretical_values[i] * fit.constant_factor;
            predictions.emplace_back(prediction_sizes[i], predicted_time);
        }
        
        return predictions;
    }
    
    /**
     * @brief Generate complexity analysis report
     */
    void generate_complexity_report() const {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "COMPLEXITY ANALYSIS REPORT\n";
        std::cout << std::string(60, '=') << "\n\n";
        
        auto time_fit = analyze_time_complexity();
        auto space_fit = analyze_space_complexity();
        
        std::cout << "Data Points: " << data_points_.size() << "\n\n";
        
        // Time complexity analysis
        std::cout << "TIME COMPLEXITY ANALYSIS:\n";
        std::cout << std::string(30, '-') << "\n";
        if (time_fit.complexity_class != ComplexityClass::UNKNOWN) {
            std::cout << "Best Fit: " << time_fit.formula << "\n";
            std::cout << "Correlation: " << std::fixed << std::setprecision(4) 
                      << time_fit.correlation_coefficient << "\n";
            std::cout << "Confidence: " << std::fixed << std::setprecision(2) 
                      << time_fit.confidence_score * 100 << "%\n";
            std::cout << "Constant Factor: " << std::scientific << std::setprecision(3) 
                      << time_fit.constant_factor << "\n";
        } else {
            std::cout << "Unable to determine complexity (insufficient data)\n";
        }
        
        std::cout << "\n";
        
        // Space complexity analysis
        std::cout << "SPACE COMPLEXITY ANALYSIS:\n";
        std::cout << std::string(30, '-') << "\n";
        if (space_fit.complexity_class != ComplexityClass::UNKNOWN) {
            std::cout << "Best Fit: " << space_fit.formula << "\n";
            std::cout << "Correlation: " << std::fixed << std::setprecision(4) 
                      << space_fit.correlation_coefficient << "\n";
            std::cout << "Confidence: " << std::fixed << std::setprecision(2) 
                      << space_fit.confidence_score * 100 << "%\n";
        } else {
            std::cout << "Unable to determine complexity (insufficient data)\n";
        }
        
        // Performance predictions
        if (time_fit.complexity_class != ComplexityClass::UNKNOWN) {
            std::cout << "\nPERFORMANCE PREDICTIONS:\n";
            std::cout << std::string(30, '-') << "\n";
            
            std::vector<size_t> prediction_sizes = {1000, 10000, 100000, 1000000};
            auto predictions = predict_performance(time_fit, prediction_sizes);
            
            std::cout << "Input Size -> Predicted Time (ms)\n";
            for (const auto& [size, time] : predictions) {
                std::cout << std::setw(10) << size << " -> " 
                          << std::fixed << std::setprecision(3) << time << "\n";
            }
        }
        
        std::cout << "\n";
    }
    
    /**
     * @brief Clear all data points
     */
    void clear() {
        data_points_.clear();
    }
    
    /**
     * @brief Get number of data points
     */
    size_t size() const {
        return data_points_.size();
    }
};

/**
 * @brief Comparative performance analyzer
 */
class ComparativeAnalyzer {
private:
    std::map<std::string, ComplexityAnalyzer> analyzers_;
    
public:
    /**
     * @brief Add algorithm data
     */
    void add_algorithm_data(const std::string& algorithm_name,
                           const std::vector<std::unique_ptr<BenchmarkResult>>& results) {
        auto& analyzer = analyzers_[algorithm_name];
        analyzer.clear();
        analyzer.add_benchmark_results(results);
    }
    
    /**
     * @brief Generate comparative analysis report
     */
    void generate_comparative_report() const {
        std::cout << "\n" << std::string(80, '=') << "\n";
        std::cout << "COMPARATIVE ALGORITHM ANALYSIS\n";
        std::cout << std::string(80, '=') << "\n\n";
        
        // Analyze each algorithm
        std::map<std::string, ComplexityAnalyzer::ComplexityFit> time_complexities;
        std::map<std::string, ComplexityAnalyzer::ComplexityFit> space_complexities;
        
        for (const auto& [name, analyzer] : analyzers_) {
            time_complexities[name] = analyzer.analyze_time_complexity();
            space_complexities[name] = analyzer.analyze_space_complexity();
        }
        
        // Time complexity comparison
        std::cout << "TIME COMPLEXITY COMPARISON:\n";
        std::cout << std::string(50, '-') << "\n";
        std::cout << std::left << std::setw(20) << "Algorithm" 
                  << std::setw(15) << "Complexity" 
                  << std::setw(12) << "Confidence" 
                  << "Constant Factor\n";
        std::cout << std::string(50, '-') << "\n";
        
        for (const auto& [name, fit] : time_complexities) {
            std::cout << std::left << std::setw(20) << name.substr(0, 19)
                      << std::setw(15) << fit.formula.substr(0, 14)
                      << std::setw(12) << std::fixed << std::setprecision(1) 
                      << fit.confidence_score * 100 << "%"
                      << std::scientific << std::setprecision(2) << fit.constant_factor << "\n";
        }
        
        std::cout << "\n";
        
        // Recommendations
        std::cout << "PERFORMANCE RECOMMENDATIONS:\n";
        std::cout << std::string(50, '-') << "\n";
        
        // Find best performing algorithm for different categories
        std::string best_overall = "";
        std::string best_small_input = "";
        std::string best_large_input = "";
        
        double best_overall_score = -1.0;
        
        for (const auto& [name, fit] : time_complexities) {
            if (fit.confidence_score > best_overall_score) {
                best_overall_score = fit.confidence_score;
                best_overall = name;
            }
        }
        
        std::cout << "Most predictable performance: " << best_overall << "\n";
        
        // Provide complexity-based recommendations
        for (const auto& [name, fit] : time_complexities) {
            if (fit.complexity_class == ComplexityAnalyzer::ComplexityClass::CONSTANT ||
                fit.complexity_class == ComplexityAnalyzer::ComplexityClass::LOGARITHMIC) {
                std::cout << "Best for all input sizes: " << name << " (" << fit.formula << ")\n";
            } else if (fit.complexity_class == ComplexityAnalyzer::ComplexityClass::LINEAR ||
                      fit.complexity_class == ComplexityAnalyzer::ComplexityClass::LINEARITHMIC) {
                std::cout << "Good scalability: " << name << " (" << fit.formula << ")\n";
            } else if (fit.complexity_class == ComplexityAnalyzer::ComplexityClass::QUADRATIC ||
                      fit.complexity_class == ComplexityAnalyzer::ComplexityClass::CUBIC) {
                std::cout << "Use with caution for large inputs: " << name << " (" << fit.formula << ")\n";
            }
        }
        
        std::cout << "\n";
    }
};

} // namespace rts::bench