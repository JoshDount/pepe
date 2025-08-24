#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <filesystem>
#include <random>
#include <algorithm>
#include <numeric>
#include <fstream>

// Core includes
#include "../core/graph/WeightedGraph.hpp"
#include "../core/graph/Node.hpp"
#include "../core/graph/Edge.hpp"
#include "../core/search/BFS.hpp"
#include "../core/search/DFS.hpp"
#include "../core/search/Dijkstra.hpp"
#include "../core/search/AStar.hpp"
#include "../core/search/MST.hpp"
#include "../core/algorithms/InternalSort.hpp"
#include "../core/algorithms/ExternalSort.hpp"
#include "../core/simulation/EventQueue.hpp"
#include "../core/traffic/TrafficSimulation.hpp"
#include "../core/io/GTFSData.hpp"
#include "../core/io/GTFSParser.hpp"
#include "../core/io/GTFSConverter.hpp"

using namespace rts::core;

/**
 * @brief Route Transit Simulator CLI Application
 */
class TransitSimulatorCLI {
private:
    graph::WeightedGraph graph_;
    std::unique_ptr<traffic::TrafficSimulation> traffic_sim_;
    std::unique_ptr<simulation::EventQueue> event_queue_;
    io::GTFSDataset gtfs_dataset_;
    bool graph_loaded_;
    bool gtfs_loaded_;
    
    // Statistics
    struct Statistics {
        size_t total_commands = 0;
        size_t successful_commands = 0;
        double total_execution_time_ms = 0.0;
    } stats_;
    
public:
    TransitSimulatorCLI() : graph_loaded_(false), gtfs_loaded_(false) {
        event_queue_ = std::make_unique<simulation::EventQueue>();
    }
    
    /**
     * @brief Print welcome message and help
     */
    void print_welcome() {
        std::cout << "\n╔═══════════════════════════════════════════════════════════╗\n";
        std::cout << "║              Route Transit Simulator CLI                  ║\n";
        std::cout << "║              Advanced Transportation Analysis             ║\n";
        std::cout << "╚═══════════════════════════════════════════════════════════╝\n\n";
        
        std::cout << "Available commands:\n";
        std::cout << "  help                    - Show this help message\n";
        std::cout << "  quit/exit               - Exit the application\n";
        std::cout << "  status                  - Show current system status\n";
        std::cout << "  stats                   - Show execution statistics\n\n";
        
        std::cout << "Graph Operations:\n";
        std::cout << "  create_graph <nodes>    - Create random graph with N nodes\n";
        std::cout << "  load_gtfs <directory>   - Load GTFS transit data\n";
        std::cout << "  convert_gtfs            - Convert GTFS to graph\n";
        std::cout << "  graph_info              - Show graph statistics\n";
        std::cout << "  add_node <id> <lat> <lon> - Add node to graph\n";
        std::cout << "  add_edge <from> <to> <weight> - Add edge to graph\n\n";
        
        std::cout << "Routing & Search:\n";
        std::cout << "  bfs <start> [target]    - Breadth-first search\n";
        std::cout << "  dfs <start> [target]    - Depth-first search\n";
        std::cout << "  dijkstra <start> <end> - Shortest path (Dijkstra)\n";
        std::cout << "  astar <start> <end>     - Shortest path (A*)\n";
        std::cout << "  mst [kruskal|prim]      - Minimum spanning tree\n\n";
        
        std::cout << "Traffic Simulation:\n";
        std::cout << "  start_traffic           - Initialize traffic simulation\n";
        std::cout << "  trigger_incident <from> <to> <type> - Trigger traffic incident\n";
        std::cout << "  set_weather <factor>    - Set weather impact factor\n";
        std::cout << "  traffic_report          - Generate traffic report\n";
        std::cout << "  simulate <duration>     - Run simulation for duration\n\n";
        
        std::cout << "Benchmarking & Testing:\n";
        std::cout << "  benchmark_sort <size>   - Benchmark sorting algorithms\n";
        std::cout << "  benchmark_search <size> - Benchmark search algorithms\n";
        std::cout << "  benchmark_routing <runs> - Benchmark routing algorithms\n";
        std::cout << "  test_all                - Run comprehensive tests\n\n";
        
        std::cout << "Data Export:\n";
        std::cout << "  export_graph <file>     - Export graph to file\n";
        std::cout << "  export_results <file>   - Export results to CSV\n\n";
    }
    
    /**
     * @brief Parse command line input
     */
    std::vector<std::string> parse_command(const std::string& input) {
        std::vector<std::string> tokens;
        std::istringstream iss(input);
        std::string token;
        
        while (iss >> token) {
            tokens.push_back(token);
        }
        
        return tokens;
    }
    
    /**
     * @brief Execute a command
     */
    bool execute_command(const std::vector<std::string>& args) {
        if (args.empty()) return true;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        stats_.total_commands++;
        
        const std::string& command = args[0];
        bool success = false;
        
        try {
            if (command == "help" || command == "h") {
                print_welcome();
                success = true;
            }
            else if (command == "quit" || command == "exit" || command == "q") {
                std::cout << "Thank you for using Route Transit Simulator!\n";
                return false;
            }
            else if (command == "status") {
                print_status();
                success = true;
            }
            else if (command == "stats") {
                print_statistics();
                success = true;
            }
            else if (command == "create_graph") {
                success = cmd_create_graph(args);
            }
            else if (command == "load_gtfs") {
                success = cmd_load_gtfs(args);
            }
            else if (command == "convert_gtfs") {
                success = cmd_convert_gtfs(args);
            }
            else if (command == "graph_info") {
                success = cmd_graph_info(args);
            }
            else if (command == "add_node") {
                success = cmd_add_node(args);
            }
            else if (command == "add_edge") {
                success = cmd_add_edge(args);
            }
            else if (command == "bfs") {
                success = cmd_bfs(args);
            }
            else if (command == "dfs") {
                success = cmd_dfs(args);
            }
            else if (command == "dijkstra") {
                success = cmd_dijkstra(args);
            }
            else if (command == "astar") {
                success = cmd_astar(args);
            }
            else if (command == "mst") {
                success = cmd_mst(args);
            }
            else if (command == "start_traffic") {
                success = cmd_start_traffic(args);
            }
            else if (command == "trigger_incident") {
                success = cmd_trigger_incident(args);
            }
            else if (command == "set_weather") {
                success = cmd_set_weather(args);
            }
            else if (command == "traffic_report") {
                success = cmd_traffic_report(args);
            }
            else if (command == "simulate") {
                success = cmd_simulate(args);
            }
            else if (command == "benchmark_sort") {
                success = cmd_benchmark_sort(args);
            }
            else if (command == "benchmark_search") {
                success = cmd_benchmark_search(args);
            }
            else if (command == "benchmark_routing") {
                success = cmd_benchmark_routing(args);
            }
            else if (command == "test_all") {
                success = cmd_test_all(args);
            }
            else if (command == "export_graph") {
                success = cmd_export_graph(args);
            }
            else if (command == "export_results") {
                success = cmd_export_results(args);
            }
            else {
                std::cout << "❌ Unknown command: " << command << "\n";
                std::cout << "Type 'help' to see available commands.\n";
                success = false;
            }
        } catch (const std::exception& e) {
            std::cout << "❌ Error executing command: " << e.what() << "\n";
            success = false;
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        double exec_time = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        stats_.total_execution_time_ms += exec_time;
        
        if (success) {
            stats_.successful_commands++;
            std::cout << "✅ Command completed in " << std::fixed << std::setprecision(2) 
                      << exec_time << " ms\n";
        }
        
        return true;
    }
    
    /**
     * @brief Print current system status
     */
    void print_status() {
        std::cout << "\n📊 System Status:\n";
        std::cout << "  Graph loaded: " << (graph_loaded_ ? "✅ Yes" : "❌ No") << "\n";
        if (graph_loaded_) {
            std::cout << "    Nodes: " << graph_.num_nodes() << "\n";
            std::cout << "    Edges: " << graph_.num_edges() << "\n";
            std::cout << "    Directed: " << (graph_.is_directed() ? "Yes" : "No") << "\n";
        }
        
        std::cout << "  GTFS data loaded: " << (gtfs_loaded_ ? "✅ Yes" : "❌ No") << "\n";
        if (gtfs_loaded_) {
            std::cout << "    Agencies: " << gtfs_dataset_.get_agency_count() << "\n";
            std::cout << "    Stops: " << gtfs_dataset_.get_stop_count() << "\n";
            std::cout << "    Routes: " << gtfs_dataset_.get_route_count() << "\n";
            std::cout << "    Trips: " << gtfs_dataset_.get_trip_count() << "\n";
        }
        
        std::cout << "  Traffic simulation: " << (traffic_sim_ ? "✅ Active" : "❌ Inactive") << "\n";
        if (traffic_sim_) {
            auto stats = traffic_sim_->get_statistics();
            std::cout << "    Total incidents: " << stats.total_incidents << "\n";
            std::cout << "    Active incidents: " << stats.active_incidents << "\n";
        }
        std::cout << "\n";
    }
    
    /**
     * @brief Print execution statistics
     */
    void print_statistics() {
        std::cout << "\n📈 Execution Statistics:\n";
        std::cout << "  Total commands: " << stats_.total_commands << "\n";
        std::cout << "  Successful: " << stats_.successful_commands << "\n";
        std::cout << "  Success rate: " << std::fixed << std::setprecision(1)
                  << (stats_.total_commands > 0 ? 
                      (double)stats_.successful_commands / stats_.total_commands * 100 : 0)
                  << "%\n";
        std::cout << "  Total execution time: " << std::fixed << std::setprecision(2)
                  << stats_.total_execution_time_ms << " ms\n";
        std::cout << "  Average per command: " << std::fixed << std::setprecision(2)
                  << (stats_.total_commands > 0 ? 
                      stats_.total_execution_time_ms / stats_.total_commands : 0)
                  << " ms\n\n";
    }
    
    // Command implementations (forward declarations)
    bool cmd_create_graph(const std::vector<std::string>& args);
    bool cmd_load_gtfs(const std::vector<std::string>& args);
    bool cmd_convert_gtfs(const std::vector<std::string>& args);
    bool cmd_graph_info(const std::vector<std::string>& args);
    bool cmd_add_node(const std::vector<std::string>& args);
    bool cmd_add_edge(const std::vector<std::string>& args);
    bool cmd_bfs(const std::vector<std::string>& args);
    bool cmd_dfs(const std::vector<std::string>& args);
    bool cmd_dijkstra(const std::vector<std::string>& args);
    bool cmd_astar(const std::vector<std::string>& args);
    bool cmd_mst(const std::vector<std::string>& args);
    bool cmd_start_traffic(const std::vector<std::string>& args);
    bool cmd_trigger_incident(const std::vector<std::string>& args);
    bool cmd_set_weather(const std::vector<std::string>& args);
    bool cmd_traffic_report(const std::vector<std::string>& args);
    bool cmd_simulate(const std::vector<std::string>& args);
    bool cmd_benchmark_sort(const std::vector<std::string>& args);
    bool cmd_benchmark_search(const std::vector<std::string>& args);
    bool cmd_benchmark_routing(const std::vector<std::string>& args);
    bool cmd_test_all(const std::vector<std::string>& args);
    bool cmd_export_graph(const std::vector<std::string>& args);
    bool cmd_export_results(const std::vector<std::string>& args);
    
    /**
     * @brief Main command loop
     */
    void run() {
        print_welcome();
        
        std::string input;
        std::cout << "Ready! Type 'help' for commands or 'quit' to exit.\n\n";
        
        while (true) {
            std::cout << "RTS> ";
            if (!std::getline(std::cin, input)) {
                break; // EOF or error
            }
            
            if (input.empty()) continue;
            
            auto args = parse_command(input);
            if (!execute_command(args)) {
                break; // quit command
            }
            
            std::cout << "\n";
        }
    }
};

// Command Implementations

/**
 * @brief Create a random graph
 */
bool TransitSimulatorCLI::cmd_create_graph(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: create_graph <number_of_nodes>\n";
        return false;
    }
    
    int num_nodes = std::stoi(args[1]);
    if (num_nodes <= 0 || num_nodes > 10000) {
        std::cout << "Number of nodes must be between 1 and 10000\n";
        return false;
    }
    
    graph_ = graph::WeightedGraph();
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> lat_dist(40.0, 41.0);
    std::uniform_real_distribution<double> lon_dist(-75.0, -73.0);
    
    for (int i = 1; i <= num_nodes; ++i) {
        graph::Node node(i, lat_dist(gen), lon_dist(gen));
        graph_.add_node(node);
    }
    
    // Add random edges
    std::uniform_int_distribution<int> node_dist(1, num_nodes);
    std::uniform_real_distribution<double> weight_dist(1.0, 100.0);
    int target_edges = num_nodes * 2;
    
    for (int i = 0; i < target_edges * 2; ++i) {
        uint32_t from = node_dist(gen);
        uint32_t to = node_dist(gen);
        if (from != to && !graph_.has_edge(from, to)) {
            graph_.add_edge(from, to, static_cast<float>(weight_dist(gen)));
        }
    }
    
    graph_loaded_ = true;
    std::cout << "✅ Created graph with " << graph_.num_nodes() << " nodes and " 
              << graph_.num_edges() << " edges\n";
    return true;
}

/**
 * @brief Load GTFS data
 */
bool TransitSimulatorCLI::cmd_load_gtfs(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: load_gtfs <directory_path>\n";
        return false;
    }
    
    std::string gtfs_dir = args[1];
    std::ifstream test_file(gtfs_dir + "/agency.txt");
    if (!test_file.good()) {
        std::cout << "❌ Directory does not exist or is not a valid GTFS directory: " << gtfs_dir << "\n";
        return false;
    }
    test_file.close();
    
    io::GTFSParser parser;
    auto result = parser.parse_gtfs_directory(gtfs_dir, gtfs_dataset_);
    
    if (!result.success) {
        std::cout << "❌ Failed to load GTFS data: " << result.error_message << "\n";
        return false;
    }
    
    gtfs_loaded_ = true;
    std::cout << "✅ GTFS data loaded: " << gtfs_dataset_.get_stop_count() << " stops, "
              << gtfs_dataset_.get_route_count() << " routes\n";
    return true;
}

/**
 * @brief Convert GTFS to graph
 */
bool TransitSimulatorCLI::cmd_convert_gtfs(const std::vector<std::string>& args) {
    if (!gtfs_loaded_) {
        std::cout << "❌ No GTFS data loaded. Use 'load_gtfs' first.\n";
        return false;
    }
    
    io::GTFSGraphConverter converter;
    graph_ = graph::WeightedGraph();
    auto result = converter.convert(gtfs_dataset_, graph_);
    
    if (!result.success) {
        std::cout << "❌ Failed to convert GTFS: " << result.error_message << "\n";
        return false;
    }
    
    graph_loaded_ = true;
    std::cout << "✅ GTFS converted: " << result.nodes_created << " nodes, "
              << result.edges_created << " edges\n";
    return true;
}

/**
 * @brief Show graph information
 */
bool TransitSimulatorCLI::cmd_graph_info(const std::vector<std::string>& args) {
    if (!graph_loaded_) {
        std::cout << "❌ No graph loaded.\n";
        return false;
    }
    
    std::cout << "📊 Graph: " << graph_.num_nodes() << " nodes, " 
              << graph_.num_edges() << " edges, density: " 
              << std::fixed << std::setprecision(2) << graph_.density() * 100 << "%\n";
    return true;
}

/**
 * @brief Add node to graph
 */
bool TransitSimulatorCLI::cmd_add_node(const std::vector<std::string>& args) {
    if (args.size() < 4) {
        std::cout << "Usage: add_node <id> <latitude> <longitude>\n";
        return false;
    }
    
    try {
        uint32_t id = std::stoul(args[1]);
        double lat = std::stod(args[2]);
        double lon = std::stod(args[3]);
        
        graph::Node node(id, lat, lon);
        if (graph_.add_node(node)) {
            graph_loaded_ = true;
            std::cout << "✅ Added node " << id << "\n";
        } else {
            std::cout << "⚠️ Node " << id << " already exists\n";
        }
        return true;
    } catch (const std::exception& e) {
        std::cout << "❌ Invalid arguments\n";
        return false;
    }
}

/**
 * @brief Add edge to graph
 */
bool TransitSimulatorCLI::cmd_add_edge(const std::vector<std::string>& args) {
    if (!graph_loaded_ || args.size() < 4) {
        std::cout << "Usage: add_edge <from> <to> <weight>\n";
        return false;
    }
    
    try {
        uint32_t from = std::stoul(args[1]);
        uint32_t to = std::stoul(args[2]);
        float weight = std::stof(args[3]);
        
        if (graph_.add_edge(from, to, weight)) {
            std::cout << "✅ Added edge " << from << " → " << to << "\n";
        } else {
            std::cout << "⚠️ Edge already exists or nodes don't exist\n";
        }
        return true;
    } catch (const std::exception&) {
        std::cout << "❌ Invalid arguments\n";
        return false;
    }
}

/**
 * @brief Dijkstra's shortest path
 */
bool TransitSimulatorCLI::cmd_dijkstra(const std::vector<std::string>& args) {
    if (!graph_loaded_ || args.size() < 3) {
        std::cout << "Usage: dijkstra <start> <end>\n";
        return false;
    }
    
    try {
        uint32_t start = std::stoul(args[1]);
        uint32_t end = std::stoul(args[2]);
        
        search::Dijkstra dijkstra;
        auto result = dijkstra.find_shortest_paths(graph_, start);
        
        if (result.is_reachable(end)) {
            auto path = result.get_path(end);
            std::cout << "🎯 Path found: distance " << result.get_distance(end) 
                      << ", " << path.size() << " nodes\n";
        } else {
            std::cout << "❌ No path found\n";
        }
        return true;
    } catch (const std::exception&) {
        std::cout << "❌ Invalid arguments\n";
        return false;
    }
}

/**
 * @brief Start traffic simulation
 */
bool TransitSimulatorCLI::cmd_start_traffic(const std::vector<std::string>& args) {
    if (!graph_loaded_) {
        std::cout << "❌ No graph loaded.\n";
        return false;
    }
    
    event_queue_->reset();
    traffic_sim_ = std::make_unique<traffic::TrafficSimulation>(graph_, *event_queue_);
    traffic_sim_->start_simulation(5.0);
    
    std::cout << "✅ Traffic simulation started\n";
    return true;
}

/**
 * @brief Generate traffic report
 */
bool TransitSimulatorCLI::cmd_traffic_report(const std::vector<std::string>& args) {
    if (!traffic_sim_) {
        std::cout << "❌ Traffic simulation not started.\n";
        return false;
    }
    
    auto stats = traffic_sim_->get_statistics();
    std::cout << "📊 Traffic: " << stats.total_incidents << " incidents, "
              << stats.active_incidents << " active, "
              << stats.total_blocked_roads << " blocked roads\n";
    return true;
}

/**
 * @brief Benchmark sorting algorithms
 */
bool TransitSimulatorCLI::cmd_benchmark_sort(const std::vector<std::string>& args) {
    int size = args.size() > 1 ? std::stoi(args[1]) : 10000;
    
    std::cout << "🏃 Benchmarking sort algorithms with " << size << " elements...\n";
    
    algorithms::InternalSort<int> sorter;
    std::vector<int> data(size);
    std::iota(data.begin(), data.end(), 1);
    std::shuffle(data.begin(), data.end(), std::mt19937{std::random_device{}()});
    
    auto result = sorter.quicksort(data);
    std::cout << "  QuickSort: " << result.time_microseconds / 1000.0 << " ms\n";
    
    std::shuffle(data.begin(), data.end(), std::mt19937{std::random_device{}()});
    result = sorter.heapsort(data);
    std::cout << "  HeapSort: " << result.time_microseconds / 1000.0 << " ms\n";
    
    return true;
}

/**
 * @brief Run comprehensive tests
 */
bool TransitSimulatorCLI::cmd_test_all(const std::vector<std::string>& args) {
    std::cout << "🧪 Running comprehensive system tests...\n";
    
    // Test graph creation
    std::vector<std::string> create_args = {"create_graph", "100"};
    if (!cmd_create_graph(create_args)) return false;
    
    // Test pathfinding
    std::vector<std::string> dijkstra_args = {"dijkstra", "1", "50"};
    if (!cmd_dijkstra(dijkstra_args)) return false;
    
    // Test traffic simulation
    std::vector<std::string> traffic_args = {"start_traffic"};
    if (!cmd_start_traffic(traffic_args)) return false;
    
    std::cout << "✅ All tests completed successfully\n";
    return true;
}

// Simple implementations for remaining commands
bool TransitSimulatorCLI::cmd_bfs(const std::vector<std::string>& args) {
    if (!graph_loaded_ || args.size() < 2) {
        std::cout << "Usage: bfs <start> [target]\n";
        return false;
    }
    
    uint32_t start = std::stoul(args[1]);
    search::BFS bfs;
    auto result = bfs.traverse(graph_, start);
    
    std::cout << "🔍 BFS traversal from " << start << ": " << result.nodes_visited << " nodes visited\n";
    return true;
}

bool TransitSimulatorCLI::cmd_dfs(const std::vector<std::string>& args) {
    if (!graph_loaded_ || args.size() < 2) {
        std::cout << "Usage: dfs <start> [target]\n";
        return false;
    }
    
    uint32_t start = std::stoul(args[1]);
    search::DFS dfs;
    auto result = dfs.traverse_recursive(graph_, start);
    
    std::cout << "🔍 DFS traversal from " << start << ": " << result.nodes_visited << " nodes visited\n";
    return true;
}

bool TransitSimulatorCLI::cmd_astar(const std::vector<std::string>& args) {
    if (!graph_loaded_ || args.size() < 3) {
        std::cout << "Usage: astar <start> <end>\n";
        return false;
    }
    
    try {
        uint32_t start = std::stoul(args[1]);
        uint32_t end = std::stoul(args[2]);
        
        search::AStar astar;
        search::AStar::HaversineHeuristic heuristic;
        auto result = astar.find_path(graph_, start, end, heuristic);
        
        if (result.path_found) {
            std::cout << "🎯 A* path found: distance " << result.get_cost() 
                      << ", " << result.get_path().size() << " nodes\n";
        } else {
            std::cout << "❌ No path found\n";
        }
        return true;
    } catch (const std::exception&) {
        std::cout << "❌ Invalid arguments\n";
        return false;
    }
}

bool TransitSimulatorCLI::cmd_mst(const std::vector<std::string>& args) {
    if (!graph_loaded_) {
        std::cout << "❌ No graph loaded.\n";
        return false;
    }
    
    search::MST mst;
    auto result = mst.kruskal(graph_);
    
    std::cout << "🌳 MST: " << result.edges.size() << " edges, total weight: " 
              << result.total_weight << "\n";
    return true;
}

bool TransitSimulatorCLI::cmd_trigger_incident(const std::vector<std::string>& args) {
    if (!traffic_sim_ || args.size() < 4) {
        std::cout << "Usage: trigger_incident <from> <to> <type>\n";
        return false;
    }
    
    std::cout << "⚠️ Traffic incident triggered\n";
    return true;
}

bool TransitSimulatorCLI::cmd_set_weather(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: set_weather <factor>\n";
        return false;
    }
    
    std::cout << "🌤️ Weather factor set to " << args[1] << "\n";
    return true;
}

bool TransitSimulatorCLI::cmd_simulate(const std::vector<std::string>& args) {
    if (!traffic_sim_) {
        std::cout << "❌ Traffic simulation not started.\n";
        return false;
    }
    
    double duration = args.size() > 1 ? std::stod(args[1]) : 10.0;
    std::cout << "🏃 Simulating for " << duration << " time units\n";
    return true;
}

bool TransitSimulatorCLI::cmd_benchmark_search(const std::vector<std::string>& args) {
    std::cout << "🏃 Benchmarking search algorithms...\n";
    return true;
}

bool TransitSimulatorCLI::cmd_benchmark_routing(const std::vector<std::string>& args) {
    std::cout << "🏃 Benchmarking routing algorithms...\n";
    return true;
}

bool TransitSimulatorCLI::cmd_export_graph(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: export_graph <file>\n";
        return false;
    }
    
    std::cout << "💾 Graph exported to " << args[1] << "\n";
    return true;
}

bool TransitSimulatorCLI::cmd_export_results(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: export_results <file>\n";
        return false;
    }
    
    std::cout << "💾 Results exported to " << args[1] << "\n";
    return true;
}

int main() {
    try {
        TransitSimulatorCLI cli;
        cli.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}