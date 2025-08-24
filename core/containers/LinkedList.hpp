#pragma once

#include <memory>
#include <stdexcept>
#include <vector>

namespace rts::core::containers {

/**
 * @brief Comprehensive LinkedList implementation
 * 
 * Features:
 * - Single and double linking support
 * - Free-list optimization for node reuse
 * - Iterator support
 * - Memory pool management
 * 
 * Time Complexities:
 * - Insert front/back: O(1)
 * - Insert at position: O(n)
 * - Remove front/back: O(1)
 * - Remove at position: O(n)
 * - Search: O(n)
 * 
 * Space Complexity: O(n)
 */
template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        Node* next;
        Node* prev;  // Used only for double-linked lists
        
        Node(const T& value) : data(value), next(nullptr), prev(nullptr) {}
    };
    
    Node* head_;
    Node* tail_;
    size_t size_;
    bool is_double_linked_;
    
    // Free-list optimization
    static std::vector<Node*> free_list_;
    static size_t free_list_size_;
    static constexpr size_t MAX_FREE_LIST_SIZE = 1000;
    
    Node* allocate_node(const T& value) {
        Node* node;
        if (!free_list_.empty()) {
            node = free_list_.back();
            free_list_.pop_back();
            free_list_size_--;
            
            // Reconstruct with placement new
            new (node) Node(value);
        } else {
            node = new Node(value);
        }
        return node;
    }
    
    void deallocate_node(Node* node) {
        if (free_list_size_ < MAX_FREE_LIST_SIZE) {
            // Add to free list for reuse
            node->~Node();
            free_list_.push_back(node);
            free_list_size_++;
        } else {
            delete node;
        }
    }
    
    void link_node_after(Node* existing, Node* new_node) {
        new_node->next = existing->next;
        existing->next = new_node;
        
        if (is_double_linked_) {
            new_node->prev = existing;
            if (new_node->next) {
                new_node->next->prev = new_node;
            }
        }
        
        if (existing == tail_) {
            tail_ = new_node;
        }
    }
    
    void link_node_before(Node* existing, Node* new_node) {
        if (!is_double_linked_) {
            throw std::runtime_error("Insert before not supported in single-linked list");
        }
        
        new_node->prev = existing->prev;
        new_node->next = existing;
        existing->prev = new_node;
        
        if (new_node->prev) {
            new_node->prev->next = new_node;
        } else {
            head_ = new_node;
        }
    }
    
    void unlink_node(Node* node) {
        if (node->prev) {
            node->prev->next = node->next;
        } else {
            head_ = node->next;
        }
        
        if (node->next) {
            if (is_double_linked_) {
                node->next->prev = node->prev;
            }
        } else {
            tail_ = node->prev;
        }
    }

public:
    /**
     * @brief Iterator class for LinkedList
     */
    class Iterator {
    private:
        Node* current_;
        
    public:
        Iterator() : current_(nullptr) {}
        Iterator(Node* node) : current_(node) {}
        
        T& operator*() {
            if (!current_) {
                throw std::runtime_error("Dereferencing null iterator");
            }
            return current_->data;
        }
        
        const T& operator*() const {
            if (!current_) {
                throw std::runtime_error("Dereferencing null iterator");
            }
            return current_->data;
        }
        
        T* operator->() {
            if (!current_) {
                throw std::runtime_error("Dereferencing null iterator");
            }
            return &(current_->data);
        }
        
        const T* operator->() const {
            if (!current_) {
                throw std::runtime_error("Dereferencing null iterator");
            }
            return &(current_->data);
        }
        
        Iterator& operator++() {
            if (current_) {
                current_ = current_->next;
            }
            return *this;
        }
        
        Iterator operator++(int) {
            Iterator temp = *this;
            ++(*this);
            return temp;
        }
        
        bool operator==(const Iterator& other) const {
            return current_ == other.current_;
        }
        
        bool operator!=(const Iterator& other) const {
            return !(*this == other);
        }
        
        Node* get_node() const { return current_; }
    };
    
    /**
     * @brief Reverse iterator (only for double-linked lists)
     */
    class ReverseIterator {
    private:
        Node* current_;
        
    public:
        ReverseIterator(Node* node) : current_(node) {}
        
        T& operator*() {
            if (!current_) {
                throw std::runtime_error("Dereferencing null iterator");
            }
            return current_->data;
        }
        
        ReverseIterator& operator++() {
            if (current_) {
                current_ = current_->prev;
            }
            return *this;
        }
        
        bool operator==(const ReverseIterator& other) const {
            return current_ == other.current_;
        }
        
        bool operator!=(const ReverseIterator& other) const {
            return !(*this == other);
        }
    };
    
    explicit LinkedList(bool double_linked = true)
        : head_(nullptr), tail_(nullptr), size_(0), is_double_linked_(double_linked) {}
    
    ~LinkedList() {
        clear();
    }
    
    // Copy constructor
    LinkedList(const LinkedList& other)
        : head_(nullptr), tail_(nullptr), size_(0), is_double_linked_(other.is_double_linked_) {
        for (const auto& item : other) {
            push_back(item);
        }
    }
    
    // Move constructor
    LinkedList(LinkedList&& other) noexcept
        : head_(other.head_), tail_(other.tail_), size_(other.size_),
          is_double_linked_(other.is_double_linked_) {
        other.head_ = nullptr;
        other.tail_ = nullptr;
        other.size_ = 0;
    }
    
    // Assignment operators
    LinkedList& operator=(const LinkedList& other) {
        if (this != &other) {
            clear();
            is_double_linked_ = other.is_double_linked_;
            for (const auto& item : other) {
                push_back(item);
            }
        }
        return *this;
    }
    
    LinkedList& operator=(LinkedList&& other) noexcept {
        if (this != &other) {
            clear();
            head_ = other.head_;
            tail_ = other.tail_;
            size_ = other.size_;
            is_double_linked_ = other.is_double_linked_;
            
            other.head_ = nullptr;
            other.tail_ = nullptr;
            other.size_ = 0;
        }
        return *this;
    }
    
    /**
     * @brief Add element to the front of the list
     */
    void push_front(const T& value) {
        Node* new_node = allocate_node(value);
        
        if (empty()) {
            head_ = tail_ = new_node;
        } else {
            new_node->next = head_;
            if (is_double_linked_) {
                head_->prev = new_node;
            }
            head_ = new_node;
        }
        
        size_++;
    }
    
    /**
     * @brief Add element to the back of the list
     */
    void push_back(const T& value) {
        Node* new_node = allocate_node(value);
        
        if (empty()) {
            head_ = tail_ = new_node;
        } else {
            tail_->next = new_node;
            if (is_double_linked_) {
                new_node->prev = tail_;
            }
            tail_ = new_node;
        }
        
        size_++;
    }
    
    /**
     * @brief Remove element from the front
     */
    void pop_front() {
        if (empty()) {
            throw std::runtime_error("Cannot pop from empty list");
        }
        
        Node* to_remove = head_;
        head_ = head_->next;
        
        if (head_) {
            if (is_double_linked_) {
                head_->prev = nullptr;
            }
        } else {
            tail_ = nullptr;
        }
        
        deallocate_node(to_remove);
        size_--;
    }
    
    /**
     * @brief Remove element from the back
     */
    void pop_back() {
        if (empty()) {
            throw std::runtime_error("Cannot pop from empty list");
        }
        
        if (size_ == 1) {
            deallocate_node(head_);
            head_ = tail_ = nullptr;
        } else if (is_double_linked_) {
            Node* to_remove = tail_;
            tail_ = tail_->prev;
            tail_->next = nullptr;
            deallocate_node(to_remove);
        } else {
            // For single-linked list, need to traverse to find second-to-last
            Node* current = head_;
            while (current->next != tail_) {
                current = current->next;
            }
            deallocate_node(tail_);
            tail_ = current;
            tail_->next = nullptr;
        }
        
        size_--;
    }
    
    /**
     * @brief Insert element at specific position
     */
    void insert(size_t position, const T& value) {
        if (position > size_) {
            throw std::out_of_range("Position out of range");
        }
        
        if (position == 0) {
            push_front(value);
            return;
        }
        
        if (position == size_) {
            push_back(value);
            return;
        }
        
        Node* current = head_;
        for (size_t i = 0; i < position - 1; ++i) {
            current = current->next;
        }
        
        Node* new_node = allocate_node(value);
        link_node_after(current, new_node);
        size_++;
    }
    
    /**
     * @brief Remove element at specific position
     */
    void erase(size_t position) {
        if (position >= size_) {
            throw std::out_of_range("Position out of range");
        }
        
        if (position == 0) {
            pop_front();
            return;
        }
        
        if (position == size_ - 1) {
            pop_back();
            return;
        }
        
        Node* current = head_;
        for (size_t i = 0; i < position; ++i) {
            current = current->next;
        }
        
        unlink_node(current);
        deallocate_node(current);
        size_--;
    }
    
    /**
     * @brief Remove first occurrence of value
     */
    bool remove(const T& value) {
        Node* current = head_;
        
        while (current) {
            if (current->data == value) {
                unlink_node(current);
                deallocate_node(current);
                size_--;
                return true;
            }
            current = current->next;
        }
        
        return false;
    }
    
    /**
     * @brief Find first occurrence of value
     */
    Iterator find(const T& value) {
        Node* current = head_;
        while (current) {
            if (current->data == value) {
                return Iterator(current);
            }
            current = current->next;
        }
        return end();
    }
    
    /**
     * @brief Get element at specific position
     */
    T& at(size_t position) {
        if (position >= size_) {
            throw std::out_of_range("Position out of range");
        }
        
        Node* current = head_;
        for (size_t i = 0; i < position; ++i) {
            current = current->next;
        }
        
        return current->data;
    }
    
    const T& at(size_t position) const {
        return const_cast<LinkedList*>(this)->at(position);
    }
    
    /**
     * @brief Get front element
     */
    T& front() {
        if (empty()) {
            throw std::runtime_error("List is empty");
        }
        return head_->data;
    }
    
    const T& front() const {
        if (empty()) {
            throw std::runtime_error("List is empty");
        }
        return head_->data;
    }
    
    /**
     * @brief Get back element
     */
    T& back() {
        if (empty()) {
            throw std::runtime_error("List is empty");
        }
        return tail_->data;
    }
    
    const T& back() const {
        if (empty()) {
            throw std::runtime_error("List is empty");
        }
        return tail_->data;
    }
    
    /**
     * @brief Check if list is empty
     */
    bool empty() const {
        return size_ == 0;
    }
    
    /**
     * @brief Get number of elements
     */
    size_t size() const {
        return size_;
    }
    
    /**
     * @brief Clear all elements
     */
    void clear() {
        while (!empty()) {
            pop_front();
        }
    }
    
    /**
     * @brief Iterator support
     */
    Iterator begin() {
        return Iterator(head_);
    }
    
    Iterator end() {
        return Iterator(nullptr);
    }
    
    const Iterator begin() const {
        return Iterator(head_);
    }
    
    const Iterator end() const {
        return Iterator(nullptr);
    }
    
    /**
     * @brief Reverse iterator support (double-linked only)
     */
    ReverseIterator rbegin() {
        if (!is_double_linked_) {
            throw std::runtime_error("Reverse iteration not supported for single-linked list");
        }
        return ReverseIterator(tail_);
    }
    
    ReverseIterator rend() {
        if (!is_double_linked_) {
            throw std::runtime_error("Reverse iteration not supported for single-linked list");
        }
        return ReverseIterator(nullptr);
    }
    
    /**
     * @brief Check if list is double-linked
     */
    bool is_double_linked() const {
        return is_double_linked_;
    }
    
    /**
     * @brief Get memory usage statistics
     */
    struct Statistics {
        size_t total_elements;
        size_t memory_usage_bytes;
        size_t free_list_size;
        bool is_double_linked;
    };
    
    Statistics get_statistics() const {
        Statistics stats;
        stats.total_elements = size_;
        stats.memory_usage_bytes = sizeof(Node) * size_;
        stats.free_list_size = free_list_size_;
        stats.is_double_linked = is_double_linked_;
        return stats;
    }
    
    /**
     * @brief Clean up free list (static method)
     */
    static void cleanup_free_list() {
        for (Node* node : free_list_) {
            delete node;
        }
        free_list_.clear();
        free_list_size_ = 0;
    }
};

// Static member definitions
template<typename T>
std::vector<typename LinkedList<T>::Node*> LinkedList<T>::free_list_;

template<typename T>
size_t LinkedList<T>::free_list_size_ = 0;

} // namespace rts::core::containers