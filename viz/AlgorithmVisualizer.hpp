#pragma once

#include "VisualizationFramework.hpp"
#include "../core/graph/WeightedGraph.hpp"
#include "../core/search/BFS.hpp"
#include "../core/search/DFS.hpp"
#include "../core/search/Dijkstra.hpp"
#include "../core/search/AStar.hpp"
#include <vector>
#include <map>
#include <set>
#include <queue>
#include <thread>
#include <chrono>

namespace rts::viz {

/**
 * @brief Graph visualization for search algorithms
 */
class GraphAlgorithmVisualizer : public Visualizer {
private:
    const rts::core::graph::WeightedGraph* graph_;
    std::map<uint32_t, std::pair<size_t, size_t>> node_positions_;
    size_t grid_width_;
    size_t grid_height_;
    bool animate_;
    int animation_delay_ms_;
    
    /**
     * @brief Calculate node positions for visualization
     */
    void calculate_node_positions() {
        if (!graph_) return;
        
        grid_width_ = std::min(width_ - 4, static_cast<size_t>(20));
        grid_height_ = std::min(height_ - 8, static_cast<size_t>(15));
        
        auto node_ids = graph_->get_all_node_ids();
        size_t nodes_per_row = static_cast<size_t>(std::ceil(std::sqrt(node_ids.size())));
        
        for (size_t i = 0; i < node_ids.size(); ++i) {
            uint32_t node_id = node_ids[i];
            size_t row = i / nodes_per_row;
            size_t col = i % nodes_per_row;
            
            // Scale to fit grid
            size_t x = (col * grid_width_) / std::max(nodes_per_row, static_cast<size_t>(1));
            size_t y = (row * grid_height_) / std::max((node_ids.size() + nodes_per_row - 1) / nodes_per_row, static_cast<size_t>(1));
            
            node_positions_[node_id] = {x + 2, y + 3}; // Offset for borders
        }\n    }\n    \npublic:\n    GraphAlgorithmVisualizer(const rts::core::graph::WeightedGraph* graph, \n                           const std::string& title = \"Graph Algorithm Visualization\",\n                           bool animate = true, int delay_ms = 500)\n        : Visualizer(title, 60, 20), graph_(graph), animate_(animate), animation_delay_ms_(delay_ms) {\n        \n        if (graph_) {\n            calculate_node_positions();\n        }\n    }\n    \n    /**\n     * @brief Set animation settings\n     */\n    void set_animation(bool enabled, int delay_ms = 500) {\n        animate_ = enabled;\n        animation_delay_ms_ = delay_ms;\n    }\n    \n    /**\n     * @brief Animate a step (with delay)\n     */\n    void animate_step() const {\n        if (animate_) {\n            std::this_thread::sleep_for(std::chrono::milliseconds(animation_delay_ms_));\n        }\n    }\n    \n    /**\n     * @brief Print the base graph\n     */\n    void print_graph(const std::set<uint32_t>& visited = {},\n                    const std::set<uint32_t>& current = {},\n                    const std::set<uint32_t>& frontier = {},\n                    const std::vector<uint32_t>& path = {}) const {\n        \n        if (!graph_) {\n            std::cout << \"No graph to visualize\\n\";\n            return;\n        }\n        \n        if (animate_) clear_screen();\n        print_title();\n        \n        // Create visualization grid\n        auto grid = create_grid(height_ - 6, width_, \" \");\n        \n        // Draw edges first\n        for (const auto& [node_id, pos] : node_positions_) {\n            auto neighbors = graph_->get_neighbors(node_id);\n            for (const auto& edge : neighbors) {\n                uint32_t neighbor_id = edge.to;\n                auto neighbor_it = node_positions_.find(neighbor_id);\n                if (neighbor_it != node_positions_.end()) {\n                    // Draw simple line representation\n                    size_t x1 = pos.first, y1 = pos.second;\n                    size_t x2 = neighbor_it->second.first, y2 = neighbor_it->second.second;\n                    \n                    if (y1 < grid.size() && x1 < grid[0].size() && \n                        y2 < grid.size() && x2 < grid[0].size()) {\n                        \n                        // Simple line drawing\n                        if (x1 == x2) {\n                            // Vertical line\n                            size_t start_y = std::min(y1, y2);\n                            size_t end_y = std::max(y1, y2);\n                            for (size_t y = start_y + 1; y < end_y; ++y) {\n                                if (y < grid.size()) {\n                                    grid[y][x1] = colorize(\"│\", Colors::DIM);\n                                }\n                            }\n                        } else if (y1 == y2) {\n                            // Horizontal line\n                            size_t start_x = std::min(x1, x2);\n                            size_t end_x = std::max(x1, x2);\n                            for (size_t x = start_x + 1; x < end_x; ++x) {\n                                if (x < grid[0].size()) {\n                                    grid[y1][x] = colorize(\"─\", Colors::DIM);\n                                }\n                            }\n                        } else {\n                            // Diagonal representation\n                            grid[y1][x1] = colorize(\"╲\", Colors::DIM);\n                        }\n                    }\n                }\n            }\n        }\n        \n        // Draw nodes\n        for (const auto& [node_id, pos] : node_positions_) {\n            size_t x = pos.first, y = pos.second;\n            \n            if (y < grid.size() && x < grid[0].size()) {\n                std::string node_char = std::to_string(node_id % 10);\n                \n                // Apply coloring based on node state\n                if (std::find(path.begin(), path.end(), node_id) != path.end()) {\n                    grid[y][x] = colorize(node_char, Colors::BG_GREEN + Colors::BLACK);\n                } else if (current.count(node_id)) {\n                    grid[y][x] = colorize(node_char, Colors::BG_RED + Colors::WHITE);\n                } else if (frontier.count(node_id)) {\n                    grid[y][x] = colorize(node_char, Colors::BG_YELLOW + Colors::BLACK);\n                } else if (visited.count(node_id)) {\n                    grid[y][x] = colorize(node_char, Colors::BG_BLUE + Colors::WHITE);\n                } else {\n                    grid[y][x] = colorize(node_char, Colors::WHITE);\n                }\n            }\n        }\n        \n        // Print the grid\n        print_grid(grid);\n        \n        // Print legend\n        std::cout << \"\\nLegend: \";\n        std::cout << colorize(\"U\", Colors::WHITE) << \"=Unvisited \";\n        std::cout << colorize(\"V\", Colors::BG_BLUE + Colors::WHITE) << \"=Visited \";\n        std::cout << colorize(\"F\", Colors::BG_YELLOW + Colors::BLACK) << \"=Frontier \";\n        std::cout << colorize(\"C\", Colors::BG_RED + Colors::WHITE) << \"=Current \";\n        std::cout << colorize(\"P\", Colors::BG_GREEN + Colors::BLACK) << \"=Path\";\n        std::cout << \"\\n\\n\";\n    }\n    \n    /**\n     * @brief Visualize BFS algorithm step by step\n     */\n    void visualize_bfs(uint32_t start_node, uint32_t target_node = 0) const {\n        if (!graph_) return;\n        \n        std::cout << \"Starting BFS Visualization...\\n\";\n        if (target_node != 0) {\n            std::cout << \"Finding path from \" << start_node << \" to \" << target_node << \"\\n\";\n        } else {\n            std::cout << \"Traversing from \" << start_node << \"\\n\";\n        }\n        \n        std::set<uint32_t> visited;\n        std::set<uint32_t> frontier;\n        std::set<uint32_t> current;\n        std::queue<uint32_t> queue;\n        std::map<uint32_t, uint32_t> parent;\n        \n        queue.push(start_node);\n        frontier.insert(start_node);\n        \n        int step = 0;\n        while (!queue.empty()) {\n            uint32_t current_node = queue.front();\n            queue.pop();\n            \n            current.clear();\n            current.insert(current_node);\n            frontier.erase(current_node);\n            visited.insert(current_node);\n            \n            std::cout << \"Step \" << ++step << \": Visiting node \" << current_node << \"\\n\";\n            print_graph(visited, current, frontier);\n            \n            if (target_node != 0 && current_node == target_node) {\n                std::cout << colorize(\"Target found!\", Colors::GREEN) << \"\\n\";\n                \n                // Reconstruct and show path\n                std::vector<uint32_t> path;\n                uint32_t node = target_node;\n                while (node != start_node) {\n                    path.push_back(node);\n                    auto it = parent.find(node);\n                    if (it == parent.end()) break;\n                    node = it->second;\n                }\n                path.push_back(start_node);\n                std::reverse(path.begin(), path.end());\n                \n                print_graph(visited, {}, {}, path);\n                return;\n            }\n            \n            // Add neighbors to frontier\n            auto neighbors = graph_->get_neighbors(current_node);\n            for (const auto& edge : neighbors) {\n                uint32_t neighbor = edge.to;\n                if (visited.find(neighbor) == visited.end() && \n                    frontier.find(neighbor) == frontier.end()) {\n                    \n                    queue.push(neighbor);\n                    frontier.insert(neighbor);\n                    parent[neighbor] = current_node;\n                }\n            }\n            \n            animate_step();\n        }\n        \n        if (target_node != 0) {\n            std::cout << colorize(\"Target not reachable!\", Colors::RED) << \"\\n\";\n        } else {\n            std::cout << colorize(\"BFS traversal complete!\", Colors::GREEN) << \"\\n\";\n        }\n    }\n    \n    /**\n     * @brief Visualize DFS algorithm step by step\n     */\n    void visualize_dfs(uint32_t start_node, uint32_t target_node = 0) const {\n        if (!graph_) return;\n        \n        std::cout << \"Starting DFS Visualization...\\n\";\n        if (target_node != 0) {\n            std::cout << \"Finding path from \" << start_node << \" to \" << target_node << \"\\n\";\n        } else {\n            std::cout << \"Traversing from \" << start_node << \"\\n\";\n        }\n        \n        std::set<uint32_t> visited;\n        std::set<uint32_t> current;\n        std::vector<uint32_t> stack;\n        std::map<uint32_t, uint32_t> parent;\n        \n        stack.push_back(start_node);\n        \n        int step = 0;\n        while (!stack.empty()) {\n            uint32_t current_node = stack.back();\n            stack.pop_back();\n            \n            if (visited.find(current_node) != visited.end()) {\n                continue; // Already visited\n            }\n            \n            current.clear();\n            current.insert(current_node);\n            visited.insert(current_node);\n            \n            std::cout << \"Step \" << ++step << \": Visiting node \" << current_node << \"\\n\";\n            print_graph(visited, current);\n            \n            if (target_node != 0 && current_node == target_node) {\n                std::cout << colorize(\"Target found!\", Colors::GREEN) << \"\\n\";\n                \n                // Reconstruct and show path\n                std::vector<uint32_t> path;\n                uint32_t node = target_node;\n                while (node != start_node) {\n                    path.push_back(node);\n                    auto it = parent.find(node);\n                    if (it == parent.end()) break;\n                    node = it->second;\n                }\n                path.push_back(start_node);\n                std::reverse(path.begin(), path.end());\n                \n                print_graph(visited, {}, {}, path);\n                return;\n            }\n            \n            // Add neighbors to stack (in reverse order for consistent traversal)\n            auto neighbors = graph_->get_neighbors(current_node);\n            for (auto it = neighbors.rbegin(); it != neighbors.rend(); ++it) {\n                uint32_t neighbor = it->to;\n                if (visited.find(neighbor) == visited.end()) {\n                    stack.push_back(neighbor);\n                    if (parent.find(neighbor) == parent.end()) {\n                        parent[neighbor] = current_node;\n                    }\n                }\n            }\n            \n            animate_step();\n        }\n        \n        if (target_node != 0) {\n            std::cout << colorize(\"Target not reachable!\", Colors::RED) << \"\\n\";\n        } else {\n            std::cout << colorize(\"DFS traversal complete!\", Colors::GREEN) << \"\\n\";\n        }\n    }\n    \n    /**\n     * @brief Visualize Dijkstra's algorithm step by step\n     */\n    void visualize_dijkstra(uint32_t start_node, uint32_t target_node = 0) const {\n        if (!graph_) return;\n        \n        std::cout << \"Starting Dijkstra's Algorithm Visualization...\\n\";\n        if (target_node != 0) {\n            std::cout << \"Finding shortest path from \" << start_node << \" to \" << target_node << \"\\n\";\n        } else {\n            std::cout << \"Finding shortest paths from \" << start_node << \"\\n\";\n        }\n        \n        // For visualization, we'll simulate the algorithm steps\n        rts::core::search::Dijkstra dijkstra;\n        auto result = dijkstra.find_shortest_paths(*graph_, start_node);\n        \n        if (target_node != 0) {\n            if (result.is_reachable(target_node)) {\n                auto path = result.get_path(target_node);\n                double distance = result.get_distance(target_node);\n                \n                std::cout << colorize(\"Shortest path found!\", Colors::GREEN) << \"\\n\";\n                std::cout << \"Distance: \" << std::fixed << std::setprecision(2) << distance << \"\\n\";\n                \n                // Show visited nodes and final path\n                auto all_distances = result.get_all_distances();\n                std::set<uint32_t> visited;\n                for (const auto& [node_id, dist] : all_distances) {\n                    visited.insert(node_id);\n                }\n                \n                print_graph(visited, {}, {}, path);\n            } else {\n                std::cout << colorize(\"No path exists!\", Colors::RED) << \"\\n\";\n            }\n        } else {\n            // Show all reachable nodes\n            auto all_distances = result.get_all_distances();\n            std::set<uint32_t> visited;\n            for (const auto& [node_id, dist] : all_distances) {\n                visited.insert(node_id);\n            }\n            \n            print_graph(visited);\n            \n            // Print distance table\n            std::cout << \"\\nShortest distances from node \" << start_node << \":\\n\";\n            std::cout << colorize(\"┌──────┬──────────┐\", Colors::CYAN) << \"\\n\";\n            std::cout << colorize(\"│ Node │ Distance │\", Colors::BOLD) << \"\\n\";\n            std::cout << colorize(\"├──────┼──────────┤\", Colors::CYAN) << \"\\n\";\n            \n            for (const auto& [node_id, dist] : all_distances) {\n                std::cout << colorize(\"│ \", Colors::CYAN) \n                          << std::setw(4) << node_id\n                          << colorize(\" │ \", Colors::CYAN)\n                          << std::setw(8) << std::fixed << std::setprecision(2) << dist\n                          << colorize(\" │\", Colors::CYAN) << \"\\n\";\n            }\n            \n            std::cout << colorize(\"└──────┴──────────┘\", Colors::CYAN) << \"\\n\";\n        }\n    }\n};\n\n} // namespace rts::viz"
                }
                }
                step++;
            }
        }
        
        // Create visualization grid
        auto grid = create_grid(height_ - 6, width_, " ");
        
        // Draw edges first
        for (const auto& [node_id, pos] : node_positions_) {
            auto neighbors = graph_->get_neighbors(node_id);
            for (const auto& edge : neighbors) {
                uint32_t neighbor_id = edge.to;
                auto neighbor_it = node_positions_.find(neighbor_id);
                if (neighbor_it != node_positions_.end()) {
                    // Draw simple line representation
                    size_t x1 = pos.first, y1 = pos.second;
                    size_t x2 = neighbor_it->second.first, y2 = neighbor_it->second.second;
                    
                    if (y1 < grid.size() && x1 < grid[0].size() && 
                        y2 < grid.size() && x2 < grid[0].size()) {
                        
                        // Simple line drawing
                        if (x1 == x2) {
                            // Vertical line
                            size_t start_y = std::min(y1, y2);
                            size_t end_y = std::max(y1, y2);
                            for (size_t y = start_y + 1; y < end_y; ++y) {
                                if (y < grid.size()) {
                                    grid[y][x1] = colorize("│", Colors::DIM);
                                }
                            }
                        } else if (y1 == y2) {
                            // Horizontal line
                            size_t start_x = std::min(x1, x2);
                            size_t end_x = std::max(x1, x2);
                            for (size_t x = start_x + 1; x < end_x; ++x) {
                                if (x < grid[0].size()) {
                                    grid[y1][x] = colorize("─", Colors::DIM);
                                }
                            }
                        } else {
                            // Diagonal representation
                            grid[y1][x1] = colorize("╲", Colors::DIM);
                        }
                    }
                }
            }
        }
        
        // Draw nodes
        for (const auto& [node_id, pos] : node_positions_) {
            size_t x = pos.first, y = pos.second;
            
            if (y < grid.size() && x < grid[0].size()) {
                std::string node_char = std::to_string(node_id % 10);
                
                // Apply coloring based on node state
                if (std::find(path.begin(), path.end(), node_id) != path.end()) {
                    grid[y][x] = colorize(node_char, Colors::BG_GREEN + Colors::BLACK);
                } else if (current.count(node_id)) {
                    grid[y][x] = colorize(node_char, Colors::BG_RED + Colors::WHITE);
                } else if (frontier.count(node_id)) {
                    grid[y][x] = colorize(node_char, Colors::BG_YELLOW + Colors::BLACK);
                } else if (visited.count(node_id)) {
                    grid[y][x] = colorize(node_char, Colors::BG_BLUE + Colors::WHITE);
                } else {
                    grid[y][x] = colorize(node_char, Colors::WHITE);
                }
            }
        }
        
        // Print the grid
        print_grid(grid);
        
        // Print legend
        std::cout << "\nLegend: ";
        std::cout << colorize("U", Colors::WHITE) << "=Unvisited ";
        std::cout << colorize("V", Colors::BG_BLUE + Colors::WHITE) << "=Visited ";
        std::cout << colorize("F", Colors::BG_YELLOW + Colors::BLACK) << "=Frontier ";
        std::cout << colorize("C", Colors::BG_RED + Colors::WHITE) << "=Current ";
        std::cout << colorize("P", Colors::BG_GREEN + Colors::BLACK) << "=Path";
        std::cout << "\n\n";
    }
    
    /**
     * @brief Visualize BFS algorithm step by step
     */
    void visualize_bfs(uint32_t start_node, uint32_t target_node = 0) const {
        if (!graph_) return;
        
        std::cout << "Starting BFS Visualization...\n";
        if (target_node != 0) {
            std::cout << "Finding path from " << start_node << " to " << target_node << "\n";
        } else {
            std::cout << "Traversing from " << start_node << "\n";
        }
        
        std::set<uint32_t> visited;
        std::set<uint32_t> frontier;
        std::set<uint32_t> current;
        std::queue<uint32_t> queue;
        std::map<uint32_t, uint32_t> parent;
        
        queue.push(start_node);
        frontier.insert(start_node);
        
        int step = 0;
        while (!queue.empty()) {
            uint32_t current_node = queue.front();
            queue.pop();
            
            current.clear();
            current.insert(current_node);
            frontier.erase(current_node);
            visited.insert(current_node);
            
            std::cout << "Step " << ++step << ": Visiting node " << current_node << "\n";
            print_graph(visited, current, frontier);
            
            if (target_node != 0 && current_node == target_node) {
                std::cout << colorize("Target found!", Colors::GREEN) << "\n";
                
                // Reconstruct and show path
                std::vector<uint32_t> path;
                uint32_t node = target_node;
                while (node != start_node) {
                    path.push_back(node);
                    auto it = parent.find(node);
                    if (it == parent.end()) break;
                    node = it->second;
                }
                path.push_back(start_node);
                std::reverse(path.begin(), path.end());
                
                print_graph(visited, {}, {}, path);
                return;
            }
            
            // Add neighbors to frontier
            auto neighbors = graph_->get_neighbors(current_node);
            for (const auto& edge : neighbors) {
                uint32_t neighbor = edge.to;
                if (visited.find(neighbor) == visited.end() && 
                    frontier.find(neighbor) == frontier.end()) {
                    
                    queue.push(neighbor);
                    frontier.insert(neighbor);
                    parent[neighbor] = current_node;
                }
            }
            
            animate_step();
        }
        
        if (target_node != 0) {
            std::cout << colorize("Target not reachable!", Colors::RED) << "\n";
        } else {
            std::cout << colorize("BFS traversal complete!", Colors::GREEN) << "\n";
        }
    }
    
    /**
     * @brief Visualize Dijkstra's algorithm step by step
     */
    void visualize_dijkstra(uint32_t start_node, uint32_t target_node = 0) const {
        if (!graph_) return;
        
        std::cout << "Starting Dijkstra's Algorithm Visualization...\n";
        if (target_node != 0) {
            std::cout << "Finding shortest path from " << start_node << " to " << target_node << "\n";
        } else {
            std::cout << "Finding shortest paths from " << start_node << "\n";
        }
        
        // For visualization, we'll use the actual algorithm
        rts::core::search::Dijkstra dijkstra;
        auto result = dijkstra.find_shortest_paths(*graph_, start_node);
        
        if (target_node != 0) {
            if (result.is_reachable(target_node)) {
                auto path = result.get_path(target_node);
                double distance = result.get_distance(target_node);
                
                std::cout << colorize("Shortest path found!", Colors::GREEN) << "\n";
                std::cout << "Distance: " << std::fixed << std::setprecision(2) << distance << "\n";
                
                // Show visited nodes and final path
                auto all_distances = result.get_all_distances();
                std::set<uint32_t> visited;
                for (const auto& [node_id, dist] : all_distances) {
                    visited.insert(node_id);
                }
                
                print_graph(visited, {}, {}, path);
            } else {
                std::cout << colorize("No path exists!", Colors::RED) << "\n";
            }
        }
    }
};

} // namespace rts::viz