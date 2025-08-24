#define _USE_MATH_DEFINES
#include "Node.hpp"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace rts::core::graph {

/**
 * @brief Calculate Haversine distance between two points on Earth
 * @param other Another node
 * @return Distance in meters
 */
double Node::distance_to(const Node& other) const {
    constexpr double EARTH_RADIUS_M = 6371000.0; // Earth's radius in meters
    constexpr double DEG_TO_RAD = M_PI / 180.0;
    
    // Convert latitude and longitude from degrees to radians
    double lat1_rad = lat * DEG_TO_RAD;
    double lon1_rad = lon * DEG_TO_RAD;
    double lat2_rad = other.lat * DEG_TO_RAD;
    double lon2_rad = other.lon * DEG_TO_RAD;
    
    // Haversine formula
    double dlat = lat2_rad - lat1_rad;
    double dlon = lon2_rad - lon1_rad;
    
    double a = std::sin(dlat / 2.0) * std::sin(dlat / 2.0) +
               std::cos(lat1_rad) * std::cos(lat2_rad) *
               std::sin(dlon / 2.0) * std::sin(dlon / 2.0);
    
    double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
    
    return EARTH_RADIUS_M * c;
}

/**
 * @brief Calculate bearing from this node to another
 * @param other Target node
 * @return Bearing in degrees (0-360, where 0 is North)
 */
double Node::bearing_to(const Node& other) const {
    constexpr double DEG_TO_RAD = M_PI / 180.0;
    constexpr double RAD_TO_DEG = 180.0 / M_PI;
    
    double lat1_rad = lat * DEG_TO_RAD;
    double lon1_rad = lon * DEG_TO_RAD;
    double lat2_rad = other.lat * DEG_TO_RAD;
    double lon2_rad = other.lon * DEG_TO_RAD;
    
    double dlon = lon2_rad - lon1_rad;
    
    double y = std::sin(dlon) * std::cos(lat2_rad);
    double x = std::cos(lat1_rad) * std::sin(lat2_rad) -
               std::sin(lat1_rad) * std::cos(lat2_rad) * std::cos(dlon);
    
    double bearing_rad = std::atan2(y, x);
    double bearing_deg = bearing_rad * RAD_TO_DEG;
    
    // Normalize to 0-360 degrees
    if (bearing_deg < 0) {
        bearing_deg += 360.0;
    }
    
    return bearing_deg;
}

} // namespace rts::core::graph