#pragma once

#include "VisualizationFramework.hpp"
#include "../core/simulation/EventQueue.hpp"
#include "../core/traffic/TrafficSimulation.hpp"
#include "../core/graph/WeightedGraph.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <map>

namespace rts::viz {

/**
 * @brief Event queue visualization for discrete event simulation
 */
class EventQueueVisualizer : public Visualizer {
private:
    const rts::core::simulation::EventQueue* event_queue_;
    
public:
    EventQueueVisualizer(const rts::core::simulation::EventQueue* queue, 
                        const std::string& title = "Event Queue")
        : Visualizer(title, 100, 30), event_queue_(queue) {}
    
    /**
     * @brief Visualize current event queue state
     */
    void visualize_current_state() {
        clear_screen();
        print_title();
        
        if (!event_queue_) {
            print_centered("No event queue available", Colors::RED);
            return;
        }
        
        // Queue statistics
        std::cout << "\n";
        print_centered("Event Queue Status", Colors::CYAN);
        print_separator('-');
        
        std::cout << "Current Time: " << colorize(std::to_string(event_queue_->get_current_time()), Colors::YELLOW) 
                  << " | Queue Size: " << colorize(std::to_string(event_queue_->size()), Colors::GREEN) << "\n";
        
        if (event_queue_->empty()) {
            print_centered("Queue is empty", Colors::DIM);
            return;
        }
        
        // Show upcoming events
        std::cout << "\nUpcoming Events:\n";
        print_separator('-');
        
        // Note: In a real implementation, you'd need access to internal heap structure
        // This is a simplified visualization showing next event info
        auto next_event = event_queue_->peek_next();
        if (next_event) {
            std::cout << "Next Event:\n";
            std::cout << "  Time: " << colorize(std::to_string(next_event->scheduled_time), Colors::YELLOW) << "\n";
            std::cout << "  Type: " << colorize(next_event->event_type, Colors::GREEN) << "\n";
            std::cout << "  ID: " << colorize(std::to_string(next_event->event_id), Colors::CYAN) << "\n";
            std::cout << "  Priority: " << colorize(std::to_string(next_event->priority), Colors::MAGENTA) << "\n";
        }
        
        // Statistics
        auto info = event_queue_->get_queue_info();
        std::cout << "\nQueue Statistics:\n";
        print_separator('-');
        std::cout << "Events Processed: " << colorize(std::to_string(info.events_processed), Colors::GREEN) << "\n";
        std::cout << "Events Cancelled: " << colorize(std::to_string(info.events_cancelled), Colors::RED) << "\n";
        std::cout << "Peak Queue Size: " << colorize(std::to_string(info.peak_queue_size), Colors::YELLOW) << "\n";
        std::cout << "Total Events Scheduled: " << colorize(std::to_string(info.total_events_scheduled), Colors::CYAN) << "\n";
    }
    
    /**
     * @brief Visualize event timeline
     */
    void visualize_timeline(double time_window = 100.0) {
        clear_screen();
        print_title();
        
        if (!event_queue_) {
            print_centered("No event queue available", Colors::RED);
            return;
        }
        
        std::cout << "\nEvent Timeline (Next " << time_window << " time units)\n";
        print_separator('=');
        
        double current_time = event_queue_->get_current_time();
        double end_time = current_time + time_window;
        
        // Create timeline visualization
        const size_t timeline_width = 60;
        std::vector<std::string> timeline(timeline_width, " ");
        std::map<size_t, std::vector<std::string>> events_at_position;
        
        // Map events to timeline positions
        auto next_event = event_queue_->peek_next();
        if (next_event && next_event->scheduled_time <= end_time) {
            size_t pos = static_cast<size_t>((next_event->scheduled_time - current_time) / time_window * timeline_width);
            if (pos < timeline_width) {
                timeline[pos] = "E";
                events_at_position[pos].push_back(next_event->event_type);
            }
        }
        
        // Print timeline
        std::cout << "Time: " << std::fixed << std::setprecision(1) << current_time 
                  << " → " << end_time << "\n";
        std::cout << "│";
        for (size_t i = 0; i < timeline_width; ++i) {
            if (timeline[i] == "E") {
                std::cout << colorize("●", Colors::RED);
            } else if (i % 10 == 0) {
                std::cout << colorize("│", Colors::DIM);
            } else {
                std::cout << colorize("─", Colors::DIM);
            }
        }
        std::cout << "│\n";
        
        // Show event details
        for (const auto& [pos, event_types] : events_at_position) {
            double event_time = current_time + (static_cast<double>(pos) / timeline_width * time_window);
            std::cout << "Time " << std::fixed << std::setprecision(1) << event_time << ": ";
            for (const auto& type : event_types) {
                std::cout << colorize(type, Colors::YELLOW) << " ";
            }
            std::cout << "\n";
        }
    }
};

/**
 * @brief Traffic simulation visualization
 */
class TrafficSimulationVisualizer : public Visualizer {
private:
    const rts::core::traffic::TrafficSimulation* traffic_sim_;
    const rts::core::graph::WeightedGraph* graph_;
    
    std::string get_traffic_level_symbol(rts::core::traffic::TrafficLevel level) const {
        switch (level) {
            case rts::core::traffic::TrafficLevel::FREE_FLOW: return "🟢";
            case rts::core::traffic::TrafficLevel::LIGHT: return "🟡";
            case rts::core::traffic::TrafficLevel::MODERATE: return "🟠";
            case rts::core::traffic::TrafficLevel::HEAVY: return "🔴";
            case rts::core::traffic::TrafficLevel::GRIDLOCK: return "⛔";
            default: return "❓";
        }
    }
    
    std::string get_traffic_level_color(rts::core::traffic::TrafficLevel level) const {
        switch (level) {
            case rts::core::traffic::TrafficLevel::FREE_FLOW: return Colors::GREEN;
            case rts::core::traffic::TrafficLevel::LIGHT: return Colors::YELLOW;
            case rts::core::traffic::TrafficLevel::MODERATE: return Colors::YELLOW + Colors::BOLD;
            case rts::core::traffic::TrafficLevel::HEAVY: return Colors::RED;
            case rts::core::traffic::TrafficLevel::GRIDLOCK: return Colors::RED + Colors::BOLD;
            default: return Colors::WHITE;
        }
    }
    
    std::string get_incident_symbol(rts::core::traffic::IncidentType incident) const {
        switch (incident) {
            case rts::core::traffic::IncidentType::NONE: return " ";
            case rts::core::traffic::IncidentType::MINOR_ACCIDENT: return "⚠️";
            case rts::core::traffic::IncidentType::MAJOR_ACCIDENT: return "💥";
            case rts::core::traffic::IncidentType::VEHICLE_BREAKDOWN: return "🔧";
            case rts::core::traffic::IncidentType::ROAD_CONSTRUCTION: return "🚧";
            case rts::core::traffic::IncidentType::EMERGENCY_CLOSURE: return "🚫";
            case rts::core::traffic::IncidentType::WEATHER_CLOSURE: return "🌧️";
            case rts::core::traffic::IncidentType::SPECIAL_EVENT: return "🎪";
            default: return "❓";
        }
    }
    
public:
    TrafficSimulationVisualizer(const rts::core::traffic::TrafficSimulation* traffic_sim,
                               const rts::core::graph::WeightedGraph* graph,
                               const std::string& title = "Traffic Simulation")
        : Visualizer(title, 120, 40), traffic_sim_(traffic_sim), graph_(graph) {}
    
    /**
     * @brief Visualize current traffic state
     */
    void visualize_traffic_state() {
        clear_screen();
        print_title();
        
        if (!traffic_sim_ || !graph_) {
            print_centered("No traffic simulation or graph available", Colors::RED);
            return;
        }
        
        std::cout << "\n";
        print_centered("Current Traffic Conditions", Colors::CYAN);
        print_separator('=');
        
        // Get all edges and their traffic states
        auto edges = graph_->get_all_edges();
        if (edges.empty()) {
            print_centered("No edges in graph", Colors::DIM);
            return;
        }
        
        std::cout << std::left << std::setw(15) << "Edge" 
                  << std::setw(12) << "Traffic" 
                  << std::setw(20) << "Incident" 
                  << std::setw(12) << "Speed %" 
                  << std::setw(15) << "Capacity %" 
                  << "Status\n";
        print_separator('-');
        
        for (const auto& edge : edges) {
            auto state = traffic_sim_->get_traffic_state(edge.from, edge.to);
            if (!state) continue;
            
            std::string edge_str = std::to_string(edge.from) + "→" + std::to_string(edge.to);
            std::string traffic_level = std::to_string(static_cast<int>(state->level));
            std::string incident_str = std::to_string(static_cast<int>(state->incident));
            
            std::cout << std::left << std::setw(15) << edge_str;
            std::cout << std::setw(12) << colorize(traffic_level, get_traffic_level_color(state->level));
            std::cout << std::setw(20) << colorize(incident_str, Colors::MAGENTA);
            std::cout << std::setw(12) << colorize(std::to_string(static_cast<int>(state->speed_factor * 100)), Colors::YELLOW);
            std::cout << std::setw(15) << colorize(std::to_string(static_cast<int>(state->capacity_factor * 100)), Colors::CYAN);
            
            if (state->is_blocked()) {
                std::cout << colorize("BLOCKED", Colors::RED + Colors::BOLD);
            } else if (state->incident != rts::core::traffic::IncidentType::NONE) {
                std::cout << colorize("INCIDENT", Colors::YELLOW + Colors::BOLD);
            } else {
                std::cout << colorize("NORMAL", Colors::GREEN);
            }
            std::cout << "\n";
        }
        
        // Show blocked roads
        auto blocked_roads = traffic_sim_->get_blocked_roads();
        if (!blocked_roads.empty()) {
            std::cout << "\n";
            print_centered("Blocked Roads", Colors::RED + Colors::BOLD);
            print_separator('-');
            for (const auto& [from, to] : blocked_roads) {
                std::cout << "  " << colorize(std::to_string(from) + " → " + std::to_string(to), Colors::RED) << "\n";
            }
        }
    }
    
    /**
     * @brief Visualize traffic statistics
     */
    void visualize_statistics() {
        clear_screen();
        print_title();
        
        if (!traffic_sim_) {
            print_centered("No traffic simulation available", Colors::RED);
            return;
        }
        
        auto stats = traffic_sim_->get_statistics();
        
        std::cout << "\n";
        print_centered("Traffic Simulation Statistics", Colors::CYAN);
        print_separator('=');
        
        // Overall statistics
        std::cout << "\nOverall Statistics:\n";
        print_separator('-');
        std::cout << "Total Incidents: " << colorize(std::to_string(stats.total_incidents), Colors::YELLOW) << "\n";
        std::cout << "Active Incidents: " << colorize(std::to_string(stats.active_incidents), Colors::RED) << "\n";
        std::cout << "Average Severity: " << colorize(std::to_string(stats.average_severity), Colors::MAGENTA) << "\n";
        
        // Traffic level distribution
        std::cout << "\nTraffic Level Distribution:\n";
        print_separator('-');
        size_t total_observations = 0;
        for (const auto& [level, count] : stats.traffic_level_counts) {
            total_observations += count;
        }
        
        if (total_observations > 0) {
            for (int level = 0; level <= 4; ++level) {
                auto it = stats.traffic_level_counts.find(static_cast<rts::core::traffic::TrafficLevel>(level));
                size_t count = (it != stats.traffic_level_counts.end()) ? it->second : 0;
                double percentage = static_cast<double>(count) / total_observations * 100;
                
                std::string level_name = get_traffic_level_name(static_cast<rts::core::traffic::TrafficLevel>(level));
                std::cout << std::left << std::setw(15) << level_name << ": ";
                std::cout << create_progress_bar(percentage / 100.0, 20) << " ";
                std::cout << colorize(std::to_string(static_cast<int>(percentage)) + "%", Colors::CYAN) << "\n";
            }
        }
        
        // Incident type distribution
        std::cout << "\nIncident Type Distribution:\n";
        print_separator('-');
        for (const auto& [incident, count] : stats.incident_counts) {
            if (count > 0) {
                std::string incident_name = get_incident_name(incident);
                std::cout << std::left << std::setw(20) << incident_name << ": ";
                std::cout << colorize(std::to_string(count), Colors::YELLOW) << "\n";
            }
        }
    }
    
    /**
     * @brief Create a simple network visualization
     */
    void visualize_network() {
        clear_screen();
        print_title();
        
        if (!graph_) {
            print_centered("No graph available", Colors::RED);
            return;
        }
        
        std::cout << "\n";
        print_centered("Network Traffic Visualization", Colors::CYAN);
        print_separator('=');
        
        auto nodes = graph_->get_all_nodes();
        auto edges = graph_->get_all_edges();
        
        if (nodes.empty()) {
            print_centered("No nodes in graph", Colors::DIM);
            return;
        }
        
        // Simple grid layout for small networks
        size_t grid_size = static_cast<size_t>(std::sqrt(nodes.size())) + 1;
        std::vector<std::vector<std::string>> grid = create_grid(grid_size * 3, grid_size * 5, " ");
        
        // Place nodes
        size_t node_idx = 0;
        std::map<uint32_t, std::pair<size_t, size_t>> node_positions;
        
        for (const auto& node : nodes) {
            if (node_idx >= grid_size * grid_size) break;
            
            size_t row = (node_idx / grid_size) * 3 + 1;
            size_t col = (node_idx % grid_size) * 5 + 2;
            
            if (row < grid.size() && col < grid[0].size()) {
                grid[row][col] = std::to_string(node.id);
                node_positions[node.id] = {row, col};
            }
            node_idx++;
        }
        
        // Add traffic state indicators around nodes
        if (traffic_sim_) {
            for (const auto& edge : edges) {
                auto state = traffic_sim_->get_traffic_state(edge.from, edge.to);
                if (!state) continue;
                
                auto from_it = node_positions.find(edge.from);
                auto to_it = node_positions.find(edge.to);
                
                if (from_it != node_positions.end() && to_it != node_positions.end()) {
                    // Add traffic level indicator between nodes (simplified)
                    size_t from_row = from_it->second.first;
                    size_t from_col = from_it->second.second;
                    
                    if (from_row > 0 && from_col > 0 && 
                        from_row < grid.size() - 1 && from_col < grid[0].size() - 1) {
                        
                        // Place traffic indicator adjacent to source node
                        char indicator = '─';
                        switch (state->level) {
                            case rts::core::traffic::TrafficLevel::FREE_FLOW: indicator = '─'; break;
                            case rts::core::traffic::TrafficLevel::LIGHT: indicator = '~'; break;
                            case rts::core::traffic::TrafficLevel::MODERATE: indicator = '≈'; break;
                            case rts::core::traffic::TrafficLevel::HEAVY: indicator = '≡'; break;
                            case rts::core::traffic::TrafficLevel::GRIDLOCK: indicator = '█'; break;
                        }
                        
                        grid[from_row][from_col + 1] = std::string(1, indicator);
                    }
                }
            }
        }
        
        // Print the grid
        for (const auto& row : grid) {
            for (const auto& cell : row) {
                if (cell != " " && std::isdigit(cell[0])) {
                    std::cout << colorize(cell, Colors::CYAN + Colors::BOLD);
                } else if (cell != " ") {
                    std::cout << colorize(cell, Colors::YELLOW);
                } else {
                    std::cout << cell;
                }
            }
            std::cout << "\n";
        }
        
        // Legend
        std::cout << "\nLegend:\n";
        print_separator('-');
        std::cout << "Traffic Levels: " 
                  << colorize("─", Colors::GREEN) << " Free Flow  "
                  << colorize("~", Colors::YELLOW) << " Light  "
                  << colorize("≈", Colors::YELLOW + Colors::BOLD) << " Moderate  "
                  << colorize("≡", Colors::RED) << " Heavy  "
                  << colorize("█", Colors::RED + Colors::BOLD) << " Gridlock\n";
    }
    
private:
    std::string get_traffic_level_name(rts::core::traffic::TrafficLevel level) const {
        switch (level) {
            case rts::core::traffic::TrafficLevel::FREE_FLOW: return "Free Flow";
            case rts::core::traffic::TrafficLevel::LIGHT: return "Light";
            case rts::core::traffic::TrafficLevel::MODERATE: return "Moderate";
            case rts::core::traffic::TrafficLevel::HEAVY: return "Heavy";
            case rts::core::traffic::TrafficLevel::GRIDLOCK: return "Gridlock";
            default: return "Unknown";
        }
    }
    
    std::string get_incident_name(rts::core::traffic::IncidentType incident) const {
        switch (incident) {
            case rts::core::traffic::IncidentType::NONE: return "None";
            case rts::core::traffic::IncidentType::MINOR_ACCIDENT: return "Minor Accident";
            case rts::core::traffic::IncidentType::MAJOR_ACCIDENT: return "Major Accident";
            case rts::core::traffic::IncidentType::VEHICLE_BREAKDOWN: return "Vehicle Breakdown";
            case rts::core::traffic::IncidentType::ROAD_CONSTRUCTION: return "Road Construction";
            case rts::core::traffic::IncidentType::EMERGENCY_CLOSURE: return "Emergency Closure";
            case rts::core::traffic::IncidentType::WEATHER_CLOSURE: return "Weather Closure";
            case rts::core::traffic::IncidentType::SPECIAL_EVENT: return "Special Event";
            default: return "Unknown";
        }
    }
};

/**
 * @brief Comprehensive simulation dashboard
 */
class SimulationDashboard : public Visualizer {
private:
    std::unique_ptr<EventQueueVisualizer> event_viz_;
    std::unique_ptr<TrafficSimulationVisualizer> traffic_viz_;
    
public:
    SimulationDashboard(const rts::core::simulation::EventQueue* event_queue,
                       const rts::core::traffic::TrafficSimulation* traffic_sim,
                       const rts::core::graph::WeightedGraph* graph)
        : Visualizer("Simulation Dashboard", 150, 50) {
        
        if (event_queue) {
            event_viz_ = std::make_unique<EventQueueVisualizer>(event_queue);
        }
        if (traffic_sim && graph) {
            traffic_viz_ = std::make_unique<TrafficSimulationVisualizer>(traffic_sim, graph);
        }
    }
    
    /**
     * @brief Show complete simulation dashboard
     */
    void show_dashboard() {
        clear_screen();
        print_title();
        
        std::cout << "\n";
        print_centered("=== SIMULATION DASHBOARD ===", Colors::CYAN + Colors::BOLD);
        std::cout << "\n";
        
        // Split screen view
        std::cout << "┌─ Event Queue ──────────────────────┬─ Traffic Simulation ─────────────────┐\n";
        
        // This is a simplified split view - in a real implementation,
        // you'd need more sophisticated layout management
        if (event_viz_) {
            std::cout << "│ Queue Size: " << std::left << std::setw(25) << "N/A" << "│";
        } else {
            std::cout << "│" << std::setw(36) << " No event queue" << "│";
        }
        
        if (traffic_viz_) {
            std::cout << " Active Incidents: " << std::left << std::setw(15) << "N/A" << "│\n";
        } else {
            std::cout << std::setw(38) << " No traffic simulation" << "│\n";
        }
        
        std::cout << "└────────────────────────────────────┴──────────────────────────────────────┘\n";
        
        std::cout << "\nAvailable Views:\n";
        print_separator('-');
        std::cout << "1. Event Queue Status\n";
        std::cout << "2. Event Timeline\n";
        std::cout << "3. Traffic State Overview\n";
        std::cout << "4. Traffic Statistics\n";
        std::cout << "5. Network Visualization\n";
        std::cout << "\nUse visualization methods individually for detailed views.\n";
    }
    
    /**
     * @brief Get event queue visualizer
     */
    EventQueueVisualizer* get_event_visualizer() const {
        return event_viz_.get();
    }
    
    /**
     * @brief Get traffic simulation visualizer
     */
    TrafficSimulationVisualizer* get_traffic_visualizer() const {
        return traffic_viz_.get();
    }
};

} // namespace rts::viz