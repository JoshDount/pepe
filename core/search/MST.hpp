#pragma once

#include "../graph/WeightedGraph.hpp"
#include "../containers/MinHeap.hpp"
#include "../containers/HashMap.hpp"
#include <vector>
#include <algorithm>
#include <functional>
#include <limits>

namespace rts::core::search {

/**
 * @brief Minimum Spanning Tree algorithms implementation
 * 
 * Implements both Kruskal's and Prim's algorithms for finding minimum spanning trees.
 * Essential for network optimization in transportation systems.
 * 
 * Kruskal: O(E log E) - edge-based, uses sorting and union-find
 * Prim: O((V + E) log V) - vertex-based, uses priority queue
 */
class MST {
public:
    /**
     * @brief MST edge representation
     */
    struct MSTEdge {
        uint32_t from;
        uint32_t to;
        float weight;
        
        MSTEdge(uint32_t f, uint32_t t, float w) : from(f), to(t), weight(w) {}
        
        bool operator<(const MSTEdge& other) const {
            return weight < other.weight;
        }
        
        bool operator>(const MSTEdge& other) const {
            return weight > other.weight;
        }
        
        bool operator==(const MSTEdge& other) const {
            return std::abs(weight - other.weight) < 1e-9f && 
                   from == other.from && to == other.to;
        }
    };
    
    /**
     * @brief MST result structure
     */
    struct Result {
        std::vector<MSTEdge> edges;          // MST edges
        float total_weight;                  // Total MST weight
        size_t nodes_in_mst;                // Number of nodes in MST
        bool is_connected;                   // Whether input graph was connected
        std::string algorithm_used;          // Algorithm name
        
        // Performance metrics
        size_t edges_considered;             // Total edges examined
        size_t union_find_operations;        // Union-Find ops (Kruskal)
        size_t heap_operations;              // Heap ops (Prim)
        size_t total_comparisons;            // Edge weight comparisons
        
        Result(const std::string& algo) : total_weight(0.0f), nodes_in_mst(0), 
                                         is_connected(false), algorithm_used(algo),
                                         edges_considered(0), union_find_operations(0),
                                         heap_operations(0), total_comparisons(0) {}
        
        /**
         * @brief Get MST density (edges / possible_edges)
         */
        double get_density() const {
            if (nodes_in_mst <= 1) return 0.0;
            size_t max_edges = nodes_in_mst - 1; // MST property
            return static_cast<double>(edges.size()) / max_edges;
        }
        
        /**
         * @brief Get all nodes in the MST
         */
        std::vector<uint32_t> get_nodes() const {
            std::vector<uint32_t> nodes;
            for (const auto& edge : edges) {
                nodes.push_back(edge.from);
                nodes.push_back(edge.to);
            }
            
            // Remove duplicates
            std::sort(nodes.begin(), nodes.end());
            nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());
            
            return nodes;
        }
        
        /**
         * @brief Check if MST is valid (connected and acyclic)
         */
        bool is_valid_mst() const {
            if (!is_connected) return false;
            if (nodes_in_mst == 0) return true;
            return edges.size() == nodes_in_mst - 1; // MST property: |E| = |V| - 1
        }
    };

private:
    /**
     * @brief Union-Find (Disjoint Set) data structure for Kruskal's algorithm
     */
    class UnionFind {
    private:
        containers::HashMap<uint32_t, uint32_t> parent_;
        containers::HashMap<uint32_t, uint32_t> rank_;
        size_t operation_count_;
        
    public:
        UnionFind() : operation_count_(0) {}
        
        void make_set(uint32_t x) {
            parent_.put(x, x);
            rank_.put(x, 0);
            operation_count_++;
        }
        
        uint32_t find(uint32_t x) {
            operation_count_++;
            auto parent_ptr = parent_.get(x);
            if (!parent_ptr) {
                make_set(x);
                return x;
            }
            
            if (*parent_ptr != x) {
                // Path compression
                uint32_t root = find(*parent_ptr);
                parent_.put(x, root);
                return root;
            }
            return x;
        }
        
        bool union_sets(uint32_t x, uint32_t y) {
            operation_count_++;
            uint32_t root_x = find(x);
            uint32_t root_y = find(y);
            
            if (root_x == root_y) {
                return false; // Already in same set (would create cycle)
            }
            
            // Union by rank
            auto rank_x_ptr = rank_.get(root_x);
            auto rank_y_ptr = rank_.get(root_y);
            uint32_t rank_x = rank_x_ptr ? *rank_x_ptr : 0;
            uint32_t rank_y = rank_y_ptr ? *rank_y_ptr : 0;
            
            if (rank_x < rank_y) {
                parent_.put(root_x, root_y);
            } else if (rank_x > rank_y) {
                parent_.put(root_y, root_x);
            } else {
                parent_.put(root_y, root_x);
                rank_.put(root_x, rank_x + 1);
            }
            
            return true;
        }
        
        size_t get_operation_count() const {
            return operation_count_;
        }
        
        void reset_operation_count() {
            operation_count_ = 0;
        }
    };
    
    // Statistics counters
    mutable size_t total_edge_comparisons_;
    mutable size_t total_heap_operations_;
    
public:
    MST() : total_edge_comparisons_(0), total_heap_operations_(0) {}
    
    /**
     * @brief Kruskal's MST algorithm using sorting and Union-Find
     * @param graph Input weighted graph
     * @return MST result with edges and statistics
     */
    Result kruskal(const graph::WeightedGraph& graph) const {
        Result result("Kruskal");
        
        if (graph.num_nodes() == 0) {
            result.is_connected = true; // Empty graph is trivially connected
            return result;
        }
        
        // Get all edges and sort by weight
        auto all_edges = graph.get_all_edges();
        std::vector<MSTEdge> edges;
        edges.reserve(all_edges.size());
        
        for (const auto& edge : all_edges) {
            edges.emplace_back(edge.from, edge.to, edge.get_effective_weight());
        }
        
        // Sort edges by weight (Kruskal's key step)
        std::sort(edges.begin(), edges.end());
        result.edges_considered = edges.size();
        total_edge_comparisons_ += edges.size() * std::log2(edges.size()); // Sorting complexity
        
        // Initialize Union-Find for all nodes
        UnionFind uf;
        auto node_ids = graph.get_all_node_ids();
        for (uint32_t node_id : node_ids) {
            uf.make_set(node_id);
        }
        
        result.nodes_in_mst = node_ids.size();
        
        // Process edges in weight order
        for (const auto& edge : edges) {
            if (uf.union_sets(edge.from, edge.to)) {
                // Edge doesn't create cycle, add to MST
                result.edges.push_back(edge);
                result.total_weight += edge.weight;
                
                // MST complete when we have |V|-1 edges
                if (result.edges.size() == result.nodes_in_mst - 1) {
                    break;
                }
            }
        }
        
        result.union_find_operations = uf.get_operation_count();
        result.total_comparisons = total_edge_comparisons_;
        result.is_connected = (result.edges.size() == result.nodes_in_mst - 1);
        
        return result;
    }
    
    /**
     * @brief Prim's MST algorithm using min-heap
     * @param graph Input weighted graph
     * @param start_node Starting node for Prim's algorithm
     * @return MST result with edges and statistics
     */
    Result prim(const graph::WeightedGraph& graph, uint32_t start_node) const {
        Result result("Prim");
        
        if (graph.num_nodes() == 0) {
            result.is_connected = true;
            return result;
        }
        
        if (!graph.has_node(start_node)) {
            return result; // Invalid start node
        }
        
        // Priority queue for edges (min-heap by weight)
        containers::MinHeap<MSTEdge> edge_queue;
        containers::HashMap<uint32_t, bool> in_mst; // Tracks nodes in MST
        
        // Start with the given node
        in_mst.put(start_node, true);
        result.nodes_in_mst = 1;
        
        // Add all edges from start node to queue
        const auto& start_edges = graph.get_neighbors(start_node);
        for (const auto& edge : start_edges) {
            if (!in_mst.contains(edge.to)) {
                edge_queue.push(MSTEdge(edge.from, edge.to, edge.get_effective_weight()));
                total_heap_operations_++;
            }
        }
        
        result.edges_considered = start_edges.size();
        
        // Main Prim's algorithm loop
        while (!edge_queue.empty() && result.edges.size() < graph.num_nodes() - 1) {
            MSTEdge min_edge = edge_queue.pop();
            total_heap_operations_++;
            
            // Skip if both endpoints already in MST
            bool from_in_mst = in_mst.contains(min_edge.from);
            bool to_in_mst = in_mst.contains(min_edge.to);
            
            if (from_in_mst && to_in_mst) {
                continue; // Would create cycle
            }
            
            // Add edge to MST
            result.edges.push_back(min_edge);
            result.total_weight += min_edge.weight;
            
            // Determine which node is new
            uint32_t new_node = to_in_mst ? min_edge.from : min_edge.to;
            in_mst.put(new_node, true);
            result.nodes_in_mst++;
            
            // Add edges from new node to queue
            const auto& new_edges = graph.get_neighbors(new_node);
            for (const auto& edge : new_edges) {
                if (!in_mst.contains(edge.to)) {
                    edge_queue.push(MSTEdge(edge.from, edge.to, edge.get_effective_weight()));
                    total_heap_operations_++;
                    result.edges_considered++;
                }
            }
        }
        
        result.heap_operations = total_heap_operations_;
        result.total_comparisons = total_edge_comparisons_;
        result.is_connected = (result.edges.size() == result.nodes_in_mst - 1);
        
        return result;
    }
    
    /**
     * @brief Prim's MST starting from node with minimum degree
     * @param graph Input weighted graph
     * @return MST result
     */
    Result prim_auto_start(const graph::WeightedGraph& graph) const {
        if (graph.num_nodes() == 0) {
            return Result("Prim (auto-start)");
        }
        
        // Find node with minimum degree for potentially better performance
        auto node_ids = graph.get_all_node_ids();
        uint32_t min_degree_node = node_ids[0];
        size_t min_degree = graph.get_neighbors(min_degree_node).size();
        
        for (uint32_t node_id : node_ids) {
            size_t degree = graph.get_neighbors(node_id).size();
            if (degree < min_degree) {
                min_degree = degree;
                min_degree_node = node_id;
            }
        }
        
        auto result = prim(graph, min_degree_node);
        result.algorithm_used = "Prim (auto-start)";
        return result;
    }
    
    /**
     * @brief Compare Kruskal vs Prim performance
     * @param graph Input weighted graph
     * @param start_node Starting node for Prim (optional)
     * @return Comparison metrics
     */
    struct ComparisonResult {
        Result kruskal_result;
        Result prim_result;
        
        // Performance comparison
        bool same_total_weight;
        bool same_mst_size;
        double kruskal_efficiency;  // operations per edge
        double prim_efficiency;     // operations per edge
        std::string recommendation;
        
        ComparisonResult(const Result& k, const Result& p) 
            : kruskal_result(k), prim_result(p) {
            
            same_total_weight = std::abs(k.total_weight - p.total_weight) < 1e-6f;
            same_mst_size = (k.edges.size() == p.edges.size());
            
            kruskal_efficiency = k.edges.size() > 0 ? 
                static_cast<double>(k.union_find_operations) / k.edges.size() : 0.0;
            prim_efficiency = p.edges.size() > 0 ? 
                static_cast<double>(p.heap_operations) / p.edges.size() : 0.0;
            
            // Simple recommendation based on graph characteristics
            if (k.edges_considered < p.edges_considered) {
                recommendation = "Kruskal (fewer edges to consider)";
            } else if (p.heap_operations < k.union_find_operations) {
                recommendation = "Prim (fewer operations)";
            } else {
                recommendation = "Similar performance";
            }
        }
    };
    
    ComparisonResult compare_algorithms(const graph::WeightedGraph& graph, 
                                      uint32_t start_node = 0) const {
        reset_statistics();
        
        auto kruskal_result = kruskal(graph);
        
        reset_statistics();
        
        Result prim_result("Prim");
        if (start_node == 0 || !graph.has_node(start_node)) {
            prim_result = prim_auto_start(graph);
        } else {
            prim_result = prim(graph, start_node);
        }
        
        return ComparisonResult(kruskal_result, prim_result);
    }
    
    /**
     * @brief Find all minimum spanning trees (if multiple exist)
     * @param graph Input weighted graph
     * @return Vector of MST results
     */
    std::vector<Result> find_all_msts(const graph::WeightedGraph& graph) const {
        std::vector<Result> msts;
        
        // For now, return just one MST using Kruskal
        // Full implementation would use modified algorithms to find all MSTs
        auto result = kruskal(graph);
        if (result.is_connected) {
            msts.push_back(result);
        }
        
        return msts;
    }
    
    /**
     * @brief Verify MST properties
     * @param mst_result MST to verify
     * @param original_graph Original graph
     * @return true if MST is valid
     */
    bool verify_mst(const Result& mst_result, const graph::WeightedGraph& original_graph) const {
        if (!mst_result.is_valid_mst()) {
            return false;
        }
        
        // Check that all edges exist in original graph
        for (const auto& mst_edge : mst_result.edges) {
            const auto* graph_edge = original_graph.get_edge(mst_edge.from, mst_edge.to);
            if (!graph_edge) {
                // Try reverse direction for undirected graphs
                graph_edge = original_graph.get_edge(mst_edge.to, mst_edge.from);
                if (!graph_edge) {
                    return false; // Edge not in original graph
                }
            }
            
            // Check weight consistency
            if (std::abs(graph_edge->get_effective_weight() - mst_edge.weight) > 1e-6f) {
                return false; // Weight mismatch
            }
        }
        
        return true;
    }
    
    /**
     * @brief Get MST performance statistics
     */
    struct Statistics {
        size_t total_edge_comparisons;
        size_t total_heap_operations;
        double avg_operations_per_edge;
    };
    
    Statistics get_statistics() const {
        Statistics stats;
        stats.total_edge_comparisons = total_edge_comparisons_;
        stats.total_heap_operations = total_heap_operations_;
        stats.avg_operations_per_edge = (total_edge_comparisons_ + total_heap_operations_) > 0 ?
            static_cast<double>(total_edge_comparisons_ + total_heap_operations_) / 
            std::max(static_cast<size_t>(1), total_edge_comparisons_ + total_heap_operations_) : 0.0;
        
        return stats;
    }
    
    /**
     * @brief Reset statistics counters
     */
    void reset_statistics() const {
        total_edge_comparisons_ = 0;
        total_heap_operations_ = 0;
    }
    
    /**
     * @brief Calculate theoretical MST lower bound (sum of lightest edges)
     * @param graph Input graph
     * @return Lower bound weight
     */
    static float calculate_mst_lower_bound(const graph::WeightedGraph& graph) {
        if (graph.num_nodes() <= 1) return 0.0f;
        
        auto all_edges = graph.get_all_edges();
        std::vector<float> weights;
        weights.reserve(all_edges.size());
        
        for (const auto& edge : all_edges) {
            weights.push_back(edge.get_effective_weight());
        }
        
        std::sort(weights.begin(), weights.end());
        
        // Sum of |V|-1 lightest edges (may not form connected MST)
        float lower_bound = 0.0f;
        size_t edges_needed = graph.num_nodes() - 1;
        for (size_t i = 0; i < std::min(edges_needed, weights.size()); ++i) {
            lower_bound += weights[i];
        }
        
        return lower_bound;
    }
};

} // namespace rts::core::search