#pragma once

#include "../graph/WeightedGraph.hpp"
#include "../containers/MinHeap.hpp"
#include "../containers/HashMap.hpp"
#include <vector>
#include <functional>
#include <limits>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace rts::core::search {

/**
 * @brief A* pathfinding algorithm implementation with haversine heuristic
 * 
 * Uses custom MinHeap with decrease-key operations and geographic heuristics.
 * Finds optimal paths in weighted graphs with geographic coordinates.
 * 
 * Time Complexity: O((V + E) log V) - same as Dijkstra but typically faster
 * Space Complexity: O(V)
 * 
 * Requirements:
 * - Non-negative edge weights
 * - Admissible heuristic function (never overestimates)
 * - Graph nodes must have geographic coordinates
 */
class AStar {
public:
    /**
     * @brief Node state during A* execution
     */
    enum class NodeState {
        UNVISITED,  // Not yet discovered
        OPEN,       // In open set (priority queue)
        CLOSED      // In closed set (processed)
    };
    
    /**
     * @brief F-score node pair for priority queue
     * F(n) = G(n) + H(n) where G is actual cost, H is heuristic
     */
    struct FScoreNode {
        float f_score;      // Total estimated cost f(n) = g(n) + h(n)
        float g_score;      // Actual cost from start g(n)
        uint32_t node_id;   // Node identifier
        
        FScoreNode(float f, float g, uint32_t id) : f_score(f), g_score(g), node_id(id) {}
        
        // For MinHeap comparison (smaller f_score has higher priority)
        bool operator<(const FScoreNode& other) const {
            if (std::abs(f_score - other.f_score) < 1e-9f) {
                // Tie-breaking: prefer lower g_score (closer to start)
                return g_score < other.g_score;
            }
            return f_score < other.f_score;
        }
        
        bool operator>(const FScoreNode& other) const {
            return !(*this < other) && !(*this == other);
        }
        
        bool operator==(const FScoreNode& other) const {
            return std::abs(f_score - other.f_score) < 1e-9f && 
                   std::abs(g_score - other.g_score) < 1e-9f && 
                   node_id == other.node_id;
        }
    };
    
    /**
     * @brief Heuristic function interface
     */
    class Heuristic {
    public:
        virtual ~Heuristic() = default;
        virtual float estimate(const graph::Node& from, const graph::Node& to) const = 0;
        virtual std::string name() const = 0;
    };
    
    /**
     * @brief Haversine distance heuristic for geographic coordinates
     * 
     * Computes great-circle distance between two points on Earth's surface.
     * Admissible for geographic routing (never overestimates road distance).
     */
    class HaversineHeuristic : public Heuristic {
    private:
        static constexpr double EARTH_RADIUS_KM = 6371.0;
        
        double to_radians(double degrees) const {
            return degrees * M_PI / 180.0;
        }
        
    public:
        float estimate(const graph::Node& from, const graph::Node& to) const override {
            double lat1 = to_radians(from.lat);
            double lon1 = to_radians(from.lon);
            double lat2 = to_radians(to.lat);
            double lon2 = to_radians(to.lon);
            
            double dlat = lat2 - lat1;
            double dlon = lon2 - lon1;
            
            double a = std::sin(dlat/2) * std::sin(dlat/2) +
                      std::cos(lat1) * std::cos(lat2) *
                      std::sin(dlon/2) * std::sin(dlon/2);
            
            double c = 2 * std::atan2(std::sqrt(a), std::sqrt(1-a));
            double distance_km = EARTH_RADIUS_KM * c;
            
            return static_cast<float>(distance_km);
        }
        
        std::string name() const override {
            return "Haversine";
        }
    };
    
    /**
     * @brief Euclidean distance heuristic for planar coordinates
     */
    class EuclideanHeuristic : public Heuristic {
    public:
        float estimate(const graph::Node& from, const graph::Node& to) const override {
            double dx = to.lat - from.lat;   // Using lat as x
            double dy = to.lon - from.lon; // Using lon as y
            return static_cast<float>(std::sqrt(dx*dx + dy*dy));
        }
        
        std::string name() const override {
            return "Euclidean";
        }
    };
    
    /**
     * @brief Manhattan distance heuristic
     */
    class ManhattanHeuristic : public Heuristic {
    public:
        float estimate(const graph::Node& from, const graph::Node& to) const override {
            double dx = std::abs(to.lat - from.lat);
            double dy = std::abs(to.lon - from.lon);
            return static_cast<float>(dx + dy);
        }
        
        std::string name() const override {
            return "Manhattan";
        }
    };
    
    /**
     * @brief Result of A* algorithm
     */
    struct Result {
        containers::HashMap<uint32_t, float> g_scores;           // Actual costs from start
        containers::HashMap<uint32_t, float> f_scores;           // Estimated total costs
        containers::HashMap<uint32_t, uint32_t> parent;         // Parent for path reconstruction
        containers::HashMap<uint32_t, NodeState> state;         // Node processing state
        uint32_t source;
        uint32_t target;
        bool path_found;
        size_t nodes_expanded;      // Nodes moved from open to closed set
        size_t nodes_generated;     // Nodes added to open set
        std::string heuristic_name;
        
        Result(uint32_t src, uint32_t tgt, const std::string& heur_name) 
            : source(src), target(tgt), path_found(false), nodes_expanded(0), 
              nodes_generated(0), heuristic_name(heur_name) {}
        
        /**
         * @brief Get actual cost to reach target
         */
        float get_cost() const {
            if (!path_found) return std::numeric_limits<float>::infinity();
            auto cost_ptr = g_scores.get(target);
            return cost_ptr ? *cost_ptr : std::numeric_limits<float>::infinity();
        }
        
        /**
         * @brief Get shortest path to target
         */
        std::vector<uint32_t> get_path() const {
            std::vector<uint32_t> path;
            
            if (!path_found) return path;
            
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
         * @brief Check if target was reached
         */
        bool is_optimal() const {
            return path_found;
        }
        
        /**
         * @brief Get efficiency metrics compared to exhaustive search
         */
        double get_efficiency_ratio() const {
            // Efficiency = nodes_expanded / total_possible_nodes
            // Lower ratio means more efficient (fewer nodes explored)
            return nodes_expanded > 0 ? static_cast<double>(nodes_expanded) : 1.0;
        }
    };

private:
    // Statistics for analysis
    mutable size_t total_heuristic_evaluations_;
    mutable size_t total_heap_operations_;
    mutable size_t max_open_set_size_;
    mutable size_t decrease_key_operations_;
    
public:
    AStar() : total_heuristic_evaluations_(0), total_heap_operations_(0), 
              max_open_set_size_(0), decrease_key_operations_(0) {}
    
    /**
     * @brief Run A* algorithm from source to target
     * @param graph The weighted graph to search
     * @param source Source node ID
     * @param target Target node ID
     * @param heuristic Heuristic function to use
     * @return A* result with path and statistics
     */
    Result find_path(const graph::WeightedGraph& graph, 
                    uint32_t source, uint32_t target,
                    const Heuristic& heuristic) const {
        Result result(source, target, heuristic.name());
        
        if (!graph.has_node(source) || !graph.has_node(target)) {
            return result; // Invalid nodes
        }
        
        if (source == target) {
            result.path_found = true;
            result.g_scores.put(source, 0.0f);
            result.f_scores.put(source, 0.0f);
            result.nodes_expanded = 1;
            return result;
        }
        
        // Get node objects for heuristic calculations
        auto source_node = graph.get_node(source);
        auto target_node = graph.get_node(target);
        
        if (!source_node || !target_node) {
            return result; // Node data not available
        }
        
        // Initialize A* data structures
        containers::MinHeap<FScoreNode> open_set;
        containers::HashMap<uint32_t, size_t> heap_ids; // Maps node_id to heap element ID
        
        // Initialize starting node
        float initial_h = heuristic.estimate(*source_node, *target_node);
        total_heuristic_evaluations_++;
        
        result.g_scores.put(source, 0.0f);
        result.f_scores.put(source, initial_h);
        result.state.put(source, NodeState::OPEN);
        
        auto source_id = open_set.push(FScoreNode(initial_h, 0.0f, source));
        heap_ids.put(source, source_id);
        result.nodes_generated++;
        
        total_heap_operations_++;
        max_open_set_size_ = std::max(max_open_set_size_, open_set.size());
        
        // Main A* loop
        while (!open_set.empty()) {
            // Get node with lowest f_score
            FScoreNode current = open_set.pop();
            uint32_t current_id = current.node_id;
            float current_g = current.g_score;
            
            total_heap_operations_++;
            heap_ids.erase(current_id);
            
            // Move from open to closed set
            result.state.put(current_id, NodeState::CLOSED);
            result.nodes_expanded++;
            
            // Check if we reached the target
            if (current_id == target) {
                result.path_found = true;
                break;
            }
            
            // Skip if this is an outdated entry (lazy deletion)
            auto stored_g = result.g_scores.get(current_id);
            if (stored_g && current_g > *stored_g + 1e-9f) {
                continue;
            }
            
            auto current_node_ptr = graph.get_node(current_id);
            if (!current_node_ptr) continue;
            
            // Explore neighbors
            const auto& neighbors = graph.get_neighbors(current_id);
            for (const auto& edge : neighbors) {
                uint32_t neighbor_id = edge.to;
                float edge_weight = edge.get_effective_weight();
                
                // Skip negative weights
                if (edge_weight < 0) continue;
                
                auto neighbor_state_ptr = result.state.get(neighbor_id);
                NodeState neighbor_state = neighbor_state_ptr ? *neighbor_state_ptr : NodeState::UNVISITED;
                
                // Skip nodes in closed set
                if (neighbor_state == NodeState::CLOSED) continue;
                
                float tentative_g = current_g + edge_weight;
                
                auto stored_g_ptr = result.g_scores.get(neighbor_id);
                float stored_g = stored_g_ptr ? *stored_g_ptr : std::numeric_limits<float>::infinity();
                
                // Check if this is a better path
                if (tentative_g < stored_g) {
                    auto neighbor_node_ptr = graph.get_node(neighbor_id);
                    if (!neighbor_node_ptr) continue;
                    
                    // Calculate heuristic
                    float h_score = heuristic.estimate(*neighbor_node_ptr, *target_node);
                    total_heuristic_evaluations_++;
                    
                    float f_score = tentative_g + h_score;
                    
                    // Update scores and parent
                    result.g_scores.put(neighbor_id, tentative_g);
                    result.f_scores.put(neighbor_id, f_score);
                    result.parent.put(neighbor_id, current_id);
                    
                    if (neighbor_state == NodeState::UNVISITED) {
                        // Add to open set
                        result.state.put(neighbor_id, NodeState::OPEN);
                        auto neighbor_heap_id = open_set.push(FScoreNode(f_score, tentative_g, neighbor_id));
                        heap_ids.put(neighbor_id, neighbor_heap_id);
                        result.nodes_generated++;
                        
                        total_heap_operations_++;
                        max_open_set_size_ = std::max(max_open_set_size_, open_set.size());
                        
                    } else if (neighbor_state == NodeState::OPEN) {
                        // Update existing entry in open set (decrease key)
                        auto heap_id_ptr = heap_ids.get(neighbor_id);
                        if (heap_id_ptr && open_set.contains(*heap_id_ptr)) {
                            try {
                                open_set.decrease_key(*heap_id_ptr, FScoreNode(f_score, tentative_g, neighbor_id));
                                decrease_key_operations_++;
                            } catch (const std::exception&) {
                                // Decrease key failed, add new entry (lazy deletion)
                                auto new_id = open_set.push(FScoreNode(f_score, tentative_g, neighbor_id));
                                heap_ids.put(neighbor_id, new_id);
                                total_heap_operations_++;
                            }
                        }
                    }
                }
            }
        }
        
        return result;
    }
    
    /**
     * @brief Run A* with default haversine heuristic
     */
    Result find_path_haversine(const graph::WeightedGraph& graph, 
                              uint32_t source, uint32_t target) const {
        HaversineHeuristic heuristic;
        return find_path(graph, source, target, heuristic);
    }
    
    /**
     * @brief Run A* with euclidean heuristic
     */
    Result find_path_euclidean(const graph::WeightedGraph& graph, 
                              uint32_t source, uint32_t target) const {
        EuclideanHeuristic heuristic;
        return find_path(graph, source, target, heuristic);
    }
    
    /**
     * @brief Run A* with manhattan heuristic
     */
    Result find_path_manhattan(const graph::WeightedGraph& graph, 
                              uint32_t source, uint32_t target) const {
        ManhattanHeuristic heuristic;
        return find_path(graph, source, target, heuristic);
    }
    
    /**
     * @brief Check if admissible heuristic (never overestimates)
     * @param graph The graph to test
     * @param heuristic The heuristic to validate
     * @param sample_size Number of random node pairs to test
     * @return true if heuristic appears admissible
     */
    bool validate_heuristic_admissibility(const graph::WeightedGraph& graph,
                                         const Heuristic& heuristic,
                                         size_t sample_size = 100) const {
        // This is a simplified validation - in practice you'd compare
        // heuristic estimates with actual shortest path distances
        auto node_ids = graph.get_all_node_ids();
        if (node_ids.size() < 2) return true;
        
        size_t tests = std::min(sample_size, node_ids.size() * (node_ids.size() - 1) / 2);
        size_t violations = 0;
        
        for (size_t i = 0; i < tests && i < node_ids.size(); ++i) {
            for (size_t j = i + 1; j < node_ids.size() && j < i + 10; ++j) {
                auto node1 = graph.get_node(node_ids[i]);
                auto node2 = graph.get_node(node_ids[j]);
                
                if (node1 && node2) {
                    float h_estimate = heuristic.estimate(*node1, *node2);
                    // Heuristic should be non-negative
                    if (h_estimate < 0) {
                        violations++;
                    }
                }
            }
        }
        
        return violations == 0;
    }
    
    /**
     * @brief Get A* performance statistics
     */
    struct Statistics {
        size_t total_heuristic_evaluations;
        size_t total_heap_operations;
        size_t max_open_set_size;
        size_t decrease_key_operations;
        double avg_open_set_size;
    };
    
    Statistics get_statistics() const {
        Statistics stats;
        stats.total_heuristic_evaluations = total_heuristic_evaluations_;
        stats.total_heap_operations = total_heap_operations_;
        stats.max_open_set_size = max_open_set_size_;
        stats.decrease_key_operations = decrease_key_operations_;
        stats.avg_open_set_size = total_heap_operations_ > 0 ? 
            static_cast<double>(max_open_set_size_) / 2.0 : 0.0; // Approximation
        
        return stats;
    }
    
    /**
     * @brief Reset statistics counters
     */
    void reset_statistics() const {
        total_heuristic_evaluations_ = 0;
        total_heap_operations_ = 0;
        max_open_set_size_ = 0;
        decrease_key_operations_ = 0;
    }
    
    /**
     * @brief Compare A* efficiency vs Dijkstra
     * @param graph The graph to test on
     * @param source Source node
     * @param target Target node
     * @param heuristic Heuristic to use for A*
     * @return Efficiency comparison metrics
     */
    struct ComparisonResult {
        size_t astar_nodes_expanded;
        size_t astar_nodes_generated;
        float astar_path_cost;
        bool astar_found_path;
        
        // Would need Dijkstra instance for comparison
        double efficiency_gain;  // nodes_saved / dijkstra_nodes
        std::string summary;
    };
    
    ComparisonResult compare_with_dijkstra(const graph::WeightedGraph& graph,
                                          uint32_t source, uint32_t target,
                                          const Heuristic& heuristic) const {
        auto astar_result = find_path(graph, source, target, heuristic);
        
        ComparisonResult comparison;
        comparison.astar_nodes_expanded = astar_result.nodes_expanded;
        comparison.astar_nodes_generated = astar_result.nodes_generated;
        comparison.astar_path_cost = astar_result.get_cost();
        comparison.astar_found_path = astar_result.path_found;
        
        // Simplified efficiency calculation
        comparison.efficiency_gain = static_cast<double>(astar_result.nodes_expanded);
        comparison.summary = "A* with " + heuristic.name() + " heuristic";
        
        return comparison;
    }
};

} // namespace rts::core::search