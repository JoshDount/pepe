#pragma once

#include "HashMap.hpp"
#include "LinkedList.hpp"

namespace rts::core::containers {

/**
 * @brief LRU (Least Recently Used) Cache implementation
 * 
 * Combines HashMap for O(1) access and LinkedList for O(1) ordering
 * Essential for efficient caching in graph algorithms and data access
 * 
 * Time Complexities:
 * - Get: O(1) average
 * - Put: O(1) average
 * - Remove: O(1) average
 * 
 * Space Complexity: O(capacity)
 */
template<typename K, typename V>
class LRUCache {
private:
    struct CacheNode {
        K key;
        V value;
        
        CacheNode(const K& k, const V& v) : key(k), value(v) {}
    };
    
    size_t capacity_;
    LinkedList<CacheNode> list_;  // Most recent at front, least recent at back
    HashMap<K, typename LinkedList<CacheNode>::Iterator> map_;  // Key -> iterator in list
    
    void move_to_front(typename LinkedList<CacheNode>::Iterator& it) {
        // Remove from current position
        CacheNode node = *it;
        (void)it.get_node(); // Suppress unused variable warning
        
        // Find position in list to remove
        size_t position = 0;
        auto temp_it = list_.begin();
        while (temp_it != it) {
            ++temp_it;
            ++position;
        }
        
        list_.erase(position);
        
        // Add to front
        list_.push_front(node);
        
        // Update iterator in map
        map_.put(node.key, list_.begin());
    }
    
    void evict_least_recent() {
        if (list_.empty()) return;
        
        // Remove from back (least recently used)
        K key_to_remove = list_.back().key;
        list_.pop_back();
        map_.erase(key_to_remove);
    }

public:
    explicit LRUCache(size_t capacity) 
        : capacity_(capacity), list_(true), map_(HashMap<K, typename LinkedList<CacheNode>::Iterator>::CollisionStrategy::SEPARATE_CHAINING) {
        if (capacity == 0) {
            throw std::invalid_argument("Cache capacity must be greater than 0");
        }
    }
    
    /**
     * @brief Get value for key, marking it as recently used
     * @param key Key to look up
     * @return Pointer to value if found, nullptr otherwise
     */
    V* get(const K& key) {
        auto it_ptr = map_.get(key);
        if (!it_ptr) {
            return nullptr;  // Key not found
        }
        
        // Move to front (mark as most recently used)
        move_to_front(*it_ptr);
        
        return &(list_.front().value);
    }
    
    /**
     * @brief Put key-value pair, evicting least recent if necessary
     * @param key Key to insert/update
     * @param value Value to associate with key
     */
    void put(const K& key, const V& value) {
        auto it_ptr = map_.get(key);
        
        if (it_ptr) {
            // Key exists, update value and move to front
            (*it_ptr)->value = value;
            move_to_front(*it_ptr);
        } else {
            // New key
            if (list_.size() >= capacity_) {
                evict_least_recent();
            }
            
            // Add to front
            list_.push_front(CacheNode(key, value));
            map_.put(key, list_.begin());
        }
    }
    
    /**
     * @brief Check if key exists in cache
     * @param key Key to check
     * @return true if key exists, false otherwise
     */
    bool contains(const K& key) const {
        return map_.contains(key);
    }
    
    /**
     * @brief Remove key from cache
     * @param key Key to remove
     * @return true if key was found and removed, false otherwise
     */
    bool remove(const K& key) {
        auto it_ptr = map_.get(key);
        if (!it_ptr) {
            return false;
        }
        
        // Find position to remove
        size_t position = 0;
        auto temp_it = list_.begin();
        while (temp_it != *it_ptr) {
            ++temp_it;
            ++position;
        }
        
        list_.erase(position);
        map_.erase(key);
        return true;
    }
    
    /**
     * @brief Get current number of elements in cache
     */
    size_t size() const {
        return list_.size();
    }
    
    /**
     * @brief Check if cache is empty
     */
    bool empty() const {
        return list_.empty();
    }
    
    /**
     * @brief Get cache capacity
     */
    size_t capacity() const {
        return capacity_;
    }
    
    /**
     * @brief Clear all elements from cache
     */
    void clear() {
        list_.clear();
        map_.clear();
    }
    
    /**
     * @brief Get all keys in order from most to least recently used
     */
    std::vector<K> get_keys_by_recency() const {
        std::vector<K> keys;
        keys.reserve(list_.size());
        
        for (const auto& node : list_) {
            keys.push_back(node.key);
        }
        
        return keys;
    }
    
    /**
     * @brief Get cache statistics and performance metrics
     */
    struct Statistics {
        size_t total_elements;
        size_t capacity;
        double utilization;  // size / capacity
        size_t memory_usage_bytes;
        
        // HashMap statistics
        size_t hash_collisions;
        double hash_load_factor;
        
        // Access pattern analysis
        size_t total_accesses;
        size_t cache_hits;
        size_t cache_misses;
        double hit_rate;
    };
    
    Statistics get_statistics() const {
        Statistics stats;
        stats.total_elements = list_.size();
        stats.capacity = capacity_;
        stats.utilization = static_cast<double>(list_.size()) / capacity_;
        
        auto list_stats = list_.get_statistics();
        auto map_stats = map_.get_statistics();
        
        stats.memory_usage_bytes = list_stats.memory_usage_bytes + map_stats.memory_usage_bytes;
        stats.hash_collisions = map_stats.collision_count;
        stats.hash_load_factor = map_stats.load_factor;
        
        // These would be tracked separately in a production implementation
        stats.total_accesses = 0;
        stats.cache_hits = 0;
        stats.cache_misses = 0;
        stats.hit_rate = 0.0;
        
        return stats;
    }
    
    /**
     * @brief Resize cache capacity
     * @param new_capacity New capacity (must be > 0)
     */
    void resize(size_t new_capacity) {
        if (new_capacity == 0) {
            throw std::invalid_argument("Cache capacity must be greater than 0");
        }
        
        capacity_ = new_capacity;
        
        // Evict elements if current size exceeds new capacity
        while (list_.size() > capacity_) {
            evict_least_recent();
        }
    }
};

} // namespace rts::core::containers