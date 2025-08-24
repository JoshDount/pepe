#pragma once

#include "GTFSData.hpp"
#include "GTFSParser.hpp"
#include "../graph/WeightedGraph.hpp"
#include "../graph/Node.hpp"
#include "../graph/Edge.hpp"
#include <chrono>
#include <algorithm>
#include <cmath>
#include <set>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace rts::core::io {

/**
 * @brief GTFS to Graph converter
 * 
 * Converts GTFS transit data into a weighted graph suitable for routing algorithms.
 * Supports different conversion strategies and optimization options.
 */
class GTFSGraphConverter {
public:
    /**
     * @brief Conversion strategies
     */
    enum class ConversionStrategy {
        STOP_TO_STOP,           // Direct connections between consecutive stops
        TIME_EXPANDED,          // Time-expanded graph with timed nodes
        FREQUENCY_BASED,        // Frequency-based representation
        TRANSFER_PATTERNS       // Transfer pattern approach
    };
    
    /**
     * @brief Conversion options
     */
    struct ConversionOptions {
        ConversionStrategy strategy;
        double walking_speed_kmh;
        double max_transfer_distance_m;
        bool include_walking_edges;
        bool include_frequency_edges;
        double frequency_headway_minutes;
        std::string start_time;
        std::string end_time;
        int time_resolution_seconds;
        
        ConversionOptions() 
            : strategy(ConversionStrategy::STOP_TO_STOP),
              walking_speed_kmh(5.0),
              max_transfer_distance_m(500.0),
              include_walking_edges(true),
              include_frequency_edges(false),
              frequency_headway_minutes(10.0),
              start_time("06:00:00"),
              end_time("22:00:00"),
              time_resolution_seconds(60) {}
    };
    
    /**
     * @brief Conversion result
     */
    struct ConversionResult {
        bool success = false;
        std::string error_message;
        size_t nodes_created = 0;
        size_t edges_created = 0;
        size_t transfer_edges = 0;
        size_t transit_edges = 0;
        double conversion_time_ms = 0.0;
        
        // Statistics
        size_t unique_stops = 0;
        size_t unique_routes = 0;
        size_t unique_trips = 0;
        size_t total_stop_times = 0;
    };
    
private:
    ConversionOptions options_;
    
    /**
     * @brief Calculate Haversine distance between two points
     */
    double haversine_distance(double lat1, double lon1, double lat2, double lon2) const {
        const double R = 6371000.0; // Earth radius in meters
        const double phi1 = lat1 * M_PI / 180.0;
        const double phi2 = lat2 * M_PI / 180.0;
        const double delta_phi = (lat2 - lat1) * M_PI / 180.0;
        const double delta_lambda = (lon2 - lon1) * M_PI / 180.0;
        
        const double a = std::sin(delta_phi / 2.0) * std::sin(delta_phi / 2.0) +
                        std::cos(phi1) * std::cos(phi2) * 
                        std::sin(delta_lambda / 2.0) * std::sin(delta_lambda / 2.0);
        const double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
        
        return R * c;
    }
    
    /**
     * @brief Parse time string to seconds since midnight
     */
    int parse_time_to_seconds(const std::string& time_str) const {
        if (time_str.length() < 8) return -1;
        
        int hours = std::stoi(time_str.substr(0, 2));
        int minutes = std::stoi(time_str.substr(3, 2));
        int seconds = std::stoi(time_str.substr(6, 2));
        
        return hours * 3600 + minutes * 60 + seconds;
    }
    
    /**
     * @brief Calculate travel time between stops based on distance and mode
     */
    double calculate_travel_time(const Stop& from_stop, const Stop& to_stop, RouteType route_type) const {
        double distance = haversine_distance(from_stop.stop_lat, from_stop.stop_lon,
                                           to_stop.stop_lat, to_stop.stop_lon);
        
        // Default speeds by transport mode (km/h)
        double speed_kmh = 25.0; // Default bus speed
        switch (route_type) {
            case RouteType::TRAM: speed_kmh = 20.0; break;
            case RouteType::SUBWAY: speed_kmh = 35.0; break;
            case RouteType::RAIL: speed_kmh = 60.0; break;
            case RouteType::BUS: speed_kmh = 25.0; break;
            case RouteType::FERRY: speed_kmh = 15.0; break;
            case RouteType::CABLE_TRAM: speed_kmh = 10.0; break;
            case RouteType::AERIAL_LIFT: speed_kmh = 8.0; break;
            case RouteType::FUNICULAR: speed_kmh = 12.0; break;
            case RouteType::TROLLEYBUS: speed_kmh = 22.0; break;
            case RouteType::MONORAIL: speed_kmh = 40.0; break;
        }
        
        // Convert to travel time in seconds
        return (distance / 1000.0) / speed_kmh * 3600.0;
    }
    
    /**
     * @brief Create stop-to-stop graph
     */
    ConversionResult convert_stop_to_stop(const GTFSDataset& dataset, graph::WeightedGraph& graph) const {
        ConversionResult result;
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            // Validate dataset before conversion
            if (!dataset.validate()) {
                result.error_message = "Invalid GTFS dataset: dataset validation failed";
                return result;
            }
            
            // Check minimum requirements for graph creation
            if (dataset.get_stop_count() == 0) {
                result.error_message = "Cannot create graph: no stops in dataset";
                return result;
            }
            
            if (dataset.get_trip_count() == 0) {
                result.error_message = "Cannot create graph: no trips in dataset";
                return result;
            }
            // Add all stops as nodes
            for (const auto& [stop_id, stop] : dataset.get_stops()) {
                graph::Node node(std::hash<std::string>{}(stop_id), stop.stop_lat, stop.stop_lon);
                graph.add_node(node);
                result.nodes_created++;
            }
            result.unique_stops = dataset.get_stop_count();
            
            // Process each trip to create edges between consecutive stops
            std::set<std::pair<std::string, std::string>> processed_edges;
            
            for (const auto& [trip_id, trip] : dataset.get_trips()) {
                auto stop_times = dataset.get_stop_times_for_trip(trip_id);
                if (stop_times.size() < 2) continue;
                
                // Sort stop times by sequence
                std::sort(stop_times.begin(), stop_times.end(),
                         [](const StopTime& a, const StopTime& b) {
                             return a.stop_sequence < b.stop_sequence;
                         });
                
                // Get route information
                const Route* route = dataset.get_route(trip.route_id);
                if (!route) continue;
                
                // Create edges between consecutive stops
                for (size_t i = 0; i < stop_times.size() - 1; ++i) {
                    const StopTime& from_stop_time = stop_times[i];
                    const StopTime& to_stop_time = stop_times[i + 1];
                    
                    // Avoid duplicate edges
                    std::pair<std::string, std::string> edge_key = {from_stop_time.stop_id, to_stop_time.stop_id};
                    if (processed_edges.find(edge_key) != processed_edges.end()) {
                        continue;
                    }
                    processed_edges.insert(edge_key);
                    
                    const Stop* from_stop = dataset.get_stop(from_stop_time.stop_id);
                    const Stop* to_stop = dataset.get_stop(to_stop_time.stop_id);
                    
                    if (!from_stop || !to_stop) continue;
                    
                    // Calculate travel time
                    double travel_time = calculate_travel_time(*from_stop, *to_stop, route->route_type);
                    
                    // If stop times are provided, use them for more accurate travel time
                    if (!from_stop_time.departure_time.empty() && !to_stop_time.arrival_time.empty()) {
                        int departure_seconds = parse_time_to_seconds(from_stop_time.departure_time);
                        int arrival_seconds = parse_time_to_seconds(to_stop_time.arrival_time);
                        if (departure_seconds >= 0 && arrival_seconds >= 0 && arrival_seconds > departure_seconds) {
                            travel_time = arrival_seconds - departure_seconds;
                        }
                    }
                    
                    // Calculate distance
                    double distance = haversine_distance(from_stop->stop_lat, from_stop->stop_lon,
                                                       to_stop->stop_lat, to_stop->stop_lon);
                    
                    // Determine transport mode
                    uint8_t transport_mode = static_cast<uint8_t>(route->route_type);
                    
                    // Add edge to graph
                    uint32_t from_node_id = std::hash<std::string>{}(from_stop_time.stop_id);
                    uint32_t to_node_id = std::hash<std::string>{}(to_stop_time.stop_id);
                    
                    graph::Edge edge(from_node_id, to_node_id, static_cast<float>(travel_time), 
                                   static_cast<float>(distance), transport_mode);
                    
                    graph.add_edge(edge);
                    result.edges_created++;
                    result.transit_edges++;
                }
            }
            
            // Add walking transfers between nearby stops
            if (options_.include_walking_edges) {
                add_walking_transfers(dataset, graph, result);
            }
            
            result.unique_routes = dataset.get_route_count();
            result.unique_trips = dataset.get_trip_count();
            result.total_stop_times = dataset.get_stop_time_count();
            result.success = true;
            
        } catch (const std::exception& e) {
            result.error_message = std::string("Conversion error: ") + e.what();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.conversion_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        
        return result;
    }
    
    /**
     * @brief Add walking transfer edges between nearby stops
     */
    void add_walking_transfers(const GTFSDataset& dataset, graph::WeightedGraph& graph, 
                              ConversionResult& result) const {
        const auto& stops = dataset.get_stops();
        std::vector<std::pair<std::string, Stop>> stop_vector(stops.begin(), stops.end());
        
        for (size_t i = 0; i < stop_vector.size(); ++i) {
            for (size_t j = i + 1; j < stop_vector.size(); ++j) {
                const Stop& stop1 = stop_vector[i].second;
                const Stop& stop2 = stop_vector[j].second;
                
                double distance = haversine_distance(stop1.stop_lat, stop1.stop_lon,
                                                   stop2.stop_lat, stop2.stop_lon);
                
                if (distance <= options_.max_transfer_distance_m) {
                    // Calculate walking time
                    double walking_time = (distance / 1000.0) / options_.walking_speed_kmh * 3600.0;
                    
                    uint32_t node1_id = std::hash<std::string>{}(stop1.stop_id);
                    uint32_t node2_id = std::hash<std::string>{}(stop2.stop_id);
                    
                    // Add bidirectional walking edges
                    graph::Edge edge1(node1_id, node2_id, static_cast<float>(walking_time), 
                                     static_cast<float>(distance), graph::Edge::WALKING);
                    graph::Edge edge2(node2_id, node1_id, static_cast<float>(walking_time), 
                                     static_cast<float>(distance), graph::Edge::WALKING);
                    
                    graph.add_edge(edge1);
                    graph.add_edge(edge2);
                    result.edges_created += 2;
                    result.transfer_edges += 2;
                }
            }
        }
    }
    
public:
    GTFSGraphConverter(const ConversionOptions& options = ConversionOptions()) : options_(options) {}
    
    /**
     * @brief Convert GTFS dataset to weighted graph
     */
    ConversionResult convert(const GTFSDataset& dataset, graph::WeightedGraph& graph) {
        switch (options_.strategy) {
            case ConversionStrategy::STOP_TO_STOP:
                return convert_stop_to_stop(dataset, graph);
            case ConversionStrategy::TIME_EXPANDED:
                // TODO: Implement time-expanded graph conversion
                break;
            case ConversionStrategy::FREQUENCY_BASED:
                // TODO: Implement frequency-based conversion
                break;
            case ConversionStrategy::TRANSFER_PATTERNS:
                // TODO: Implement transfer pattern conversion
                break;
        }
        
        ConversionResult result;
        result.error_message = "Conversion strategy not implemented";
        return result;
    }
    
    /**
     * @brief Get conversion options
     */
    const ConversionOptions& get_options() const { return options_; }
    
    /**
     * @brief Set conversion options
     */
    void set_options(const ConversionOptions& options) { options_ = options; }
    
    /**
     * @brief Create sample GTFS data for testing
     */
    static GTFSDataset create_sample_dataset() {
        GTFSDataset dataset;
        
        // Add sample agency
        Agency agency("SAMPLE", "Sample Transit Agency", "http://sample.transit.com", "America/New_York");
        dataset.add_agency(agency);
        
        // Add sample stops
        dataset.add_stop(Stop("STOP1", "Downtown Station", 40.7128, -74.0060));
        dataset.add_stop(Stop("STOP2", "Midtown Station", 40.7589, -73.9851));
        dataset.add_stop(Stop("STOP3", "Uptown Station", 40.7831, -73.9712));
        dataset.add_stop(Stop("STOP4", "Airport Terminal", 40.6892, -74.1745));
        
        // Add sample route
        Route route("ROUTE1", "SAMPLE", "R1", "Downtown Express", RouteType::SUBWAY);
        dataset.add_route(route);
        
        // Add sample trips
        Trip trip1("ROUTE1", "SERVICE1", "TRIP1", "Uptown");
        Trip trip2("ROUTE1", "SERVICE1", "TRIP2", "Downtown");
        dataset.add_trip(trip1);
        dataset.add_trip(trip2);
        
        // Add sample stop times for trip1 (Downtown -> Uptown)
        dataset.add_stop_time(StopTime("TRIP1", "08:00:00", "08:00:00", "STOP1", 1));
        dataset.add_stop_time(StopTime("TRIP1", "08:05:00", "08:05:00", "STOP2", 2));
        dataset.add_stop_time(StopTime("TRIP1", "08:10:00", "08:10:00", "STOP3", 3));
        
        // Add sample stop times for trip2 (Uptown -> Downtown)
        dataset.add_stop_time(StopTime("TRIP2", "08:15:00", "08:15:00", "STOP3", 1));
        dataset.add_stop_time(StopTime("TRIP2", "08:20:00", "08:20:00", "STOP2", 2));
        dataset.add_stop_time(StopTime("TRIP2", "08:25:00", "08:25:00", "STOP1", 3));
        
        // Add sample calendar
        Calendar calendar("SERVICE1", true, true, true, true, true, false, false, 
                         "20240101", "20241231");
        dataset.add_calendar(calendar);
        
        return dataset;
    }
    
    /**
     * @brief Generate sample GTFS files for testing
     */
    static bool generate_sample_gtfs_files(const std::string& output_directory) {
        try {
            std::filesystem::create_directories(output_directory);
            
            // agency.txt
            {
                std::ofstream file(output_directory + "/agency.txt");
                file << "agency_id,agency_name,agency_url,agency_timezone\n";
                file << "SAMPLE,Sample Transit Agency,http://sample.transit.com,America/New_York\n";
            }
            
            // stops.txt
            {
                std::ofstream file(output_directory + "/stops.txt");
                file << "stop_id,stop_name,stop_lat,stop_lon\n";
                file << "STOP1,\"Downtown Station\",40.7128,-74.0060\n";
                file << "STOP2,\"Midtown Station\",40.7589,-73.9851\n";
                file << "STOP3,\"Uptown Station\",40.7831,-73.9712\n";
                file << "STOP4,\"Airport Terminal\",40.6892,-74.1745\n";
            }
            
            // routes.txt
            {
                std::ofstream file(output_directory + "/routes.txt");
                file << "route_id,agency_id,route_short_name,route_long_name,route_type\n";
                file << "ROUTE1,SAMPLE,R1,\"Downtown Express\",1\n";
                file << "ROUTE2,SAMPLE,B1,\"Airport Bus\",3\n";
            }
            
            // trips.txt
            {
                std::ofstream file(output_directory + "/trips.txt");
                file << "route_id,service_id,trip_id,trip_headsign\n";
                file << "ROUTE1,SERVICE1,TRIP1,Uptown\n";
                file << "ROUTE1,SERVICE1,TRIP2,Downtown\n";
                file << "ROUTE2,SERVICE1,TRIP3,\"To Airport\"\n";
            }
            
            // stop_times.txt
            {
                std::ofstream file(output_directory + "/stop_times.txt");
                file << "trip_id,arrival_time,departure_time,stop_id,stop_sequence\n";
                file << "TRIP1,08:00:00,08:00:00,STOP1,1\n";
                file << "TRIP1,08:05:00,08:05:00,STOP2,2\n";
                file << "TRIP1,08:10:00,08:10:00,STOP3,3\n";
                file << "TRIP2,08:15:00,08:15:00,STOP3,1\n";
                file << "TRIP2,08:20:00,08:20:00,STOP2,2\n";
                file << "TRIP2,08:25:00,08:25:00,STOP1,3\n";
                file << "TRIP3,09:00:00,09:00:00,STOP1,1\n";
                file << "TRIP3,09:30:00,09:30:00,STOP4,2\n";
            }
            
            // calendar.txt
            {
                std::ofstream file(output_directory + "/calendar.txt");
                file << "service_id,monday,tuesday,wednesday,thursday,friday,saturday,sunday,start_date,end_date\n";
                file << "SERVICE1,1,1,1,1,1,0,0,20240101,20241231\n";
            }
            
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
};

} // namespace rts::core::io