#pragma once

#include "../graph/WeightedGraph.hpp"
#include "../containers/Stack.hpp"
#include "../containers/HashMap.hpp"
#include <vector>
#include <functional>
#include <algorithm>

namespace rts::core::search {

/**
 * @brief Depth-First Search implementation
 * 
 * Provides both recursive and iterative implementations.
 * Used for topological sorting, cycle detection, and connectivity analysis.
 * 
 * Time Complexity: O(V + E)
 * Space Complexity: O(V) for iterative, O(V) for recursive (stack depth)
 */
class DFS {
public:
    /**
     * @brief DFS traversal order types
     */
    enum class TraversalOrder {
        PREORDER,   // Visit node before its children
        POSTORDER   // Visit node after its children
    };
    
    /**
     * @brief Result of DFS traversal
     */
    struct Result {
        std::vector<uint32_t> preorder;         // Preorder traversal
        std::vector<uint32_t> postorder;        // Postorder traversal
        containers::HashMap<uint32_t, uint32_t> discovery_time;  // Discovery time
        containers::HashMap<uint32_t, uint32_t> finish_time;     // Finish time
        containers::HashMap<uint32_t, uint32_t> parent;          // Parent in DFS tree
        containers::HashMap<uint32_t, bool> visited;             // Visited status
        uint32_t start_node;
        size_t nodes_visited;
        uint32_t time_counter;
        
        Result(uint32_t start) : start_node(start), nodes_visited(0), time_counter(0) {}
        
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
                if (!parent_ptr) break; // Should not happen if DFS was correct
                current = *parent_ptr;
            }
            path.push_back(start_node);
            
            // Reverse to get path from start to target
            std::reverse(path.begin(), path.end());
            return path;
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
        
        /**
         * @brief Check if there's a back edge (indicating a cycle in directed graphs)
         * @param u Source node
         * @param v Target node
         * @return true if edge u->v is a back edge
         */
        bool is_back_edge(uint32_t u, uint32_t v) const {
            auto u_discovery = discovery_time.get(u);
            auto v_discovery = discovery_time.get(v);
            auto u_finish = finish_time.get(u);
            auto v_finish = finish_time.get(v);
            
            if (!u_discovery || !v_discovery || !u_finish || !v_finish) {
                return false;
            }
            
            // Back edge: v was discovered before u and not yet finished when u->v is explored
            return *v_discovery < *u_discovery && *v_finish > *u_finish;
        }
    };

private:
    // Statistics for analysis
    mutable size_t total_stack_operations_;
    mutable size_t max_stack_size_;
    mutable size_t max_recursion_depth_;
    
    /**
     * @brief Recursive DFS helper
     */
    void dfs_recursive_helper(const graph::WeightedGraph& graph, 
                             uint32_t current, 
                             Result& result,
                             size_t depth = 0) const {
        
        max_recursion_depth_ = std::max(max_recursion_depth_, depth);
        
        // Mark as visited and record discovery time
        result.visited.put(current, true);
        result.discovery_time.put(current, result.time_counter++);
        result.preorder.push_back(current);
        result.nodes_visited++;
        
        // Explore all neighbors
        const auto& neighbors = graph.get_neighbors(current);
        for (const auto& edge : neighbors) {
            uint32_t neighbor = edge.to;
            
            auto visited_ptr = result.visited.get(neighbor);
            if (!visited_ptr || !*visited_ptr) {
                // Tree edge
                result.parent.put(neighbor, current);
                dfs_recursive_helper(graph, neighbor, result, depth + 1);
            }
            // Note: We could detect back edges, forward edges, cross edges here
        }
        
        // Record finish time
        result.finish_time.put(current, result.time_counter++);
        result.postorder.push_back(current);
    }

public:
    DFS() : total_stack_operations_(0), max_stack_size_(0), max_recursion_depth_(0) {}
    
    /**
     * @brief Perform recursive DFS traversal from a starting node
     * @param graph The graph to traverse
     * @param start_node Starting node ID
     * @return DFS result with discovery/finish times and traversal orders
     */
    Result traverse_recursive(const graph::WeightedGraph& graph, uint32_t start_node) const {
        Result result(start_node);
        
        if (!graph.has_node(start_node)) {
            return result; // Start node doesn't exist
        }
        
        max_recursion_depth_ = 0;
        dfs_recursive_helper(graph, start_node, result);
        
        return result;
    }
    
    /**
     * @brief Perform iterative DFS traversal using explicit stack
     * @param graph The graph to traverse
     * @param start_node Starting node ID
     * @param order Traversal order (preorder or postorder)
     * @return DFS result with traversal order
     */
    Result traverse_iterative(const graph::WeightedGraph& graph, 
                             uint32_t start_node,
                             TraversalOrder order = TraversalOrder::PREORDER) const {
        Result result(start_node);
        
        if (!graph.has_node(start_node)) {
            return result; // Start node doesn't exist
        }
        
        containers::Stack<std::pair<uint32_t, bool>> stack; // (node_id, processed)
        
        // Initialize
        stack.push({start_node, false});
        total_stack_operations_++;
        max_stack_size_ = std::max(max_stack_size_, stack.size());
        
        while (!stack.empty()) {
            auto top_pair = stack.top();
            uint32_t current = top_pair.first;
            bool processed = top_pair.second;
            stack.pop_void();
            total_stack_operations_++;
            
            if (processed) {
                // Post-processing (for postorder)
                if (order == TraversalOrder::POSTORDER) {
                    result.postorder.push_back(current);
                }
                result.finish_time.put(current, result.time_counter++);
            } else {
                // First time visiting this node
                auto visited_ptr = result.visited.get(current);
                if (visited_ptr && *visited_ptr) {
                    continue; // Already processed
                }
                
                result.visited.put(current, true);
                result.discovery_time.put(current, result.time_counter++);
                result.nodes_visited++;
                
                if (order == TraversalOrder::PREORDER) {
                    result.preorder.push_back(current);
                }
                
                // Push back for post-processing
                stack.push({current, true});
                total_stack_operations_++;
                max_stack_size_ = std::max(max_stack_size_, stack.size());
                
                // Push all unvisited neighbors
                const auto& neighbors = graph.get_neighbors(current);
                for (const auto& edge : neighbors) {
                    uint32_t neighbor = edge.to;
                    auto neighbor_visited = result.visited.get(neighbor);
                    
                    if (!neighbor_visited || !*neighbor_visited) {
                        result.parent.put(neighbor, current);
                        stack.push({neighbor, false});
                        total_stack_operations_++;
                        max_stack_size_ = std::max(max_stack_size_, stack.size());
                    }
                }
            }
        }
        
        return result;
    }
    
    /**
     * @brief Find path between two nodes using DFS
     * @param graph The graph to search
     * @param start Start node ID
     * @param target Target node ID
     * @param recursive Use recursive implementation if true, iterative if false
     * @return Path from start to target (empty if no path exists)
     */
    std::vector<uint32_t> find_path(const graph::WeightedGraph& graph, 
                                   uint32_t start, uint32_t target,
                                   bool recursive = true) const {
        Result result = recursive ? 
            traverse_recursive(graph, start) : 
            traverse_iterative(graph, start);
        
        return result.get_path(target);
    }
    
    /**
     * @brief Check if there's a path between two nodes
     * @param graph The graph to search
     * @param start Start node ID
     * @param target Target node ID
     * @param recursive Use recursive implementation if true, iterative if false
     * @return true if path exists, false otherwise
     */
    bool has_path(const graph::WeightedGraph& graph, 
                  uint32_t start, uint32_t target,
                  bool recursive = true) const {
        Result result = recursive ? 
            traverse_recursive(graph, start) : 
            traverse_iterative(graph, start);
        
        return result.is_reachable(target);
    }
    
    /**
     * @brief Find all nodes reachable from start node
     * @param graph The graph to search
     * @param start Start node ID
     * @param recursive Use recursive implementation if true, iterative if false
     * @return Vector of all reachable node IDs in traversal order
     */
    std::vector<uint32_t> find_connected_component(const graph::WeightedGraph& graph, 
                                                  uint32_t start,
                                                  bool recursive = true) const {
        Result result = recursive ? 
            traverse_recursive(graph, start) : 
            traverse_iterative(graph, start);
        
        return result.preorder;
    }
    
    /**
     * @brief Detect cycles in directed graph using DFS
     * @param graph The graph to analyze
     * @param start Starting node for DFS
     * @return true if cycle is detected, false otherwise
     */
    bool has_cycle_directed(const graph::WeightedGraph& graph, uint32_t start) const {
        if (!graph.is_directed()) {
            return false; // This method is for directed graphs only
        }
        
        auto result = traverse_recursive(graph, start);
        
        // Check for back edges (which indicate cycles in directed graphs)
        for (uint32_t node_id : result.preorder) {
            const auto& neighbors = graph.get_neighbors(node_id);
            for (const auto& edge : neighbors) {
                if (result.is_back_edge(node_id, edge.to)) {
                    return true;
                }
            }
        }
        
        return false;
    }
    
    /**
     * @brief Perform DFS with custom visitor functions
     * @param graph The graph to traverse
     * @param start_node Starting node ID
     * @param pre_visitor Function called when node is first discovered
     * @param post_visitor Function called when node processing is finished
     * @param recursive Use recursive implementation if true, iterative if false
     * @return Number of nodes visited
     */
    size_t traverse_with_visitors(const graph::WeightedGraph& graph, 
                                 uint32_t start_node,
                                 std::function<void(uint32_t)> pre_visitor,
                                 std::function<void(uint32_t)> post_visitor,
                                 bool recursive = true) const {
        Result result = recursive ? 
            traverse_recursive(graph, start_node) : 
            traverse_iterative(graph, start_node, TraversalOrder::PREORDER);
        
        // Call pre_visitor for each node in preorder
        for (uint32_t node_id : result.preorder) {
            if (pre_visitor) pre_visitor(node_id);
        }
        
        // Call post_visitor for each node in postorder
        for (uint32_t node_id : result.postorder) {
            if (post_visitor) post_visitor(node_id);
        }
        
        return result.nodes_visited;
    }
    
    /**
     * @brief Get DFS performance statistics
     */
    struct Statistics {
        size_t total_stack_operations;
        size_t max_stack_size;
        size_t max_recursion_depth;
        double avg_stack_size;
    };
    
    Statistics get_statistics() const {
        Statistics stats;
        stats.total_stack_operations = total_stack_operations_;
        stats.max_stack_size = max_stack_size_;
        stats.max_recursion_depth = max_recursion_depth_;
        stats.avg_stack_size = total_stack_operations_ > 0 ? 
            static_cast<double>(max_stack_size_) / 2.0 : 0.0; // Approximation
        
        return stats;
    }
    
    /**
     * @brief Reset statistics counters
     */
    void reset_statistics() const {
        total_stack_operations_ = 0;
        max_stack_size_ = 0;
        max_recursion_depth_ = 0;
    }
};

} // namespace rts::core::search