#include "../../core/algorithms/InternalSort.hpp"
#include "../TestFramework.hpp"

void test_sorting_algorithms() {
    using namespace rts::core::algorithms;
    
    // Test QuickSort
    {
        InternalSort<int> sorter;
        std::vector<int> data = {64, 34, 25, 12, 22, 11, 90};
        std::vector<int> expected = {11, 12, 22, 25, 34, 64, 90};
        
        auto result = sorter.quicksort(data);
        
        TestFramework::assert_true(result.is_sorted, "QuickSort: Should sort correctly");
        TestFramework::assert_equal_string("QuickSort", result.algorithm_name, "QuickSort: Should have correct name");
        TestFramework::assert_equal_size_t(size_t(7), result.array_size, "QuickSort: Should track array size");
        TestFramework::assert_true(result.comparisons > 0, "QuickSort: Should have comparisons");
        TestFramework::assert_true(result.swaps > 0, "QuickSort: Should have swaps");
        TestFramework::assert_true(result.time_microseconds >= 0, "QuickSort: Should track time");
        TestFramework::assert_true(result.is_in_place, "QuickSort: Should be in-place");
        TestFramework::assert_true(!result.is_stable, "QuickSort: Should not be stable");
        
        for (size_t i = 0; i < data.size(); ++i) {
            TestFramework::assert_equal_int(expected[i], data[i], "QuickSort: Should match expected result");
        }
    }
    
    // Test HeapSort
    {
        InternalSort<int> sorter;
        std::vector<int> data = {64, 34, 25, 12, 22, 11, 90};
        std::vector<int> expected = {11, 12, 22, 25, 34, 64, 90};
        
        auto result = sorter.heapsort(data);
        
        TestFramework::assert_true(result.is_sorted, "HeapSort: Should sort correctly");
        TestFramework::assert_equal_string("HeapSort", result.algorithm_name, "HeapSort: Should have correct name");
        TestFramework::assert_true(result.comparisons > 0, "HeapSort: Should have comparisons");
        TestFramework::assert_true(result.swaps > 0, "HeapSort: Should have swaps");
        TestFramework::assert_true(result.is_in_place, "HeapSort: Should be in-place");
        TestFramework::assert_true(!result.is_stable, "HeapSort: Should not be stable");
        
        for (size_t i = 0; i < data.size(); ++i) {
            TestFramework::assert_equal_int(expected[i], data[i], "HeapSort: Should match expected result");
        }
    }
    
    // Test ShellSort
    {
        InternalSort<int> sorter;
        std::vector<int> data = {64, 34, 25, 12, 22, 11, 90};
        std::vector<int> expected = {11, 12, 22, 25, 34, 64, 90};
        
        auto result = sorter.shellsort(data);
        
        TestFramework::assert_true(result.is_sorted, "ShellSort: Should sort correctly");
        TestFramework::assert_equal_string("ShellSort", result.algorithm_name, "ShellSort: Should have correct name");
        TestFramework::assert_true(result.comparisons > 0, "ShellSort: Should have comparisons");
        TestFramework::assert_true(result.swaps > 0, "ShellSort: Should have swaps");
        TestFramework::assert_true(result.is_in_place, "ShellSort: Should be in-place");
        TestFramework::assert_true(!result.is_stable, "ShellSort: Should not be stable");
        
        for (size_t i = 0; i < data.size(); ++i) {
            TestFramework::assert_equal_int(expected[i], data[i], "ShellSort: Should match expected result");
        }
    }
    
    // Test MergeSort
    {
        InternalSort<int> sorter;
        std::vector<int> data = {64, 34, 25, 12, 22, 11, 90};
        std::vector<int> expected = {11, 12, 22, 25, 34, 64, 90};
        
        auto result = sorter.mergesort(data);
        
        TestFramework::assert_true(result.is_sorted, "MergeSort: Should sort correctly");
        TestFramework::assert_equal_string("MergeSort", result.algorithm_name, "MergeSort: Should have correct name");
        TestFramework::assert_true(result.comparisons > 0, "MergeSort: Should have comparisons");
        TestFramework::assert_true(result.auxiliary_memory_bytes > 0, "MergeSort: Should use auxiliary memory");
        TestFramework::assert_true(!result.is_in_place, "MergeSort: Should not be in-place");
        TestFramework::assert_true(result.is_stable, "MergeSort: Should be stable");
        
        for (size_t i = 0; i < data.size(); ++i) {
            TestFramework::assert_equal_int(expected[i], data[i], "MergeSort: Should match expected result");
        }
    }
    
    // Test InsertionSort
    {
        InternalSort<int> sorter;
        std::vector<int> data = {64, 34, 25, 12, 22, 11, 90};
        std::vector<int> expected = {11, 12, 22, 25, 34, 64, 90};
        
        auto result = sorter.insertionsort(data);
        
        TestFramework::assert_true(result.is_sorted, "InsertionSort: Should sort correctly");
        TestFramework::assert_equal_string("InsertionSort", result.algorithm_name, "InsertionSort: Should have correct name");
        TestFramework::assert_true(result.comparisons > 0, "InsertionSort: Should have comparisons");
        TestFramework::assert_true(result.swaps > 0, "InsertionSort: Should have swaps");
        TestFramework::assert_true(result.is_in_place, "InsertionSort: Should be in-place");
        TestFramework::assert_true(result.is_stable, "InsertionSort: Should be stable");
        
        for (size_t i = 0; i < data.size(); ++i) {
            TestFramework::assert_equal_int(expected[i], data[i], "InsertionSort: Should match expected result");
        }
    }
    
    // Test edge cases
    {
        InternalSort<int> sorter;
        
        // Empty array
        std::vector<int> empty_data;
        auto empty_result = sorter.quicksort(empty_data);
        TestFramework::assert_true(empty_result.is_sorted, "Sorting: Should handle empty array");
        TestFramework::assert_equal_size_t(size_t(0), empty_result.comparisons, "Sorting: Empty array should have 0 comparisons");
        
        // Single element
        std::vector<int> single_data = {42};
        auto single_result = sorter.quicksort(single_data);
        TestFramework::assert_true(single_result.is_sorted, "Sorting: Should handle single element");
        TestFramework::assert_equal_int(42, single_data[0], "Sorting: Single element should remain unchanged");
        
        // Already sorted
        std::vector<int> sorted_data = {1, 2, 3, 4, 5};
        auto sorted_result = sorter.quicksort(sorted_data);
        TestFramework::assert_true(sorted_result.is_sorted, "Sorting: Should handle already sorted array");
        
        // Reverse sorted
        std::vector<int> reverse_data = {5, 4, 3, 2, 1};
        auto reverse_result = sorter.quicksort(reverse_data);
        TestFramework::assert_true(reverse_result.is_sorted, "Sorting: Should handle reverse sorted array");
        
        // All duplicates
        std::vector<int> duplicate_data = {5, 5, 5, 5, 5};
        auto duplicate_result = sorter.quicksort(duplicate_data);
        TestFramework::assert_true(duplicate_result.is_sorted, "Sorting: Should handle all duplicates");
    }
    
    // Test custom comparator
    {
        InternalSort<int> sorter;
        std::vector<int> data = {1, 2, 3, 4, 5};
        
        // Sort in descending order
        auto result = sorter.quicksort(data, std::greater<int>());
        
        TestFramework::assert_true(result.is_sorted, "Sorting: Should work with custom comparator");
        TestFramework::assert_equal_int(5, data[0], "Sorting: Should sort in descending order");
        TestFramework::assert_equal_int(1, data[4], "Sorting: Should sort in descending order");
    }
    
    // Test algorithm comparison
    {
        InternalSort<int> sorter;
        
        // Create test data
        auto test_data = InternalSort<int>::generate_random_data(100, 1, 1000);
        
        auto comparison = sorter.compare_algorithms(test_data);
        
        TestFramework::assert_true(comparison.results.size() >= 4, "Comparison: Should test multiple algorithms");
        TestFramework::assert_true(!comparison.fastest_algorithm.empty(), "Comparison: Should identify fastest algorithm");
        TestFramework::assert_true(!comparison.most_efficient_algorithm.empty(), "Comparison: Should identify most efficient algorithm");
        
        // Verify all results are sorted
        for (const auto& result : comparison.results) {
            TestFramework::assert_true(result.is_sorted, "Comparison: All algorithms should produce sorted results");
            TestFramework::assert_true(result.time_microseconds >= 0, "Comparison: Should track execution time");
            TestFramework::assert_true(result.get_efficiency_score() >= 0, "Comparison: Should calculate efficiency score");
        }
    }
    
    // Test data generators
    {
        // Random data
        auto random_data = InternalSort<int>::generate_random_data(50, 1, 100);
        TestFramework::assert_equal_size_t(size_t(50), random_data.size(), "Generator: Should create correct size random data");
        
        // Sorted data
        auto sorted_data = InternalSort<int>::generate_sorted_data(10, true);
        TestFramework::assert_equal_size_t(size_t(10), sorted_data.size(), "Generator: Should create correct size sorted data");
        TestFramework::assert_true(std::is_sorted(sorted_data.begin(), sorted_data.end()), "Generator: Should create sorted data");
        
        auto reverse_sorted = InternalSort<int>::generate_sorted_data(10, false);
        TestFramework::assert_true(std::is_sorted(reverse_sorted.begin(), reverse_sorted.end(), std::greater<int>()), 
                                   "Generator: Should create reverse sorted data");
        
        // Nearly sorted data
        auto nearly_sorted = InternalSort<int>::generate_nearly_sorted_data(20, 0.1);
        TestFramework::assert_equal_size_t(size_t(20), nearly_sorted.size(), "Generator: Should create correct size nearly sorted data");
    }
    
    // Test performance characteristics
    {
        InternalSort<int> sorter;
        
        // Small array - InsertionSort should be competitive
        auto small_data = InternalSort<int>::generate_random_data(20, 1, 100);
        auto small_comparison = sorter.compare_algorithms(small_data);
        
        TestFramework::assert_true(small_comparison.results.size() >= 4, "Performance: Should compare algorithms on small data");
        
        // Find InsertionSort result
        auto insertion_it = std::find_if(small_comparison.results.begin(), small_comparison.results.end(),
            [](const auto& result) { return result.algorithm_name == "InsertionSort"; });
        
        if (insertion_it != small_comparison.results.end()) {
            TestFramework::assert_true(insertion_it->time_microseconds >= 0, "Performance: InsertionSort should have reasonable time");
        }
        
        // Larger array - QuickSort/HeapSort should be better
        auto large_data = InternalSort<int>::generate_random_data(500, 1, 1000);
        auto large_comparison = sorter.compare_algorithms(large_data);
        
        TestFramework::assert_true(large_comparison.results.size() >= 4, "Performance: Should compare algorithms on large data");
        
        // Verify efficiency scores make sense
        for (const auto& result : large_comparison.results) {
            double efficiency = result.get_efficiency_score();
            TestFramework::assert_true(efficiency >= 1.0, "Performance: Efficiency score should be reasonable");
            TestFramework::assert_true(efficiency < result.array_size * 2, "Performance: Efficiency should not be too high");
        }
    }
    
    // Test stability property
    {
        InternalSort<std::pair<int, char>> sorter;
        
        // Create data where stability matters
        std::vector<std::pair<int, char>> data = {
            {3, 'a'}, {1, 'b'}, {3, 'c'}, {2, 'd'}, {1, 'e'}
        };
        
        auto stable_comparator = [](const std::pair<int, char>& a, const std::pair<int, char>& b) {
            return a.first < b.first;
        };
        
        // Test stable algorithm (MergeSort)
        auto stable_data = data;
        auto merge_result = sorter.mergesort(stable_data, stable_comparator);
        
        TestFramework::assert_true(merge_result.is_stable, "Stability: MergeSort should be stable");
        TestFramework::assert_true(merge_result.is_sorted, "Stability: Stable sort should still sort correctly");
        
        // Verify stability: elements with same key should maintain relative order
        // First 1 should be 'b', second 1 should be 'e'
        // First 3 should be 'a', second 3 should be 'c'
        bool found_first_1 = false, found_second_1 = false;
        bool found_first_3 = false, found_second_3 = false;
        
        for (const auto& item : stable_data) {
            if (item.first == 1 && !found_first_1) {
                TestFramework::assert_equal_int('b', item.second, "Stability: First '1' should be 'b'");
                found_first_1 = true;
            } else if (item.first == 1 && found_first_1 && !found_second_1) {
                TestFramework::assert_equal_int('e', item.second, "Stability: Second '1' should be 'e'");
                found_second_1 = true;
            }
            
            if (item.first == 3 && !found_first_3) {
                TestFramework::assert_equal_int('a', item.second, "Stability: First '3' should be 'a'");
                found_first_3 = true;
            } else if (item.first == 3 && found_first_3 && !found_second_3) {
                TestFramework::assert_equal_int('c', item.second, "Stability: Second '3' should be 'c'");
                found_second_3 = true;
            }
        }
    }
}