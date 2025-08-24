#pragma once

#include "LinkedList.hpp"

namespace rts::core::containers {

/**
 * @brief Queue implementation using LinkedList
 * 
 * FIFO (First-In-First-Out) data structure
 * Essential for BFS algorithm and event processing
 * 
 * Time Complexities:
 * - Enqueue: O(1)
 * - Dequeue: O(1)
 * - Front: O(1)
 * - Empty: O(1)
 * 
 * Space Complexity: O(n)
 */
template<typename T>
class Queue {
private:
    LinkedList<T> list_;

public:
    Queue() : list_(true) {}  // Use double-linked for efficiency
    
    /**
     * @brief Add element to the back of the queue
     */
    void enqueue(const T& value) {
        list_.push_back(value);
    }
    
    /**
     * @brief Add element to the back of the queue (alias for enqueue)
     */
    void push(const T& value) {
        enqueue(value);
    }
    
    /**
     * @brief Remove and return element from the front of the queue
     */
    T dequeue() {
        if (empty()) {
            throw std::runtime_error("Cannot dequeue from empty queue");
        }
        T value = list_.front();
        list_.pop_front();
        return value;
    }
    
    /**
     * @brief Remove element from the front of the queue (alias for dequeue)
     */
    void pop() {
        if (empty()) {
            throw std::runtime_error("Cannot pop from empty queue");
        }
        list_.pop_front();
    }
    
    /**
     * @brief Get the front element without removing it
     */
    const T& front() const {
        return list_.front();
    }
    
    /**
     * @brief Get the back element without removing it
     */
    const T& back() const {
        return list_.back();
    }
    
    /**
     * @brief Check if queue is empty
     */
    bool empty() const {
        return list_.empty();
    }
    
    /**
     * @brief Get number of elements in queue
     */
    size_t size() const {
        return list_.size();
    }
    
    /**
     * @brief Clear all elements from queue
     */
    void clear() {
        list_.clear();
    }
    
    /**
     * @brief Get queue statistics
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