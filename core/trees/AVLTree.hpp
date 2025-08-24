#pragma once

#include <algorithm>
#include <vector>
#include <functional>
#include <stdexcept>

namespace rts::core::trees {

/**
 * @brief AVL Tree implementation - Self-balancing Binary Search Tree
 * 
 * Maintains balance through rotations, guaranteeing O(log n) operations.
 * Essential for ordered indexing, range queries, and sorted data access.
 * 
 * Time Complexities:
 * - Insert: O(log n)
 * - Delete: O(log n)
 * - Search: O(log n)
 * - Range Query: O(log n + k) where k is number of results
 * 
 * Space Complexity: O(n)
 */
template<typename K, typename V, typename Compare = std::less<K>>
class AVLTree {
private:
    struct Node {
        K key;
        V value;
        Node* left;
        Node* right;
        int height;
        size_t subtree_size;  // For order statistics
        
        Node(const K& k, const V& v) 
            : key(k), value(v), left(nullptr), right(nullptr), height(1), subtree_size(1) {}
    };
    
    Node* root_;
    Compare comp_;
    size_t size_;
    
    // Helper methods
    int get_height(Node* node) const {
        return node ? node->height : 0;
    }
    
    size_t get_subtree_size(Node* node) const {
        return node ? node->subtree_size : 0;
    }
    
    int get_balance_factor(Node* node) const {
        return node ? get_height(node->left) - get_height(node->right) : 0;
    }
    
    void update_node_info(Node* node) {
        if (node) {
            node->height = 1 + std::max(get_height(node->left), get_height(node->right));
            node->subtree_size = 1 + get_subtree_size(node->left) + get_subtree_size(node->right);
        }
    }
    
    Node* rotate_right(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        
        // Perform rotation
        x->right = y;
        y->left = T2;
        
        // Update heights and sizes
        update_node_info(y);
        update_node_info(x);
        
        return x;  // New root
    }
    
    Node* rotate_left(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        
        // Perform rotation
        y->left = x;
        x->right = T2;
        
        // Update heights and sizes
        update_node_info(x);
        update_node_info(y);
        
        return y;  // New root
    }
    
    Node* insert_recursive(Node* node, const K& key, const V& value, bool& inserted) {
        // 1. Perform normal BST insertion
        if (!node) {
            inserted = true;
            size_++;
            return new Node(key, value);
        }
        
        if (comp_(key, node->key)) {
            node->left = insert_recursive(node->left, key, value, inserted);
        } else if (comp_(node->key, key)) {
            node->right = insert_recursive(node->right, key, value, inserted);
        } else {
            // Key already exists, update value
            node->value = value;
            inserted = false;
            return node;
        }
        
        // 2. Update height and size
        update_node_info(node);
        
        // 3. Get balance factor
        int balance = get_balance_factor(node);
        
        // 4. Perform rotations if unbalanced
        
        // Left Left Case
        if (balance > 1 && comp_(key, node->left->key)) {
            return rotate_right(node);
        }
        
        // Right Right Case
        if (balance < -1 && comp_(node->right->key, key)) {
            return rotate_left(node);
        }
        
        // Left Right Case
        if (balance > 1 && comp_(node->left->key, key)) {
            node->left = rotate_left(node->left);
            return rotate_right(node);
        }
        
        // Right Left Case
        if (balance < -1 && comp_(key, node->right->key)) {
            node->right = rotate_right(node->right);
            return rotate_left(node);
        }
        
        return node;
    }
    
    Node* find_min(Node* node) const {
        while (node && node->left) {
            node = node->left;
        }
        return node;
    }
    
    Node* remove_recursive(Node* node, const K& key, bool& removed) {
        if (!node) {
            removed = false;
            return nullptr;
        }
        
        if (comp_(key, node->key)) {
            node->left = remove_recursive(node->left, key, removed);
        } else if (comp_(node->key, key)) {
            node->right = remove_recursive(node->right, key, removed);
        } else {
            // This is the node to be deleted
            removed = true;
            size_--;
            
            if (!node->left || !node->right) {
                // Node with only one child or no child
                Node* temp = node->left ? node->left : node->right;
                
                if (!temp) {
                    // No child case
                    temp = node;
                    node = nullptr;
                } else {
                    // One child case
                    *node = *temp;  // Copy contents
                }
                delete temp;
            } else {
                // Node with two children
                Node* temp = find_min(node->right);
                
                // Copy the inorder successor's data to this node
                node->key = temp->key;
                node->value = temp->value;
                
                // Delete the inorder successor
                bool dummy;
                node->right = remove_recursive(node->right, temp->key, dummy);
            }
        }
        
        if (!node) return node;
        
        // Update height and size
        update_node_info(node);
        
        // Get balance factor
        int balance = get_balance_factor(node);
        
        // Perform rotations if unbalanced
        
        // Left Left Case
        if (balance > 1 && get_balance_factor(node->left) >= 0) {
            return rotate_right(node);
        }
        
        // Left Right Case
        if (balance > 1 && get_balance_factor(node->left) < 0) {
            node->left = rotate_left(node->left);
            return rotate_right(node);
        }
        
        // Right Right Case
        if (balance < -1 && get_balance_factor(node->right) <= 0) {
            return rotate_left(node);
        }
        
        // Right Left Case
        if (balance < -1 && get_balance_factor(node->right) > 0) {
            node->right = rotate_right(node->right);
            return rotate_left(node);
        }
        
        return node;
    }
    
    Node* find_recursive(Node* node, const K& key) const {
        if (!node || (!comp_(node->key, key) && !comp_(key, node->key))) {
            return node;
        }
        
        if (comp_(key, node->key)) {
            return find_recursive(node->left, key);
        } else {
            return find_recursive(node->right, key);
        }
    }
    
    void inorder_recursive(Node* node, std::vector<std::pair<K, V>>& result) const {
        if (node) {
            inorder_recursive(node->left, result);
            result.emplace_back(node->key, node->value);
            inorder_recursive(node->right, result);
        }
    }
    
    void range_query_recursive(Node* node, const K& min_key, const K& max_key, 
                              std::vector<std::pair<K, V>>& result) const {
        if (!node) return;
        
        // If current key is greater than min, explore left subtree
        if (comp_(min_key, node->key)) {
            range_query_recursive(node->left, min_key, max_key, result);
        }
        
        // If current key is in range, add to result
        if (!comp_(node->key, min_key) && !comp_(max_key, node->key)) {
            result.emplace_back(node->key, node->value);
        }
        
        // If current key is less than max, explore right subtree
        if (comp_(node->key, max_key)) {
            range_query_recursive(node->right, min_key, max_key, result);
        }
    }
    
    Node* find_kth_recursive(Node* node, size_t k) const {
        if (!node) return nullptr;
        
        size_t left_size = get_subtree_size(node->left);
        
        if (k == left_size) {
            return node;
        } else if (k < left_size) {
            return find_kth_recursive(node->left, k);
        } else {
            return find_kth_recursive(node->right, k - left_size - 1);
        }
    }
    
    size_t rank_recursive(Node* node, const K& key) const {
        if (!node) return 0;
        
        if (comp_(key, node->key)) {
            return rank_recursive(node->left, key);
        } else if (comp_(node->key, key)) {
            return get_subtree_size(node->left) + 1 + rank_recursive(node->right, key);
        } else {
            return get_subtree_size(node->left);
        }
    }
    
    void destroy_tree(Node* node) {
        if (node) {
            destroy_tree(node->left);
            destroy_tree(node->right);
            delete node;
        }
    }
    
    bool validate_avl_recursive(Node* node) const {
        if (!node) return true;
        
        int balance = get_balance_factor(node);
        if (balance < -1 || balance > 1) return false;
        
        // Check height consistency
        int expected_height = 1 + std::max(get_height(node->left), get_height(node->right));
        if (node->height != expected_height) return false;
        
        // Check size consistency
        size_t expected_size = 1 + get_subtree_size(node->left) + get_subtree_size(node->right);
        if (node->subtree_size != expected_size) return false;
        
        // Check BST property
        if (node->left && !comp_(node->left->key, node->key)) return false;
        if (node->right && !comp_(node->key, node->right->key)) return false;
        
        return validate_avl_recursive(node->left) && validate_avl_recursive(node->right);
    }

public:
    AVLTree() : root_(nullptr), size_(0) {}
    
    explicit AVLTree(Compare comp) : root_(nullptr), comp_(comp), size_(0) {}
    
    ~AVLTree() {
        clear();
    }
    
    // Copy constructor
    AVLTree(const AVLTree& other) : root_(nullptr), comp_(other.comp_), size_(0) {
        auto pairs = other.inorder();
        for (const auto& pair : pairs) {
            insert(pair.first, pair.second);
        }
    }
    
    // Move constructor
    AVLTree(AVLTree&& other) noexcept 
        : root_(other.root_), comp_(std::move(other.comp_)), size_(other.size_) {
        other.root_ = nullptr;
        other.size_ = 0;
    }
    
    // Assignment operators
    AVLTree& operator=(const AVLTree& other) {
        if (this != &other) {
            clear();
            comp_ = other.comp_;
            auto pairs = other.inorder();
            for (const auto& pair : pairs) {
                insert(pair.first, pair.second);
            }
        }
        return *this;
    }
    
    AVLTree& operator=(AVLTree&& other) noexcept {
        if (this != &other) {
            clear();
            root_ = other.root_;
            comp_ = std::move(other.comp_);
            size_ = other.size_;
            
            other.root_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    /**
     * @brief Insert or update a key-value pair
     * @param key Key to insert
     * @param value Value to associate with key
     * @return true if new key was inserted, false if existing key was updated
     */
    bool insert(const K& key, const V& value) {
        bool inserted = false;
        root_ = insert_recursive(root_, key, value, inserted);
        return inserted;
    }
    
    /**
     * @brief Remove a key from the tree
     * @param key Key to remove
     * @return true if key was found and removed, false otherwise
     */
    bool remove(const K& key) {
        bool removed = false;
        root_ = remove_recursive(root_, key, removed);
        return removed;
    }
    
    /**
     * @brief Find value associated with key
     * @param key Key to search for
     * @return Pointer to value if found, nullptr otherwise
     */
    V* find(const K& key) {
        Node* node = find_recursive(root_, key);
        return node ? &(node->value) : nullptr;
    }
    
    const V* find(const K& key) const {
        Node* node = find_recursive(root_, key);
        return node ? &(node->value) : nullptr;
    }
    
    /**
     * @brief Check if key exists in the tree
     * @param key Key to search for
     * @return true if key exists, false otherwise
     */
    bool contains(const K& key) const {
        return find(key) != nullptr;
    }
    
    /**
     * @brief Get all key-value pairs in sorted order
     * @return Vector of key-value pairs
     */
    std::vector<std::pair<K, V>> inorder() const {
        std::vector<std::pair<K, V>> result;
        result.reserve(size_);
        inorder_recursive(root_, result);
        return result;
    }
    
    /**
     * @brief Range query: find all keys in [min_key, max_key]
     * @param min_key Minimum key (inclusive)
     * @param max_key Maximum key (inclusive)
     * @return Vector of key-value pairs in range
     */
    std::vector<std::pair<K, V>> range_query(const K& min_key, const K& max_key) const {
        std::vector<std::pair<K, V>> result;
        range_query_recursive(root_, min_key, max_key, result);
        return result;
    }
    
    /**
     * @brief Find the k-th smallest element (0-indexed)
     * @param k Index (0-based)
     * @return Pointer to key-value pair if found, nullptr otherwise
     */
    std::pair<K, V>* find_kth(size_t k) {
        if (k >= size_) return nullptr;
        
        Node* node = find_kth_recursive(root_, k);
        static std::pair<K, V> result;
        if (node) {
            result = {node->key, node->value};
            return &result;
        }
        return nullptr;
    }
    
    /**
     * @brief Get rank of key (number of keys smaller than given key)
     * @param key Key to find rank for
     * @return Rank (0-based index in sorted order)
     */
    size_t rank(const K& key) const {
        return rank_recursive(root_, key);
    }
    
    /**
     * @brief Find smallest key greater than or equal to given key
     * @param key Key to search from
     * @return Pointer to key-value pair if found, nullptr otherwise
     */
    std::pair<K, V>* lower_bound(const K& key) {
        Node* result = nullptr;
        Node* current = root_;
        
        while (current) {
            if (!comp_(current->key, key)) {  // current->key >= key
                result = current;
                current = current->left;
            } else {
                current = current->right;
            }
        }
        
        if (result) {
            static std::pair<K, V> bound_result;
            bound_result = {result->key, result->value};
            return &bound_result;
        }
        return nullptr;
    }
    
    /**
     * @brief Find smallest key greater than given key
     * @param key Key to search from
     * @return Pointer to key-value pair if found, nullptr otherwise
     */
    std::pair<K, V>* upper_bound(const K& key) {
        Node* result = nullptr;
        Node* current = root_;
        
        while (current) {
            if (comp_(key, current->key)) {  // key < current->key
                result = current;
                current = current->left;
            } else {
                current = current->right;
            }
        }
        
        if (result) {
            static std::pair<K, V> bound_result;
            bound_result = {result->key, result->value};
            return &bound_result;
        }
        return nullptr;
    }
    
    /**
     * @brief Get minimum key-value pair
     */
    std::pair<K, V>* min() {
        if (!root_) return nullptr;
        
        Node* min_node = find_min(root_);
        static std::pair<K, V> min_result;
        min_result = {min_node->key, min_node->value};
        return &min_result;
    }
    
    /**
     * @brief Get maximum key-value pair
     */
    std::pair<K, V>* max() {
        if (!root_) return nullptr;
        
        Node* current = root_;
        while (current->right) {
            current = current->right;
        }
        
        static std::pair<K, V> max_result;
        max_result = {current->key, current->value};
        return &max_result;
    }
    
    /**
     * @brief Check if tree is empty
     */
    bool empty() const {
        return size_ == 0;
    }
    
    /**
     * @brief Get number of elements in tree
     */
    size_t size() const {
        return size_;
    }
    
    /**
     * @brief Get height of the tree
     */
    int height() const {
        return get_height(root_);
    }
    
    /**
     * @brief Clear all elements from tree
     */
    void clear() {
        destroy_tree(root_);
        root_ = nullptr;
        size_ = 0;
    }
    
    /**
     * @brief Validate AVL tree properties
     * @return true if tree maintains AVL invariants
     */
    bool validate() const {
        return validate_avl_recursive(root_);
    }
    
    /**
     * @brief Get tree statistics for analysis
     */
    struct Statistics {
        size_t total_nodes;
        int height;
        int min_depth;
        double avg_depth;
        size_t memory_usage_bytes;
        double balance_factor_variance;
    };
    
    Statistics get_statistics() const {
        Statistics stats;
        stats.total_nodes = size_;
        stats.height = height();
        stats.memory_usage_bytes = sizeof(Node) * size_;
        
        // Calculate depth statistics (simplified)
        if (size_ > 0) {
            stats.min_depth = 0;
            stats.avg_depth = static_cast<double>(stats.height) / 2.0;  // Approximation
            stats.balance_factor_variance = 0.5;  // AVL trees are well-balanced
        } else {
            stats.min_depth = 0;
            stats.avg_depth = 0.0;
            stats.balance_factor_variance = 0.0;
        }
        
        return stats;
    }
    
    /**
     * @brief Operator[] for convenient access (creates entry if not exists)
     */
    V& operator[](const K& key) {
        Node* node = find_recursive(root_, key);
        if (node) {
            return node->value;
        }
        
        insert(key, V{});
        return *find(key);
    }
};

} // namespace rts::core::trees