#include "../../core/trees/AVLTree.hpp"
#include "../TestFramework.hpp"

void test_avl_tree() {
    using namespace rts::core::trees;
    
    // Test basic AVL tree operations
    {
        AVLTree<int, std::string> tree;
        TestFramework::assert_true(tree.empty(), "New tree should be empty");
        
        tree.insert(5, "five");
        tree.insert(3, "three");
        tree.insert(7, "seven");
        tree.insert(1, "one");
        tree.insert(9, "nine");
        
        TestFramework::assert_equal(size_t(5), tree.size(), "Tree should have 5 elements");
        TestFramework::assert_true(tree.validate(), "Tree should maintain AVL property");
        
        auto value = tree.find(5);
        TestFramework::assert_not_null(value, "Should find inserted key");
        TestFramework::assert_equal(std::string("five"), *value, "Should return correct value");
        
        // Test inorder traversal
        auto pairs = tree.inorder();
        TestFramework::assert_equal(size_t(5), pairs.size(), "Inorder should return all elements");
        
        // Should be in sorted order
        for (size_t i = 1; i < pairs.size(); ++i) {
            TestFramework::assert_true(pairs[i-1].first < pairs[i].first, "Inorder should be sorted");
        }
    }
    
    // Test range queries
    {
        AVLTree<int, std::string> tree;
        for (int i = 0; i < 10; ++i) {
            tree.insert(i, "value" + std::to_string(i));
        }
        
        auto range = tree.range_query(3, 7);
        TestFramework::assert_equal(size_t(5), range.size(), "Range query should return correct count");
        
        for (const auto& pair : range) {
            TestFramework::assert_true(pair.first >= 3 && pair.first <= 7, "Range query should respect bounds");
        }
    }
}