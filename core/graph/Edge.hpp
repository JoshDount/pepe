#pragma once

#include <cstdint>
#include <limits>

namespace rts::core::graph {

/**
 * @brief Edge representation in the transportation graph
 * 
 * Represents a connection between two nodes with associated costs and constraints.
 * Optimized for memory efficiency and cache performance.
 */
struct Edge {
    uint32_t from;         // Source node ID
    uint32_t to;           // Target node ID
    float weight;          // Base travel time in seconds
    float length;          // Physical distance in meters
    uint16_t capacity;     // Maximum vehicles/passengers per hour
    uint16_t profile_id;   // Profile for different cost calculations
    uint8_t flags;         // Bit flags for various properties
    uint8_t transport_mode; // Transportation mode
    
    Edge() : from(0), to(0), weight(0.0f), length(0.0f), 
             capacity(1000), profile_id(0), flags(0), transport_mode(0) {}
    
    Edge(uint32_t from, uint32_t to, float weight) 
        : from(from), to(to), weight(weight), length(weight * 10.0f), // Approximate
          capacity(1000), profile_id(0), flags(0), transport_mode(ROAD) {}
    
    Edge(uint32_t from, uint32_t to, float weight, float length)
        : from(from), to(to), weight(weight), length(length),
          capacity(1000), profile_id(0), flags(0), transport_mode(ROAD) {}
    
    Edge(uint32_t from, uint32_t to, float weight, float length, 
         uint8_t mode, uint16_t cap = 1000)
        : from(from), to(to), weight(weight), length(length),
          capacity(cap), profile_id(0), flags(0), transport_mode(mode) {}
    
    /**
     * @brief Check if edge has specific flag set
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
        ONE_WAY = 0,           // One-way edge
        TOLL_ROAD = 1,         // Requires toll payment
        BRIDGE = 2,            // Bridge segment
        TUNNEL = 3,            // Tunnel segment
        HIGHWAY = 4,           // Highway/freeway
        RESTRICTED = 5,        // Access restrictions
        TEMPORARY_CLOSURE = 6, // Temporarily closed
        CONSTRUCTION = 7       // Under construction
    };
    
    /**
     * @brief Transportation mode definitions
     */
    enum TransportMode {
        ROAD = 0,        // Car/bus on road
        WALKING = 1,     // Pedestrian path
        CYCLING = 2,     // Bicycle path
        BUS = 3,         // Bus route
        TRAIN = 4,       // Train connection
        SUBWAY = 5,      // Subway/metro
        TRAM = 6,        // Tram/light rail
        FERRY = 7,       // Ferry connection
        FLIGHT = 8       // Flight connection
    };
    
    /**
     * @brief Get human-readable transport mode string
     */
    const char* get_mode_string() const {
        switch (transport_mode) {
            case ROAD: return "Road";
            case WALKING: return "Walking";
            case CYCLING: return "Cycling";
            case BUS: return "Bus";
            case TRAIN: return "Train";
            case SUBWAY: return "Subway";
            case TRAM: return "Tram";
            case FERRY: return "Ferry";
            case FLIGHT: return "Flight";
            default: return "Unknown";
        }
    }
    
    /**
     * @brief Check if this is a one-way edge
     */
    bool is_one_way() const {
        return has_flag(ONE_WAY);
    }
    
    /**
     * @brief Check if edge is currently accessible
     */
    bool is_accessible() const {
        return !has_flag(TEMPORARY_CLOSURE) && !has_flag(CONSTRUCTION);
    }
    
    /**
     * @brief Get speed in km/h based on length and weight
     */
    float get_speed_kmh() const {
        if (weight <= 0.0f) return 0.0f;
        return (length / 1000.0f) / (weight / 3600.0f);  // km/h
    }
    
    /**
     * @brief Get effective weight considering current modifiers
     * @param congestion_factor Multiplier for traffic congestion (1.0 = normal)
     * @param mode_preference Preference multiplier for transport mode
     */
    float get_effective_weight(float congestion_factor = 1.0f, 
                              float mode_preference = 1.0f) const {
        if (!is_accessible()) {
            return std::numeric_limits<float>::infinity();
        }
        
        float effective = weight * congestion_factor * mode_preference;
        
        // Add penalties for certain edge types
        if (has_flag(TOLL_ROAD)) effective *= 1.2f;        // 20% penalty for tolls
        if (has_flag(CONSTRUCTION)) effective *= 1.5f;      // 50% penalty for construction
        
        return effective;
    }
    
    /**
     * @brief Calculate travel cost based on distance and time
     * @param time_weight Weight for time component (0.0 to 1.0)
     * @param distance_weight Weight for distance component (0.0 to 1.0)
     */
    float get_travel_cost(float time_weight = 0.7f, float distance_weight = 0.3f) const {
        return time_weight * weight + distance_weight * (length / 1000.0f);
    }
    
    /**
     * @brief Get turn restriction flags for path planning
     */
    struct TurnRestrictions {
        bool no_left_turn : 1;
        bool no_right_turn : 1;
        bool no_u_turn : 1;
        bool no_straight : 1;
        uint8_t reserved : 4;
    };
    
    /**
     * @brief Equality operator
     */
    bool operator==(const Edge& other) const {
        return from == other.from && to == other.to;
    }
    
    /**
     * @brief Less than operator for sorting
     */
    bool operator<(const Edge& other) const {
        if (from != other.from) return from < other.from;
        return to < other.to;
    }
    
    /**
     * @brief Reverse edge (for undirected graphs)
     */
    Edge reverse() const {
        Edge rev = *this;
        rev.from = to;
        rev.to = from;
        return rev;
    }
    
    /**
     * @brief Check if edge connects given nodes
     */
    bool connects(uint32_t node1, uint32_t node2) const {
        return (from == node1 && to == node2) || 
               (!is_one_way() && from == node2 && to == node1);
    }
    
    /**
     * @brief Get the other end of the edge given one node
     */
    uint32_t get_other_end(uint32_t node_id) const {
        if (from == node_id) return to;
        if (to == node_id) return from;
        return std::numeric_limits<uint32_t>::max(); // Invalid
    }
};

} // namespace rts::core::graph