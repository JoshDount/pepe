#include "../../core/containers/LinkedList.hpp"
#include "../TestFramework.hpp"

void test_linkedlist() {
    using namespace rts::core::containers;
    
    // Test 1: Basic single-linked list operations
    {
        LinkedList<int> list(false);  // Single-linked
        TestFramework::assert_true(list.empty(), "New list should be empty");
        TestFramework::assert_equal(size_t(0), list.size(), "New list size should be 0");
        
        list.push_back(1);
        list.push_back(2);
        list.push_front(0);
        
        TestFramework::assert_equal(size_t(3), list.size(), "List should have 3 elements");
        TestFramework::assert_equal(0, list.front(), "Front should be 0");
        TestFramework::assert_equal(2, list.back(), "Back should be 2");
    }
    
    // Test 2: Double-linked list operations
    {
        LinkedList<int> list(true);  // Double-linked
        
        for (int i = 0; i < 5; ++i) {
            list.push_back(i);
        }
        
        TestFramework::assert_equal(size_t(5), list.size(), "Double-linked list should have 5 elements");
        
        // Test reverse iteration
        bool reverse_works = true;
        try {
            auto rit = list.rbegin();
            int expected = 4;
            while (rit != list.rend()) {
                if (*rit != expected--) {
                    reverse_works = false;
                    break;
                }
                ++rit;
            }
        } catch (...) {
            reverse_works = false;
        }
        
        TestFramework::assert_true(reverse_works, "Reverse iteration should work for double-linked list");
    }
    
    // Test 3: Iterator operations
    {
        LinkedList<int> list;
        for (int i = 0; i < 5; ++i) {
            list.push_back(i);
        }
        
        int count = 0;
        for (auto it = list.begin(); it != list.end(); ++it) {
            TestFramework::assert_equal(count, *it, "Iterator should access correct elements");
            count++;
        }
        TestFramework::assert_equal(5, count, "Iterator should traverse all elements");
    }
}