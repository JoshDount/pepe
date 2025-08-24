#pragma once

#include "VisualizationFramework.hpp"
#include "../core/containers/MinHeap.hpp"
#include "../core/containers/HashMap.hpp"
#include "../core/containers/LinkedList.hpp"
#include "../core/trees/AVLTree.hpp"
#include "../core/containers/Queue.hpp"
#include "../core/containers/Stack.hpp"
#include <vector>
#include <map>
#include <string>
#include <sstream>
#include <iomanip>

namespace rts::viz {

/**
 * @brief MinHeap visualization
 */
class MinHeapVisualizer : public Visualizer {
private:
    std::vector<int> heap_data_;
    
public:
    MinHeapVisualizer(const std::string& title = "MinHeap Visualization")
        : Visualizer(title, 80, 20) {}
    
    /**
     * @brief Update heap data for visualization
     */
    void update_heap(const std::vector<int>& data) {
        heap_data_ = data;
    }
    
    /**
     * @brief Visualize heap as a tree structure
     */
    void visualize_tree() const {
        if (heap_data_.empty()) {
            std::cout << "Heap is empty\n";
            return;
        }
        
        clear_screen();
        print_title();
        
        size_t levels = static_cast<size_t>(std::floor(std::log2(heap_data_.size()))) + 1;
        
        for (size_t level = 0; level < levels; ++level) {
            size_t start_idx = (1 << level) - 1;  // 2^level - 1
            size_t end_idx = std::min(static_cast<size_t>((1 << (level + 1)) - 1), heap_data_.size());
            
            // Calculate spacing for this level
            size_t spacing = width_ / (end_idx - start_idx + 1);
            size_t initial_spacing = spacing / 2;
            
            std::cout << std::string(initial_spacing, ' ');
            
            for (size_t i = start_idx; i < end_idx; ++i) {
                if (i < heap_data_.size()) {
                    std::string value = std::to_string(heap_data_[i]);
                    
                    // Color coding: root in red, others in blue
                    if (i == 0) {
                        std::cout << colorize("[" + value + "]", Colors::BG_RED + Colors::WHITE);
                    } else {
                        std::cout << colorize("[" + value + "]", Colors::BG_BLUE + Colors::WHITE);
                    }\n                    \n                    if (i < end_idx - 1) {\n                        std::cout << std::string(spacing - value.length() - 2, ' ');\n                    }\n                } else {\n                    std::cout << \"[ ]\";\n                }\n            }\n            \n            std::cout << \"\\n\\n\";\n        }\n        \n        // Print heap properties\n        std::cout << \"\\nHeap Properties:\\n\";\n        std::cout << \"Size: \" << heap_data_.size() << \"\\n\";\n        std::cout << \"Root (Min): \" << (heap_data_.empty() ? \"N/A\" : std::to_string(heap_data_[0])) << \"\\n\";\n        std::cout << \"Height: \" << levels << \"\\n\";\n        \n        // Verify heap property\n        bool is_valid_heap = true;\n        for (size_t i = 0; i < heap_data_.size(); ++i) {\n            size_t left_child = 2 * i + 1;\n            size_t right_child = 2 * i + 2;\n            \n            if (left_child < heap_data_.size() && heap_data_[i] > heap_data_[left_child]) {\n                is_valid_heap = false;\n                break;\n            }\n            if (right_child < heap_data_.size() && heap_data_[i] > heap_data_[right_child]) {\n                is_valid_heap = false;\n                break;\n            }\n        }\n        \n        std::cout << \"Valid Min-Heap: \" << colorize(is_valid_heap ? \"Yes\" : \"No\", \n                                                    is_valid_heap ? Colors::GREEN : Colors::RED) << \"\\n\";\n    }\n    \n    /**\n     * @brief Visualize heap as an array\n     */\n    void visualize_array() const {\n        if (heap_data_.empty()) {\n            std::cout << \"Heap is empty\\n\";\n            return;\n        }\n        \n        print_title();\n        \n        std::cout << \"Array Representation:\\n\";\n        std::cout << \"Index: \";\n        for (size_t i = 0; i < heap_data_.size(); ++i) {\n            std::cout << std::setw(4) << i;\n        }\n        std::cout << \"\\n\";\n        \n        std::cout << \"Value: \";\n        for (size_t i = 0; i < heap_data_.size(); ++i) {\n            std::cout << std::setw(4) << heap_data_[i];\n        }\n        std::cout << \"\\n\\n\";\n        \n        // Show parent-child relationships\n        std::cout << \"Parent-Child Relationships:\\n\";\n        for (size_t i = 0; i < heap_data_.size(); ++i) {\n            std::cout << \"Node \" << i << \" (value: \" << heap_data_[i] << \"):\";\n            \n            if (i > 0) {\n                size_t parent = (i - 1) / 2;\n                std::cout << \" Parent: \" << parent << \"(\" << heap_data_[parent] << \")\";\n            } else {\n                std::cout << \" Parent: ROOT\";\n            }\n            \n            size_t left = 2 * i + 1;\n            size_t right = 2 * i + 2;\n            \n            if (left < heap_data_.size()) {\n                std::cout << \" Left: \" << left << \"(\" << heap_data_[left] << \")\";\n            }\n            \n            if (right < heap_data_.size()) {\n                std::cout << \" Right: \" << right << \"(\" << heap_data_[right] << \")\";\n            }\n            \n            std::cout << \"\\n\";\n        }\n    }\n};\n\n/**\n * @brief HashMap visualization\n */\nclass HashMapVisualizer : public Visualizer {\nprivate:\n    std::vector<std::vector<std::pair<int, int>>> buckets_;\n    size_t bucket_count_;\n    size_t total_elements_;\n    \npublic:\n    HashMapVisualizer(const std::string& title = \"HashMap Visualization\")\n        : Visualizer(title, 80, 25), bucket_count_(0), total_elements_(0) {}\n    \n    /**\n     * @brief Update hashmap data for visualization\n     */\n    void update_hashmap(const std::vector<std::vector<std::pair<int, int>>>& buckets) {\n        buckets_ = buckets;\n        bucket_count_ = buckets_.size();\n        total_elements_ = 0;\n        \n        for (const auto& bucket : buckets_) {\n            total_elements_ += bucket.size();\n        }\n    }\n    \n    /**\n     * @brief Visualize hashmap structure\n     */\n    void visualize_structure() const {\n        clear_screen();\n        print_title();\n        \n        std::cout << \"HashMap Structure (\" << bucket_count_ << \" buckets, \" \n                  << total_elements_ << \" elements):\\n\\n\";\n        \n        // Calculate load factor\n        double load_factor = bucket_count_ > 0 ? \n            static_cast<double>(total_elements_) / bucket_count_ : 0.0;\n        \n        std::cout << \"Load Factor: \" << std::fixed << std::setprecision(2) << load_factor;\n        if (load_factor > 0.75) {\n            std::cout << colorize(\" (HIGH - Consider resizing)\", Colors::RED);\n        } else if (load_factor < 0.25) {\n            std::cout << colorize(\" (LOW - Memory efficient)\", Colors::GREEN);\n        } else {\n            std::cout << colorize(\" (GOOD)\", Colors::YELLOW);\n        }\n        std::cout << \"\\n\\n\";\n        \n        // Display buckets\n        for (size_t i = 0; i < bucket_count_; ++i) {\n            std::cout << colorize(\"Bucket \" + std::to_string(i) + \":\", Colors::CYAN);\n            \n            if (buckets_[i].empty()) {\n                std::cout << colorize(\" [empty]\", Colors::DIM) << \"\\n\";\n            } else {\n                std::cout << \" \";\n                for (size_t j = 0; j < buckets_[i].size(); ++j) {\n                    const auto& [key, value] = buckets_[i][j];\n                    std::string pair_str = \"(\" + std::to_string(key) + \",\" + std::to_string(value) + \")\";\n                    \n                    if (buckets_[i].size() > 1) {\n                        std::cout << colorize(pair_str, Colors::YELLOW); // Collision highlighted\n                    } else {\n                        std::cout << colorize(pair_str, Colors::WHITE);\n                    }\n                    \n                    if (j < buckets_[i].size() - 1) {\n                        std::cout << colorize(\" -> \", Colors::RED); // Chain indicator\n                    }\n                }\n                \n                if (buckets_[i].size() > 1) {\n                    std::cout << colorize(\" [COLLISION]\", Colors::BG_RED + Colors::WHITE);\n                }\n                \n                std::cout << \"\\n\";\n            }\n        }\n        \n        // Statistics\n        size_t collisions = 0;\n        size_t longest_chain = 0;\n        for (const auto& bucket : buckets_) {\n            if (bucket.size() > 1) {\n                collisions++;\n                longest_chain = std::max(longest_chain, bucket.size());\n            }\n        }\n        \n        std::cout << \"\\nStatistics:\\n\";\n        std::cout << \"Empty buckets: \" << (bucket_count_ - total_elements_ + collisions) << \"\\n\";\n        std::cout << \"Collisions: \" << collisions << \"\\n\";\n        std::cout << \"Longest chain: \" << longest_chain << \"\\n\";\n        \n        // Hash distribution visualization\n        std::cout << \"\\nBucket Utilization:\\n\";\n        for (size_t i = 0; i < bucket_count_; ++i) {\n            std::cout << std::setw(2) << i << \": \";\n            size_t bar_length = buckets_[i].size() * 10; // Scale for visualization\n            \n            if (bar_length > 0) {\n                std::string bar(bar_length, '█');\n                if (buckets_[i].size() > 1) {\n                    std::cout << colorize(bar, Colors::RED); // Collision in red\n                } else {\n                    std::cout << colorize(bar, Colors::GREEN);\n                }\n            } else {\n                std::cout << colorize(\"░\", Colors::DIM);\n            }\n            \n            std::cout << \" (\" << buckets_[i].size() << \")\\n\";\n        }\n    }\n};\n\n/**\n * @brief LinkedList visualization\n */\nclass LinkedListVisualizer : public Visualizer {\nprivate:\n    std::vector<int> list_data_;\n    \npublic:\n    LinkedListVisualizer(const std::string& title = \"LinkedList Visualization\")\n        : Visualizer(title, 80, 15) {}\n    \n    /**\n     * @brief Update list data\n     */\n    void update_list(const std::vector<int>& data) {\n        list_data_ = data;\n    }\n    \n    /**\n     * @brief Visualize linked list structure\n     */\n    void visualize_structure() const {\n        clear_screen();\n        print_title();\n        \n        if (list_data_.empty()) {\n            std::cout << \"List is empty\\n\";\n            std::cout << colorize(\"HEAD -> NULL\", Colors::CYAN) << \"\\n\";\n            return;\n        }\n        \n        std::cout << \"Linked List Structure (\" << list_data_.size() << \" nodes):\\n\\n\";\n        \n        // Draw the list horizontally\n        std::cout << colorize(\"HEAD\", Colors::CYAN) << \" -> \";\n        \n        for (size_t i = 0; i < list_data_.size(); ++i) {\n            // Node representation\n            std::string node_str = \"[\" + std::to_string(list_data_[i]) + \"]\";\n            \n            if (i == 0) {\n                std::cout << colorize(node_str, Colors::BG_GREEN + Colors::BLACK); // Head node\n            } else if (i == list_data_.size() - 1) {\n                std::cout << colorize(node_str, Colors::BG_RED + Colors::WHITE); // Tail node\n            } else {\n                std::cout << colorize(node_str, Colors::BG_BLUE + Colors::WHITE); // Regular node\n            }\n            \n            if (i < list_data_.size() - 1) {\n                std::cout << colorize(\" -> \", Colors::YELLOW);\n            }\n        }\n        \n        std::cout << colorize(\" -> NULL\", Colors::CYAN) << \"\\n\\n\";\n        \n        // Node information\n        std::cout << \"Node Information:\\n\";\n        for (size_t i = 0; i < list_data_.size(); ++i) {\n            std::cout << \"Node \" << i << \": Value = \" << list_data_[i];\n            \n            if (i == 0) {\n                std::cout << colorize(\" [HEAD]\", Colors::GREEN);\n            }\n            if (i == list_data_.size() - 1) {\n                std::cout << colorize(\" [TAIL]\", Colors::RED);\n            }\n            \n            std::cout << \"\\n\";\n        }\n        \n        // Memory layout visualization (conceptual)\n        std::cout << \"\\nConceptual Memory Layout:\\n\";\n        for (size_t i = 0; i < list_data_.size(); ++i) {\n            std::cout << \"┌─────────┬─────────┐\\n\";\n            std::cout << \"│ Data: \" << std::setw(2) << list_data_[i] << \" │ Next: \";\n            \n            if (i < list_data_.size() - 1) {\n                std::cout << std::setw(2) << (i + 1) << \" │\";\n            } else {\n                std::cout << colorize(\"NULL\", Colors::RED) << \"│\";\n            }\n            \n            std::cout << \"\\n\";\n            std::cout << \"└─────────┴─────────┘\\n\";\n            \n            if (i < list_data_.size() - 1) {\n                std::cout << \"         │\\n\";\n                std::cout << \"         ▼\\n\";\n            }\n        }\n    }\n};\n\n/**\n * @brief Stack and Queue visualization\n */\nclass StackQueueVisualizer : public Visualizer {\nprivate:\n    std::vector<int> data_;\n    bool is_stack_;\n    \npublic:\n    StackQueueVisualizer(bool is_stack = true, \n                        const std::string& title = \"Stack/Queue Visualization\")\n        : Visualizer(title, 40, 20), is_stack_(is_stack) {}\n    \n    /**\n     * @brief Update data\n     */\n    void update_data(const std::vector<int>& data) {\n        data_ = data;\n    }\n    \n    /**\n     * @brief Visualize stack or queue\n     */\n    void visualize_structure() const {\n        clear_screen();\n        print_title();\n        \n        if (data_.empty()) {\n            std::cout << (is_stack_ ? \"Stack\" : \"Queue\") << \" is empty\\n\";\n            return;\n        }\n        \n        if (is_stack_) {\n            visualize_stack();\n        } else {\n            visualize_queue();\n        }\n    }\n    \nprivate:\n    void visualize_stack() const {\n        std::cout << \"Stack (LIFO - Last In, First Out):\\n\\n\";\n        \n        // Draw stack vertically (top to bottom)\n        for (int i = static_cast<int>(data_.size()) - 1; i >= 0; --i) {\n            if (i == static_cast<int>(data_.size()) - 1) {\n                std::cout << colorize(\"TOP -> \", Colors::RED);\n            } else {\n                std::cout << \"       \";\n            }\n            \n            std::cout << \"┌─────────┐\\n\";\n            std::cout << \"       │ \" << std::setw(7) << data_[i] << \" │\";\n            \n            if (i == static_cast<int>(data_.size()) - 1) {\n                std::cout << colorize(\" <- TOP\", Colors::RED);\n            }\n            \n            std::cout << \"\\n       └─────────┘\\n\";\n        }\n        \n        std::cout << \"\\nOperations:\\n\";\n        std::cout << \"• \" << colorize(\"push(x)\", Colors::GREEN) << \": Add element to top\\n\";\n        std::cout << \"• \" << colorize(\"pop()\", Colors::YELLOW) << \": Remove element from top\\n\";\n        std::cout << \"• \" << colorize(\"top()\", Colors::CYAN) << \": Peek at top element\\n\";\n    }\n    \n    void visualize_queue() const {\n        std::cout << \"Queue (FIFO - First In, First Out):\\n\\n\";\n        \n        // Draw queue horizontally\n        std::cout << colorize(\"FRONT\", Colors::GREEN) << \" -> \";\n        \n        for (size_t i = 0; i < data_.size(); ++i) {\n            std::cout << \"[\" << data_[i] << \"]\";\n            \n            if (i < data_.size() - 1) {\n                std::cout << \" <- \";\n            }\n        }\n        \n        std::cout << \" <- \" << colorize(\"REAR\", Colors::RED) << \"\\n\\n\";\n        \n        // Detailed view\n        std::cout << \"Detailed View:\\n\";\n        std::cout << \"┌\";\n        for (size_t i = 0; i < data_.size(); ++i) {\n            std::cout << \"─────────\";\n            if (i < data_.size() - 1) std::cout << \"┬\";\n        }\n        std::cout << \"┐\\n\";\n        \n        std::cout << \"│\";\n        for (size_t i = 0; i < data_.size(); ++i) {\n            std::cout << \" \" << std::setw(7) << data_[i] << \" │\";\n        }\n        std::cout << \"\\n\";\n        \n        std::cout << \"└\";\n        for (size_t i = 0; i < data_.size(); ++i) {\n            std::cout << \"─────────\";\n            if (i < data_.size() - 1) std::cout << \"┴\";\n        }\n        std::cout << \"┘\\n\";\n        \n        std::cout << colorize(\"▲ FRONT\", Colors::GREEN) << std::string(data_.size() * 9 - 8, ' ') \n                  << colorize(\"REAR ▲\", Colors::RED) << \"\\n\\n\";\n        \n        std::cout << \"Operations:\\n\";\n        std::cout << \"• \" << colorize(\"enqueue(x)\", Colors::GREEN) << \": Add element to rear\\n\";\n        std::cout << \"• \" << colorize(\"dequeue()\", Colors::YELLOW) << \": Remove element from front\\n\";\n        std::cout << \"• \" << colorize(\"front()\", Colors::CYAN) << \": Peek at front element\\n\";\n    }\n};\n\n} // namespace rts::viz"
                }
            }
            
            std::cout << "\n\n";
        }
        
        // Print heap properties
        std::cout << "\nHeap Properties:\n";
        std::cout << "Size: " << heap_data_.size() << "\n";
        std::cout << "Root (Min): " << (heap_data_.empty() ? "N/A" : std::to_string(heap_data_[0])) << "\n";
        std::cout << "Height: " << levels << "\n";
        
        // Verify heap property
        bool is_valid_heap = true;
        for (size_t i = 0; i < heap_data_.size(); ++i) {
            size_t left_child = 2 * i + 1;
            size_t right_child = 2 * i + 2;
            
            if (left_child < heap_data_.size() && heap_data_[i] > heap_data_[left_child]) {
                is_valid_heap = false;
                break;
            }
            if (right_child < heap_data_.size() && heap_data_[i] > heap_data_[right_child]) {
                is_valid_heap = false;
                break;
            }
        }
        
        std::cout << "Valid Min-Heap: " << colorize(is_valid_heap ? "Yes" : "No", 
                                                    is_valid_heap ? Colors::GREEN : Colors::RED) << "\n";
    }
    
    /**
     * @brief Visualize heap as an array
     */
    void visualize_array() const {
        if (heap_data_.empty()) {
            std::cout << "Heap is empty\n";
            return;
        }
        
        print_title();
        
        std::cout << "Array Representation:\n";
        std::cout << "Index: ";
        for (size_t i = 0; i < heap_data_.size(); ++i) {
            std::cout << std::setw(4) << i;
        }
        std::cout << "\n";
        
        std::cout << "Value: ";
        for (size_t i = 0; i < heap_data_.size(); ++i) {
            std::cout << std::setw(4) << heap_data_[i];
        }
        std::cout << "\n\n";
        
        // Show parent-child relationships
        std::cout << "Parent-Child Relationships:\n";
        for (size_t i = 0; i < heap_data_.size(); ++i) {
            std::cout << "Node " << i << " (value: " << heap_data_[i] << "):";
            
            if (i > 0) {
                size_t parent = (i - 1) / 2;
                std::cout << " Parent: " << parent << "(" << heap_data_[parent] << ")";
            } else {
                std::cout << " Parent: ROOT";
            }
            
            size_t left = 2 * i + 1;
            size_t right = 2 * i + 2;
            
            if (left < heap_data_.size()) {
                std::cout << " Left: " << left << "(" << heap_data_[left] << ")";
            }
            
            if (right < heap_data_.size()) {
                std::cout << " Right: " << right << "(" << heap_data_[right] << ")";
            }
            
            std::cout << "\n";
        }
    }
};

/**
 * @brief HashMap visualization
 */
class HashMapVisualizer : public Visualizer {
private:
    std::vector<std::vector<std::pair<int, int>>> buckets_;
    size_t bucket_count_;
    size_t total_elements_;
    
public:
    HashMapVisualizer(const std::string& title = "HashMap Visualization")
        : Visualizer(title, 80, 25), bucket_count_(0), total_elements_(0) {}
    
    /**
     * @brief Update hashmap data for visualization
     */
    void update_hashmap(const std::vector<std::vector<std::pair<int, int>>>& buckets) {
        buckets_ = buckets;
        bucket_count_ = buckets_.size();
        total_elements_ = 0;
        
        for (const auto& bucket : buckets_) {
            total_elements_ += bucket.size();
        }
    }
    
    /**
     * @brief Visualize hashmap structure
     */
    void visualize_structure() const {
        clear_screen();
        print_title();
        
        std::cout << "HashMap Structure (" << bucket_count_ << " buckets, " 
                  << total_elements_ << " elements):\n\n";
        
        // Calculate load factor
        double load_factor = bucket_count_ > 0 ? 
            static_cast<double>(total_elements_) / bucket_count_ : 0.0;
        
        std::cout << "Load Factor: " << std::fixed << std::setprecision(2) << load_factor;
        if (load_factor > 0.75) {
            std::cout << colorize(" (HIGH - Consider resizing)", Colors::RED);
        } else if (load_factor < 0.25) {
            std::cout << colorize(" (LOW - Memory efficient)", Colors::GREEN);
        } else {
            std::cout << colorize(" (GOOD)", Colors::YELLOW);
        }
        std::cout << "\n\n";
        
        // Display buckets
        for (size_t i = 0; i < std::min(bucket_count_, static_cast<size_t>(10)); ++i) {
            std::cout << colorize("Bucket " + std::to_string(i) + ":", Colors::CYAN);
            
            if (buckets_[i].empty()) {
                std::cout << colorize(" [empty]", Colors::DIM) << "\n";
            } else {
                std::cout << " ";
                for (size_t j = 0; j < buckets_[i].size(); ++j) {
                    const auto& [key, value] = buckets_[i][j];
                    std::string pair_str = "(" + std::to_string(key) + "," + std::to_string(value) + ")";
                    
                    if (buckets_[i].size() > 1) {
                        std::cout << colorize(pair_str, Colors::YELLOW); // Collision highlighted
                    } else {
                        std::cout << colorize(pair_str, Colors::WHITE);
                    }
                    
                    if (j < buckets_[i].size() - 1) {
                        std::cout << colorize(" -> ", Colors::RED); // Chain indicator
                    }
                }
                
                if (buckets_[i].size() > 1) {
                    std::cout << colorize(" [COLLISION]", Colors::BG_RED + Colors::WHITE);
                }
                
                std::cout << "\n";
            }
        }
        
        if (bucket_count_ > 10) {
            std::cout << "... (" << (bucket_count_ - 10) << " more buckets)\n";
        }
    }
};

} // namespace rts::viz