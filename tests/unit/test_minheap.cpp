#include "../../core/containers/MinHeap.hpp"
#include "../TestFramework.hpp"
#include <vector>
#include <random>
#include <algorithm>

void test_minheap() {
    using namespace rts::core::containers;
    
    // Test 1: Basic heap operations
    {
        MinHeap<int> heap;
        TestFramework::assert_true(heap.empty(), "New heap should be empty");
        TestFramework::assert_equal_size_t(size_t(0), heap.size(), "New heap size should be 0");
    }
    
    // Test 2: Insert and extract min
    {
        MinHeap<int> heap;
        auto id1 = heap.push(10);
        auto id2 = heap.push(5);
        auto id3 = heap.push(15);
        auto id4 = heap.push(3);
        
        TestFramework::assert_equal_size_t(size_t(4), heap.size(), "Heap size after 4 inserts");
        TestFramework::assert_equal_int(3, heap.top(), "Min element should be 3");
        
        TestFramework::assert_equal_int(3, heap.pop(), "First pop should return 3");
        TestFramework::assert_equal_int(5, heap.pop(), "Second pop should return 5");
        TestFramework::assert_equal_int(10, heap.pop(), "Third pop should return 10");
        TestFramework::assert_equal_int(15, heap.pop(), "Fourth pop should return 15");
        
        TestFramework::assert_true(heap.empty(), "Heap should be empty after all pops");
    }
    
    // Test 3: Decrease key operation
    {
        MinHeap<int> heap;
        auto id1 = heap.push(10);
        auto id2 = heap.push(20);
        auto id3 = heap.push(30);
        
        TestFramework::assert_equal_int(10, heap.top(), "Initial min should be 10");
        
        heap.decrease_key(id2, 5);  // Decrease 20 to 5
        TestFramework::assert_equal_int(5, heap.top(), "After decrease key, min should be 5");
        
        TestFramework::assert_equal_int(5, heap.pop(), "Pop should return decreased value");
        TestFramework::assert_equal_int(10, heap.top(), "Next min should be 10");
    }
    
    // Test 4: Contains and get operations
    {
        MinHeap<int> heap;
        auto id1 = heap.push(100);
        auto id2 = heap.push(200);
        
        TestFramework::assert_true(heap.contains(id1), "Should contain first element");
        TestFramework::assert_true(heap.contains(id2), "Should contain second element");
        TestFramework::assert_equal(100, heap.get(id1), "Get should return correct value for id1");
        TestFramework::assert_equal(200, heap.get(id2), "Get should return correct value for id2");
        
        heap.pop();  // Remove min (100)
        TestFramework::assert_true(!heap.contains(id1), "Should not contain removed element");
        TestFramework::assert_true(heap.contains(id2), "Should still contain second element");
    }
    
    // Test 5: Heap property validation
    {
        MinHeap<int> heap;
        std::vector<int> values = {50, 30, 70, 20, 40, 60, 80, 10};
        
        for (int val : values) {
            heap.push(val);
        }
        
        TestFramework::assert_true(heap.validate_heap_property(), "Heap property should be maintained");
        
        // Extract all elements - should come out in sorted order
        std::vector<int> extracted;
        while (!heap.empty()) {
            extracted.push_back(heap.pop());
        }
        
        std::vector<int> expected = {10, 20, 30, 40, 50, 60, 70, 80};
        TestFramework::assert_true(extracted == expected, "Elements should be extracted in sorted order");
    }
    
    // Test 6: Stress test with random data
    {
        MinHeap<int> heap;
        std::vector<std::pair<int, size_t>> values;  // value, id
        
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(1, 1000);
        
        // Insert 100 random values
        for (int i = 0; i < 100; ++i) {
            int val = dis(gen);
            auto id = heap.push(val);
            values.emplace_back(val, id);
        }
        
        TestFramework::assert_true(heap.validate_heap_property(), "Heap property after random inserts");
        
        // Perform some decrease key operations
        for (int i = 0; i < 20; ++i) {
            int idx = dis(gen) % values.size();
            if (heap.contains(values[idx].second)) {
                int new_val = values[idx].first / 2;  // Decrease by half
                try {
                    heap.decrease_key(values[idx].second, new_val);
                    values[idx].first = new_val;
                } catch (const std::exception&) {
                    // decrease_key failed because new value wasn't smaller
                }
            }
        }
        
        TestFramework::assert_true(heap.validate_heap_property(), "Heap property after decrease keys");
        
        // Extract all and verify they come out in sorted order
        std::vector<int> extracted;
        while (!heap.empty()) {
            extracted.push_back(heap.pop());
        }
        
        TestFramework::assert_true(std::is_sorted(extracted.begin(), extracted.end()), 
                                  "Random stress test should maintain sorted order");
    }
    
    // Test 7: Custom comparator
    {
        // Max heap using greater comparator
        MinHeap<int, std::greater<int>> max_heap;
        
        max_heap.push(10);
        max_heap.push(20);
        max_heap.push(5);
        max_heap.push(15);
        
        TestFramework::assert_equal(20, max_heap.top(), "Max heap should have largest element on top");
        TestFramework::assert_equal(20, max_heap.pop(), "Should extract largest first");
        TestFramework::assert_equal(15, max_heap.pop(), "Should extract second largest");
    }
    
    // Test 8: Statistics
    {
        MinHeap<int> heap;
        
        // Insert elements to create a specific tree structure
        for (int i = 1; i <= 15; ++i) {
            heap.push(i);
        }
        
        auto stats = heap.get_statistics();
        TestFramework::assert_equal(size_t(15), stats.total_elements, "Statistics should show correct element count");
        TestFramework::assert_true(stats.max_depth <= 4, "Max depth should be reasonable for 15 elements"); // log2(15) ≈ 4
        TestFramework::assert_true(stats.memory_usage_bytes > 0, "Should report non-zero memory usage");
    }
    
    // Test 9: Error conditions
    {
        MinHeap<int> heap;
        
        try {
            heap.pop();  // Should throw on empty heap
            TestFramework::assert_true(false, "Should throw exception when popping empty heap");
        } catch (const std::runtime_error&) {
            TestFramework::assert_true(true, "Correctly throws exception on empty pop");
        }
        
        try {
            heap.top();  // Should throw on empty heap
            TestFramework::assert_true(false, "Should throw exception when accessing top of empty heap");
        } catch (const std::runtime_error&) {
            TestFramework::assert_true(true, "Correctly throws exception on empty top");
        }
        
        auto id = heap.push(10);
        try {
            heap.decrease_key(id, 20);  // Should throw - new value is larger
            TestFramework::assert_true(false, "Should throw when new value is not smaller");
        } catch (const std::runtime_error&) {
            TestFramework::assert_true(true, "Correctly throws when decrease_key value is not smaller");
        }
    }
}