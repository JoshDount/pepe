#pragma once

#include "../graph/WeightedGraph.hpp"
#include "../containers/MinHeap.hpp"
#include "../containers/HashMap.hpp"
#include <vector>
#include <functional>
#include <limits>
#include <algorithm>

namespace rts::core::search {

/**
 * @brief Dijkstra's shortest path algorithm implementation
 * 
 * Uses custom MinHeap with decrease-key operations for optimal performance.
 * Finds shortest paths from a source to all reachable nodes in weighted graphs.
 * 
 * Time Complexity: O((V + E) log V)
 * Space Complexity: O(V)
 * 
 * Requirements:
 * - Non-negative edge weights
 * - Works on both directed and undirected graphs
 */
class Dijkstra {
public:
    /**
     * @brief Node state during Dijkstra execution
     */
    enum class NodeState {
        UNVISITED,  // Not yet discovered
        IN_QUEUE,   // In priority queue but not processed
        VISITED     // Processed (shortest path found)
    };
    
    /**
     * @brief Distance-node pair for priority queue
     */
    struct DistanceNode {
        float distance;
        uint32_t node_id;
        
        DistanceNode(float d, uint32_t id) : distance(d), node_id(id) {}
        
        // For MinHeap comparison (smaller distance has higher priority)
        bool operator<(const DistanceNode& other) const {
            return distance < other.distance;
        }
        
        bool operator>(const DistanceNode& other) const {
            return distance > other.distance;
        }
        
        bool operator==(const DistanceNode& other) const {
            return std::abs(distance - other.distance) < 1e-9f && node_id == other.node_id;
        }
    };
    
    /**
     * @brief Result of Dijkstra's algorithm
     */
    struct Result {
        containers::HashMap<uint32_t, float> distances;        // Shortest distances
        containers::HashMap<uint32_t, uint32_t> parent;       // Parent for path reconstruction
        containers::HashMap<uint32_t, NodeState> state;       // Node processing state
        uint32_t source;
        size_t nodes_processed;
        
        Result(uint32_t src) : source(src), nodes_processed(0) {}
        
        /**
         * @brief Get shortest distance to target
         * @param target Target node ID
         * @return Shortest distance (infinity if unreachable)
         */
        float get_distance(uint32_t target) const {
            auto dist_ptr = distances.get(target);
            return dist_ptr ? *dist_ptr : std::numeric_limits<float>::infinity();
        }
        
        /**
         * @brief Check if target is reachable from source
         * @param target Target node ID
         * @return true if reachable, false otherwise
         */
        bool is_reachable(uint32_t target) const {
            auto dist_ptr = distances.get(target);
            return dist_ptr && *dist_ptr < std::numeric_limits<float>::infinity();
        }
        
        /**
         * @brief Get shortest path to target
         * @param target Target node ID
         * @return Vector of node IDs representing the path (empty if unreachable)
         */
        std::vector<uint32_t> get_path(uint32_t target) const {
            std::vector<uint32_t> path;
            
            if (!is_reachable(target)) {
                return path; // Empty path - target not reachable
            }
            
            // Reconstruct path by following parent pointers
            uint32_t current = target;
            while (current != source) {
                path.push_back(current);
                auto parent_ptr = parent.get(current);
                if (!parent_ptr) break; // Should not happen if algorithm was correct
                current = *parent_ptr;
            }
            path.push_back(source);
            
            // Reverse to get path from source to target
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        /**
         * @brief Get all reachable nodes with their distances
         * @return Vector of (node_id, distance) pairs
         */
        std::vector<std::pair<uint32_t, float>> get_all_distances() const {
            std::vector<std::pair<uint32_t, float>> result;
            
            auto keys = distances.keys();
            for (uint32_t node_id : keys) {
                float dist = get_distance(node_id);
                if (dist < std::numeric_limits<float>::infinity()) {
                    result.emplace_back(node_id, dist);
                }
            }
            
            return result;
        }
    };

private:
    // Statistics for analysis
    mutable size_t total_heap_operations_;
    mutable size_t max_heap_size_;
    mutable size_t decrease_key_operations_;
    mutable size_t edge_relaxations_;
    
public:
    Dijkstra() : total_heap_operations_(0), max_heap_size_(0), 
                decrease_key_operations_(0), edge_relaxations_(0) {}
    
    /**
     * @brief Run Dijkstra's algorithm from a source node
     * @param graph The weighted graph to search
     * @param source Source node ID
     * @return Dijkstra result with distances and paths
     */
    Result find_shortest_paths(const graph::WeightedGraph& graph, uint32_t source) const {
        Result result(source);
        
        if (!graph.has_node(source)) {
            return result; // Source node doesn't exist
        }
        
        // Initialize distances and priority queue
        containers::MinHeap<DistanceNode> pq;
        containers::HashMap<uint32_t, size_t> heap_ids; // Maps node_id to heap element ID
        
        // Initialize source
        result.distances.put(source, 0.0f);
        result.state.put(source, NodeState::IN_QUEUE);
        auto source_id = pq.push(DistanceNode(0.0f, source));
        heap_ids.put(source, source_id);
        
        total_heap_operations_++;
        max_heap_size_ = std::max(max_heap_size_, pq.size());
        
        // Main algorithm loop
        while (!pq.empty()) {
            // Extract minimum distance node
            DistanceNode min_node = pq.pop();
            uint32_t u = min_node.node_id;
            float dist_u = min_node.distance;
            
            total_heap_operations_++;
            heap_ids.erase(u);
            
            // Mark as visited
            result.state.put(u, NodeState::VISITED);
            result.nodes_processed++;
            
            // Skip if we've already found a shorter path
            if (dist_u > result.get_distance(u)) {
                continue;
            }
            
            // Explore all neighbors
            const auto& neighbors = graph.get_neighbors(u);
            for (const auto& edge : neighbors) {
                uint32_t v = edge.to;
                float weight = edge.get_effective_weight(); // Handles dynamic weights
                
                edge_relaxations_++;
                
                // Skip negative weights (algorithm requirement)
                if (weight < 0) {
                    continue;
                }
                
                float new_distance = dist_u + weight;
                float current_distance = result.get_distance(v);
                
                // Relaxation: found shorter path to v
                if (new_distance < current_distance) {
                    result.distances.put(v, new_distance);
                    result.parent.put(v, u);
                    
                    auto state_ptr = result.state.get(v);
                    NodeState v_state = state_ptr ? *state_ptr : NodeState::UNVISITED;
                    
                    if (v_state == NodeState::UNVISITED) {
                        // First time discovering v
                        result.state.put(v, NodeState::IN_QUEUE);
                        auto v_id = pq.push(DistanceNode(new_distance, v));
                        heap_ids.put(v, v_id);
                        
                        total_heap_operations_++;
                        max_heap_size_ = std::max(max_heap_size_, pq.size());
                        
                    } else if (v_state == NodeState::IN_QUEUE) {
                        // v is in queue, need decrease-key
                        auto heap_id_ptr = heap_ids.get(v);
                        if (heap_id_ptr && pq.contains(*heap_id_ptr)) {
                            try {
                                pq.decrease_key(*heap_id_ptr, DistanceNode(new_distance, v));
                                decrease_key_operations_++;
                            } catch (const std::exception&) {
                                // Decrease key failed, add new entry (lazy deletion approach)
                                auto new_id = pq.push(DistanceNode(new_distance, v));
                                heap_ids.put(v, new_id);
                                total_heap_operations_++;
                            }
                        }
                    }
                    // If v_state == VISITED, ignore (already processed)
                }
            }
        }
        
        return result;
    }
    
    /**
     * @brief Find shortest path between two specific nodes
     * @param graph The weighted graph to search
     * @param source Source node ID
     * @param target Target node ID
     * @return Shortest path as vector of node IDs (empty if no path)
     */
    std::vector<uint32_t> find_path(const graph::WeightedGraph& graph, 
                                   uint32_t source, uint32_t target) const {
        auto result = find_shortest_paths(graph, source);
        return result.get_path(target);
    }
    
    /**
     * @brief Find shortest distance between two specific nodes
     * @param graph The weighted graph to search
     * @param source Source node ID
     * @param target Target node ID
     * @return Shortest distance (infinity if no path)
     */
    float find_distance(const graph::WeightedGraph& graph, 
                       uint32_t source, uint32_t target) const {
        auto result = find_shortest_paths(graph, source);
        return result.get_distance(target);
    }
    
    /**
     * @brief Check if there's a path between two nodes
     * @param graph The weighted graph to search
     * @param source Source node ID
     * @param target Target node ID
     * @return true if path exists, false otherwise
     */
    bool has_path(const graph::WeightedGraph& graph, 
                  uint32_t source, uint32_t target) const {
        auto result = find_shortest_paths(graph, source);
        return result.is_reachable(target);
    }
    
    /**
     * @brief Find shortest paths with early termination when target is reached
     * @param graph The weighted graph to search
     * @param source Source node ID
     * @param target Target node ID (algorithm stops when reached)
     * @return Dijkstra result (may be partial if target was reached early)
     */
    Result find_shortest_path_to_target(const graph::WeightedGraph& graph, 
                                       uint32_t source, uint32_t target) const {
        Result result(source);
        
        if (!graph.has_node(source) || !graph.has_node(target)) {
            return result;
        }
        
        if (source == target) {
            result.distances.put(source, 0.0f);
            result.state.put(source, NodeState::VISITED);
            result.nodes_processed = 1;
            return result;
        }
        
        // Same as main algorithm but with early termination
        containers::MinHeap<DistanceNode> pq;
        containers::HashMap<uint32_t, size_t> heap_ids;
        
        result.distances.put(source, 0.0f);
        result.state.put(source, NodeState::IN_QUEUE);
        auto source_id = pq.push(DistanceNode(0.0f, source));
        heap_ids.put(source, source_id);
        
        while (!pq.empty()) {
            DistanceNode min_node = pq.pop();
            uint32_t u = min_node.node_id;
            float dist_u = min_node.distance;
            
            heap_ids.erase(u);
            result.state.put(u, NodeState::VISITED);
            result.nodes_processed++;
            
            // Early termination if target reached
            if (u == target) {
                break;
            }
            
            if (dist_u > result.get_distance(u)) {
                continue;
            }
            
            // Same neighbor processing as main algorithm
            const auto& neighbors = graph.get_neighbors(u);
            for (const auto& edge : neighbors) {
                uint32_t v = edge.to;
                float weight = edge.get_effective_weight();
                
                if (weight < 0) continue;
                
                float new_distance = dist_u + weight;
                float current_distance = result.get_distance(v);
                
                if (new_distance < current_distance) {
                    result.distances.put(v, new_distance);
                    result.parent.put(v, u);
                    
                    auto state_ptr = result.state.get(v);
                    NodeState v_state = state_ptr ? *state_ptr : NodeState::UNVISITED;
                    
                    if (v_state == NodeState::UNVISITED) {
                        result.state.put(v, NodeState::IN_QUEUE);
                        auto v_id = pq.push(DistanceNode(new_distance, v));
                        heap_ids.put(v, v_id);
                    } else if (v_state == NodeState::IN_QUEUE) {
                        auto heap_id_ptr = heap_ids.get(v);
                        if (heap_id_ptr && pq.contains(*heap_id_ptr)) {
                            try {
                                pq.decrease_key(*heap_id_ptr, DistanceNode(new_distance, v));
                            } catch (const std::exception&) {
                                auto new_id = pq.push(DistanceNode(new_distance, v));
                                heap_ids.put(v, new_id);
                            }
                        }
                    }
                }
            }
        }
        
        return result;
    }
    
    /**
     * @brief Get Dijkstra performance statistics
     */
    struct Statistics {
        size_t total_heap_operations;
        size_t max_heap_size;
        size_t decrease_key_operations;
        size_t edge_relaxations;
        double avg_heap_size;
    };
    
    Statistics get_statistics() const {
        Statistics stats;
        stats.total_heap_operations = total_heap_operations_;
        stats.max_heap_size = max_heap_size_;
        stats.decrease_key_operations = decrease_key_operations_;
        stats.edge_relaxations = edge_relaxations_;
        stats.avg_heap_size = total_heap_operations_ > 0 ? 
            static_cast<double>(max_heap_size_) / 2.0 : 0.0; // Approximation
        
        return stats;
    }
    
    /**
     * @brief Reset statistics counters
     */
    void reset_statistics() const {
        total_heap_operations_ = 0;
        max_heap_size_ = 0;
        decrease_key_operations_ = 0;
        edge_relaxations_ = 0;
    }
    
    /**
     * @brief Validate that graph has non-negative weights
     * @param graph The graph to validate
     * @return true if all weights are non-negative, false otherwise
     */
    static bool validate_non_negative_weights(const graph::WeightedGraph& graph) {
        auto all_edges = graph.get_all_edges();
        for (const auto& edge : all_edges) {
            if (edge.get_effective_weight() < 0) {
                return false;
            }
        }
        return true;
    }
};

} // namespace rts::core::search