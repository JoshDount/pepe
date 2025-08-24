#include "../../core/containers/LRUCache.hpp"
#include "../TestFramework.hpp"

void test_lru_cache() {
    using namespace rts::core::containers;
    
    // Test basic LRU operations
    {
        LRUCache<int, std::string> cache(3);  // Capacity of 3
        
        cache.put(1, "one");
        cache.put(2, "two");
        cache.put(3, "three");
        
        TestFramework::assert_equal(size_t(3), cache.size(), "Cache should be at capacity");
        TestFramework::assert_true(cache.contains(1), "Should contain key 1");
        
        // Access key 1 to make it most recently used
        auto value = cache.get(1);
        TestFramework::assert_not_null(value, "Should find key 1");
        TestFramework::assert_equal(std::string("one"), *value, "Should return correct value");
        
        // Add another item, should evict least recently used (key 2)
        cache.put(4, "four");
        
        TestFramework::assert_true(!cache.contains(2), "Should have evicted key 2");
        TestFramework::assert_true(cache.contains(1), "Should still contain key 1 (recently used)");
        TestFramework::assert_true(cache.contains(4), "Should contain newly added key 4");
    }
}