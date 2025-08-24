#pragma once

#include "Node.hpp"
#include "Edge.hpp"
#include "../containers/HashMap.hpp"
#include "../containers/LinkedList.hpp"
#include <vector>
#include <memory>
#include <stdexcept>
#include <algorithm>

namespace rts::core::graph {

/**
 * @brief Weighted Graph implementation using adjacency list representation
 * 
 * Optimized for transportation networks with support for:
 * - Dynamic weight updates (traffic simulation)
 * - Multiple edge types and transport modes
 * - Efficient neighbor iteration
 * - Memory-conscious storage
 * 
 * Time Complexities:
 * - Add Node: O(1) amortized
 * - Add Edge: O(1) amortized
 * - Get Neighbors: O(degree)
 * - Remove Node: O(degree + V) worst case
 * - Remove Edge: O(degree)
 * 
 * Space Complexity: O(V + E)
 */
class WeightedGraph {
private:
    // Primary storage: adjacency list using vectors for cache efficiency
    std::vector<Node> nodes_;
    std::vector<std::vector<Edge>> adjacency_list_;
    
    // Index mappings for external ID to internal index
    containers::HashMap<uint32_t, uint32_t> node_id_to_index_;
    
    // Graph metadata
    size_t num_edges_;
    bool is_directed_;
    
    // For ID compaction and cache locality
    std::vector<uint32_t> free_node_indices_;
    uint32_t next_internal_id_;
    
    // Statistics for analysis
    mutable size_t access_count_;
    mutable size_t cache_hits_;
    
    uint32_t get_internal_index(uint32_t external_id) const {
        uint32_t* index = node_id_to_index_.get(external_id);
        if (!index) {
            throw std::runtime_error("Node ID not found: " + std::to_string(external_id));
        }
        access_count_++;
        cache_hits_++;
        return *index;
    }
    
    bool has_node_internal(uint32_t external_id) const {
        access_count_++;
        return node_id_to_index_.contains(external_id);
    }
    
    void compact_if_needed() {
        // Compact storage if fragmentation is high
        if (free_node_indices_.size() > nodes_.size() / 4) {
            compact_storage();
        }
    }
    
    void compact_storage() {
        std::vector<Node> new_nodes;
        std::vector<std::vector<Edge>> new_adjacency;
        containers::HashMap<uint32_t, uint32_t> new_mapping;
        
        new_nodes.reserve(nodes_.size() - free_node_indices_.size());
        new_adjacency.reserve(nodes_.size() - free_node_indices_.size());
        
        uint32_t new_index = 0;
        for (uint32_t old_index = 0; old_index < nodes_.size(); ++old_index) {
            // Skip free indices
            if (std::find(free_node_indices_.begin(), free_node_indices_.end(), old_index) 
                != free_node_indices_.end()) {
                continue;
            }
            
            new_nodes.push_back(nodes_[old_index]);
            new_adjacency.push_back(std::move(adjacency_list_[old_index]));
            new_mapping.put(nodes_[old_index].id, new_index);
            
            // Update edge indices in the adjacency list
            for (auto& edge : new_adjacency[new_index]) {
                uint32_t* from_new = new_mapping.get(edge.from);
                uint32_t* to_new = new_mapping.get(edge.to);
                if (from_new && to_new) {
                    // Update internal references if needed
                }
            }
            
            new_index++;
        }
        
        nodes_ = std::move(new_nodes);
        adjacency_list_ = std::move(new_adjacency);
        node_id_to_index_ = std::move(new_mapping);
        free_node_indices_.clear();
        next_internal_id_ = new_index;
    }

public:
    /**
     * @brief Constructor
     * @param directed Whether the graph is directed
     * @param initial_capacity Initial capacity for nodes
     */
    explicit WeightedGraph(bool directed = true, size_t initial_capacity = 1000)
        : is_directed_(directed), num_edges_(0), next_internal_id_(0),
          access_count_(0), cache_hits_(0) {
        nodes_.reserve(initial_capacity);
        adjacency_list_.reserve(initial_capacity);
    }
    
    /**
     * @brief Add a node to the graph
     * @param node Node to add
     * @return true if node was added, false if it already exists
     */
    bool add_node(const Node& node) {
        if (has_node_internal(node.id)) {
            return false; // Node already exists
        }
        
        uint32_t internal_index;
        if (!free_node_indices_.empty()) {
            internal_index = free_node_indices_.back();
            free_node_indices_.pop_back();
            nodes_[internal_index] = node;
            adjacency_list_[internal_index].clear();
        } else {
            internal_index = next_internal_id_++;
            nodes_.push_back(node);
            adjacency_list_.emplace_back();
        }
        
        node_id_to_index_.put(node.id, internal_index);
        return true;
    }
    
    /**
     * @brief Add multiple nodes efficiently
     */
    void add_nodes(const std::vector<Node>& new_nodes) {
        for (const auto& node : new_nodes) {
            add_node(node);
        }
    }
    
    /**
     * @brief Add an edge to the graph
     * @param edge Edge to add
     * @return true if edge was added successfully
     */
    bool add_edge(const Edge& edge) {
        if (!has_node_internal(edge.from) || !has_node_internal(edge.to)) {
            throw std::runtime_error("Cannot add edge: one or both nodes do not exist");
        }
        
        uint32_t from_index = get_internal_index(edge.from);
        
        // Check if edge already exists
        for (const auto& existing_edge : adjacency_list_[from_index]) {
            if (existing_edge.to == edge.to) {
                return false; // Edge already exists
            }
        }
        
        adjacency_list_[from_index].push_back(edge);
        nodes_[from_index].degree++;
        num_edges_++;
        
        // Add reverse edge for undirected graphs
        if (!is_directed_ && edge.from != edge.to) {
            Edge reverse_edge = edge.reverse();
            uint32_t to_index = get_internal_index(edge.to);
            adjacency_list_[to_index].push_back(reverse_edge);
            nodes_[to_index].degree++;
            // Note: num_edges_ represents directed edges, so we don't double count
        }
        
        return true;
    }
    
    /**
     * @brief Add edge with automatic reverse for undirected graphs
     */
    bool add_edge(uint32_t from, uint32_t to, float weight, float length = 0.0f) {
        if (length == 0.0f) length = weight * 10.0f; // Estimate
        return add_edge(Edge(from, to, weight, length));
    }
    
    /**
     * @brief Remove a node and all its edges
     * @param node_id ID of node to remove
     * @return true if node was found and removed
     */
    bool remove_node(uint32_t node_id) {
        if (!has_node_internal(node_id)) {
            return false;
        }
        
        uint32_t index = get_internal_index(node_id);
        
        // Remove all outgoing edges
        num_edges_ -= adjacency_list_[index].size();
        adjacency_list_[index].clear();
        
        // Remove all incoming edges (scan all other nodes)
        for (uint32_t i = 0; i < adjacency_list_.size(); ++i) {
            if (i == index) continue;
            
            auto& edges = adjacency_list_[i];
            edges.erase(
                std::remove_if(edges.begin(), edges.end(),
                    [node_id](const Edge& e) { return e.to == node_id; }),
                edges.end()
            );
        }
        
        // Mark index as free
        free_node_indices_.push_back(index);
        node_id_to_index_.erase(node_id);
        
        compact_if_needed();
        return true;
    }
    
    /**
     * @brief Remove an edge
     * @param from Source node ID
     * @param to Target node ID
     * @return true if edge was found and removed
     */
    bool remove_edge(uint32_t from, uint32_t to) {
        if (!has_node_internal(from)) {
            return false;
        }
        
        uint32_t from_index = get_internal_index(from);
        auto& edges = adjacency_list_[from_index];
        
        auto it = std::find_if(edges.begin(), edges.end(),
            [to](const Edge& e) { return e.to == to; });
        
        if (it != edges.end()) {
            edges.erase(it);
            nodes_[from_index].degree--;
            num_edges_--;
            
            // Remove reverse edge for undirected graphs
            if (!is_directed_ && from != to) {
                remove_edge(to, from);
            }
            
            return true;
        }
        
        return false;
    }
    
    /**
     * @brief Get node by ID
     * @param node_id Node ID to look up
     * @return Pointer to node if found, nullptr otherwise
     */
    const Node* get_node(uint32_t node_id) const {
        if (!has_node_internal(node_id)) {
            return nullptr;
        }
        
        uint32_t index = get_internal_index(node_id);
        return &nodes_[index];
    }
    
    /**
     * @brief Get mutable node by ID
     */
    Node* get_node_mutable(uint32_t node_id) {
        return const_cast<Node*>(static_cast<const WeightedGraph*>(this)->get_node(node_id));
    }
    
    /**
     * @brief Get all neighbors of a node
     * @param node_id Node ID
     * @return Vector of outgoing edges
     */
    const std::vector<Edge>& get_neighbors(uint32_t node_id) const {
        uint32_t index = get_internal_index(node_id);
        return adjacency_list_[index];
    }
    
    /**
     * @brief Get edge between two nodes
     * @param from Source node ID
     * @param to Target node ID
     * @return Pointer to edge if found, nullptr otherwise
     */
    const Edge* get_edge(uint32_t from, uint32_t to) const {
        if (!has_node_internal(from)) {
            return nullptr;
        }
        
        const auto& edges = get_neighbors(from);
        auto it = std::find_if(edges.begin(), edges.end(),
            [to](const Edge& e) { return e.to == to; });
        
        return (it != edges.end()) ? &(*it) : nullptr;
    }
    
    /**
     * @brief Get mutable edge between two nodes
     */
    Edge* get_edge_mutable(uint32_t from, uint32_t to) {
        return const_cast<Edge*>(static_cast<const WeightedGraph*>(this)->get_edge(from, to));
    }
    
    /**
     * @brief Check if node exists
     */
    bool has_node(uint32_t node_id) const {
        return has_node_internal(node_id);
    }
    
    /**
     * @brief Check if edge exists
     */
    bool has_edge(uint32_t from, uint32_t to) const {
        return get_edge(from, to) != nullptr;
    }
    
    /**
     * @brief Get number of nodes
     */
    size_t num_nodes() const {
        return nodes_.size() - free_node_indices_.size();
    }
    
    /**
     * @brief Get number of edges
     */
    size_t num_edges() const {
        return num_edges_;
    }
    
    /**
     * @brief Check if graph is directed
     */
    bool is_directed() const {
        return is_directed_;
    }
    
    /**
     * @brief Get all node IDs
     */
    std::vector<uint32_t> get_all_node_ids() const {
        return node_id_to_index_.keys();
    }
    
    /**
     * @brief Get all nodes
     */
    std::vector<Node> get_all_nodes() const {
        std::vector<Node> result;
        result.reserve(num_nodes());
        
        for (size_t i = 0; i < nodes_.size(); ++i) {
            if (std::find(free_node_indices_.begin(), free_node_indices_.end(), i) 
                == free_node_indices_.end()) {
                result.push_back(nodes_[i]);
            }
        }
        
        return result;
    }
    
    /**
     * @brief Get all edges
     */
    std::vector<Edge> get_all_edges() const {
        std::vector<Edge> result;
        result.reserve(num_edges_);
        
        for (size_t i = 0; i < adjacency_list_.size(); ++i) {
            if (std::find(free_node_indices_.begin(), free_node_indices_.end(), i) 
                == free_node_indices_.end()) {
                const auto& edges = adjacency_list_[i];
                result.insert(result.end(), edges.begin(), edges.end());
            }
        }
        
        return result;
    }
    
    /**
     * @brief Update edge weight dynamically (for traffic simulation)
     */
    bool update_edge_weight(uint32_t from, uint32_t to, float new_weight) {
        Edge* edge = get_edge_mutable(from, to);
        if (edge) {
            edge->weight = new_weight;
            return true;
        }
        return false;
    }
    
    /**
     * @brief Apply weight modifier to all edges from a node
     */
    void apply_weight_modifier(uint32_t node_id, float modifier) {
        if (!has_node_internal(node_id)) return;
        
        uint32_t index = get_internal_index(node_id);
        for (auto& edge : adjacency_list_[index]) {
            edge.weight *= modifier;
        }
    }
    
    /**
     * @brief Get graph density
     */
    double density() const {
        size_t n = num_nodes();
        if (n <= 1) return 0.0;
        
        size_t max_edges = is_directed_ ? n * (n - 1) : n * (n - 1) / 2;
        return static_cast<double>(num_edges_) / max_edges;
    }
    
    /**
     * @brief Get degree statistics
     */
    struct DegreeStats {
        uint32_t min_degree;
        uint32_t max_degree;
        double avg_degree;
        double std_dev;
    };
    
    DegreeStats get_degree_statistics() const {
        if (num_nodes() == 0) {
            return {0, 0, 0.0, 0.0};
        }
        
        std::vector<uint32_t> degrees;
        degrees.reserve(num_nodes());
        
        for (size_t i = 0; i < nodes_.size(); ++i) {
            if (std::find(free_node_indices_.begin(), free_node_indices_.end(), i) 
                == free_node_indices_.end()) {
                degrees.push_back(nodes_[i].degree);
            }
        }
        
        auto [min_it, max_it] = std::minmax_element(degrees.begin(), degrees.end());
        
        double sum = 0.0;
        for (uint32_t deg : degrees) {
            sum += deg;
        }
        double avg = sum / degrees.size();
        
        double variance = 0.0;
        for (uint32_t deg : degrees) {
            variance += (deg - avg) * (deg - avg);
        }
        variance /= degrees.size();
        
        return {*min_it, *max_it, avg, std::sqrt(variance)};
    }
    
    /**
     * @brief Get comprehensive graph statistics
     */
    struct GraphStatistics {
        size_t num_nodes;
        size_t num_edges;
        double density;
        DegreeStats degree_stats;
        size_t memory_usage_bytes;
        double cache_hit_rate;
        size_t num_components;  // Connected components
    };
    
    GraphStatistics get_statistics() const {
        GraphStatistics stats;
        stats.num_nodes = num_nodes();
        stats.num_edges = num_edges_;
        stats.density = density();
        stats.degree_stats = get_degree_statistics();
        stats.memory_usage_bytes = sizeof(Node) * nodes_.capacity() + 
                                  sizeof(Edge) * num_edges_ +
                                  node_id_to_index_.get_statistics().memory_usage_bytes;
        stats.cache_hit_rate = access_count_ > 0 ? 
            static_cast<double>(cache_hits_) / access_count_ : 0.0;
        stats.num_components = 0; // Would require BFS/DFS to compute
        
        return stats;
    }
    
    /**
     * @brief Clear all nodes and edges
     */
    void clear() {
        nodes_.clear();
        adjacency_list_.clear();
        node_id_to_index_.clear();
        free_node_indices_.clear();
        num_edges_ = 0;
        next_internal_id_ = 0;
        access_count_ = 0;
        cache_hits_ = 0;
    }
    
    /**
     * @brief Reserve capacity for nodes and edges
     */
    void reserve(size_t node_capacity, size_t edge_capacity = 0) {
        nodes_.reserve(node_capacity);
        adjacency_list_.reserve(node_capacity);
        if (edge_capacity > 0) {
            // Estimate average degree and reserve accordingly
            size_t avg_degree = edge_capacity / std::max(node_capacity, size_t(1));
            for (auto& adj_list : adjacency_list_) {
                adj_list.reserve(avg_degree);
            }
        }
    }
    
    /**
     * @brief Validate graph consistency
     */
    bool validate() const {
        // Check node-edge consistency
        for (size_t i = 0; i < adjacency_list_.size(); ++i) {
            if (std::find(free_node_indices_.begin(), free_node_indices_.end(), i) 
                != free_node_indices_.end()) {
                continue; // Skip free indices
            }
            
            const auto& edges = adjacency_list_[i];
            if (edges.size() != nodes_[i].degree) {
                return false; // Degree mismatch
            }
            
            for (const auto& edge : edges) {
                if (!has_node_internal(edge.to)) {
                    return false; // Edge points to non-existent node
                }
            }
        }
        
        return true;
    }
};

} // namespace rts::core::graph