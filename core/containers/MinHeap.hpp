#pragma once

#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <functional>

namespace rts::core::containers {

/**
 * @brief Binary Min-Heap implementation with decrease-key support
 * 
 * This implementation is specifically designed for graph algorithms like
 * Dijkstra and A* that require efficient decrease-key operations.
 * 
 * Time Complexities:
 * - Insert: O(log n)
 * - Extract Min: O(log n)
 * - Decrease Key: O(log n)
 * - Peek Min: O(1)
 * 
 * Space Complexity: O(n)
 */
template<typename T, typename Compare = std::less<T>>
class MinHeap {
private:
    struct HeapNode {
        T data;
        size_t id;  // Unique identifier for decrease-key operations
        
        HeapNode(const T& data, size_t id) : data(data), id(id) {}
    };
    
    std::vector<HeapNode> heap_;
    std::unordered_map<size_t, size_t> id_to_index_; // Maps element ID to heap index
    Compare comp_;
    size_t next_id_;
    
    // Helper methods
    size_t parent(size_t i) const { return (i - 1) / 2; }
    size_t left_child(size_t i) const { return 2 * i + 1; }
    size_t right_child(size_t i) const { return 2 * i + 2; }
    
    void swap_nodes(size_t i, size_t j) {
        // Update the ID to index mapping
        id_to_index_[heap_[i].id] = j;
        id_to_index_[heap_[j].id] = i;
        
        // Swap the actual nodes
        std::swap(heap_[i], heap_[j]);
    }
    
    void heapify_up(size_t index) {
        while (index > 0) {
            size_t parent_idx = parent(index);
            if (!comp_(heap_[index].data, heap_[parent_idx].data)) {
                break;
            }
            swap_nodes(index, parent_idx);
            index = parent_idx;
        }
    }
    
    void heapify_down(size_t index) {
        while (true) {
            size_t smallest = index;
            size_t left = left_child(index);
            size_t right = right_child(index);
            
            if (left < heap_.size() && comp_(heap_[left].data, heap_[smallest].data)) {
                smallest = left;
            }
            
            if (right < heap_.size() && comp_(heap_[right].data, heap_[smallest].data)) {
                smallest = right;
            }
            
            if (smallest == index) {
                break;
            }
            
            swap_nodes(index, smallest);
            index = smallest;
        }
    }

public:
    MinHeap() : next_id_(0) {}
    
    explicit MinHeap(Compare comp) : comp_(comp), next_id_(0) {}
    
    /**
     * @brief Insert an element into the heap
     * @param data Element to insert
     * @return Unique ID for the inserted element (used for decrease-key)
     */
    size_t push(const T& data) {
        size_t id = next_id_++;
        heap_.emplace_back(data, id);
        size_t index = heap_.size() - 1;
        
        id_to_index_[id] = index;
        heapify_up(index);
        
        return id;
    }
    
    /**
     * @brief Extract the minimum element
     * @return The minimum element
     * @throws std::runtime_error if heap is empty
     */
    T pop() {
        if (empty()) {
            throw std::runtime_error("Cannot pop from empty heap");
        }
        
        T min_data = heap_[0].data;
        size_t min_id = heap_[0].id;
        
        // Move last element to root
        heap_[0] = heap_.back();
        heap_.pop_back();
        
        // Update mapping
        id_to_index_.erase(min_id);
        if (!empty()) {
            id_to_index_[heap_[0].id] = 0;
            heapify_down(0);
        }
        
        return min_data;
    }
    
    /**
     * @brief Get the minimum element without removing it
     * @return Reference to the minimum element
     * @throws std::runtime_error if heap is empty
     */
    const T& top() const {
        if (empty()) {
            throw std::runtime_error("Cannot access top of empty heap");
        }
        return heap_[0].data;
    }
    
    /**
     * @brief Decrease the key of an element
     * @param id Unique ID of the element (returned by push)
     * @param new_data New value (must be smaller than current)
     * @throws std::runtime_error if ID not found or new value is larger
     */
    void decrease_key(size_t id, const T& new_data) {
        auto it = id_to_index_.find(id);
        if (it == id_to_index_.end()) {
            throw std::runtime_error("Element ID not found in heap");
        }
        
        size_t index = it->second;
        if (!comp_(new_data, heap_[index].data)) {
            throw std::runtime_error("New value is not smaller than current value");
        }
        
        heap_[index].data = new_data;
        heapify_up(index);
    }
    
    /**
     * @brief Check if an element exists in the heap
     * @param id Unique ID of the element
     * @return true if element exists, false otherwise
     */
    bool contains(size_t id) const {
        return id_to_index_.find(id) != id_to_index_.end();
    }
    
    /**
     * @brief Get the current value of an element
     * @param id Unique ID of the element
     * @return Current value of the element
     * @throws std::runtime_error if ID not found
     */
    const T& get(size_t id) const {
        auto it = id_to_index_.find(id);
        if (it == id_to_index_.end()) {
            throw std::runtime_error("Element ID not found in heap");
        }
        return heap_[it->second].data;
    }
    
    /**
     * @brief Check if heap is empty
     * @return true if empty, false otherwise
     */
    bool empty() const {
        return heap_.empty();
    }
    
    /**
     * @brief Get the number of elements in the heap
     * @return Number of elements
     */
    size_t size() const {
        return heap_.size();
    }
    
    /**
     * @brief Clear all elements from the heap
     */
    void clear() {
        heap_.clear();
        id_to_index_.clear();
        next_id_ = 0;
    }
    
    /**
     * @brief Validate heap property (for debugging)
     * @return true if heap property is maintained
     */
    bool validate_heap_property() const {
        for (size_t i = 0; i < heap_.size(); ++i) {
            size_t left = left_child(i);
            size_t right = right_child(i);
            
            if (left < heap_.size() && comp_(heap_[left].data, heap_[i].data)) {
                return false;
            }
            
            if (right < heap_.size() && comp_(heap_[right].data, heap_[i].data)) {
                return false;
            }
        }
        return true;
    }
    
    /**
     * @brief Get heap statistics for analysis
     */
    struct Statistics {
        size_t total_elements;
        size_t max_depth;
        double avg_depth;
        size_t memory_usage_bytes;
    };
    
    Statistics get_statistics() const {
        Statistics stats;
        stats.total_elements = heap_.size();
        stats.memory_usage_bytes = sizeof(HeapNode) * heap_.capacity() + 
                                 sizeof(std::pair<size_t, size_t>) * id_to_index_.size();
        
        if (heap_.empty()) {
            stats.max_depth = 0;
            stats.avg_depth = 0.0;
            return stats;
        }
        
        // Calculate depth statistics
        size_t total_depth = 0;
        stats.max_depth = 0;
        
        for (size_t i = 0; i < heap_.size(); ++i) {
            size_t depth = 0;
            size_t node = i;
            while (node > 0) {
                node = parent(node);
                depth++;
            }
            total_depth += depth;
            stats.max_depth = std::max(stats.max_depth, depth);
        }
        
        stats.avg_depth = static_cast<double>(total_depth) / heap_.size();
        return stats;
    }
};

} // namespace rts::core::containers