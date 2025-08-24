#include "../viz/VisualizationFramework.hpp"
#include "../viz/AlgorithmVisualizer.hpp"
#include "../viz/DataStructureVisualizer.hpp"
#include "../viz/SimulationVisualizer.hpp"
#include "../core/graph/WeightedGraph.hpp"
#include "../core/containers/MinHeap.hpp"
#include "../core/containers/HashMap.hpp"
#include "../core/simulation/EventQueue.hpp"
#include "../core/traffic/TrafficSimulation.hpp"
#include <iostream>
#include <thread>
#include <chrono>

/**
 * @brief Test visualization components
 */
int main() {
    std::cout << "Route Transit Simulator - Visualization Test\n";
    std::cout << std::string(50, '=') << "\n\n";
    
    try {
        // Test basic visualization framework
        {
            std::cout << "Testing Basic Visualization Framework...\n";
            rts::viz::Visualizer basic_viz("Test Visualization", 80, 20);
            basic_viz.print_title();
            basic_viz.print_centered("Basic visualization working!", rts::viz::Colors::GREEN);
            basic_viz.print_separator();
            
            // Test progress bar
            std::cout << "Progress: " << basic_viz.create_progress_bar(0.7) << " 70%\n\n";
        }
        
        // Test data structure visualization
        {
            std::cout << "Testing Data Structure Visualization...\n";
            
            // Create and populate a MinHeap
            rts::core::containers::MinHeap<int> heap;
            heap.push(15);
            heap.push(10);
            heap.push(8);
            heap.push(5);
            heap.push(3);
            heap.push(12);
            
            rts::viz::MinHeapVisualizer heap_viz(&heap, "Test MinHeap");
            heap_viz.visualize_array();
            std::cout << "\n";
            
            // Test HashMap visualization
            rts::core::containers::HashMap<int, std::string> map;
            map.put(1, "One");
            map.put(2, "Two");
            map.put(15, "Fifteen");  // Will cause collision if bucket size is small
            
            rts::viz::HashMapVisualizer<int, std::string> map_viz(&map, "Test HashMap");
            map_viz.visualize_buckets();
            std::cout << "\n";
        }
        
        // Test graph algorithm visualization
        {
            std::cout << "Testing Algorithm Visualization...\n";
            
            // Create a small test graph
            rts::core::graph::WeightedGraph graph;
            graph.add_node(rts::core::graph::Node(1, 40.7128, -74.0060));  // NYC
            graph.add_node(rts::core::graph::Node(2, 40.7589, -73.9851));  // Times Square
            graph.add_node(rts::core::graph::Node(3, 40.6892, -74.0445));  // Statue of Liberty
            graph.add_node(rts::core::graph::Node(4, 40.7505, -73.9934));  // Empire State
            
            graph.add_edge(1, 2, 2.5f);
            graph.add_edge(1, 3, 8.0f);
            graph.add_edge(2, 4, 1.2f);
            graph.add_edge(3, 4, 7.5f);
            graph.add_edge(2, 3, 6.0f);
            
            rts::viz::GraphAlgorithmVisualizer graph_viz(&graph, "Test Graph");
            
            // Test BFS visualization
            std::cout << "BFS Traversal from node 1:\n";
            graph_viz.visualize_bfs(1);
            std::cout << "\n";
        }
        
        // Test simulation visualization
        {
            std::cout << "Testing Simulation Visualization...\n";
            
            // Create simulation components
            rts::core::simulation::EventQueue event_queue;
            rts::core::graph::WeightedGraph graph;
            
            // Setup a simple graph
            graph.add_node(rts::core::graph::Node(1, 40.7128, -74.0060));
            graph.add_node(rts::core::graph::Node(2, 40.7589, -73.9851));
            graph.add_edge(1, 2, 10.0f);
            
            // Create traffic simulation
            rts::core::traffic::TrafficSimulation traffic_sim(graph, event_queue, 42);
            
            // Test event queue visualization
            rts::viz::EventQueueVisualizer event_viz(&event_queue, "Test Event Queue");
            event_viz.visualize_current_state();
            std::cout << "\n";
            
            // Test traffic simulation visualization
            rts::viz::TrafficSimulationVisualizer traffic_viz(&traffic_sim, &graph, "Test Traffic");
            traffic_viz.visualize_traffic_state();
            std::cout << "\n";
            
            // Test simulation dashboard
            rts::viz::SimulationDashboard dashboard(&event_queue, &traffic_sim, &graph);
            dashboard.show_dashboard();
            std::cout << "\n";
        }
        
        std::cout << "All visualization tests completed successfully!\n";
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Visualization test failed: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Visualization test failed with unknown error" << std::endl;
        return 1;
    }
}