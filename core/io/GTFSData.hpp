#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>

namespace rts::core::io {

/**
 * @brief GTFS Route Types (simplified from GTFS specification)
 */
enum class RouteType {
    TRAM = 0,           // Tram, Streetcar, Light rail
    SUBWAY = 1,         // Subway, Metro
    RAIL = 2,           // Rail (intercity or long-distance)
    BUS = 3,            // Bus
    FERRY = 4,          // Ferry
    CABLE_TRAM = 5,     // Cable tram
    AERIAL_LIFT = 6,    // Aerial lift, suspended cable car
    FUNICULAR = 7,      // Funicular
    TROLLEYBUS = 11,    // Trolleybus
    MONORAIL = 12       // Monorail
};

/**
 * @brief GTFS Trip Direction
 */
enum class TripDirection {
    OUTBOUND = 0,       // Travel in one direction (e.g., outbound travel)
    INBOUND = 1         // Travel in the opposite direction (e.g., inbound travel)
};

/**
 * @brief GTFS Pickup/Drop-off Type
 */
enum class PickupDropoffType {
    REGULARLY_SCHEDULED = 0,    // Regularly scheduled pickup/drop-off
    NO_PICKUP_DROPOFF = 1,      // No pickup/drop-off available
    PHONE_AGENCY = 2,           // Must phone agency to arrange pickup/drop-off
    COORDINATE_WITH_DRIVER = 3  // Must coordinate with driver to arrange pickup/drop-off
};

/**
 * @brief GTFS Agency (agencies.txt)
 */
struct Agency {
    std::string agency_id;
    std::string agency_name;
    std::string agency_url;
    std::string agency_timezone;
    std::string agency_lang;
    std::string agency_phone;
    std::string agency_fare_url;
    std::string agency_email;
    
    Agency() = default;
    Agency(const std::string& id, const std::string& name, const std::string& url, const std::string& timezone)
        : agency_id(id), agency_name(name), agency_url(url), agency_timezone(timezone) {}
};

/**
 * @brief GTFS Stop (stops.txt)
 */
struct Stop {
    std::string stop_id;
    std::string stop_code;
    std::string stop_name;
    std::string stop_desc;
    double stop_lat;
    double stop_lon;
    std::string zone_id;
    std::string stop_url;
    int location_type;          // 0=stop, 1=station, 2=station entrance/exit
    std::string parent_station;
    std::string stop_timezone;
    int wheelchair_boarding;    // 0=no info, 1=accessible, 2=not accessible
    std::string level_id;
    std::string platform_code;
    
    Stop() : stop_lat(0.0), stop_lon(0.0), location_type(0), wheelchair_boarding(0) {}
    Stop(const std::string& id, const std::string& name, double lat, double lon)
        : stop_id(id), stop_name(name), stop_lat(lat), stop_lon(lon), 
          location_type(0), wheelchair_boarding(0) {}
};

/**
 * @brief GTFS Route (routes.txt)
 */
struct Route {
    std::string route_id;
    std::string agency_id;
    std::string route_short_name;
    std::string route_long_name;
    std::string route_desc;
    RouteType route_type;
    std::string route_url;
    std::string route_color;
    std::string route_text_color;
    int route_sort_order;
    int continuous_pickup;      // 0=continuous stopping, 1=no continuous stopping
    int continuous_drop_off;    // 0=continuous dropping off, 1=no continuous dropping off
    
    Route() : route_type(RouteType::BUS), route_sort_order(0), continuous_pickup(1), continuous_drop_off(1) {}
    Route(const std::string& id, const std::string& agency, const std::string& short_name, 
          const std::string& long_name, RouteType type)
        : route_id(id), agency_id(agency), route_short_name(short_name), 
          route_long_name(long_name), route_type(type), route_sort_order(0),
          continuous_pickup(1), continuous_drop_off(1) {}
};

/**
 * @brief GTFS Trip (trips.txt)
 */
struct Trip {
    std::string route_id;
    std::string service_id;
    std::string trip_id;
    std::string trip_headsign;
    std::string trip_short_name;
    TripDirection direction_id;
    std::string block_id;
    std::string shape_id;
    int wheelchair_accessible;  // 0=no info, 1=accessible, 2=not accessible
    int bikes_allowed;          // 0=no info, 1=allowed, 2=not allowed
    
    Trip() : direction_id(TripDirection::OUTBOUND), wheelchair_accessible(0), bikes_allowed(0) {}
    Trip(const std::string& route_id, const std::string& service_id, const std::string& trip_id, 
         const std::string& headsign)
        : route_id(route_id), service_id(service_id), trip_id(trip_id), trip_headsign(headsign),
          direction_id(TripDirection::OUTBOUND), wheelchair_accessible(0), bikes_allowed(0) {}
};

/**
 * @brief GTFS Stop Time (stop_times.txt)
 */
struct StopTime {
    std::string trip_id;
    std::string arrival_time;       // HH:MM:SS format
    std::string departure_time;     // HH:MM:SS format
    std::string stop_id;
    uint32_t stop_sequence;
    std::string stop_headsign;
    PickupDropoffType pickup_type;
    PickupDropoffType drop_off_type;
    int continuous_pickup;
    int continuous_drop_off;
    double shape_dist_traveled;
    int timepoint;              // 0=times are approximate, 1=times are exact
    
    StopTime() : stop_sequence(0), pickup_type(PickupDropoffType::REGULARLY_SCHEDULED),
                drop_off_type(PickupDropoffType::REGULARLY_SCHEDULED), continuous_pickup(1),
                continuous_drop_off(1), shape_dist_traveled(0.0), timepoint(1) {}
    
    StopTime(const std::string& trip_id, const std::string& arrival, const std::string& departure,
             const std::string& stop_id, uint32_t sequence)
        : trip_id(trip_id), arrival_time(arrival), departure_time(departure), 
          stop_id(stop_id), stop_sequence(sequence), pickup_type(PickupDropoffType::REGULARLY_SCHEDULED),
          drop_off_type(PickupDropoffType::REGULARLY_SCHEDULED), continuous_pickup(1),
          continuous_drop_off(1), shape_dist_traveled(0.0), timepoint(1) {}
};

/**
 * @brief GTFS Calendar (calendar.txt)
 */
struct Calendar {
    std::string service_id;
    bool monday;
    bool tuesday;
    bool wednesday;
    bool thursday;
    bool friday;
    bool saturday;
    bool sunday;
    std::string start_date;     // YYYYMMDD format
    std::string end_date;       // YYYYMMDD format
    
    Calendar() : monday(false), tuesday(false), wednesday(false), thursday(false),
                friday(false), saturday(false), sunday(false) {}
    
    Calendar(const std::string& service_id, bool mon, bool tue, bool wed, bool thu, 
             bool fri, bool sat, bool sun, const std::string& start, const std::string& end)
        : service_id(service_id), monday(mon), tuesday(tue), wednesday(wed), thursday(thu),
          friday(fri), saturday(sat), sunday(sun), start_date(start), end_date(end) {}
};

/**
 * @brief GTFS Calendar Date (calendar_dates.txt)
 */
struct CalendarDate {
    std::string service_id;
    std::string date;           // YYYYMMDD format
    int exception_type;         // 1=service added, 2=service removed
    
    CalendarDate() : exception_type(1) {}
    CalendarDate(const std::string& service_id, const std::string& date, int exception)
        : service_id(service_id), date(date), exception_type(exception) {}
};

/**
 * @brief GTFS Shape Point (shapes.txt)
 */
struct ShapePoint {
    std::string shape_id;
    double shape_pt_lat;
    double shape_pt_lon;
    uint32_t shape_pt_sequence;
    double shape_dist_traveled;
    
    ShapePoint() : shape_pt_lat(0.0), shape_pt_lon(0.0), shape_pt_sequence(0), shape_dist_traveled(0.0) {}
    ShapePoint(const std::string& shape_id, double lat, double lon, uint32_t sequence, double dist)
        : shape_id(shape_id), shape_pt_lat(lat), shape_pt_lon(lon), 
          shape_pt_sequence(sequence), shape_dist_traveled(dist) {}
};

/**
 * @brief GTFS Feed Info (feed_info.txt)
 */
struct FeedInfo {
    std::string feed_publisher_name;
    std::string feed_publisher_url;
    std::string feed_lang;
    std::string default_lang;
    std::string feed_start_date;    // YYYYMMDD format
    std::string feed_end_date;      // YYYYMMDD format
    std::string feed_version;
    std::string feed_contact_email;
    std::string feed_contact_url;
    
    FeedInfo() = default;
    FeedInfo(const std::string& publisher_name, const std::string& publisher_url, 
             const std::string& lang, const std::string& version)
        : feed_publisher_name(publisher_name), feed_publisher_url(publisher_url),
          feed_lang(lang), feed_version(version) {}
};

/**
 * @brief GTFS Dataset container
 */
class GTFSDataset {
private:
    std::unordered_map<std::string, Agency> agencies_;
    std::unordered_map<std::string, Stop> stops_;
    std::unordered_map<std::string, Route> routes_;
    std::unordered_map<std::string, Trip> trips_;
    std::vector<StopTime> stop_times_;                      // Ordered by trip_id, stop_sequence
    std::unordered_map<std::string, Calendar> calendars_;
    std::vector<CalendarDate> calendar_dates_;
    std::vector<ShapePoint> shapes_;                        // Ordered by shape_id, shape_pt_sequence
    FeedInfo feed_info_;
    
    // Indexes for fast lookup
    std::unordered_map<std::string, std::vector<size_t>> trip_stop_times_index_;
    std::unordered_map<std::string, std::vector<size_t>> shape_points_index_;
    std::unordered_map<std::string, std::vector<std::string>> route_trips_index_;
    
public:
    GTFSDataset() = default;
    
    // Agency operations
    void add_agency(const Agency& agency) { agencies_[agency.agency_id] = agency; }
    const Agency* get_agency(const std::string& agency_id) const {
        auto it = agencies_.find(agency_id);
        return it != agencies_.end() ? &it->second : nullptr;
    }
    const std::unordered_map<std::string, Agency>& get_agencies() const { return agencies_; }
    
    // Stop operations
    void add_stop(const Stop& stop) { stops_[stop.stop_id] = stop; }
    const Stop* get_stop(const std::string& stop_id) const {
        auto it = stops_.find(stop_id);
        return it != stops_.end() ? &it->second : nullptr;
    }
    const std::unordered_map<std::string, Stop>& get_stops() const { return stops_; }
    
    // Route operations
    void add_route(const Route& route) { routes_[route.route_id] = route; }
    const Route* get_route(const std::string& route_id) const {
        auto it = routes_.find(route_id);
        return it != routes_.end() ? &it->second : nullptr;
    }
    const std::unordered_map<std::string, Route>& get_routes() const { return routes_; }
    
    // Trip operations
    void add_trip(const Trip& trip) { 
        trips_[trip.trip_id] = trip; 
        route_trips_index_[trip.route_id].push_back(trip.trip_id);
    }
    const Trip* get_trip(const std::string& trip_id) const {
        auto it = trips_.find(trip_id);
        return it != trips_.end() ? &it->second : nullptr;
    }
    const std::unordered_map<std::string, Trip>& get_trips() const { return trips_; }
    std::vector<std::string> get_trips_for_route(const std::string& route_id) const {
        auto it = route_trips_index_.find(route_id);
        return it != route_trips_index_.end() ? it->second : std::vector<std::string>();
    }
    
    // Stop time operations
    void add_stop_time(const StopTime& stop_time) {
        stop_times_.push_back(stop_time);
        trip_stop_times_index_[stop_time.trip_id].push_back(stop_times_.size() - 1);
    }
    const std::vector<StopTime>& get_stop_times() const { return stop_times_; }
    std::vector<StopTime> get_stop_times_for_trip(const std::string& trip_id) const {
        std::vector<StopTime> result;
        auto it = trip_stop_times_index_.find(trip_id);
        if (it != trip_stop_times_index_.end()) {
            for (size_t index : it->second) {
                result.push_back(stop_times_[index]);
            }
        }
        return result;
    }
    
    // Calendar operations
    void add_calendar(const Calendar& calendar) { calendars_[calendar.service_id] = calendar; }
    const Calendar* get_calendar(const std::string& service_id) const {
        auto it = calendars_.find(service_id);
        return it != calendars_.end() ? &it->second : nullptr;
    }
    const std::unordered_map<std::string, Calendar>& get_calendars() const { return calendars_; }
    
    // Calendar date operations
    void add_calendar_date(const CalendarDate& calendar_date) { calendar_dates_.push_back(calendar_date); }
    const std::vector<CalendarDate>& get_calendar_dates() const { return calendar_dates_; }
    
    // Shape operations
    void add_shape_point(const ShapePoint& shape_point) {
        shapes_.push_back(shape_point);
        shape_points_index_[shape_point.shape_id].push_back(shapes_.size() - 1);
    }
    const std::vector<ShapePoint>& get_shapes() const { return shapes_; }
    std::vector<ShapePoint> get_shape_points(const std::string& shape_id) const {
        std::vector<ShapePoint> result;
        auto it = shape_points_index_.find(shape_id);
        if (it != shape_points_index_.end()) {
            for (size_t index : it->second) {
                result.push_back(shapes_[index]);
            }
        }
        return result;
    }
    
    // Feed info operations
    void set_feed_info(const FeedInfo& feed_info) { feed_info_ = feed_info; }
    const FeedInfo& get_feed_info() const { return feed_info_; }
    
    // Statistics
    size_t get_agency_count() const { return agencies_.size(); }
    size_t get_stop_count() const { return stops_.size(); }
    size_t get_route_count() const { return routes_.size(); }
    size_t get_trip_count() const { return trips_.size(); }
    size_t get_stop_time_count() const { return stop_times_.size(); }
    size_t get_calendar_count() const { return calendars_.size(); }
    size_t get_calendar_date_count() const { return calendar_dates_.size(); }
    size_t get_shape_point_count() const { return shapes_.size(); }
    
    // Validation
    bool validate() const {
        // Basic validation checks
        if (agencies_.empty()) return false;
        if (stops_.empty()) return false;
        if (routes_.empty()) return false;
        if (trips_.empty()) return false;
        if (stop_times_.empty()) return false;
        
        // Check referential integrity
        for (const auto& [route_id, route] : routes_) {
            if (agencies_.find(route.agency_id) == agencies_.end()) {
                return false; // Route references non-existent agency
            }
        }
        
        for (const auto& [trip_id, trip] : trips_) {
            if (routes_.find(trip.route_id) == routes_.end()) {
                return false; // Trip references non-existent route
            }
        }
        
        for (const auto& stop_time : stop_times_) {
            if (trips_.find(stop_time.trip_id) == trips_.end()) {
                return false; // Stop time references non-existent trip
            }
            if (stops_.find(stop_time.stop_id) == stops_.end()) {
                return false; // Stop time references non-existent stop
            }
        }
        
        return true;
    }
    
    // Clear all data
    void clear() {
        agencies_.clear();
        stops_.clear();
        routes_.clear();
        trips_.clear();
        stop_times_.clear();
        calendars_.clear();
        calendar_dates_.clear();
        shapes_.clear();
        trip_stop_times_index_.clear();
        shape_points_index_.clear();
        route_trips_index_.clear();
        feed_info_ = FeedInfo();
    }
};

} // namespace rts::core::io