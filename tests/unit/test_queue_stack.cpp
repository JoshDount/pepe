#include "../../core/containers/Queue.hpp"
#include "../../core/containers/Stack.hpp"
#include "../TestFramework.hpp"

void test_queue_stack() {
    using namespace rts::core::containers;
    
    // Test Queue (FIFO)
    {
        Queue<int> queue;
        TestFramework::assert_true(queue.empty(), "New queue should be empty");
        
        queue.enqueue(1);
        queue.enqueue(2);
        queue.enqueue(3);
        
        TestFramework::assert_equal(size_t(3), queue.size(), "Queue should have 3 elements");
        TestFramework::assert_equal(1, queue.front(), "Front should be first inserted");
        TestFramework::assert_equal(3, queue.back(), "Back should be last inserted");
        
        TestFramework::assert_equal(1, queue.dequeue(), "Should dequeue in FIFO order");
        TestFramework::assert_equal(2, queue.dequeue(), "Should dequeue in FIFO order");
        TestFramework::assert_equal(1, queue.size(), "Size should decrease");
    }
    
    // Test Stack (LIFO)
    {
        Stack<int> stack;
        TestFramework::assert_true(stack.empty(), "New stack should be empty");
        
        stack.push(1);
        stack.push(2);
        stack.push(3);
        
        TestFramework::assert_equal(size_t(3), stack.size(), "Stack should have 3 elements");
        TestFramework::assert_equal(3, stack.top(), "Top should be last inserted");
        
        TestFramework::assert_equal(3, stack.pop(), "Should pop in LIFO order");
        TestFramework::assert_equal(2, stack.pop(), "Should pop in LIFO order");
        TestFramework::assert_equal(1, stack.size(), "Size should decrease");
    }
}