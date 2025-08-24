#include "../../core/traffic/TrafficSimulation.hpp"
#include "../../core/graph/WeightedGraph.hpp"
#include "../../core/simulation/EventQueue.hpp"
#include "../TestFramework.hpp"

void test_traffic_simulation() {
    using namespace rts::core::traffic;
    using namespace rts::core::graph;
    using namespace rts::core::simulation;
    
    // Test 1: Basic traffic state functionality
    {
        TrafficState state;
        
        TestFramework::assert_equal_int(static_cast<int>(TrafficLevel::FREE_FLOW), static_cast<int>(state.level), "TrafficState: Should start with free flow");
        TestFramework::assert_equal_int(static_cast<int>(IncidentType::NONE), static_cast<int>(state.incident), "TrafficState: Should start with no incident");
        TestFramework::assert_equal_double(1.0, state.speed_factor, "TrafficState: Should start with normal speed");
        TestFramework::assert_equal_double(1.0, state.capacity_factor, "TrafficState: Should start with normal capacity");
        TestFramework::assert_true(!state.is_blocked(), "TrafficState: Should not be blocked initially");
        TestFramework::assert_equal_double(1.0, state.get_travel_time_multiplier(), "TrafficState: Should have normal travel time");
        TestFramework::assert_equal_double(0.0, state.get_severity(), "TrafficState: Should have zero severity");
        
        // Test blocking
        state.speed_factor = 0.0;
        TestFramework::assert_true(state.is_blocked(), "TrafficState: Should be blocked with zero speed");
        TestFramework::assert_true(std::isinf(state.get_travel_time_multiplier()), "TrafficState: Should have infinite travel time when blocked");
        TestFramework::assert_equal_double(1.0, state.get_severity(), "TrafficState: Should have maximum severity when blocked");
        
        // Test moderate traffic
        state.speed_factor = 0.6;
        state.level = TrafficLevel::MODERATE;
        TestFramework::assert_true(!state.is_blocked(), "TrafficState: Should not be blocked with moderate traffic");
        TestFramework::assert_equal_double(1.0/0.6, state.get_travel_time_multiplier(), "TrafficState: Should have correct travel time multiplier");
        TestFramework::assert_true(state.get_severity() > 0.0 && state.get_severity() < 1.0, "TrafficState: Should have moderate severity");
    }
    
    // Test 2: Traffic simulation initialization
    {
        WeightedGraph graph;
        EventQueue event_queue;
        
        // Create test graph
        graph.add_node(Node(1, 0.0, 0.0));
        graph.add_node(Node(2, 1.0, 0.0));
        graph.add_node(Node(3, 1.0, 1.0));
        graph.add_edge(1, 2, 10.0);
        graph.add_edge(2, 3, 15.0);
        graph.add_edge(1, 3, 20.0);
        
        TrafficSimulation traffic_sim(graph, event_queue, 42);
        
        // Check initial states
        auto state_1_2 = traffic_sim.get_traffic_state(1, 2);
        auto state_2_3 = traffic_sim.get_traffic_state(2, 3);
        auto state_1_3 = traffic_sim.get_traffic_state(1, 3);
        
        TestFramework::assert_true(state_1_2 != nullptr, "TrafficSimulation: Should have state for edge 1->2");
        TestFramework::assert_true(state_2_3 != nullptr, "TrafficSimulation: Should have state for edge 2->3");
        TestFramework::assert_true(state_1_3 != nullptr, "TrafficSimulation: Should have state for edge 1->3");
        
        TestFramework::assert_equal_int(static_cast<int>(TrafficLevel::FREE_FLOW), static_cast<int>(state_1_2->level), "TrafficSimulation: Should initialize with free flow");
        TestFramework::assert_equal_int(static_cast<int>(IncidentType::NONE), static_cast<int>(state_1_2->incident), "TrafficSimulation: Should initialize with no incidents");
        
        auto stats = traffic_sim.get_statistics();
        TestFramework::assert_equal_size_t(size_t(0), stats.total_incidents, "TrafficSimulation: Should start with zero incidents");
        TestFramework::assert_equal_size_t(size_t(0), stats.active_incidents, "TrafficSimulation: Should start with zero active incidents");
    }
    
    // Test 3: Manual incident triggering
    {
        WeightedGraph graph;
        EventQueue event_queue;
        
        graph.add_node(Node(1, 0.0, 0.0));
        graph.add_node(Node(2, 1.0, 0.0));
        graph.add_edge(1, 2, 10.0);
        
        TrafficSimulation traffic_sim(graph, event_queue, 42);
        
        // Trigger minor accident
        traffic_sim.trigger_incident(1, 2, IncidentType::MINOR_ACCIDENT, 30.0);
        
        auto state = traffic_sim.get_traffic_state(1, 2);
        TestFramework::assert_true(state != nullptr, "TrafficSimulation: Should have state after incident");
        TestFramework::assert_equal_int(static_cast<int>(IncidentType::MINOR_ACCIDENT), static_cast<int>(state->incident), "TrafficSimulation: Should have minor accident");
        TestFramework::assert_true(state->speed_factor < 1.0, "TrafficSimulation: Should reduce speed factor");
        TestFramework::assert_true(state->capacity_factor < 1.0, "TrafficSimulation: Should reduce capacity factor");
        TestFramework::assert_true(!state->description.empty(), "TrafficSimulation: Should have incident description");
        
        auto stats = traffic_sim.get_statistics();
        TestFramework::assert_equal_size_t(size_t(1), stats.total_incidents, "TrafficSimulation: Should count triggered incident");
        TestFramework::assert_equal_size_t(size_t(1), stats.active_incidents, "TrafficSimulation: Should have one active incident");
        
        // Check that event was scheduled for resolution
        TestFramework::assert_true(!event_queue.empty(), "TrafficSimulation: Should schedule incident resolution");
        TestFramework::assert_equal_double(30.0, event_queue.get_next_event_time(), "TrafficSimulation: Should schedule resolution at correct time");
        
        // Process incident resolution
        event_queue.process_next();
        
        state = traffic_sim.get_traffic_state(1, 2);
        TestFramework::assert_equal_int(static_cast<int>(IncidentType::NONE), static_cast<int>(state->incident), "TrafficSimulation: Should resolve incident");
        TestFramework::assert_true(state->description.empty(), "TrafficSimulation: Should clear incident description");
        
        auto updated_stats = traffic_sim.get_statistics();
        TestFramework::assert_equal_size_t(size_t(0), updated_stats.active_incidents, "TrafficSimulation: Should have zero active incidents after resolution");
    }
    
    // Test 4: Different incident types and their impacts
    {
        WeightedGraph graph;
        EventQueue event_queue;
        
        graph.add_node(Node(1, 0.0, 0.0));
        graph.add_node(Node(2, 1.0, 0.0));
        graph.add_edge(1, 2, 10.0);
        
        TrafficSimulation traffic_sim(graph, event_queue, 42);
        
        // Test emergency closure (complete blocking)
        traffic_sim.trigger_incident(1, 2, IncidentType::EMERGENCY_CLOSURE, 60.0);
        auto state = traffic_sim.get_traffic_state(1, 2);
        TestFramework::assert_true(state->is_blocked(), "TrafficSimulation: Emergency closure should block road");
        TestFramework::assert_equal_double(0.0, state->speed_factor, "TrafficSimulation: Emergency closure should have zero speed");
        
        // Resolve and test major accident
        event_queue.process_next();
        traffic_sim.trigger_incident(1, 2, IncidentType::MAJOR_ACCIDENT, 90.0);
        state = traffic_sim.get_traffic_state(1, 2);
        TestFramework::assert_true(!state->is_blocked(), "TrafficSimulation: Major accident should not completely block");
        TestFramework::assert_true(state->speed_factor < 0.5, "TrafficSimulation: Major accident should severely reduce speed");
        
        // Resolve and test vehicle breakdown
        event_queue.process_next();
        traffic_sim.trigger_incident(1, 2, IncidentType::VEHICLE_BREAKDOWN, 15.0);
        state = traffic_sim.get_traffic_state(1, 2);
        TestFramework::assert_true(state->speed_factor > 0.5, "TrafficSimulation: Vehicle breakdown should have moderate impact");
        
        event_queue.process_next(); // Resolve breakdown
        
        auto final_stats = traffic_sim.get_statistics();
        TestFramework::assert_equal_size_t(size_t(3), final_stats.total_incidents, "TrafficSimulation: Should count all triggered incidents");
        TestFramework::assert_equal_size_t(size_t(0), final_stats.active_incidents, "TrafficSimulation: Should have no active incidents");
    }
    
    // Test 5: Blocked roads detection
    {
        WeightedGraph graph;
        EventQueue event_queue;
        
        graph.add_node(Node(1, 0.0, 0.0));
        graph.add_node(Node(2, 1.0, 0.0));
        graph.add_node(Node(3, 1.0, 1.0));
        graph.add_edge(1, 2, 10.0);
        graph.add_edge(2, 3, 15.0);
        graph.add_edge(1, 3, 20.0);
        
        TrafficSimulation traffic_sim(graph, event_queue, 42);
        
        auto initial_blocked = traffic_sim.get_blocked_roads();
        TestFramework::assert_equal_size_t(size_t(0), initial_blocked.size(), "TrafficSimulation: Should have no blocked roads initially");
        
        // Block two roads
        traffic_sim.trigger_incident(1, 2, IncidentType::EMERGENCY_CLOSURE, 60.0);
        traffic_sim.trigger_incident(2, 3, IncidentType::WEATHER_CLOSURE, 120.0);
        
        auto blocked_roads = traffic_sim.get_blocked_roads();
        TestFramework::assert_equal_size_t(size_t(2), blocked_roads.size(), "TrafficSimulation: Should detect two blocked roads");
        
        // Check that the correct roads are blocked
        bool found_1_2 = false, found_2_3 = false;
        for (const auto& [from, to] : blocked_roads) {
            if (from == 1 && to == 2) found_1_2 = true;
            if (from == 2 && to == 3) found_2_3 = true;
        }
        TestFramework::assert_true(found_1_2, "TrafficSimulation: Should block road 1->2");
        TestFramework::assert_true(found_2_3, "TrafficSimulation: Should block road 2->3");
    }
    
    // Test 6: Weather factor effects
    {
        WeightedGraph graph;
        EventQueue event_queue;
        
        graph.add_node(Node(1, 0.0, 0.0));
        graph.add_node(Node(2, 1.0, 0.0));
        graph.add_edge(1, 2, 10.0);
        
        TrafficSimulation traffic_sim(graph, event_queue, 42);
        auto params = traffic_sim.get_parameters();
        
        // Test normal weather
        TestFramework::assert_equal_double(1.0, params.weather_factor, "TrafficSimulation: Should start with normal weather");
        
        // Set bad weather
        traffic_sim.set_weather_factor(2.0);
        auto updated_params = traffic_sim.get_parameters();
        TestFramework::assert_equal_double(2.0, updated_params.weather_factor, "TrafficSimulation: Should update weather factor");
    }
    
    // Test 7: Traffic simulation with periodic updates
    {
        WeightedGraph graph;
        EventQueue event_queue;
        
        graph.add_node(Node(1, 0.0, 0.0));
        graph.add_node(Node(2, 1.0, 0.0));
        graph.add_node(Node(3, 1.0, 1.0));
        graph.add_edge(1, 2, 10.0);
        graph.add_edge(2, 3, 15.0);
        
        TrafficSimulation traffic_sim(graph, event_queue, 42);
        
        // Start simulation with 10-minute update intervals
        traffic_sim.start_simulation(10.0);
        
        TestFramework::assert_true(!event_queue.empty(), "TrafficSimulation: Should schedule update events");
        TestFramework::assert_equal_double(10.0, event_queue.get_next_event_time(), "TrafficSimulation: Should schedule first update at correct time");
        
        // Process several update cycles
        size_t initial_events = event_queue.size();
        TestFramework::assert_true(initial_events > 0, "TrafficSimulation: Should have scheduled initial update events");
        
        // Process one update cycle
        auto processed_event = event_queue.process_next();
        TestFramework::assert_true(processed_event != nullptr, "TrafficSimulation: Should process update event");
        TestFramework::assert_equal_string("TrafficUpdate", processed_event->event_type, "TrafficSimulation: Should process TrafficUpdate event");
        
        // Should schedule next update
        TestFramework::assert_true(!event_queue.empty(), "TrafficSimulation: Should schedule next update");
    }
    
    // Test 8: Graph edge weight updates
    {
        WeightedGraph graph;
        EventQueue event_queue;
        
        graph.add_node(Node(1, 0.0, 0.0));
        graph.add_node(Node(2, 1.0, 0.0));
        graph.add_edge(1, 2, 10.0);
        
        TrafficSimulation traffic_sim(graph, event_queue, 42);
        
        // Get initial edge weight
        auto initial_neighbors = graph.get_neighbors(1);
        float initial_weight = 0.0;
        for (const auto& edge : initial_neighbors) {
            if (edge.to == 2) {
                initial_weight = edge.weight;
                break;
            }
        }
        TestFramework::assert_equal_float(10.0f, initial_weight, "TrafficSimulation: Should have initial edge weight");
        
        // Trigger incident that affects travel time
        traffic_sim.trigger_incident(1, 2, IncidentType::MAJOR_ACCIDENT, 60.0);
        
        // Check updated edge weight
        auto updated_neighbors = graph.get_neighbors(1);
        float updated_weight = 0.0;
        for (const auto& edge : updated_neighbors) {
            if (edge.to == 2) {
                updated_weight = edge.weight;
                break;
            }
        }
        TestFramework::assert_true(updated_weight > initial_weight, "TrafficSimulation: Should increase edge weight due to incident");
        
        // Resolve incident
        event_queue.process_next();
        
        // Check weight restoration (should be close to original, but might have some traffic level effects)
        auto final_neighbors = graph.get_neighbors(1);
        float final_weight = 0.0;
        for (const auto& edge : final_neighbors) {
            if (edge.to == 2) {
                final_weight = edge.weight;
                break;
            }
        }
        TestFramework::assert_true(final_weight < updated_weight, "TrafficSimulation: Should reduce edge weight after incident resolution");
    }
    
    // Test 9: Simulation reset
    {
        WeightedGraph graph;
        EventQueue event_queue;
        
        graph.add_node(Node(1, 0.0, 0.0));
        graph.add_node(Node(2, 1.0, 0.0));
        graph.add_edge(1, 2, 10.0);
        
        TrafficSimulation traffic_sim(graph, event_queue, 42);
        
        // Create some traffic conditions
        traffic_sim.trigger_incident(1, 2, IncidentType::MINOR_ACCIDENT, 30.0);
        
        auto state_before = traffic_sim.get_traffic_state(1, 2);
        TestFramework::assert_equal_int(static_cast<int>(IncidentType::MINOR_ACCIDENT), static_cast<int>(state_before->incident), "TrafficSimulation: Should have incident before reset");
        
        auto stats_before = traffic_sim.get_statistics();
        TestFramework::assert_equal_size_t(size_t(1), stats_before.total_incidents, "TrafficSimulation: Should have incidents before reset");
        
        // Reset simulation
        traffic_sim.reset();
        
        auto state_after = traffic_sim.get_traffic_state(1, 2);
        TestFramework::assert_equal_int(static_cast<int>(IncidentType::NONE), static_cast<int>(state_after->incident), "TrafficSimulation: Should clear incidents after reset");
        TestFramework::assert_equal_int(static_cast<int>(TrafficLevel::FREE_FLOW), static_cast<int>(state_after->level), "TrafficSimulation: Should reset traffic level");
        
        auto stats_after = traffic_sim.get_statistics();
        TestFramework::assert_equal_size_t(size_t(0), stats_after.total_incidents, "TrafficSimulation: Should reset incident count");
        TestFramework::assert_equal_size_t(size_t(0), stats_after.active_incidents, "TrafficSimulation: Should reset active incident count");
    }
    
    // Test 10: Traffic report generation
    {
        WeightedGraph graph;
        EventQueue event_queue;
        
        graph.add_node(Node(1, 0.0, 0.0));
        graph.add_node(Node(2, 1.0, 0.0));
        graph.add_node(Node(3, 1.0, 1.0));
        graph.add_edge(1, 2, 10.0);
        graph.add_edge(2, 3, 15.0);
        
        TrafficSimulation traffic_sim(graph, event_queue, 42);
        
        // Create some incidents for reporting
        traffic_sim.trigger_incident(1, 2, IncidentType::MINOR_ACCIDENT, 30.0);
        traffic_sim.trigger_incident(2, 3, IncidentType::MAJOR_ACCIDENT, 60.0);
        
        std::string report = traffic_sim.generate_traffic_report();
        
        TestFramework::assert_true(!report.empty(), "TrafficSimulation: Should generate non-empty report");
        TestFramework::assert_true(report.find("Traffic Simulation Report") != std::string::npos, "TrafficSimulation: Should contain report header");
        TestFramework::assert_true(report.find("Total Incidents: 2") != std::string::npos, "TrafficSimulation: Should show correct incident count");
        TestFramework::assert_true(report.find("Active Incidents: 2") != std::string::npos, "TrafficSimulation: Should show correct active incidents");
        TestFramework::assert_true(report.find("Incident Types:") != std::string::npos, "TrafficSimulation: Should include incident type breakdown");
    }
    
    // Test 11: Edge cases and error handling
    {
        WeightedGraph graph;
        EventQueue event_queue;
        
        graph.add_node(Node(1, 0.0, 0.0));
        graph.add_node(Node(2, 1.0, 0.0));
        graph.add_edge(1, 2, 10.0);
        
        TrafficSimulation traffic_sim(graph, event_queue, 42);
        
        // Test non-existent edge
        auto non_existent_state = traffic_sim.get_traffic_state(1, 99);
        TestFramework::assert_true(non_existent_state == nullptr, "TrafficSimulation: Should return null for non-existent edge");
        
        // Test triggering incident on non-existent edge (should not crash)
        traffic_sim.trigger_incident(1, 99, IncidentType::MINOR_ACCIDENT, 30.0);
        auto stats = traffic_sim.get_statistics();
        TestFramework::assert_equal_size_t(size_t(0), stats.total_incidents, "TrafficSimulation: Should not count incidents on non-existent edges");
        
        // Test multiple incidents on same edge
        traffic_sim.trigger_incident(1, 2, IncidentType::MINOR_ACCIDENT, 30.0);
        traffic_sim.trigger_incident(1, 2, IncidentType::MAJOR_ACCIDENT, 60.0); // Should overwrite
        
        auto state = traffic_sim.get_traffic_state(1, 2);
        TestFramework::assert_equal_int(static_cast<int>(IncidentType::MAJOR_ACCIDENT), static_cast<int>(state->incident), "TrafficSimulation: Should overwrite previous incident");
        
        auto final_stats = traffic_sim.get_statistics();
        TestFramework::assert_equal_size_t(size_t(2), final_stats.total_incidents, "TrafficSimulation: Should count both incident triggers");
    }
    
    // Test 12: Traffic level progression and dynamics
    {
        WeightedGraph graph;
        EventQueue event_queue;
        
        graph.add_node(Node(1, 0.0, 0.0));
        graph.add_node(Node(2, 1.0, 0.0));
        graph.add_edge(1, 2, 10.0);
        
        TrafficSimulation traffic_sim(graph, event_queue, 42);
        
        // Start simulation to trigger traffic updates
        traffic_sim.start_simulation(5.0);
        
        // Process multiple update cycles to see traffic level changes
        size_t updates_processed = 0;
        while (!event_queue.empty() && updates_processed < 10) {
            auto event = event_queue.process_next();
            if (event && event->event_type == "TrafficUpdate") {
                updates_processed++;
            }
        }
        
        TestFramework::assert_true(updates_processed > 0, "TrafficSimulation: Should process traffic update events");
        
        // Check that traffic statistics are being updated
        auto stats = traffic_sim.get_statistics();
        TestFramework::assert_true(stats.traffic_level_counts.size() > 0, "TrafficSimulation: Should track traffic level counts");
        
        // The specific traffic levels depend on random simulation, but we can verify the system is working
        auto state = traffic_sim.get_traffic_state(1, 2);
        TestFramework::assert_true(state != nullptr, "TrafficSimulation: Should maintain traffic state");
        TestFramework::assert_true(static_cast<int>(state->level) >= 0 && static_cast<int>(state->level) <= 4, "TrafficSimulation: Should have valid traffic level");
    }
}