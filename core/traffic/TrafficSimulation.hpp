#pragma once

#include "../simulation/EventQueue.hpp"
#include "../graph/WeightedGraph.hpp"
#include <random>
#include <unordered_set>
#include <memory>

namespace rts::core::traffic {

/**
 * @brief Traffic condition levels affecting road capacity and speed
 */
enum class TrafficLevel {
    FREE_FLOW = 0,      // No congestion, optimal speed
    LIGHT = 1,          // Slight slowdown
    MODERATE = 2,       // Noticeable delays
    HEAVY = 3,          // Significant congestion
    GRIDLOCK = 4        // Complete standstill
};

/**
 * @brief Types of traffic incidents
 */
enum class IncidentType {
    NONE = 0,
    MINOR_ACCIDENT,     // Fender bender, one lane blocked
    MAJOR_ACCIDENT,     // Multi-vehicle, multiple lanes blocked
    VEHICLE_BREAKDOWN,  // Disabled vehicle
    ROAD_CONSTRUCTION,  // Planned construction work
    EMERGENCY_CLOSURE,  // Police/fire/medical response
    WEATHER_CLOSURE,    // Snow, ice, flooding
    SPECIAL_EVENT      // Parade, marathon, etc.
};

/**
 * @brief Traffic state for a road segment
 */
struct TrafficState {
    TrafficLevel level;
    IncidentType incident;
    double speed_factor;        // Multiplier for normal speed (0.0 = blocked, 1.0 = normal)
    double capacity_factor;     // Multiplier for normal capacity
    simulation::SimulationTime duration_remaining;  // How long incident will last
    std::string description;
    
    TrafficState() : level(TrafficLevel::FREE_FLOW), incident(IncidentType::NONE),
                    speed_factor(1.0), capacity_factor(1.0), duration_remaining(0.0) {}
    
    /**
     * @brief Check if road is completely blocked
     */
    bool is_blocked() const {
        return speed_factor <= 0.001 || incident == IncidentType::EMERGENCY_CLOSURE;
    }
    
    /**
     * @brief Get effective travel time multiplier
     */
    double get_travel_time_multiplier() const {
        if (is_blocked()) return std::numeric_limits<double>::infinity();
        return 1.0 / speed_factor;
    }
    
    /**
     * @brief Get severity score (0 = normal, 1 = worst possible)
     */
    double get_severity() const {
        if (is_blocked()) return 1.0;
        double traffic_severity = static_cast<double>(level) / 4.0;
        double incident_severity = incident == IncidentType::NONE ? 0.0 : 
                                  (static_cast<double>(incident) / 7.0);
        return std::max(traffic_severity, incident_severity);
    }
};

/**
 * @brief Traffic simulation engine
 */
class TrafficSimulation {
private:
    graph::WeightedGraph& graph_;
    simulation::EventQueue& event_queue_;
    
    // Traffic state for each edge
    std::unordered_map<uint64_t, TrafficState> edge_traffic_states_;
    
    // Original edge weights (before traffic modifications)
    std::unordered_map<uint64_t, float> original_edge_weights_;
    
    // Simulation parameters
    struct Parameters {
        double base_congestion_rate = 0.1;      // Probability per hour of congestion increase
        double congestion_decay_rate = 0.3;     // Rate at which congestion naturally decreases
        double incident_base_rate = 0.05;       // Base incident probability per hour
        double incident_duration_mean = 30.0;   // Average incident duration (minutes)
        double incident_duration_std = 15.0;    // Standard deviation of incident duration
        double weather_factor = 1.0;            // Weather multiplier for incident rates
        double rush_hour_factor = 2.0;          // Rush hour multiplier for congestion
        
        // Rush hour periods (in hours since midnight)
        double morning_rush_start = 7.0;
        double morning_rush_end = 9.0;
        double evening_rush_start = 17.0;
        double evening_rush_end = 19.0;
    } params_;
    
    // Random number generation
    mutable std::mt19937 rng_;
    mutable std::uniform_real_distribution<double> uniform_dist_;
    mutable std::normal_distribution<double> normal_dist_;
    
    // Statistics
    struct Statistics {
        size_t total_incidents = 0;
        size_t active_incidents = 0;
        double average_congestion_level = 0.0;
        double total_blocked_roads = 0;
        std::unordered_map<IncidentType, size_t> incident_counts;
        std::unordered_map<TrafficLevel, size_t> traffic_level_counts;
    } stats_;
    
    /**
     * @brief Generate edge key for hash map
     */
    uint64_t get_edge_key(uint32_t from, uint32_t to) const {
        return (static_cast<uint64_t>(from) << 32) | to;
    }
    
    /**
     * @brief Check if current time is rush hour
     */
    bool is_rush_hour(simulation::SimulationTime time) const {
        double hour = fmod(time / 60.0, 24.0); // Convert to hour of day
        return (hour >= params_.morning_rush_start && hour <= params_.morning_rush_end) ||
               (hour >= params_.evening_rush_start && hour <= params_.evening_rush_end);
    }
    
    /**
     * @brief Update traffic level based on congestion dynamics
     */
    void update_traffic_level(uint64_t edge_key, TrafficState& state, 
                             simulation::SimulationTime current_time) const {
        (void)edge_key; // Suppress unused parameter warning
        double congestion_change_prob = params_.base_congestion_rate;
        
        // Apply rush hour factor
        if (is_rush_hour(current_time)) {
            congestion_change_prob *= params_.rush_hour_factor;
        }
        
        // Apply weather factor
        congestion_change_prob *= params_.weather_factor;
        
        // Randomly increase or decrease congestion
        if (uniform_dist_(rng_) < congestion_change_prob) {
            // Increase congestion
            if (state.level < TrafficLevel::GRIDLOCK) {
                state.level = static_cast<TrafficLevel>(static_cast<int>(state.level) + 1);
            }
        } else if (uniform_dist_(rng_) < params_.congestion_decay_rate) {
            // Decrease congestion
            if (state.level > TrafficLevel::FREE_FLOW) {
                state.level = static_cast<TrafficLevel>(static_cast<int>(state.level) - 1);
            }
        }
        
        // Update speed factor based on traffic level
        switch (state.level) {
            case TrafficLevel::FREE_FLOW:
                state.speed_factor = 1.0;
                state.capacity_factor = 1.0;
                break;
            case TrafficLevel::LIGHT:
                state.speed_factor = 0.85;
                state.capacity_factor = 0.9;
                break;
            case TrafficLevel::MODERATE:
                state.speed_factor = 0.6;
                state.capacity_factor = 0.7;
                break;
            case TrafficLevel::HEAVY:
                state.speed_factor = 0.3;
                state.capacity_factor = 0.4;
                break;
            case TrafficLevel::GRIDLOCK:
                state.speed_factor = 0.05;
                state.capacity_factor = 0.1;
                break;
        }
    }
    
    /**
     * @brief Generate random incident
     */
    IncidentType generate_random_incident() const {
        double incident_prob = uniform_dist_(rng_);
        
        if (incident_prob < 0.4) return IncidentType::MINOR_ACCIDENT;
        if (incident_prob < 0.6) return IncidentType::VEHICLE_BREAKDOWN;
        if (incident_prob < 0.75) return IncidentType::MAJOR_ACCIDENT;
        if (incident_prob < 0.85) return IncidentType::ROAD_CONSTRUCTION;
        if (incident_prob < 0.95) return IncidentType::EMERGENCY_CLOSURE;
        if (incident_prob < 0.98) return IncidentType::WEATHER_CLOSURE;
        return IncidentType::SPECIAL_EVENT;
    }
    
    /**
     * @brief Get incident impact on traffic
     */
    void apply_incident_impact(TrafficState& state, IncidentType incident) const {
        state.incident = incident;
        
        switch (incident) {
            case IncidentType::NONE:
                // No additional impact
                break;
            case IncidentType::MINOR_ACCIDENT:
                state.speed_factor *= 0.7;
                state.capacity_factor *= 0.8;
                state.description = "Minor accident - one lane blocked";
                break;
            case IncidentType::MAJOR_ACCIDENT:
                state.speed_factor *= 0.3;
                state.capacity_factor *= 0.4;
                state.description = "Major accident - multiple lanes blocked";
                break;
            case IncidentType::VEHICLE_BREAKDOWN:
                state.speed_factor *= 0.8;
                state.capacity_factor *= 0.9;
                state.description = "Vehicle breakdown - shoulder blocked";
                break;
            case IncidentType::ROAD_CONSTRUCTION:
                state.speed_factor *= 0.5;
                state.capacity_factor *= 0.6;
                state.description = "Road construction - lane closures";
                break;
            case IncidentType::EMERGENCY_CLOSURE:
                state.speed_factor = 0.0;
                state.capacity_factor = 0.0;
                state.description = "Emergency closure - road blocked";
                break;
            case IncidentType::WEATHER_CLOSURE:
                state.speed_factor = 0.0;
                state.capacity_factor = 0.0;
                state.description = "Weather closure - unsafe conditions";
                break;
            case IncidentType::SPECIAL_EVENT:
                state.speed_factor *= 0.4;
                state.capacity_factor *= 0.5;
                state.description = "Special event - heavy pedestrian traffic";
                break;
        }
    }
    
    /**
     * @brief Generate incident duration
     */
    simulation::SimulationTime generate_incident_duration(IncidentType incident) const {
        double base_duration = params_.incident_duration_mean;
        
        // Adjust base duration by incident type
        switch (incident) {
            case IncidentType::MINOR_ACCIDENT:
                base_duration = 20.0;
                break;
            case IncidentType::MAJOR_ACCIDENT:
                base_duration = 60.0;
                break;
            case IncidentType::VEHICLE_BREAKDOWN:
                base_duration = 15.0;
                break;
            case IncidentType::ROAD_CONSTRUCTION:
                base_duration = 240.0; // 4 hours
                break;
            case IncidentType::EMERGENCY_CLOSURE:
                base_duration = 45.0;
                break;
            case IncidentType::WEATHER_CLOSURE:
                base_duration = 120.0; // 2 hours
                break;
            case IncidentType::SPECIAL_EVENT:
                base_duration = 180.0; // 3 hours
                break;
            default:
                break;
        }
        
        // Add random variation
        normal_dist_.param(std::normal_distribution<double>::param_type(
            base_duration, params_.incident_duration_std));
        double duration = normal_dist_(rng_);
        
        return std::max(5.0, duration); // Minimum 5 minutes
    }

public:
    /**
     * @brief Traffic update event
     */
    class TrafficUpdateEvent : public simulation::Event {
    private:
        TrafficSimulation* traffic_sim_;
        uint64_t edge_key_;
        
    public:
        TrafficUpdateEvent(simulation::SimulationTime time, TrafficSimulation* sim, 
                          uint64_t edge_key, int priority = 0)
            : Event(time, 0, "TrafficUpdate", priority), traffic_sim_(sim), edge_key_(edge_key) {}
        
        void execute(simulation::SimulationTime current_time) override {
            if (traffic_sim_) {
                traffic_sim_->update_edge_traffic(edge_key_, current_time);
            }
        }
        
        std::unique_ptr<simulation::Event> clone() const override {
            return std::make_unique<TrafficUpdateEvent>(scheduled_time, traffic_sim_, edge_key_, priority);
        }
    };
    
    /**
     * @brief Incident resolution event
     */
    class IncidentResolutionEvent : public simulation::Event {
    private:
        TrafficSimulation* traffic_sim_;
        uint64_t edge_key_;
        
    public:
        IncidentResolutionEvent(simulation::SimulationTime time, TrafficSimulation* sim,
                               uint64_t edge_key, int priority = 0)
            : Event(time, 0, "IncidentResolution", priority), traffic_sim_(sim), edge_key_(edge_key) {}
        
        void execute(simulation::SimulationTime current_time) override {
            if (traffic_sim_) {
                traffic_sim_->resolve_incident(edge_key_, current_time);
            }
        }
        
        std::unique_ptr<simulation::Event> clone() const override {
            return std::make_unique<IncidentResolutionEvent>(scheduled_time, traffic_sim_, edge_key_, priority);
        }
    };
    
    TrafficSimulation(graph::WeightedGraph& graph, simulation::EventQueue& event_queue, 
                     uint32_t seed = 42)
        : graph_(graph), event_queue_(event_queue), rng_(seed), 
          uniform_dist_(0.0, 1.0), normal_dist_(0.0, 1.0) {
        initialize_traffic_states();
    }
    
    /**
     * @brief Initialize traffic states for all edges
     */
    void initialize_traffic_states() {
        auto edges = graph_.get_all_edges();
        for (const auto& edge : edges) {
            uint64_t key = get_edge_key(edge.from, edge.to);
            edge_traffic_states_[key] = TrafficState();
            original_edge_weights_[key] = edge.weight;  // Store original weight
        }
    }
    
    /**
     * @brief Start traffic simulation with periodic updates
     */
    void start_simulation(simulation::SimulationTime update_interval = 5.0) {
        auto edges = graph_.get_all_edges();
        
        for (const auto& edge : edges) {
            uint64_t key = get_edge_key(edge.from, edge.to);
            
            // Schedule periodic traffic updates
            auto update_event = std::make_shared<TrafficUpdateEvent>(
                update_interval, this, key);
            event_queue_.schedule_event(update_event);
        }
    }
    
    /**
     * @brief Update traffic state for a specific edge
     */
    void update_edge_traffic(uint64_t edge_key, simulation::SimulationTime current_time) {
        auto it = edge_traffic_states_.find(edge_key);
        if (it == edge_traffic_states_.end()) return;
        
        TrafficState& state = it->second;
        
        // Update traffic level
        update_traffic_level(edge_key, state, current_time);
        
        // Check for new incidents
        double incident_prob = params_.incident_base_rate * params_.weather_factor;
        if (uniform_dist_(rng_) < incident_prob && state.incident == IncidentType::NONE) {
            // Generate new incident
            IncidentType new_incident = generate_random_incident();
            apply_incident_impact(state, new_incident);
            state.duration_remaining = generate_incident_duration(new_incident);
            
            // Schedule incident resolution
            auto resolution_event = std::make_shared<IncidentResolutionEvent>(
                current_time + state.duration_remaining, this, edge_key);
            event_queue_.schedule_event(resolution_event);
            
            stats_.total_incidents++;
            stats_.active_incidents++;
            stats_.incident_counts[new_incident]++;
        }
        
        // Update statistics
        stats_.traffic_level_counts[state.level]++;
        
        // Schedule next update
        auto next_update = std::make_shared<TrafficUpdateEvent>(
            current_time + 5.0, this, edge_key); // Update every 5 minutes
        event_queue_.schedule_event(next_update);
        
        // Apply traffic state to graph edge
        update_graph_edge_weights();
    }
    
    /**
     * @brief Resolve an incident
     */
    void resolve_incident(uint64_t edge_key, simulation::SimulationTime current_time) {
        auto it = edge_traffic_states_.find(edge_key);
        if (it == edge_traffic_states_.end()) return;
        
        TrafficState& state = it->second;
        
        if (state.incident != IncidentType::NONE) {
            state.incident = IncidentType::NONE;
            state.duration_remaining = 0.0;
            state.description.clear();
            
            // Restore normal speed/capacity factors based on traffic level only
            update_traffic_level(edge_key, state, current_time);
            
            stats_.active_incidents--;
            
            // Apply changes to graph
            update_graph_edge_weights();
        }
    }
    
    /**
     * @brief Manually trigger an incident
     */
    void trigger_incident(uint32_t from_node, uint32_t to_node, IncidentType incident,
                         simulation::SimulationTime duration = -1) {
        uint64_t key = get_edge_key(from_node, to_node);
        auto it = edge_traffic_states_.find(key);
        if (it == edge_traffic_states_.end()) return;
        
        TrafficState& state = it->second;
        apply_incident_impact(state, incident);
        
        if (duration < 0) {
            state.duration_remaining = generate_incident_duration(incident);
        } else {
            state.duration_remaining = duration;
        }
        
        // Schedule resolution
        auto resolution_event = std::make_shared<IncidentResolutionEvent>(
            event_queue_.get_current_time() + state.duration_remaining, this, key);
        event_queue_.schedule_event(resolution_event);
        
        stats_.total_incidents++;
        stats_.active_incidents++;
        stats_.incident_counts[incident]++;
        
        update_graph_edge_weights();
    }
    
    /**
     * @brief Set weather conditions
     */
    void set_weather_factor(double factor) {
        params_.weather_factor = factor;
    }
    
    /**
     * @brief Get traffic state for an edge
     */
    const TrafficState* get_traffic_state(uint32_t from_node, uint32_t to_node) const {
        uint64_t key = get_edge_key(from_node, to_node);
        auto it = edge_traffic_states_.find(key);
        return it != edge_traffic_states_.end() ? &it->second : nullptr;
    }
    
    /**
     * @brief Get all blocked roads
     */
    std::vector<std::pair<uint32_t, uint32_t>> get_blocked_roads() const {
        std::vector<std::pair<uint32_t, uint32_t>> blocked;
        
        for (const auto& [key, state] : edge_traffic_states_) {
            if (state.is_blocked()) {
                uint32_t from = static_cast<uint32_t>(key >> 32);
                uint32_t to = static_cast<uint32_t>(key & 0xFFFFFFFF);
                blocked.emplace_back(from, to);
            }
        }
        
        return blocked;
    }
    
    /**
     * @brief Update graph edge weights based on traffic conditions
     */
    void update_graph_edge_weights() {
        for (const auto& [key, state] : edge_traffic_states_) {
            uint32_t from = static_cast<uint32_t>(key >> 32);
            uint32_t to = static_cast<uint32_t>(key & 0xFFFFFFFF);
            
            // Get original edge weight
            auto it = original_edge_weights_.find(key);
            if (it != original_edge_weights_.end()) {
                // Calculate new weight based on original weight and traffic conditions
                double original_weight = it->second;
                double new_weight = original_weight * state.get_travel_time_multiplier();
                
                // Update the edge weight in the graph
                graph_.update_edge_weight(from, to, static_cast<float>(new_weight));
            }
        }
    }
    
    /**
     * @brief Get simulation statistics
     */
    const Statistics& get_statistics() const {
        // Update average congestion level
        if (!edge_traffic_states_.empty()) {
            double total_congestion = 0.0;
            size_t blocked_count = 0;
            
            for (const auto& [key, state] : edge_traffic_states_) {
                total_congestion += static_cast<double>(state.level);
                if (state.is_blocked()) blocked_count++;
            }
            
            const_cast<Statistics&>(stats_).average_congestion_level = 
                total_congestion / edge_traffic_states_.size();
            const_cast<Statistics&>(stats_).total_blocked_roads = blocked_count;
        }
        
        return stats_;
    }
    
    /**
     * @brief Get simulation parameters
     */
    Parameters& get_parameters() { return params_; }
    const Parameters& get_parameters() const { return params_; }
    
    /**
     * @brief Reset simulation state
     */
    void reset() {
        // Clear all traffic states
        for (auto& [key, state] : edge_traffic_states_) {
            state = TrafficState();
        }
        
        // Reset statistics
        stats_ = Statistics();
        
        // Restore original graph weights
        for (const auto& [key, original_weight] : original_edge_weights_) {
            uint32_t from = static_cast<uint32_t>(key >> 32);
            uint32_t to = static_cast<uint32_t>(key & 0xFFFFFFFF);
            graph_.update_edge_weight(from, to, original_weight);
        }
    }
    
    /**
     * @brief Generate traffic report
     */
    std::string generate_traffic_report() const {
        std::string report = "Traffic Simulation Report:\n";
        report += "========================\n";
        
        auto stats = get_statistics();
        report += "Total Incidents: " + std::to_string(stats.total_incidents) + "\n";
        report += "Active Incidents: " + std::to_string(stats.active_incidents) + "\n";
        report += "Average Congestion Level: " + std::to_string(stats.average_congestion_level) + "\n";
        report += "Blocked Roads: " + std::to_string(stats.total_blocked_roads) + "\n\n";
        
        report += "Incident Types:\n";
        for (const auto& [type, count] : stats.incident_counts) {
            report += "  " + std::to_string(static_cast<int>(type)) + ": " + std::to_string(count) + "\n";
        }
        
        report += "\nTraffic Levels:\n";
        for (const auto& [level, count] : stats.traffic_level_counts) {
            report += "  " + std::to_string(static_cast<int>(level)) + ": " + std::to_string(count) + "\n";
        }
        
        return report;
    }
};

} // namespace rts::core::traffic