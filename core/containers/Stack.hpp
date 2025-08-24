#pragma once

#include "LinkedList.hpp"

namespace rts::core::containers {

/**
 * @brief Stack implementation using LinkedList
 * 
 * LIFO (Last-In-First-Out) data structure
 * Essential for DFS algorithm, backtracking, and expression parsing
 * 
 * Time Complexities:
 * - Push: O(1)
 * - Pop: O(1)
 * - Top: O(1)
 * - Empty: O(1)
 * 
 * Space Complexity: O(n)
 */
template<typename T>
class Stack {
private:
    LinkedList<T> list_;

public:
    Stack() : list_(false) {}  // Single-linked is sufficient for stack
    
    /**
     * @brief Add element to the top of the stack
     */
    void push(const T& value) {
        list_.push_front(value);
    }
    
    /**
     * @brief Remove and return element from the top of the stack
     */
    T pop() {
        if (empty()) {
            throw std::runtime_error("Cannot pop from empty stack");
        }
        T value = list_.front();
        list_.pop_front();
        return value;
    }
    
    /**
     * @brief Remove element from the top of the stack (void version)
     */
    void pop_void() {
        if (empty()) {
            throw std::runtime_error("Cannot pop from empty stack");
        }
        list_.pop_front();
    }
    
    /**
     * @brief Get the top element without removing it
     */
    const T& top() const {
        return list_.front();
    }
    
    /**
     * @brief Get reference to top element for modification
     */
    T& top() {
        return list_.front();
    }
    
    /**
     * @brief Check if stack is empty
     */
    bool empty() const {
        return list_.empty();
    }
    
    /**
     * @brief Get number of elements in stack
     */
    size_t size() const {
        return list_.size();
    }
    
    /**
     * @brief Clear all elements from stack
     */
    void clear() {
        list_.clear();
    }
    
    /**
     * @brief Get stack statistics
     */
    struct Statistics {
        size_t total_elements;
        size_t memory_usage_bytes;
    };
    
    Statistics get_statistics() const {
        auto list_stats = list_.get_statistics();
        Statistics stats;
        stats.total_elements = list_stats.total_elements;
        stats.memory_usage_bytes = list_stats.memory_usage_bytes;
        return stats;
    }
};

} // namespace rts::core::containers