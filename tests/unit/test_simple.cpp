#include "../../core/containers/MinHeap.hpp"
#include "../../core/containers/HashMap.hpp"
#include "../../core/containers/LinkedList.hpp"
#include "../../core/containers/Queue.hpp"
#include "../../core/containers/Stack.hpp"
#include "../../core/containers/LRUCache.hpp"
#include "../../core/trees/AVLTree.hpp"
#include "../../core/graph/WeightedGraph.hpp"
#include "../TestFramework.hpp"

void test_simple() {
    using namespace rts::core::containers;
    using namespace rts::core::trees;
    using namespace rts::core::graph;
    
    // Test MinHeap
    {
        MinHeap<int> heap;
        heap.push(5);
        heap.push(3);
        heap.push(8);
        heap.push(1);
        
        TestFramework::assert_equal_int(1, heap.top(), "MinHeap: smallest element should be on top");
        TestFramework::assert_equal_int(1, heap.pop(), "MinHeap: pop should return smallest");
        TestFramework::assert_equal_int(3, heap.pop(), "MinHeap: second smallest should be 3");
    }
    
    // Test HashMap
    {
        HashMap<int, std::string> map;
        map.put(1, "one");
        map.put(2, "two");
        map.put(3, "three");
        
        auto value = map.get(2);
        TestFramework::assert_not_null(value, "HashMap: should find key 2");
        TestFramework::assert_equal_string(std::string("two"), *value, "HashMap: should return correct value");
    }
    
    // Test LinkedList
    {
        LinkedList<int> list;
        list.push_back(1);
        list.push_back(2);
        list.push_back(3);
        
        TestFramework::assert_equal_size_t(size_t(3), list.size(), "LinkedList: should have 3 elements");
        TestFramework::assert_equal_int(1, list.front(), "LinkedList: front should be 1");
        TestFramework::assert_equal_int(3, list.back(), "LinkedList: back should be 3");
    }
    
    // Test Queue
    {
        Queue<int> queue;
        queue.enqueue(1);
        queue.enqueue(2);
        queue.enqueue(3);
        
        TestFramework::assert_equal_int(1, queue.dequeue(), "Queue: FIFO - first out should be 1");
        TestFramework::assert_equal_int(2, queue.dequeue(), "Queue: FIFO - second out should be 2");
    }
    
    // Test Stack
    {
        Stack<int> stack;
        stack.push(1);
        stack.push(2);
        stack.push(3);
        
        TestFramework::assert_equal_int(3, stack.pop(), "Stack: LIFO - first out should be 3");
        TestFramework::assert_equal_int(2, stack.pop(), "Stack: LIFO - second out should be 2");
    }
    
    // Test LRU Cache
    {
        LRUCache<int, std::string> cache(2);
        cache.put(1, "one");
        cache.put(2, "two");
        
        auto value = cache.get(1);
        TestFramework::assert_not_null(value, "LRUCache: should find key 1");
        TestFramework::assert_equal_string(std::string("one"), *value, "LRUCache: should return correct value");
    }
    
    // Test AVL Tree
    {
        AVLTree<int, std::string> tree;
        tree.insert(5, "five");
        tree.insert(3, "three");
        tree.insert(7, "seven");
        
        TestFramework::assert_equal_size_t(size_t(3), tree.size(), "AVLTree: should have 3 elements");
        
        auto value = tree.find(5);
        TestFramework::assert_not_null(value, "AVLTree: should find key 5");
        TestFramework::assert_equal_string(std::string("five"), *value, "AVLTree: should return correct value");
    }
    
    // Test WeightedGraph
    {
        WeightedGraph graph;
        
        Node node1(1, 40.7128, -74.0060);  // NYC
        Node node2(2, 34.0522, -118.2437); // LA
        
        TestFramework::assert_true(graph.add_node(node1), "Graph: should add node 1");
        TestFramework::assert_true(graph.add_node(node2), "Graph: should add node 2");
        TestFramework::assert_true(graph.add_edge(1, 2, 100.0f), "Graph: should add edge");
        
        TestFramework::assert_equal_size_t(size_t(2), graph.num_nodes(), "Graph: should have 2 nodes");
        TestFramework::assert_equal_size_t(size_t(1), graph.num_edges(), "Graph: should have 1 edge");
        TestFramework::assert_true(graph.has_edge(1, 2), "Graph: should contain edge 1->2");
    }
}