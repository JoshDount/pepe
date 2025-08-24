#include "../../core/graph/WeightedGraph.hpp"
#include "../TestFramework.hpp"

void test_graph() {
    using namespace rts::core::graph;
    
    // Test basic graph operations
    {
        WeightedGraph graph(true);  // Directed graph
        TestFramework::assert_equal_size_t(size_t(0), graph.num_nodes(), "New graph should have no nodes");
        TestFramework::assert_equal_size_t(size_t(0), graph.num_edges(), "New graph should have no edges");
        
        // Add nodes
        Node node1(1, 40.7128, -74.0060);  // NYC coordinates
        Node node2(2, 34.0522, -118.2437); // LA coordinates
        Node node3(3, 41.8781, -87.6298);  // Chicago coordinates
        
        TestFramework::assert_true(graph.add_node(node1), "Should add new node");
        TestFramework::assert_true(graph.add_node(node2), "Should add new node");
        TestFramework::assert_true(graph.add_node(node3), "Should add new node");
        TestFramework::assert_true(!graph.add_node(node1), "Should not add duplicate node");
        
        TestFramework::assert_equal_size_t(size_t(3), graph.num_nodes(), "Should have 3 nodes");
        
        // Add edges
        TestFramework::assert_true(graph.add_edge(1, 2, 100.0f), "Should add new edge");
        TestFramework::assert_true(graph.add_edge(2, 3, 150.0f), "Should add new edge");
        TestFramework::assert_true(graph.add_edge(1, 3, 200.0f), "Should add new edge");
        
        TestFramework::assert_equal_size_t(size_t(3), graph.num_edges(), "Should have 3 edges");
        
        // Test node and edge queries
        TestFramework::assert_true(graph.has_node(1), "Should contain node 1");
        TestFramework::assert_true(graph.has_edge(1, 2), "Should contain edge 1->2");
        TestFramework::assert_true(!graph.has_edge(2, 1), "Should not contain reverse edge in directed graph");
        
        const auto* found_node = graph.get_node(1);
        TestFramework::assert_not_null(found_node, "Should find node by ID");
        TestFramework::assert_equal_uint32_t(uint32_t(1), found_node->id, "Should return correct node");
        
        const auto* found_edge = graph.get_edge(1, 2);
        TestFramework::assert_not_null(found_edge, "Should find edge");
        TestFramework::assert_equal_float(100.0f, found_edge->weight, "Should return correct edge weight");
    }
    
    // Test undirected graph
    {
        WeightedGraph graph(false);  // Undirected graph
        
        Node node1(1, 0, 0);
        Node node2(2, 1, 1);
        
        graph.add_node(node1);
        graph.add_node(node2);
        graph.add_edge(1, 2, 50.0f);
        
        TestFramework::assert_true(graph.has_edge(1, 2), "Should have edge 1->2");
        TestFramework::assert_true(graph.has_edge(2, 1), "Should have reverse edge in undirected graph");
    }
    
    // Test graph statistics
    {
        WeightedGraph graph(true);
        
        for (int i = 0; i < 5; ++i) {
            graph.add_node(Node(i, i * 10.0, i * 10.0));
        }
        
        // Create a simple chain: 0->1->2->3->4
        for (int i = 0; i < 4; ++i) {
            graph.add_edge(i, i + 1, 10.0f);
        }
        
        auto stats = graph.get_statistics();
        TestFramework::assert_equal_size_t(size_t(5), stats.num_nodes, "Stats should show correct node count");
        TestFramework::assert_equal_size_t(size_t(4), stats.num_edges, "Stats should show correct edge count");
        TestFramework::assert_true(stats.memory_usage_bytes > 0, "Should report memory usage");
    }
}