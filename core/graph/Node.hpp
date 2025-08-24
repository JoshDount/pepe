#pragma once

#include <cstdint>
#include <string>

namespace rts::core::graph {

/**
 * @brief Node representation in the transportation graph
 * 
 * Represents a location in the transportation network (bus stop, intersection, etc.)
 * Optimized for cache locality and memory efficiency.
 */
struct Node {
    uint32_t id;           // Unique identifier
    double lat;            // Latitude in decimal degrees
    double lon;            // Longitude in decimal degrees
    uint32_t first_edge;   // Index of first outgoing edge in edge array
    uint32_t degree;       // Number of outgoing edges
    
    // Additional attributes for transit systems
    uint16_t zone_id;      // Fare zone or traffic zone
    uint8_t node_type;     // 0=intersection, 1=bus_stop, 2=train_station, etc.
    uint8_t flags;         // Bit flags: accessible, traffic_light, etc.
    
    Node() : id(0), lat(0.0), lon(0.0), first_edge(0), degree(0), 
             zone_id(0), node_type(0), flags(0) {}
    
    Node(uint32_t id, double lat, double lon) 
        : id(id), lat(lat), lon(lon), first_edge(0), degree(0),
          zone_id(0), node_type(0), flags(0) {}
    
    Node(uint32_t id, double lat, double lon, uint16_t zone, uint8_t type)
        : id(id), lat(lat), lon(lon), first_edge(0), degree(0),
          zone_id(zone), node_type(type), flags(0) {}
    
    /**
     * @brief Check if node has specific flag set
     */
    bool has_flag(uint8_t flag_bit) const {
        return (flags & (1 << flag_bit)) != 0;
    }
    
    /**
     * @brief Set specific flag
     */
    void set_flag(uint8_t flag_bit, bool value = true) {
        if (value) {
            flags |= (1 << flag_bit);
        } else {
            flags &= ~(1 << flag_bit);
        }
    }
    
    /**
     * @brief Flag definitions
     */
    enum Flags {
        ACCESSIBLE = 0,      // Wheelchair accessible
        TRAFFIC_LIGHT = 1,   // Has traffic light
        MAJOR_INTERSECTION = 2,
        TRANSIT_HUB = 3,
        RESTRICTED_ACCESS = 4,
        TEMPORARY_CLOSURE = 5
    };
    
    /**
     * @brief Node type definitions
     */
    enum Type {
        INTERSECTION = 0,
        BUS_STOP = 1,
        TRAIN_STATION = 2,
        SUBWAY_STATION = 3,
        AIRPORT = 4,
        FERRY_TERMINAL = 5,
        PARKING = 6,
        POI = 7  // Point of Interest
    };
    
    /**
     * @brief Get human-readable type string
     */
    const char* get_type_string() const {
        switch (node_type) {
            case INTERSECTION: return "Intersection";
            case BUS_STOP: return "Bus Stop";
            case TRAIN_STATION: return "Train Station";
            case SUBWAY_STATION: return "Subway Station";
            case AIRPORT: return "Airport";
            case FERRY_TERMINAL: return "Ferry Terminal";
            case PARKING: return "Parking";
            case POI: return "Point of Interest";
            default: return "Unknown";
        }
    }
    
    /**
     * @brief Calculate approximate distance to another node (Haversine)
     */
    double distance_to(const Node& other) const;
    
    /**
     * @brief Calculate bearing to another node
     */
    double bearing_to(const Node& other) const;
    
    /**
     * @brief Equality operator
     */
    bool operator==(const Node& other) const {
        return id == other.id;
    }
    
    /**
     * @brief Less than operator for sorting
     */
    bool operator<(const Node& other) const {
        return id < other.id;
    }
};

} // namespace rts::core::graph