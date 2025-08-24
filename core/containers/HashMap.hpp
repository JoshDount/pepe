#pragma once

#include <vector>
#include <list>
#include <functional>
#include <stdexcept>
#include <memory>

namespace rts::core::containers {

/**
 * @brief Hash table implementation with configurable collision resolution
 * 
 * Supports both open addressing (linear probing) and separate chaining.
 * Demonstrates different collision resolution strategies and their trade-offs.
 * 
 * Time Complexities (average case):
 * - Insert: O(1)
 * - Search: O(1)
 * - Delete: O(1)
 * 
 * Worst case: O(n) for all operations
 * Space Complexity: O(n)
 */
template<typename K, typename V, typename Hash = std::hash<K>>
class HashMap {
public:
    enum class CollisionStrategy {
        LINEAR_PROBING,     // Open addressing with linear probing
        SEPARATE_CHAINING   // Chaining with linked lists
    };

private:
    struct Entry {
        K key;
        V value;
        bool is_deleted;  // For open addressing tombstone
        
        Entry() : is_deleted(true) {}
        Entry(const K& k, const V& v) : key(k), value(v), is_deleted(false) {}
    };
    
    // For separate chaining
    using ChainNode = std::pair<K, V>;
    using Chain = std::list<ChainNode>;
    
    CollisionStrategy strategy_;
    Hash hasher_;
    
    // Open addressing storage
    std::vector<Entry> table_;
    
    // Separate chaining storage
    std::vector<Chain> chains_;
    
    size_t size_;
    size_t capacity_;
    double max_load_factor_;
    
    // Statistics for analysis
    mutable size_t collision_count_;
    mutable size_t probe_count_;
    
    size_t hash(const K& key) const {
        return hasher_(key) % capacity_;
    }
    
    void resize_and_rehash() {
        size_t old_capacity = capacity_;
        capacity_ = capacity_ * 2;
        
        if (strategy_ == CollisionStrategy::LINEAR_PROBING) {
            std::vector<Entry> old_table = std::move(table_);
            table_ = std::vector<Entry>(capacity_);
            size_ = 0;
            
            for (const auto& entry : old_table) {
                if (!entry.is_deleted) {
                    insert_open_addressing(entry.key, entry.value);
                }
            }
        } else {
            std::vector<Chain> old_chains = std::move(chains_);
            chains_ = std::vector<Chain>(capacity_);
            size_ = 0;
            
            for (const auto& chain : old_chains) {
                for (const auto& node : chain) {
                    insert_chaining(node.first, node.second);
                }
            }
        }
    }
    
    // Open addressing implementation
    bool insert_open_addressing(const K& key, const V& value) {
        if (load_factor() >= max_load_factor_) {
            resize_and_rehash();
        }
        
        size_t index = hash(key);
        size_t original_index = index;
        size_t probes = 0;
        
        do {
            if (table_[index].is_deleted) {
                table_[index] = Entry(key, value);
                size_++;
                probe_count_ += probes;
                return true;
            }
            
            if (table_[index].key == key) {
                table_[index].value = value; // Update existing
                return false;
            }
            
            index = (index + 1) % capacity_;
            probes++;
            if (probes > 0) collision_count_++;
            
        } while (index != original_index);
        
        throw std::runtime_error("Hash table is full");
    }
    
    std::pair<bool, V*> find_open_addressing(const K& key) const {
        size_t index = hash(key);
        size_t original_index = index;
        size_t probes = 0;
        
        do {
            if (table_[index].is_deleted && probes == 0) {
                return {false, nullptr}; // Empty slot, key not found
            }
            
            if (!table_[index].is_deleted && table_[index].key == key) {
                probe_count_ += probes;
                return {true, const_cast<V*>(&table_[index].value)};
            }
            
            index = (index + 1) % capacity_;
            probes++;
            
        } while (index != original_index);
        
        return {false, nullptr};
    }
    
    bool remove_open_addressing(const K& key) {
        size_t index = hash(key);
        size_t original_index = index;
        
        do {
            if (table_[index].is_deleted) {
                return false; // Empty slot, key not found
            }
            
            if (table_[index].key == key) {
                table_[index].is_deleted = true;
                size_--;
                return true;
            }
            
            index = (index + 1) % capacity_;
            
        } while (index != original_index);
        
        return false;
    }
    
    // Separate chaining implementation
    bool insert_chaining(const K& key, const V& value) {
        if (load_factor() >= max_load_factor_) {
            resize_and_rehash();
        }
        
        size_t index = hash(key);
        Chain& chain = chains_[index];
        
        // Check if key already exists
        for (auto& node : chain) {
            if (node.first == key) {
                node.second = value; // Update existing
                return false;
            }
        }
        
        // Add new key-value pair
        if (!chain.empty()) {
            collision_count_++;
        }
        chain.emplace_back(key, value);
        size_++;
        return true;
    }
    
    std::pair<bool, V*> find_chaining(const K& key) const {
        size_t index = hash(key);
        const Chain& chain = chains_[index];
        
        for (const auto& node : chain) {
            if (node.first == key) {
                return {true, const_cast<V*>(&node.second)};
            }
        }
        
        return {false, nullptr};
    }
    
    bool remove_chaining(const K& key) {
        size_t index = hash(key);
        Chain& chain = chains_[index];
        
        for (auto it = chain.begin(); it != chain.end(); ++it) {
            if (it->first == key) {
                chain.erase(it);
                size_--;
                return true;
            }
        }
        
        return false;
    }

public:
    explicit HashMap(CollisionStrategy strategy = CollisionStrategy::LINEAR_PROBING,
                    size_t initial_capacity = 16,
                    double max_load_factor = 0.75)
        : strategy_(strategy), capacity_(initial_capacity), size_(0),
          max_load_factor_(max_load_factor), collision_count_(0), probe_count_(0) {
        
        if (strategy_ == CollisionStrategy::LINEAR_PROBING) {
            table_.resize(capacity_);
        } else {
            chains_.resize(capacity_);
        }
    }
    
    /**
     * @brief Insert or update a key-value pair
     * @param key Key to insert
     * @param value Value to associate with key
     * @return true if new key was inserted, false if existing key was updated
     */
    bool put(const K& key, const V& value) {
        if (strategy_ == CollisionStrategy::LINEAR_PROBING) {
            return insert_open_addressing(key, value);
        } else {
            return insert_chaining(key, value);
        }
    }
    
    /**
     * @brief Get value associated with key
     * @param key Key to search for
     * @return Pointer to value if found, nullptr otherwise
     */
    V* get(const K& key) const {
        if (strategy_ == CollisionStrategy::LINEAR_PROBING) {
            return find_open_addressing(key).second;
        } else {
            return find_chaining(key).second;
        }
    }
    
    /**
     * @brief Check if key exists in the hash table
     * @param key Key to search for
     * @return true if key exists, false otherwise
     */
    bool contains(const K& key) const {
        return get(key) != nullptr;
    }
    
    /**
     * @brief Remove a key-value pair
     * @param key Key to remove
     * @return true if key was found and removed, false otherwise
     */
    bool erase(const K& key) {
        if (strategy_ == CollisionStrategy::LINEAR_PROBING) {
            return remove_open_addressing(key);
        } else {
            return remove_chaining(key);
        }
    }
    
    /**
     * @brief Get the number of key-value pairs
     * @return Number of elements
     */
    size_t size() const {
        return size_;
    }
    
    /**
     * @brief Check if hash table is empty
     * @return true if empty, false otherwise
     */
    bool empty() const {
        return size_ == 0;
    }
    
    /**
     * @brief Get current load factor
     * @return Load factor (size / capacity)
     */
    double load_factor() const {
        return static_cast<double>(size_) / capacity_;
    }
    
    /**
     * @brief Clear all key-value pairs
     */
    void clear() {
        size_ = 0;
        collision_count_ = 0;
        probe_count_ = 0;
        
        if (strategy_ == CollisionStrategy::LINEAR_PROBING) {
            table_.clear();
            table_.resize(capacity_);
        } else {
            for (auto& chain : chains_) {
                chain.clear();
            }
        }
    }
    
    /**
     * @brief Get hash table statistics for analysis
     */
    struct Statistics {
        size_t total_elements;
        size_t capacity;
        double load_factor;
        size_t collision_count;
        double avg_probe_length;
        size_t max_chain_length;  // For chaining only
        size_t empty_slots;
        size_t memory_usage_bytes;
        CollisionStrategy strategy;
    };
    
    Statistics get_statistics() const {
        Statistics stats;
        stats.total_elements = size_;
        stats.capacity = capacity_;
        stats.load_factor = load_factor();
        stats.collision_count = collision_count_;
        stats.strategy = strategy_;
        
        if (strategy_ == CollisionStrategy::LINEAR_PROBING) {
            stats.avg_probe_length = size_ > 0 ? static_cast<double>(probe_count_) / size_ : 0.0;
            stats.max_chain_length = 0;
            
            size_t empty_count = 0;
            for (const auto& entry : table_) {
                if (entry.is_deleted) empty_count++;
            }
            stats.empty_slots = empty_count;
            stats.memory_usage_bytes = sizeof(Entry) * capacity_;
            
        } else {
            stats.avg_probe_length = 0.0; // Not applicable for chaining
            stats.max_chain_length = 0;
            stats.empty_slots = 0;
            
            for (const auto& chain : chains_) {
                stats.max_chain_length = std::max(stats.max_chain_length, chain.size());
                if (chain.empty()) stats.empty_slots++;
            }
            
            stats.memory_usage_bytes = sizeof(Chain) * capacity_ + 
                                     sizeof(ChainNode) * size_;
        }
        
        return stats;
    }
    
    /**
     * @brief Operator[] for convenient access (creates entry if not exists)
     */
    V& operator[](const K& key) {
        V* value_ptr = get(key);
        if (value_ptr) {
            return *value_ptr;
        }
        
        put(key, V{});
        return *get(key);
    }
    
    /**
     * @brief Get all keys (for iteration)
     */
    std::vector<K> keys() const {
        std::vector<K> result;
        result.reserve(size_);
        
        if (strategy_ == CollisionStrategy::LINEAR_PROBING) {
            for (const auto& entry : table_) {
                if (!entry.is_deleted) {
                    result.push_back(entry.key);
                }
            }
        } else {
            for (const auto& chain : chains_) {
                for (const auto& node : chain) {
                    result.push_back(node.first);
                }
            }
        }
        
        return result;
    }
};

} // namespace rts::core::containers