#pragma once

#include "../graph/WeightedGraph.hpp"
#include "../containers/Queue.hpp"
#include "../containers/HashMap.hpp"
#include <vector>
#include <functional>
#include <algorithm>

namespace rts::core::search {

/**
 * @brief Breadth-First Search implementation
 * 
 * Iterative implementation using Queue for level-by-level traversal.
 * Used for shortest path in unweighted graphs and connectivity analysis.
 * 
 * Time Complexity: O(V + E)
 * Space Complexity: O(V)
 */
class BFS {
public:
    /**
     * @brief Result of BFS traversal
     */
    struct Result {
        std::vector<uint32_t> visited_order;    // Order nodes were visited
        containers::HashMap<uint32_t, uint32_t> distances;  // Distance from start
        containers::HashMap<uint32_t, uint32_t> parent;     // Parent for path reconstruction
        containers::HashMap<uint32_t, bool> visited;        // Visited status
        uint32_t start_node;
        size_t nodes_visited;
        
        Result(uint32_t start) : start_node(start), nodes_visited(0) {}
        
        /**
         * @brief Get path from start to target node
         * @param target Target node ID
         * @return Vector of node IDs representing the path (empty if no path)
         */
        std::vector<uint32_t> get_path(uint32_t target) const {
            std::vector<uint32_t> path;
            
            // Check if target is reachable
            if (!visited.contains(target) || !*visited.get(target)) {
                return path; // Empty path - target not reachable
            }
            
            // Reconstruct path by following parent pointers
            uint32_t current = target;
            while (current != start_node) {
                path.push_back(current);
                auto parent_ptr = parent.get(current);
                if (!parent_ptr) break; // Should not happen if BFS was correct
                current = *parent_ptr;
            }
            path.push_back(start_node);
            
            // Reverse to get path from start to target
            std::reverse(path.begin(), path.end());
            return path;
        }
        
        /**
         * @brief Get distance to target node
         * @param target Target node ID
         * @return Distance from start (UINT32_MAX if unreachable)
         */
        uint32_t get_distance(uint32_t target) const {
            auto dist_ptr = distances.get(target);
            return dist_ptr ? *dist_ptr : UINT32_MAX;
        }
        
        /**
         * @brief Check if target is reachable from start
         * @param target Target node ID
         * @return true if reachable, false otherwise
         */
        bool is_reachable(uint32_t target) const {
            auto visited_ptr = visited.get(target);
            return visited_ptr && *visited_ptr;
        }
    };

private:
    // Statistics for analysis
    mutable size_t total_queue_operations_;
    mutable size_t max_queue_size_;
    
public:
    BFS() : total_queue_operations_(0), max_queue_size_(0) {}
    
    /**
     * @brief Perform BFS traversal from a starting node
     * @param graph The graph to traverse
     * @param start_node Starting node ID
     * @return BFS result with distances, paths, and traversal order
     */
    Result traverse(const graph::WeightedGraph& graph, uint32_t start_node) const {
        Result result(start_node);
        
        if (!graph.has_node(start_node)) {
            return result; // Start node doesn't exist
        }
        
        containers::Queue<uint32_t> queue;
        
        // Initialize
        queue.enqueue(start_node);
        result.visited.put(start_node, true);
        result.distances.put(start_node, 0);
        result.visited_order.push_back(start_node);
        result.nodes_visited++;
        
        total_queue_operations_++;
        max_queue_size_ = std::max(max_queue_size_, queue.size());
        
        while (!queue.empty()) {
            uint32_t current = queue.dequeue();
            total_queue_operations_++;
            
            // Explore all neighbors
            const auto& neighbors = graph.get_neighbors(current);
            for (const auto& edge : neighbors) {
                uint32_t neighbor = edge.to;
                
                // Skip if already visited
                auto visited_ptr = result.visited.get(neighbor);
                if (visited_ptr && *visited_ptr) {
                    continue;
                }
                
                // Mark as visited and enqueue
                result.visited.put(neighbor, true);
                result.distances.put(neighbor, result.get_distance(current) + 1);
                result.parent.put(neighbor, current);
                result.visited_order.push_back(neighbor);
                result.nodes_visited++;
                
                queue.enqueue(neighbor);
                total_queue_operations_++;
                max_queue_size_ = std::max(max_queue_size_, queue.size());
            }
        }
        
        return result;
    }
    
    /**
     * @brief Find shortest path between two nodes in unweighted graph
     * @param graph The graph to search
     * @param start Start node ID
     * @param target Target node ID
     * @return Path from start to target (empty if no path exists)
     */
    std::vector<uint32_t> find_path(const graph::WeightedGraph& graph, 
                                   uint32_t start, uint32_t target) const {
        auto result = traverse(graph, start);
        return result.get_path(target);
    }
    
    /**
     * @brief Check if there's a path between two nodes
     * @param graph The graph to search
     * @param start Start node ID
     * @param target Target node ID
     * @return true if path exists, false otherwise
     */
    bool has_path(const graph::WeightedGraph& graph, 
                  uint32_t start, uint32_t target) const {
        auto result = traverse(graph, start);
        return result.is_reachable(target);
    }
    
    /**
     * @brief Find all nodes reachable from start node
     * @param graph The graph to search
     * @param start Start node ID
     * @return Vector of all reachable node IDs
     */
    std::vector<uint32_t> find_connected_component(const graph::WeightedGraph& graph, 
                                                  uint32_t start) const {
        auto result = traverse(graph, start);
        return result.visited_order;
    }
    
    /**
     * @brief Perform BFS with custom visitor function
     * @param graph The graph to traverse
     * @param start_node Starting node ID
     * @param visitor Function called for each visited node (node_id, distance)
     * @return Number of nodes visited
     */
    size_t traverse_with_visitor(const graph::WeightedGraph& graph, 
                                uint32_t start_node,
                                std::function<void(uint32_t, uint32_t)> visitor) const {
        auto result = traverse(graph, start_node);
        
        for (uint32_t node_id : result.visited_order) {
            visitor(node_id, result.get_distance(node_id));
        }
        
        return result.nodes_visited;
    }
    
    /**
     * @brief Get BFS performance statistics
     */
    struct Statistics {
        size_t total_queue_operations;
        size_t max_queue_size;
        double avg_queue_size;
    };
    
    Statistics get_statistics() const {
        Statistics stats;
        stats.total_queue_operations = total_queue_operations_;
        stats.max_queue_size = max_queue_size_;
        stats.avg_queue_size = total_queue_operations_ > 0 ? 
            static_cast<double>(max_queue_size_) / 2.0 : 0.0; // Approximation
        
        return stats;
    }
    
    /**
     * @brief Reset statistics counters
     */
    void reset_statistics() const {
        total_queue_operations_ = 0;
        max_queue_size_ = 0;
    }
};

} // namespace rts::core::search