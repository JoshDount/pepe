#pragma once

#include "../BenchmarkFramework.hpp"
#include "../ComplexityAnalysis.hpp"
#include "../../core/graph/WeightedGraph.hpp"
#include "../../core/search/BFS.hpp"
#include "../../core/search/DFS.hpp"
#include "../../core/search/Dijkstra.hpp"
#include "../../core/search/AStar.hpp"
#include "../../core/search/MST.hpp"
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
#include <set>

namespace rts::bench::scenarios {

/**
 * @brief Graph algorithms benchmark scenarios
 */
class GraphAlgorithmBenchmarks {
private:
    std::mt19937 rng_;
    
    /**
     * @brief Graph generation patterns
     */
    enum class GraphPattern {
        SPARSE,          // Low edge density (E ≈ V)
        DENSE,           // High edge density (E ≈ V²)
        GRID,            // Grid-like structure
        TREE,            // Tree structure (E = V-1)
        SCALE_FREE,      // Scale-free network (realistic)
        COMPLETE,        // Complete graph (maximum edges)
        PLANAR           // Planar graph structure
    };
    
    /**
     * @brief Generate graphs with different patterns
     */
    rts::core::graph::WeightedGraph generate_graph(size_t num_nodes, GraphPattern pattern) {
        using namespace rts::core::graph;
        WeightedGraph graph;
        
        // Add nodes with geographic coordinates
        std::uniform_real_distribution<double> lat_dist(40.0, 41.0);
        std::uniform_real_distribution<double> lon_dist(-75.0, -73.0);
        
        for (size_t i = 1; i <= num_nodes; ++i) {
            Node node(static_cast<uint32_t>(i), lat_dist(rng_), lon_dist(rng_));
            graph.add_node(node);
        }
        
        // Add edges based on pattern
        std::uniform_real_distribution<float> weight_dist(1.0f, 100.0f);
        
        switch (pattern) {
            case GraphPattern::SPARSE: {
                // Each node connected to 2-3 other nodes on average
                std::uniform_int_distribution<uint32_t> node_dist(1, static_cast<uint32_t>(num_nodes));
                size_t target_edges = num_nodes * 2;
                
                for (size_t i = 0; i < target_edges * 2; ++i) {
                    uint32_t from = node_dist(rng_);
                    uint32_t to = node_dist(rng_);
                    if (from != to && !graph.has_edge(from, to)) {
                        graph.add_edge(from, to, weight_dist(rng_));
                    }
                }
                break;
            }
            
            case GraphPattern::DENSE: {
                // Each node connected to ~50% of other nodes
                std::uniform_real_distribution<double> prob_dist(0.0, 1.0);
                double connection_prob = 0.5;
                
                for (uint32_t i = 1; i <= num_nodes; ++i) {
                    for (uint32_t j = i + 1; j <= num_nodes; ++j) {
                        if (prob_dist(rng_) < connection_prob) {
                            graph.add_edge(i, j, weight_dist(rng_));
                        }
                    }
                }
                break;
            }
            
            case GraphPattern::GRID: {
                // Grid structure: sqrt(n) x sqrt(n)
                size_t grid_size = static_cast<size_t>(std::sqrt(num_nodes));
                if (grid_size * grid_size < num_nodes) grid_size++;
                
                for (size_t i = 0; i < grid_size; ++i) {
                    for (size_t j = 0; j < grid_size; ++j) {
                        uint32_t current = static_cast<uint32_t>(i * grid_size + j + 1);
                        if (current > num_nodes) break;
                        
                        // Connect to right neighbor
                        if (j + 1 < grid_size) {
                            uint32_t right = static_cast<uint32_t>(i * grid_size + j + 2);
                            if (right <= num_nodes) {
                                graph.add_edge(current, right, weight_dist(rng_));
                            }
                        }
                        
                        // Connect to bottom neighbor
                        if (i + 1 < grid_size) {
                            uint32_t bottom = static_cast<uint32_t>((i + 1) * grid_size + j + 1);
                            if (bottom <= num_nodes) {
                                graph.add_edge(current, bottom, weight_dist(rng_));
                            }
                        }
                    }
                }
                break;
            }
            
            case GraphPattern::TREE: {
                // Random spanning tree
                std::vector<uint32_t> nodes;
                for (uint32_t i = 1; i <= num_nodes; ++i) {
                    nodes.push_back(i);
                }
                std::shuffle(nodes.begin(), nodes.end(), rng_);
                
                // Connect each node to a random previous node
                for (size_t i = 1; i < nodes.size(); ++i) {
                    std::uniform_int_distribution<size_t> parent_dist(0, i - 1);
                    size_t parent_idx = parent_dist(rng_);
                    graph.add_edge(nodes[parent_idx], nodes[i], weight_dist(rng_));
                }
                break;
            }
            
            case GraphPattern::SCALE_FREE: {
                // Preferential attachment model
                std::vector<uint32_t> degree_list;
                
                // Start with a small complete graph
                size_t initial_nodes = std::min(static_cast<size_t>(5), num_nodes);
                for (uint32_t i = 1; i <= initial_nodes; ++i) {
                    for (uint32_t j = i + 1; j <= initial_nodes; ++j) {
                        graph.add_edge(i, j, weight_dist(rng_));
                        degree_list.push_back(i);
                        degree_list.push_back(j);
                    }
                }
                
                // Add remaining nodes with preferential attachment
                for (uint32_t new_node = static_cast<uint32_t>(initial_nodes + 1); new_node <= num_nodes; ++new_node) {
                    size_t connections = std::min(static_cast<size_t>(3), degree_list.size());
                    std::set<uint32_t> connected;
                    
                    for (size_t i = 0; i < connections; ++i) {
                        std::uniform_int_distribution<size_t> target_dist(0, degree_list.size() - 1);
                        uint32_t target = degree_list[target_dist(rng_)];
                        
                        if (connected.find(target) == connected.end()) {
                            graph.add_edge(new_node, target, weight_dist(rng_));
                            degree_list.push_back(new_node);
                            degree_list.push_back(target);
                            connected.insert(target);
                        }
                    }
                }
                break;
            }
            
            case GraphPattern::COMPLETE: {
                // Complete graph - every node connected to every other
                for (uint32_t i = 1; i <= num_nodes; ++i) {
                    for (uint32_t j = i + 1; j <= num_nodes; ++j) {
                        graph.add_edge(i, j, weight_dist(rng_));
                    }
                }
                break;
            }
            
            case GraphPattern::PLANAR: {
                // Simple planar graph (Delaunay-like structure)
                // Connect each node to its k nearest neighbors
                size_t k = std::min(static_cast<size_t>(6), num_nodes - 1);
                
                for (uint32_t i = 1; i <= num_nodes; ++i) {
                    std::vector<std::pair<double, uint32_t>> distances;
                    
                    for (uint32_t j = 1; j <= num_nodes; ++j) {
                        if (i != j) {
                            // Simple distance based on node IDs
                            double dist = std::abs(static_cast<double>(i) - static_cast<double>(j));
                            distances.emplace_back(dist, j);
                        }
                    }
                    
                    std::sort(distances.begin(), distances.end());
                    
                    for (size_t idx = 0; idx < std::min(k, distances.size()); ++idx) {
                        uint32_t neighbor = distances[idx].second;
                        if (!graph.has_edge(i, neighbor)) {
                            graph.add_edge(i, neighbor, weight_dist(rng_));
                        }
                    }
                }
                break;
            }
        }
        
        return graph;
    }
    
    std::string pattern_name(GraphPattern pattern) const {
        switch (pattern) {
            case GraphPattern::SPARSE: return "Sparse";
            case GraphPattern::DENSE: return "Dense";
            case GraphPattern::GRID: return "Grid";
            case GraphPattern::TREE: return "Tree";
            case GraphPattern::SCALE_FREE: return "ScaleFree";
            case GraphPattern::COMPLETE: return "Complete";
            case GraphPattern::PLANAR: return "Planar";
            default: return "Unknown";
        }
    }
    
public:
    GraphAlgorithmBenchmarks() : rng_(std::random_device{}()) {}
    
    /**
     * @brief Run comprehensive graph algorithm benchmarks
     */
    void run_comprehensive_benchmarks(BenchmarkSuite& suite) {
        std::cout << "Running Graph Algorithm Benchmarks...\n";
        
        // Test different graph sizes
        std::vector<size_t> sizes = {50, 100, 250, 500, 1000, 2500};
        
        // Test different graph patterns
        std::vector<GraphPattern> patterns = {
            GraphPattern::SPARSE,
            GraphPattern::DENSE,
            GraphPattern::GRID,
            GraphPattern::TREE,
            GraphPattern::SCALE_FREE
        };
        
        // Benchmark each algorithm with each pattern and size
        for (GraphPattern pattern : patterns) {
            std::string pattern_str = pattern_name(pattern);
            
            for (size_t size : sizes) {
                auto graph = generate_graph(size, pattern);
                
                // BFS benchmarks
                suite.add_benchmark("BFS_" + pattern_str + "_n" + std::to_string(size),
                    [graph, size]() {
                        rts::core::search::BFS bfs;
                        std::uniform_int_distribution<uint32_t> node_dist(1, static_cast<uint32_t>(size));
                        std::mt19937 local_rng(std::random_device{}());
                        uint32_t start = node_dist(local_rng);
                        bfs.traverse(graph, start);
                    });
                
                // DFS benchmarks
                suite.add_benchmark("DFS_" + pattern_str + "_n" + std::to_string(size),
                    [graph, size]() {
                        rts::core::search::DFS dfs;
                        std::uniform_int_distribution<uint32_t> node_dist(1, static_cast<uint32_t>(size));
                        std::mt19937 local_rng(std::random_device{}());
                        uint32_t start = node_dist(local_rng);
                        dfs.traverse_recursive(graph, start);
                    });
                
                // Dijkstra benchmarks
                suite.add_benchmark("Dijkstra_" + pattern_str + "_n" + std::to_string(size),
                    [graph, size]() {
                        rts::core::search::Dijkstra dijkstra;
                        std::uniform_int_distribution<uint32_t> node_dist(1, static_cast<uint32_t>(size));
                        std::mt19937 local_rng(std::random_device{}());
                        uint32_t start = node_dist(local_rng);
                        dijkstra.find_shortest_paths(graph, start);
                    });
                
                // A* benchmarks (only for smaller graphs due to complexity)
                if (size <= 500) {
                    suite.add_benchmark("AStar_" + pattern_str + "_n" + std::to_string(size),
                        [graph, size]() {
                            rts::core::search::AStar astar;
                            rts::core::search::AStar::HaversineHeuristic heuristic;
                            std::uniform_int_distribution<uint32_t> node_dist(1, static_cast<uint32_t>(size));
                            std::mt19937 local_rng(std::random_device{}());
                            uint32_t start = node_dist(local_rng);
                            uint32_t end = node_dist(local_rng);
                            if (start != end) {
                                astar.find_path(graph, start, end, heuristic);
                            }
                        });
                }
                
                // MST benchmarks (only for connected patterns)
                if (pattern != GraphPattern::TREE) {
                    suite.add_benchmark("MST_Kruskal_" + pattern_str + "_n" + std::to_string(size),
                        [graph]() {
                            rts::core::search::MST mst;
                            mst.kruskal(graph);
                        });
                    
                    suite.add_benchmark("MST_Prim_" + pattern_str + "_n" + std::to_string(size),
                        [graph]() {
                            rts::core::search::MST mst;
                            // Use first node as starting point for Prim's algorithm
                            mst.prim(graph, 1);
                        });
                }
            }
        }
    }
    
    /**
     * @brief Run pathfinding algorithm comparison
     */
    void run_pathfinding_comparison(BenchmarkSuite& suite) {
        std::cout << "Running Pathfinding Algorithm Comparison...\n";
        
        std::vector<size_t> sizes = {100, 250, 500, 1000};
        
        for (size_t size : sizes) {
            auto graph = generate_graph(size, GraphPattern::SPARSE);
            
            // BFS pathfinding
            suite.add_benchmark("BFS_Pathfinding_n" + std::to_string(size),
                [graph, size]() {
                    rts::core::search::BFS bfs;
                    std::uniform_int_distribution<uint32_t> node_dist(1, static_cast<uint32_t>(size));
                    std::mt19937 local_rng(std::random_device{}());
                    uint32_t start = node_dist(local_rng);
                    uint32_t end = node_dist(local_rng);
                    if (start != end) {
                        bfs.find_path(graph, start, end);
                    }
                });
            
            // Dijkstra pathfinding
            suite.add_benchmark("Dijkstra_Pathfinding_n" + std::to_string(size),
                [graph, size]() {
                    rts::core::search::Dijkstra dijkstra;
                    std::uniform_int_distribution<uint32_t> node_dist(1, static_cast<uint32_t>(size));
                    std::mt19937 local_rng(std::random_device{}());
                    uint32_t start = node_dist(local_rng);
                    auto result = dijkstra.find_shortest_paths(graph, start);
                });
            
            // A* pathfinding
            if (size <= 500) {
                suite.add_benchmark("AStar_Pathfinding_n" + std::to_string(size),
                    [graph, size]() {
                        rts::core::search::AStar astar;
                        rts::core::search::AStar::HaversineHeuristic heuristic;
                        std::uniform_int_distribution<uint32_t> node_dist(1, static_cast<uint32_t>(size));
                        std::mt19937 local_rng(std::random_device{}());
                        uint32_t start = node_dist(local_rng);
                        uint32_t end = node_dist(local_rng);
                        if (start != end) {
                            astar.find_path(graph, start, end, heuristic);
                        }
                    });
            }
        }
    }
    
    /**
     * @brief Analyze graph algorithm complexity
     */
    void analyze_graph_complexity(const std::vector<std::unique_ptr<BenchmarkResult>>& results) {
        std::cout << "\nAnalyzing Graph Algorithm Complexity...\n";
        
        // Separate results by algorithm
        std::map<std::string, std::vector<const BenchmarkResult*>> algorithm_results;
        
        for (const auto& result : results) {
            // Extract algorithm name from benchmark name
            std::string algo_name = result->name;
            size_t underscore_pos = algo_name.find('_');
            if (underscore_pos != std::string::npos) {
                algo_name = algo_name.substr(0, underscore_pos);
            }
            
            algorithm_results[algo_name].push_back(result.get());
        }
        
        // Analyze each algorithm
        for (const auto& [algo_name, algo_results] : algorithm_results) {
            ComplexityAnalyzer analyzer;
            
            for (const auto* result : algo_results) {
                if (result->input_size > 0) {
                    analyzer.add_data_point(result->input_size, 
                                          result->execution_time_ms.mean,
                                          result->memory_usage_mb.mean);
                }
            }
            
            std::cout << "\n" << algo_name << " Complexity Analysis:\n";
            std::cout << std::string(40, '-') << "\n";
            
            auto time_fit = analyzer.analyze_time_complexity();
            if (time_fit.complexity_class != ComplexityAnalyzer::ComplexityClass::UNKNOWN) {
                std::cout << "Time Complexity: " << time_fit.formula 
                          << " (confidence: " << std::fixed << std::setprecision(1) 
                          << time_fit.confidence_score * 100 << "%)\n";
                
                // Compare with theoretical complexity
                std::string expected_complexity = get_expected_complexity(algo_name);
                if (!expected_complexity.empty()) {
                    std::cout << "Expected: " << expected_complexity << "\n";
                    std::cout << "Match: " << (time_fit.formula == expected_complexity ? "✓" : "✗") << "\n";
                }
            }
        }
    }
    
private:
    std::string get_expected_complexity(const std::string& algorithm) const {
        if (algorithm == "BFS" || algorithm == "DFS") return "O(n)";
        if (algorithm == "Dijkstra") return "O(n log n)";
        if (algorithm == "AStar") return "O(n log n)";
        if (algorithm == "MST") return "O(n log n)";
        return "";
    }
};

} // namespace rts::bench::scenarios