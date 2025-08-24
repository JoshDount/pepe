#pragma once

#include <vector>
#include <functional>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <string>
#include <random>

namespace rts::core::algorithms {

/**
 * @brief Internal sorting algorithms implementation
 * 
 * Core sorting algorithms for performance comparison:
 * - QuickSort: O(n log n) average, O(n²) worst case
 * - HeapSort: O(n log n) guaranteed
 * - ShellSort: O(n^1.25) with gap sequences
 * - MergeSort: O(n log n) stable
 * - InsertionSort: O(n²) efficient for small arrays
 */
template<typename T>
class InternalSort {
public:
    enum class Algorithm {
        QUICKSORT, HEAPSORT, SHELLSORT, MERGESORT, INSERTIONSORT, INTROSORT
    };
    
    struct SortResult {
        Algorithm algorithm;
        std::string algorithm_name;
        size_t array_size;
        bool is_sorted;
        
        size_t comparisons;
        size_t swaps;
        double time_microseconds;
        size_t max_stack_depth;
        size_t auxiliary_memory_bytes;
        bool is_stable;
        bool is_in_place;
        
        SortResult(Algorithm algo, const std::string& name, size_t size) 
            : algorithm(algo), algorithm_name(name), array_size(size), is_sorted(false),
              comparisons(0), swaps(0), time_microseconds(0.0), max_stack_depth(0),
              auxiliary_memory_bytes(0), is_stable(false), is_in_place(true) {}
        
        double get_efficiency_score() const {
            return array_size > 0 ? static_cast<double>(comparisons + swaps) / array_size : 0.0;
        }
    };

private:
    mutable size_t comparison_count_;
    mutable size_t swap_count_;
    mutable size_t current_depth_;
    mutable size_t max_depth_;
    mutable std::function<bool(const T&, const T&)> compare_func_;
    
    bool compare_and_count(const T& a, const T& b) const {
        comparison_count_++;
        return compare_func_(a, b);
    }
    
    void swap_and_count(T& a, T& b) const {
        swap_count_++;
        std::swap(a, b);
    }
    
    void reset_counters() const {
        comparison_count_ = 0;
        swap_count_ = 0;
        current_depth_ = 0;
        max_depth_ = 0;
    }

public:
    InternalSort() : comparison_count_(0), swap_count_(0), current_depth_(0), max_depth_(0) {}
    
    /**
     * @brief QuickSort with median-of-three pivot
     */
    SortResult quicksort(std::vector<T>& data, 
                        std::function<bool(const T&, const T&)> comp = std::less<T>()) const {
        auto start = std::chrono::high_resolution_clock::now();
        
        SortResult result(Algorithm::QUICKSORT, "QuickSort", data.size());
        reset_counters();
        compare_func_ = comp;
        
        if (data.size() > 1) {
            quicksort_recursive(data, 0, data.size() - 1);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.time_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        result.comparisons = comparison_count_;
        result.swaps = swap_count_;
        result.max_stack_depth = max_depth_;
        result.is_sorted = std::is_sorted(data.begin(), data.end(), comp);
        result.is_stable = false;
        result.is_in_place = true;
        
        return result;
    }
    
private:
    void quicksort_recursive(std::vector<T>& data, size_t low, size_t high) const {
        if (low >= high) return;
        
        current_depth_++;
        max_depth_ = std::max(max_depth_, current_depth_);
        
        size_t pivot = partition(data, low, high);
        if (pivot > 0) quicksort_recursive(data, low, pivot - 1);
        quicksort_recursive(data, pivot + 1, high);
        
        current_depth_--;
    }
    
    size_t partition(std::vector<T>& data, size_t low, size_t high) const {
        // Median-of-three
        size_t mid = low + (high - low) / 2;
        if (compare_and_count(data[mid], data[low])) swap_and_count(data[low], data[mid]);
        if (compare_and_count(data[high], data[low])) swap_and_count(data[low], data[high]);
        if (compare_and_count(data[high], data[mid])) swap_and_count(data[mid], data[high]);
        
        T pivot = data[mid];
        swap_and_count(data[mid], data[high]);
        
        size_t i = low;
        for (size_t j = low; j < high; ++j) {
            if (compare_and_count(data[j], pivot)) {
                swap_and_count(data[i], data[j]);
                i++;
            }
        }
        swap_and_count(data[i], data[high]);
        return i;
    }

public:
    /**
     * @brief HeapSort implementation
     */
    SortResult heapsort(std::vector<T>& data,
                       std::function<bool(const T&, const T&)> comp = std::less<T>()) const {
        auto start = std::chrono::high_resolution_clock::now();
        
        SortResult result(Algorithm::HEAPSORT, "HeapSort", data.size());
        reset_counters();
        compare_func_ = comp; // Use original comparator
        
        if (data.size() > 1) {
            // Build max heap
            for (int i = static_cast<int>(data.size() / 2) - 1; i >= 0; --i) {
                heapify_down(data, data.size(), i);
            }
            
            // Extract elements (largest first, place at end)
            for (size_t i = data.size() - 1; i > 0; --i) {
                swap_and_count(data[0], data[i]); // Move max to end
                heapify_down(data, i, 0);         // Restore heap property
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.time_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        result.comparisons = comparison_count_;
        result.swaps = swap_count_;
        result.is_sorted = std::is_sorted(data.begin(), data.end(), comp);
        result.is_stable = false;
        result.is_in_place = true;
        
        return result;
    }
    
private:
    void heapify_down(std::vector<T>& data, size_t heap_size, size_t root) const {
        size_t largest = root;
        size_t left = 2 * root + 1;
        size_t right = 2 * root + 2;
        
        // For max heap: find the largest among root, left child, and right child
        // If left child is greater than root (comp(data[largest], data[left]) is true)
        if (left < heap_size && compare_and_count(data[largest], data[left])) {
            largest = left;
        }
        // If right child is greater than current largest
        if (right < heap_size && compare_and_count(data[largest], data[right])) {
            largest = right;
        }
        
        if (largest != root) {
            swap_and_count(data[root], data[largest]);
            heapify_down(data, heap_size, largest);
        }
    }

public:
    /**
     * @brief ShellSort with Knuth sequence
     */
    SortResult shellsort(std::vector<T>& data,
                        std::function<bool(const T&, const T&)> comp = std::less<T>()) const {
        auto start = std::chrono::high_resolution_clock::now();
        
        SortResult result(Algorithm::SHELLSORT, "ShellSort", data.size());
        reset_counters();
        compare_func_ = comp;
        
        // Knuth sequence: h = 3*h + 1
        std::vector<size_t> gaps;
        size_t gap = 1;
        while (gap < data.size()) {
            gaps.push_back(gap);
            gap = gap * 3 + 1;
        }
        
        for (auto it = gaps.rbegin(); it != gaps.rend(); ++it) {
            gap = *it;
            for (size_t i = gap; i < data.size(); ++i) {
                T temp = data[i];
                size_t j = i;
                
                while (j >= gap && compare_and_count(temp, data[j - gap])) {
                    data[j] = data[j - gap];
                    j -= gap;
                    swap_count_++;
                }
                data[j] = temp;
            }
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.time_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        result.comparisons = comparison_count_;
        result.swaps = swap_count_;
        result.is_sorted = std::is_sorted(data.begin(), data.end(), comp);
        result.is_stable = false;
        result.is_in_place = true;
        
        return result;
    }
    
    /**
     * @brief MergeSort (stable)
     */
    SortResult mergesort(std::vector<T>& data,
                        std::function<bool(const T&, const T&)> comp = std::less<T>()) const {
        auto start = std::chrono::high_resolution_clock::now();
        
        SortResult result(Algorithm::MERGESORT, "MergeSort", data.size());
        reset_counters();
        compare_func_ = comp;
        
        std::vector<T> aux(data.size());
        result.auxiliary_memory_bytes = data.size() * sizeof(T);
        
        if (data.size() > 1) {
            mergesort_recursive(data, aux, 0, data.size() - 1);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.time_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        result.comparisons = comparison_count_;
        result.swaps = swap_count_;
        result.max_stack_depth = max_depth_;
        result.is_sorted = std::is_sorted(data.begin(), data.end(), comp);
        result.is_stable = true;
        result.is_in_place = false;
        
        return result;
    }
    
private:
    void mergesort_recursive(std::vector<T>& data, std::vector<T>& aux, size_t low, size_t high) const {
        if (low >= high) return;
        
        current_depth_++;
        max_depth_ = std::max(max_depth_, current_depth_);
        
        size_t mid = low + (high - low) / 2;
        mergesort_recursive(data, aux, low, mid);
        mergesort_recursive(data, aux, mid + 1, high);
        merge(data, aux, low, mid, high);
        
        current_depth_--;
    }
    
    void merge(std::vector<T>& data, std::vector<T>& aux, size_t low, size_t mid, size_t high) const {
        for (size_t i = low; i <= high; ++i) {
            aux[i] = data[i];
        }
        
        size_t i = low, j = mid + 1;
        for (size_t k = low; k <= high; ++k) {
            if (i > mid) {
                data[k] = aux[j++];
            } else if (j > high) {
                data[k] = aux[i++];
            } else if (compare_and_count(aux[j], aux[i])) {
                data[k] = aux[j++];
            } else {
                data[k] = aux[i++];
            }
        }
    }

public:
    /**
     * @brief InsertionSort (efficient for small arrays)
     */
    SortResult insertionsort(std::vector<T>& data,
                           std::function<bool(const T&, const T&)> comp = std::less<T>()) const {
        auto start = std::chrono::high_resolution_clock::now();
        
        SortResult result(Algorithm::INSERTIONSORT, "InsertionSort", data.size());
        reset_counters();
        compare_func_ = comp;
        
        for (size_t i = 1; i < data.size(); ++i) {
            T key = data[i];
            int j = static_cast<int>(i) - 1;
            
            while (j >= 0 && compare_and_count(key, data[j])) {
                data[j + 1] = data[j];
                j--;
                swap_count_++;
            }
            data[j + 1] = key;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        result.time_microseconds = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        result.comparisons = comparison_count_;
        result.swaps = swap_count_;
        result.is_sorted = std::is_sorted(data.begin(), data.end(), comp);
        result.is_stable = true;
        result.is_in_place = true;
        
        return result;
    }
    
    /**
     * @brief Compare multiple algorithms
     */
    struct ComparisonResult {
        std::vector<SortResult> results;
        std::string fastest_algorithm;
        std::string most_efficient_algorithm;
        
        void add_result(const SortResult& result) {
            results.push_back(result);
            update_best();
        }
        
    private:
        void update_best() {
            if (results.empty()) return;
            
            auto fastest = std::min_element(results.begin(), results.end(),
                [](const SortResult& a, const SortResult& b) {
                    return a.time_microseconds < b.time_microseconds;
                });
            fastest_algorithm = fastest->algorithm_name;
            
            auto efficient = std::min_element(results.begin(), results.end(),
                [](const SortResult& a, const SortResult& b) {
                    return a.get_efficiency_score() < b.get_efficiency_score();
                });
            most_efficient_algorithm = efficient->algorithm_name;
        }
    };
    
    ComparisonResult compare_algorithms(std::vector<T> data,
                                      std::function<bool(const T&, const T&)> comp = std::less<T>()) const {
        ComparisonResult comparison;
        
        std::vector<T> copy;
        
        copy = data; comparison.add_result(quicksort(copy, comp));
        copy = data; comparison.add_result(heapsort(copy, comp));
        copy = data; comparison.add_result(shellsort(copy, comp));
        copy = data; comparison.add_result(mergesort(copy, comp));
        
        if (data.size() <= 1000) {
            copy = data; comparison.add_result(insertionsort(copy, comp));
        }
        
        return comparison;
    }
    
    /**
     * @brief Generate test data
     */
    static std::vector<int> generate_random_data(size_t size, int min_val = 0, int max_val = 1000) {
        std::vector<int> data;
        data.reserve(size);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<int> dist(min_val, max_val);
        
        for (size_t i = 0; i < size; ++i) {
            data.push_back(dist(gen));
        }
        return data;
    }
    
    static std::vector<int> generate_sorted_data(size_t size, bool ascending = true) {
        std::vector<int> data;
        data.reserve(size);
        
        for (size_t i = 0; i < size; ++i) {
            data.push_back(ascending ? static_cast<int>(i) : static_cast<int>(size - i - 1));
        }
        return data;
    }
    
    static std::vector<int> generate_nearly_sorted_data(size_t size, double disorder_ratio = 0.1) {
        auto data = generate_sorted_data(size, true);
        
        std::random_device rd;
        std::mt19937 gen(rd());
        size_t swaps = static_cast<size_t>(size * disorder_ratio);
        
        for (size_t i = 0; i < swaps; ++i) {
            std::uniform_int_distribution<size_t> dist(0, size - 1);
            size_t idx1 = dist(gen);
            size_t idx2 = dist(gen);
            std::swap(data[idx1], data[idx2]);
        }
        
        return data;
    }
};

} // namespace rts::core::algorithms