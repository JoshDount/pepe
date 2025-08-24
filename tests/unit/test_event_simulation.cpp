#include "../../core/simulation/EventQueue.hpp"
#include "../../core/simulation/Events.hpp"
#include "../TestFramework.hpp"
#include <memory>
#include <vector>

void test_discrete_event_simulation() {
    using namespace rts::core::simulation;
    
    // Test 1: Basic EventQueue functionality
    {
        EventQueue queue;
        
        TestFramework::assert_true(queue.empty(), "EventQueue: Should start empty");
        TestFramework::assert_equal_size_t(size_t(0), queue.size(), "EventQueue: Should have size 0");
        TestFramework::assert_equal_double(0.0, queue.get_current_time(), "EventQueue: Should start at time 0");
        
        // Test event scheduling
        int counter = 0;
        auto event1 = std::make_shared<CounterEvent>(10.0, &counter, 5);
        auto event2 = std::make_shared<CounterEvent>(5.0, &counter, 3);
        auto event3 = std::make_shared<CounterEvent>(15.0, &counter, 2);
        
        uint32_t id1 = queue.schedule_event(event1);
        uint32_t id2 = queue.schedule_event(event2);
        uint32_t id3 = queue.schedule_event(event3);
        
        TestFramework::assert_true(id1 != id2 && id2 != id3 && id1 != id3, "EventQueue: Should generate unique IDs");
        TestFramework::assert_true(!queue.empty(), "EventQueue: Should not be empty after scheduling");
        TestFramework::assert_equal_size_t(size_t(3), queue.size(), "EventQueue: Should have 3 events");
        
        // Test event ordering (should process earliest time first)
        auto next = queue.peek_next();
        TestFramework::assert_true(next != nullptr, "EventQueue: Should have next event");
        TestFramework::assert_equal_double(5.0, next->scheduled_time, "EventQueue: Next event should be at time 5.0");
        
        // Process events and verify order
        auto processed1 = queue.process_next();
        TestFramework::assert_equal_double(5.0, processed1->scheduled_time, "EventQueue: First processed should be time 5.0");
        TestFramework::assert_equal_double(5.0, queue.get_current_time(), "EventQueue: Current time should be 5.0");
        TestFramework::assert_equal_int(3, counter, "EventQueue: Counter should be 3 after first event");
        
        auto processed2 = queue.process_next();
        TestFramework::assert_equal_double(10.0, processed2->scheduled_time, "EventQueue: Second processed should be time 10.0");
        TestFramework::assert_equal_double(10.0, queue.get_current_time(), "EventQueue: Current time should be 10.0");
        TestFramework::assert_equal_int(8, counter, "EventQueue: Counter should be 8 after second event");
        
        auto processed3 = queue.process_next();
        TestFramework::assert_equal_double(15.0, processed3->scheduled_time, "EventQueue: Third processed should be time 15.0");
        TestFramework::assert_equal_double(15.0, queue.get_current_time(), "EventQueue: Current time should be 15.0");
        TestFramework::assert_equal_int(10, counter, "EventQueue: Counter should be 10 after all events");
        
        TestFramework::assert_true(queue.empty(), "EventQueue: Should be empty after processing all events");
    }
    
    // Test 2: Event priority handling
    {
        EventQueue queue;
        std::vector<int> execution_order;
        
        // Create events at same time with different priorities
        auto event1 = std::make_shared<MessageEvent>(10.0, "High priority", 1);   // Lower number = higher priority
        auto event2 = std::make_shared<MessageEvent>(10.0, "Low priority", 5);
        auto event3 = std::make_shared<MessageEvent>(10.0, "Medium priority", 3);
        
        queue.schedule_event(event2); // Schedule in non-priority order
        queue.schedule_event(event1);
        queue.schedule_event(event3);
        
        // Process and verify priority order
        auto proc1 = queue.process_next();
        auto proc2 = queue.process_next();
        auto proc3 = queue.process_next();
        
        TestFramework::assert_equal_int(1, proc1->priority, "EventQueue: First processed should have priority 1");
        TestFramework::assert_equal_int(3, proc2->priority, "EventQueue: Second processed should have priority 3");
        TestFramework::assert_equal_int(5, proc3->priority, "EventQueue: Third processed should have priority 5");
    }
    
    // Test 3: process_until functionality
    {
        EventQueue queue;
        int counter = 0;
        
        queue.schedule_event(std::make_shared<CounterEvent>(5.0, &counter, 1));
        queue.schedule_event(std::make_shared<CounterEvent>(10.0, &counter, 1));
        queue.schedule_event(std::make_shared<CounterEvent>(15.0, &counter, 1));
        queue.schedule_event(std::make_shared<CounterEvent>(20.0, &counter, 1));
        
        size_t processed = queue.process_until(12.0);
        
        TestFramework::assert_equal_size_t(size_t(2), processed, "EventQueue: Should process 2 events until time 12.0");
        TestFramework::assert_equal_int(2, counter, "EventQueue: Counter should be 2 after partial processing");
        TestFramework::assert_equal_double(12.0, queue.get_current_time(), "EventQueue: Current time should be 12.0");
        TestFramework::assert_equal_size_t(size_t(2), queue.size(), "EventQueue: Should have 2 events remaining");
        
        // Process remaining events
        size_t remaining = queue.process_until(25.0);
        TestFramework::assert_equal_size_t(size_t(2), remaining, "EventQueue: Should process 2 remaining events");
        TestFramework::assert_equal_int(4, counter, "EventQueue: Counter should be 4 after all events");
    }
    
    // Test 4: process_events functionality
    {
        EventQueue queue;
        int counter = 0;
        
        for (int i = 1; i <= 5; ++i) {
            queue.schedule_event(std::make_shared<CounterEvent>(i * 5.0, &counter, 1));
        }
        
        size_t processed = queue.process_events(3);
        
        TestFramework::assert_equal_size_t(size_t(3), processed, "EventQueue: Should process exactly 3 events");
        TestFramework::assert_equal_int(3, counter, "EventQueue: Counter should be 3 after processing 3 events");
        TestFramework::assert_equal_size_t(size_t(2), queue.size(), "EventQueue: Should have 2 events remaining");
    }
    
    // Test 5: Event scheduling with delays
    {
        EventQueue queue;
        queue.set_current_time(100.0);
        
        int counter = 0;
        auto event = std::make_shared<CounterEvent>(0.0, &counter, 1); // Time will be overwritten
        
        uint32_t id = queue.schedule_after(25.0, event);
        
        TestFramework::assert_true(id > 0, "EventQueue: Should return valid event ID");
        TestFramework::assert_equal_double(125.0, queue.peek_next()->scheduled_time, "EventQueue: Event should be scheduled 25 units after current time");
        
        queue.process_next();
        TestFramework::assert_equal_int(1, counter, "EventQueue: Event should execute properly");
        TestFramework::assert_equal_double(125.0, queue.get_current_time(), "EventQueue: Current time should advance to event time");
    }
    
    // Test 6: Event cancellation by type
    {
        EventQueue queue;
        
        queue.schedule_event(std::make_shared<MessageEvent>(10.0, "Message 1"));
        queue.schedule_event(std::make_shared<MessageEvent>(20.0, "Message 2"));
        
        int counter = 0;
        queue.schedule_event(std::make_shared<CounterEvent>(15.0, &counter, 1));
        queue.schedule_event(std::make_shared<CounterEvent>(25.0, &counter, 1));
        
        TestFramework::assert_equal_size_t(size_t(4), queue.size(), "EventQueue: Should have 4 events before cancellation");
        
        size_t cancelled = queue.cancel_events_by_type("MessageEvent");
        
        TestFramework::assert_equal_size_t(size_t(2), cancelled, "EventQueue: Should cancel 2 MessageEvents");
        TestFramework::assert_equal_size_t(size_t(2), queue.size(), "EventQueue: Should have 2 events remaining");
        
        // Process remaining events and verify only CounterEvents remain
        queue.process_until(30.0);
        TestFramework::assert_equal_int(2, counter, "EventQueue: Only CounterEvents should remain and execute");
    }
    
    // Test 7: Statistics and debugging
    {
        EventQueue queue;
        
        queue.schedule_event(std::make_shared<MessageEvent>(10.0, "Test"));
        queue.schedule_event(std::make_shared<MessageEvent>(20.0, "Test"));
        
        int counter = 0;
        queue.schedule_event(std::make_shared<CounterEvent>(15.0, &counter, 1));
        
        auto stats = queue.get_statistics();
        
        TestFramework::assert_equal_size_t(size_t(3), stats.events_scheduled, "EventQueue: Should track scheduled events");
        TestFramework::assert_equal_size_t(size_t(0), stats.events_processed, "EventQueue: Should track processed events");
        TestFramework::assert_equal_size_t(size_t(3), stats.events_pending, "EventQueue: Should track pending events");
        TestFramework::assert_equal_double(0.0, stats.current_time, "EventQueue: Should track current time");
        TestFramework::assert_equal_double(10.0, stats.next_event_time, "EventQueue: Should track next event time");
        
        // Verify event type counts
        auto type_counts = stats.event_type_counts;
        TestFramework::assert_equal_size_t(size_t(2), type_counts["MessageEvent"], "EventQueue: Should count MessageEvents");
        TestFramework::assert_equal_size_t(size_t(1), type_counts["CounterEvent"], "EventQueue: Should count CounterEvents");
        
        // Process some events and check updated stats
        queue.process_events(2);
        
        auto updated_stats = queue.get_statistics();
        TestFramework::assert_equal_size_t(size_t(2), updated_stats.events_processed, "EventQueue: Should update processed count");
        TestFramework::assert_equal_size_t(size_t(1), updated_stats.events_pending, "EventQueue: Should update pending count");
        TestFramework::assert_true(updated_stats.get_processing_efficiency() > 0.5, "EventQueue: Should calculate processing efficiency");
    }
    
    // Test 8: Recurring events
    {
        EventQueue queue;
        int execution_count = 0;
        
        auto recurring = std::make_shared<RecurringEvent>(
            5.0, &queue, 10.0, 3, &execution_count);
        
        queue.schedule_event(recurring);
        
        // Process all events
        queue.process_until(50.0);
        
        TestFramework::assert_equal_int(3, execution_count, "EventQueue: Recurring event should execute 3 times");
        TestFramework::assert_true(queue.empty(), "EventQueue: Queue should be empty after recurring events complete");
        
        auto stats = queue.get_statistics();
        TestFramework::assert_equal_size_t(size_t(3), stats.events_processed, "EventQueue: Should process 3 recurring events");
    }
    
    // Test 9: Trigger events
    {
        EventQueue queue;
        int counter = 0;
        
        auto trigger = std::make_shared<TriggerEvent>(10.0, &queue);
        trigger->add_triggered_event(std::make_shared<CounterEvent>(0.0, &counter, 5));
        trigger->add_triggered_event(std::make_shared<CounterEvent>(0.0, &counter, 3));
        
        queue.schedule_event(trigger);
        
        TestFramework::assert_equal_size_t(size_t(1), queue.size(), "EventQueue: Should have 1 trigger event initially");
        
        // Process trigger event
        queue.process_next();
        
        TestFramework::assert_equal_size_t(size_t(2), queue.size(), "EventQueue: Should have 2 triggered events after trigger");
        
        // Process triggered events
        queue.process_until(15.0);
        
        TestFramework::assert_equal_int(8, counter, "EventQueue: Triggered events should execute (5 + 3 = 8)");
    }
    
    // Test 10: Conditional events
    {
        EventQueue queue;
        int counter = 0;
        bool condition = false;
        
        auto target_event = std::make_shared<CounterEvent>(0.0, &counter, 10);
        auto conditional = std::make_shared<ConditionalEvent>(
            10.0, [&condition](){ return condition; }, target_event);
        
        queue.schedule_event(conditional);
        
        // Process with condition false
        queue.process_next();
        TestFramework::assert_equal_int(0, counter, "EventQueue: Conditional event should not execute when condition is false");
        
        // Reset and test with condition true
        queue.reset();
        condition = true;
        counter = 0;
        
        queue.schedule_event(std::make_shared<ConditionalEvent>(
            10.0, [&condition](){ return condition; }, 
            std::make_shared<CounterEvent>(0.0, &counter, 10)));
        
        queue.process_next();
        TestFramework::assert_equal_int(10, counter, "EventQueue: Conditional event should execute when condition is true");
    }
    
    // Test 11: Event time windows
    {
        EventQueue queue;
        
        queue.schedule_event(std::make_shared<MessageEvent>(5.0, "Early"));
        queue.schedule_event(std::make_shared<MessageEvent>(15.0, "Middle"));
        queue.schedule_event(std::make_shared<MessageEvent>(25.0, "Late"));
        
        auto window_events = queue.get_events_in_window(10.0, 20.0);
        
        TestFramework::assert_equal_size_t(size_t(1), window_events.size(), "EventQueue: Should find 1 event in window [10, 20]");
        TestFramework::assert_equal_double(15.0, window_events[0]->scheduled_time, "EventQueue: Window event should be at time 15.0");
    }
    
    // Test 12: Error handling
    {
        EventQueue queue;
        
        try {
            queue.schedule_event(nullptr);
            TestFramework::assert_true(false, "EventQueue: Should throw exception for null event");
        } catch (const std::invalid_argument&) {
            TestFramework::assert_true(true, "EventQueue: Should throw exception for null event");
        }
        
        queue.set_current_time(50.0);
        
        try {
            auto past_event = std::make_shared<MessageEvent>(25.0, "Past event");
            queue.schedule_event(past_event);
            TestFramework::assert_true(false, "EventQueue: Should throw exception for past event");
        } catch (const std::invalid_argument&) {
            TestFramework::assert_true(true, "EventQueue: Should throw exception for past event");
        }
        
        try {
            queue.schedule_after(-10.0, std::make_shared<MessageEvent>(0.0, "Negative delay"));
            TestFramework::assert_true(false, "EventQueue: Should throw exception for negative delay");
        } catch (const std::invalid_argument&) {
            TestFramework::assert_true(true, "EventQueue: Should throw exception for negative delay");
        }
    }
    
    // Test 13: Complete simulation test
    {
        SampleSimulation sim(100.0);
        
        TestFramework::assert_equal_double(100.0, sim.get_duration(), "Simulation: Should have correct duration");
        TestFramework::assert_true(!sim.is_running(), "Simulation: Should not be running initially");
        
        sim.run();
        
        TestFramework::assert_true(!sim.is_running(), "Simulation: Should not be running after completion");
        TestFramework::assert_equal_double(100.0, sim.get_current_time(), "Simulation: Should reach end time");
        
        // Verify simulation results
        TestFramework::assert_equal_int(15, sim.get_counter_value(), "Simulation: Counter should have correct final value");
        TestFramework::assert_equal_int(5, sim.get_recurring_executions(), "Simulation: Should have correct recurring executions");
        
        auto& queue = sim.get_event_queue();
        auto stats = queue.get_statistics();
        
        TestFramework::assert_true(stats.events_processed > 0, "Simulation: Should have processed events");
        TestFramework::assert_true(stats.get_processing_efficiency() > 0.8, "Simulation: Should have high processing efficiency");
    }
    
    // Test 14: Queue reset functionality
    {
        EventQueue queue;
        
        queue.schedule_event(std::make_shared<MessageEvent>(10.0, "Test"));
        queue.schedule_event(std::make_shared<MessageEvent>(20.0, "Test"));
        queue.set_current_time(15.0);
        
        TestFramework::assert_true(!queue.empty(), "EventQueue: Should have events before reset");
        TestFramework::assert_equal_double(15.0, queue.get_current_time(), "EventQueue: Should have advanced time before reset");
        
        queue.reset();
        
        TestFramework::assert_true(queue.empty(), "EventQueue: Should be empty after reset");
        TestFramework::assert_equal_double(0.0, queue.get_current_time(), "EventQueue: Should reset time to 0");
        
        auto stats = queue.get_statistics();
        TestFramework::assert_equal_size_t(size_t(0), stats.events_scheduled, "EventQueue: Should reset all counters");
        TestFramework::assert_equal_size_t(size_t(0), stats.events_processed, "EventQueue: Should reset all counters");
    }
    
    // Test 15: Debug information
    {
        EventQueue queue;
        
        queue.schedule_event(std::make_shared<MessageEvent>(10.0, "Debug test"));
        int counter = 0;
        queue.schedule_event(std::make_shared<CounterEvent>(20.0, &counter, 1));
        
        std::string debug_info = queue.get_debug_info();
        
        TestFramework::assert_true(!debug_info.empty(), "EventQueue: Should provide debug information");
        TestFramework::assert_true(debug_info.find("EventQueue Debug Info") != std::string::npos, "EventQueue: Debug info should contain header");
        TestFramework::assert_true(debug_info.find("Events Pending: 2") != std::string::npos, "EventQueue: Debug info should show pending events");
        TestFramework::assert_true(debug_info.find("MessageEvent: 1") != std::string::npos, "EventQueue: Debug info should show event type counts");
    }
}