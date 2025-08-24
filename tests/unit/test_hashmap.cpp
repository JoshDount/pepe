#include "../../core/containers/HashMap.hpp"
#include "../TestFramework.hpp"
#include <string>
#include <vector>

void test_hashmap() {
    using namespace rts::core::containers;
    
    // Test both collision strategies
    std::vector<HashMap<int, std::string>::CollisionStrategy> strategies = {
        HashMap<int, std::string>::CollisionStrategy::LINEAR_PROBING,
        HashMap<int, std::string>::CollisionStrategy::SEPARATE_CHAINING
    };
    
    for (auto strategy : strategies) {
        std::string strategy_name = (strategy == HashMap<int, std::string>::CollisionStrategy::LINEAR_PROBING) 
            ? "Linear Probing" : "Separate Chaining";
        
        // Test 1: Basic operations
        {
            HashMap<int, std::string> map(strategy);
            TestFramework::assert_true(map.empty(), strategy_name + " - New map should be empty");
            TestFramework::assert_equal(size_t(0), map.size(), strategy_name + " - New map size should be 0");
            
            // Insert
            TestFramework::assert_true(map.put(1, "one"), strategy_name + " - Should insert new key");
            TestFramework::assert_true(!map.empty(), strategy_name + " - Map should not be empty after insert");
            TestFramework::assert_equal(size_t(1), map.size(), strategy_name + " - Size should be 1 after insert");
            
            // Get
            auto value = map.get(1);
            TestFramework::assert_not_null(value, strategy_name + " - Should find inserted key");
            TestFramework::assert_equal(std::string("one"), *value, strategy_name + " - Should return correct value");
            
            // Contains
            TestFramework::assert_true(map.contains(1), strategy_name + " - Should contain inserted key");
            TestFramework::assert_true(!map.contains(2), strategy_name + " - Should not contain non-existent key");
        }
        
        // Test 2: Update existing key
        {
            HashMap<int, std::string> map(strategy);
            map.put(1, "one");
            
            TestFramework::assert_true(!map.put(1, "ONE"), strategy_name + " - Should update existing key");
            TestFramework::assert_equal(size_t(1), map.size(), strategy_name + " - Size should remain 1 after update");
            
            auto value = map.get(1);
            TestFramework::assert_equal(std::string("ONE"), *value, strategy_name + " - Should return updated value");
        }
        
        // Test 3: Multiple insertions
        {
            HashMap<int, std::string> map(strategy);
            
            for (int i = 0; i < 10; ++i) {
                map.put(i, "value" + std::to_string(i));
            }
            
            TestFramework::assert_equal(size_t(10), map.size(), strategy_name + " - Should contain all inserted elements");
            
            for (int i = 0; i < 10; ++i) {
                TestFramework::assert_true(map.contains(i), strategy_name + " - Should contain key " + std::to_string(i));
                auto value = map.get(i);
                TestFramework::assert_not_null(value, strategy_name + " - Should find key " + std::to_string(i));
                TestFramework::assert_equal(std::string("value" + std::to_string(i)), *value, 
                                          strategy_name + " - Should have correct value for key " + std::to_string(i));
            }
        }
        
        // Test 4: Erase operations
        {
            HashMap<int, std::string> map(strategy);
            
            for (int i = 0; i < 5; ++i) {
                map.put(i, "value" + std::to_string(i));
            }
            
            TestFramework::assert_true(map.erase(2), strategy_name + " - Should erase existing key");
            TestFramework::assert_true(!map.contains(2), strategy_name + " - Should not contain erased key");
            TestFramework::assert_equal(size_t(4), map.size(), strategy_name + " - Size should decrease after erase");
            
            TestFramework::assert_true(!map.erase(2), strategy_name + " - Should not erase non-existent key");
            TestFramework::assert_equal(size_t(4), map.size(), strategy_name + " - Size should not change for non-existent erase");
        }
        
        // Test 5: Operator[] access
        {
            HashMap<int, std::string> map(strategy);
            
            map[1] = "one";
            map[2] = "two";
            
            TestFramework::assert_equal(std::string("one"), map[1], strategy_name + " - Operator[] should return correct value");
            TestFramework::assert_equal(std::string("two"), map[2], strategy_name + " - Operator[] should return correct value");
            
            // Accessing non-existent key should create it
            std::string val = map[3];  // Should create with default value
            TestFramework::assert_true(map.contains(3), strategy_name + " - Operator[] should create non-existent key");
        }
        
        // Test 6: Load factor and resizing
        {
            HashMap<int, std::string> map(strategy, 4, 0.75);  // Small initial capacity
            
            // Fill beyond initial capacity to trigger resize
            for (int i = 0; i < 10; ++i) {
                map.put(i, "value" + std::to_string(i));
            }
            
            TestFramework::assert_equal(size_t(10), map.size(), strategy_name + " - Should contain all elements after resize");
            
            for (int i = 0; i < 10; ++i) {
                TestFramework::assert_true(map.contains(i), strategy_name + " - Should contain key after resize");
            }
            
            auto stats = map.get_statistics();
            TestFramework::assert_true(stats.load_factor <= 0.75, strategy_name + " - Load factor should be within limit");
        }
        
        // Test 7: Keys retrieval
        {
            HashMap<int, std::string> map(strategy);
            std::vector<int> inserted_keys = {5, 2, 8, 1, 9};
            
            for (int key : inserted_keys) {
                map.put(key, "value" + std::to_string(key));
            }
            
            auto keys = map.keys();
            TestFramework::assert_equal(inserted_keys.size(), keys.size(), strategy_name + " - Keys should have correct size");
            
            // Check that all inserted keys are in the returned keys
            for (int key : inserted_keys) {
                TestFramework::assert_true(std::find(keys.begin(), keys.end(), key) != keys.end(),
                                          strategy_name + " - Keys should contain " + std::to_string(key));
            }
        }
        
        // Test 8: Clear operation
        {
            HashMap<int, std::string> map(strategy);
            
            for (int i = 0; i < 5; ++i) {
                map.put(i, "value" + std::to_string(i));
            }
            
            TestFramework::assert_equal(size_t(5), map.size(), strategy_name + " - Should have 5 elements before clear");
            
            map.clear();
            TestFramework::assert_equal(size_t(0), map.size(), strategy_name + " - Should be empty after clear");
            TestFramework::assert_true(map.empty(), strategy_name + " - Should be empty after clear");
            
            for (int i = 0; i < 5; ++i) {
                TestFramework::assert_true(!map.contains(i), strategy_name + " - Should not contain any keys after clear");
            }
        }
        
        // Test 9: Statistics
        {
            HashMap<int, std::string> map(strategy);
            
            for (int i = 0; i < 20; ++i) {
                map.put(i, "value" + std::to_string(i));
            }
            
            auto stats = map.get_statistics();
            TestFramework::assert_equal(size_t(20), stats.total_elements, strategy_name + " - Statistics should show correct count");
            TestFramework::assert_true(stats.memory_usage_bytes > 0, strategy_name + " - Should report memory usage");
            TestFramework::assert_equal(strategy, stats.strategy, strategy_name + " - Should report correct strategy");
            
            if (strategy == HashMap<int, std::string>::CollisionStrategy::LINEAR_PROBING) {
                TestFramework::assert_true(stats.avg_probe_length >= 1.0, strategy_name + " - Should have reasonable probe length");
            } else {
                TestFramework::assert_true(stats.max_chain_length >= 0, strategy_name + " - Should report chain statistics");
            }
        }
    }
    
    // Test 10: Collision handling stress test
    {
        // Use a hash map with very small capacity to force collisions
        HashMap<int, std::string> map(HashMap<int, std::string>::CollisionStrategy::SEPARATE_CHAINING, 2, 0.75);
        
        // Insert many elements to force heavy collision handling
        for (int i = 0; i < 50; ++i) {
            map.put(i, "value" + std::to_string(i));
        }
        
        TestFramework::assert_equal(size_t(50), map.size(), "Collision stress test - Should handle many collisions");
        
        // Verify all elements are still accessible
        for (int i = 0; i < 50; ++i) {
            auto value = map.get(i);
            TestFramework::assert_not_null(value, "Collision stress test - Should find all keys");
            TestFramework::assert_equal(std::string("value" + std::to_string(i)), *value, 
                                      "Collision stress test - Should have correct values");
        }
        
        auto stats = map.get_statistics();
        TestFramework::assert_true(stats.collision_count > 0, "Collision stress test - Should have recorded collisions");
    }
}