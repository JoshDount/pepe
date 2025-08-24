#pragma once

#include "GTFSData.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <functional>

namespace rts::core::io {

/**
 * @brief CSV Parser utility class
 */
class CSVParser {
private:
    std::vector<std::string> headers_;
    std::unordered_map<std::string, size_t> header_index_;
    
public:
    /**
     * @brief Parse CSV header line
     */
    bool parse_headers(const std::string& header_line) {
        headers_.clear();
        header_index_.clear();
        
        std::stringstream ss(header_line);
        std::string field;
        size_t index = 0;
        
        while (std::getline(ss, field, ',')) {
            // Trim whitespace and quotes
            field = trim(field);
            if (field.front() == '"' && field.back() == '"') {
                field = field.substr(1, field.length() - 2);
            }
            
            headers_.push_back(field);
            header_index_[field] = index++;
        }
        
        return !headers_.empty();
    }
    
    /**
     * @brief Parse CSV data line
     */
    std::unordered_map<std::string, std::string> parse_line(const std::string& line) const {
        std::unordered_map<std::string, std::string> result;
        std::stringstream ss(line);
        std::string field;
        size_t index = 0;
        
        while (std::getline(ss, field, ',') && index < headers_.size()) {
            // Handle quoted fields
            if (!field.empty() && field.front() == '"') {
                // Field starts with quote, need to handle multi-line or embedded commas
                std::string full_field = field;
                while (full_field.back() != '"' || full_field.length() == 1) {
                    if (std::getline(ss, field, ',')) {
                        full_field += "," + field;
                    } else {
                        break;
                    }
                }
                field = full_field;
                
                // Remove quotes
                if (field.front() == '"' && field.back() == '"') {
                    field = field.substr(1, field.length() - 2);
                }
                
                // Handle escaped quotes
                size_t pos = 0;
                while ((pos = field.find("\"\"", pos)) != std::string::npos) {
                    field.replace(pos, 2, "\"");
                    pos += 1;
                }
            }
            
            field = trim(field);
            result[headers_[index]] = field;
            index++;
        }
        
        return result;
    }
    
    /**
     * @brief Get field value with default
     */
    template<typename T>
    T get_field(const std::unordered_map<std::string, std::string>& fields, 
                const std::string& field_name, const T& default_value) const {
        auto it = fields.find(field_name);
        if (it != fields.end() && !it->second.empty()) {
            return convert_field<T>(it->second);
        }
        return default_value;
    }
    
    const std::vector<std::string>& get_headers() const { return headers_; }
    
private:
    std::string trim(const std::string& str) const {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    template<typename T>
    T convert_field(const std::string& value) const;
};

// Template specializations for field conversion
template<>
inline std::string CSVParser::convert_field<std::string>(const std::string& value) const {
    return value;
}

template<>
inline int CSVParser::convert_field<int>(const std::string& value) const {
    return value.empty() ? 0 : std::stoi(value);
}

template<>
inline double CSVParser::convert_field<double>(const std::string& value) const {
    return value.empty() ? 0.0 : std::stod(value);
}

template<>
inline uint32_t CSVParser::convert_field<uint32_t>(const std::string& value) const {
    return value.empty() ? 0u : static_cast<uint32_t>(std::stoul(value));
}

template<>
inline bool CSVParser::convert_field<bool>(const std::string& value) const {
    return value == "1" || value == "true" || value == "True" || value == "TRUE";
}

template<>
inline RouteType CSVParser::convert_field<RouteType>(const std::string& value) const {
    int route_type = value.empty() ? 3 : std::stoi(value);
    switch (route_type) {
        case 0: return RouteType::TRAM;
        case 1: return RouteType::SUBWAY;
        case 2: return RouteType::RAIL;
        case 3: return RouteType::BUS;
        case 4: return RouteType::FERRY;
        case 5: return RouteType::CABLE_TRAM;
        case 6: return RouteType::AERIAL_LIFT;
        case 7: return RouteType::FUNICULAR;
        case 11: return RouteType::TROLLEYBUS;
        case 12: return RouteType::MONORAIL;
        default: return RouteType::BUS;
    }
}

template<>
inline TripDirection CSVParser::convert_field<TripDirection>(const std::string& value) const {
    return (value == "1") ? TripDirection::INBOUND : TripDirection::OUTBOUND;
}

template<>
inline PickupDropoffType CSVParser::convert_field<PickupDropoffType>(const std::string& value) const {
    int type = value.empty() ? 0 : std::stoi(value);
    switch (type) {
        case 0: return PickupDropoffType::REGULARLY_SCHEDULED;
        case 1: return PickupDropoffType::NO_PICKUP_DROPOFF;
        case 2: return PickupDropoffType::PHONE_AGENCY;
        case 3: return PickupDropoffType::COORDINATE_WITH_DRIVER;
        default: return PickupDropoffType::REGULARLY_SCHEDULED;
    }
}

/**
 * @brief GTFS Parser for reading GTFS feeds from files or directories
 */
class GTFSParser {
private:
    CSVParser csv_parser_;
    
public:
    /**
     * @brief Parse result structure
     */
    struct ParseResult {
        bool success;
        std::string error_message;
        size_t lines_processed;
        size_t lines_skipped;
        double parse_time_ms;
        
        ParseResult() : success(false), lines_processed(0), lines_skipped(0), parse_time_ms(0.0) {}
    };
    
    /**
     * @brief Parse GTFS feed from directory
     */
    ParseResult parse_gtfs_directory(const std::string& directory_path, GTFSDataset& dataset) {
        ParseResult result;
        
        if (!std::filesystem::exists(directory_path) || !std::filesystem::is_directory(directory_path)) {
            result.error_message = "Directory does not exist: " + directory_path;
            return result;
        }
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        try {
            // Clear existing data
            dataset.clear();
            
            // Parse required files
            auto agency_result = parse_agencies(directory_path + "/agency.txt", dataset);
            auto stops_result = parse_stops(directory_path + "/stops.txt", dataset);
            auto routes_result = parse_routes(directory_path + "/routes.txt", dataset);
            auto trips_result = parse_trips(directory_path + "/trips.txt", dataset);
            auto stop_times_result = parse_stop_times(directory_path + "/stop_times.txt", dataset);
            
            result.lines_processed += agency_result.lines_processed + stops_result.lines_processed +
                                    routes_result.lines_processed + trips_result.lines_processed +
                                    stop_times_result.lines_processed;
            
            result.lines_skipped += agency_result.lines_skipped + stops_result.lines_skipped +
                                  routes_result.lines_skipped + trips_result.lines_skipped +
                                  stop_times_result.lines_skipped;
            
            // Parse optional files
            parse_calendar(directory_path + "/calendar.txt", dataset);
            parse_calendar_dates(directory_path + "/calendar_dates.txt", dataset);
            parse_shapes(directory_path + "/shapes.txt", dataset);
            parse_feed_info(directory_path + "/feed_info.txt", dataset);
            
            // Validate dataset
            if (!dataset.validate()) {
                result.error_message = "Dataset validation failed";
                return result;
            }
            
            result.success = true;
            
        } catch (const std::exception& e) {
            result.error_message = std::string("Parsing error: ") + e.what();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        result.parse_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        
        return result;
    }
    
    /**
     * @brief Parse agencies.txt
     */
    ParseResult parse_agencies(const std::string& file_path, GTFSDataset& dataset) {
        return parse_file<Agency>(file_path, [&](const std::unordered_map<std::string, std::string>& fields) {
            Agency agency;
            agency.agency_id = csv_parser_.get_field(fields, "agency_id", std::string(""));
            agency.agency_name = csv_parser_.get_field(fields, "agency_name", std::string(""));
            agency.agency_url = csv_parser_.get_field(fields, "agency_url", std::string(""));
            agency.agency_timezone = csv_parser_.get_field(fields, "agency_timezone", std::string(""));
            agency.agency_lang = csv_parser_.get_field(fields, "agency_lang", std::string(""));
            agency.agency_phone = csv_parser_.get_field(fields, "agency_phone", std::string(""));
            agency.agency_fare_url = csv_parser_.get_field(fields, "agency_fare_url", std::string(""));
            agency.agency_email = csv_parser_.get_field(fields, "agency_email", std::string(""));
            
            dataset.add_agency(agency);
            return true;
        });
    }
    
    /**
     * @brief Parse stops.txt
     */
    ParseResult parse_stops(const std::string& file_path, GTFSDataset& dataset) {
        return parse_file<Stop>(file_path, [&](const std::unordered_map<std::string, std::string>& fields) {
            Stop stop;
            stop.stop_id = csv_parser_.get_field(fields, "stop_id", std::string(""));
            stop.stop_code = csv_parser_.get_field(fields, "stop_code", std::string(""));
            stop.stop_name = csv_parser_.get_field(fields, "stop_name", std::string(""));
            stop.stop_desc = csv_parser_.get_field(fields, "stop_desc", std::string(""));
            stop.stop_lat = csv_parser_.get_field(fields, "stop_lat", 0.0);
            stop.stop_lon = csv_parser_.get_field(fields, "stop_lon", 0.0);
            stop.zone_id = csv_parser_.get_field(fields, "zone_id", std::string(""));
            stop.stop_url = csv_parser_.get_field(fields, "stop_url", std::string(""));
            stop.location_type = csv_parser_.get_field(fields, "location_type", 0);
            stop.parent_station = csv_parser_.get_field(fields, "parent_station", std::string(""));
            stop.stop_timezone = csv_parser_.get_field(fields, "stop_timezone", std::string(""));
            stop.wheelchair_boarding = csv_parser_.get_field(fields, "wheelchair_boarding", 0);
            stop.level_id = csv_parser_.get_field(fields, "level_id", std::string(""));
            stop.platform_code = csv_parser_.get_field(fields, "platform_code", std::string(""));
            
            dataset.add_stop(stop);
            return true;
        });
    }
    
    /**
     * @brief Parse routes.txt
     */
    ParseResult parse_routes(const std::string& file_path, GTFSDataset& dataset) {
        return parse_file<Route>(file_path, [&](const std::unordered_map<std::string, std::string>& fields) {
            Route route;
            route.route_id = csv_parser_.get_field(fields, "route_id", std::string(""));
            route.agency_id = csv_parser_.get_field(fields, "agency_id", std::string(""));
            route.route_short_name = csv_parser_.get_field(fields, "route_short_name", std::string(""));
            route.route_long_name = csv_parser_.get_field(fields, "route_long_name", std::string(""));
            route.route_desc = csv_parser_.get_field(fields, "route_desc", std::string(""));
            route.route_type = csv_parser_.get_field(fields, "route_type", RouteType::BUS);
            route.route_url = csv_parser_.get_field(fields, "route_url", std::string(""));
            route.route_color = csv_parser_.get_field(fields, "route_color", std::string(""));
            route.route_text_color = csv_parser_.get_field(fields, "route_text_color", std::string(""));
            route.route_sort_order = csv_parser_.get_field(fields, "route_sort_order", 0);
            route.continuous_pickup = csv_parser_.get_field(fields, "continuous_pickup", 1);
            route.continuous_drop_off = csv_parser_.get_field(fields, "continuous_drop_off", 1);
            
            dataset.add_route(route);
            return true;
        });
    }
    
    /**
     * @brief Parse trips.txt
     */
    ParseResult parse_trips(const std::string& file_path, GTFSDataset& dataset) {
        return parse_file<Trip>(file_path, [&](const std::unordered_map<std::string, std::string>& fields) {
            Trip trip;
            trip.route_id = csv_parser_.get_field(fields, "route_id", std::string(""));
            trip.service_id = csv_parser_.get_field(fields, "service_id", std::string(""));
            trip.trip_id = csv_parser_.get_field(fields, "trip_id", std::string(""));
            trip.trip_headsign = csv_parser_.get_field(fields, "trip_headsign", std::string(""));
            trip.trip_short_name = csv_parser_.get_field(fields, "trip_short_name", std::string(""));
            trip.direction_id = csv_parser_.get_field(fields, "direction_id", TripDirection::OUTBOUND);
            trip.block_id = csv_parser_.get_field(fields, "block_id", std::string(""));
            trip.shape_id = csv_parser_.get_field(fields, "shape_id", std::string(""));
            trip.wheelchair_accessible = csv_parser_.get_field(fields, "wheelchair_accessible", 0);
            trip.bikes_allowed = csv_parser_.get_field(fields, "bikes_allowed", 0);
            
            dataset.add_trip(trip);
            return true;
        });
    }
    
    /**
     * @brief Parse stop_times.txt
     */
    ParseResult parse_stop_times(const std::string& file_path, GTFSDataset& dataset) {
        return parse_file<StopTime>(file_path, [&](const std::unordered_map<std::string, std::string>& fields) {
            StopTime stop_time;
            stop_time.trip_id = csv_parser_.get_field(fields, "trip_id", std::string(""));
            stop_time.arrival_time = csv_parser_.get_field(fields, "arrival_time", std::string(""));
            stop_time.departure_time = csv_parser_.get_field(fields, "departure_time", std::string(""));
            stop_time.stop_id = csv_parser_.get_field(fields, "stop_id", std::string(""));
            stop_time.stop_sequence = csv_parser_.get_field(fields, "stop_sequence", 0u);
            stop_time.stop_headsign = csv_parser_.get_field(fields, "stop_headsign", std::string(""));
            stop_time.pickup_type = csv_parser_.get_field(fields, "pickup_type", PickupDropoffType::REGULARLY_SCHEDULED);
            stop_time.drop_off_type = csv_parser_.get_field(fields, "drop_off_type", PickupDropoffType::REGULARLY_SCHEDULED);
            stop_time.continuous_pickup = csv_parser_.get_field(fields, "continuous_pickup", 1);
            stop_time.continuous_drop_off = csv_parser_.get_field(fields, "continuous_drop_off", 1);
            stop_time.shape_dist_traveled = csv_parser_.get_field(fields, "shape_dist_traveled", 0.0);
            stop_time.timepoint = csv_parser_.get_field(fields, "timepoint", 1);
            
            dataset.add_stop_time(stop_time);
            return true;
        });
    }
    
    /**
     * @brief Parse calendar.txt (optional)
     */
    ParseResult parse_calendar(const std::string& file_path, GTFSDataset& dataset) {
        if (!std::filesystem::exists(file_path)) {
            ParseResult result;
            result.success = true; // Optional file
            return result;
        }
        
        return parse_file<Calendar>(file_path, [&](const std::unordered_map<std::string, std::string>& fields) {
            Calendar calendar;
            calendar.service_id = csv_parser_.get_field(fields, "service_id", std::string(""));
            calendar.monday = csv_parser_.get_field(fields, "monday", false);
            calendar.tuesday = csv_parser_.get_field(fields, "tuesday", false);
            calendar.wednesday = csv_parser_.get_field(fields, "wednesday", false);
            calendar.thursday = csv_parser_.get_field(fields, "thursday", false);
            calendar.friday = csv_parser_.get_field(fields, "friday", false);
            calendar.saturday = csv_parser_.get_field(fields, "saturday", false);
            calendar.sunday = csv_parser_.get_field(fields, "sunday", false);
            calendar.start_date = csv_parser_.get_field(fields, "start_date", std::string(""));
            calendar.end_date = csv_parser_.get_field(fields, "end_date", std::string(""));
            
            dataset.add_calendar(calendar);
            return true;
        });
    }
    
    /**
     * @brief Parse calendar_dates.txt (optional)
     */
    ParseResult parse_calendar_dates(const std::string& file_path, GTFSDataset& dataset) {
        if (!std::filesystem::exists(file_path)) {
            ParseResult result;
            result.success = true; // Optional file
            return result;
        }
        
        return parse_file<CalendarDate>(file_path, [&](const std::unordered_map<std::string, std::string>& fields) {
            CalendarDate calendar_date;
            calendar_date.service_id = csv_parser_.get_field(fields, "service_id", std::string(""));
            calendar_date.date = csv_parser_.get_field(fields, "date", std::string(""));
            calendar_date.exception_type = csv_parser_.get_field(fields, "exception_type", 1);
            
            dataset.add_calendar_date(calendar_date);
            return true;
        });
    }
    
    /**
     * @brief Parse shapes.txt (optional)
     */
    ParseResult parse_shapes(const std::string& file_path, GTFSDataset& dataset) {
        if (!std::filesystem::exists(file_path)) {
            ParseResult result;
            result.success = true; // Optional file
            return result;
        }
        
        return parse_file<ShapePoint>(file_path, [&](const std::unordered_map<std::string, std::string>& fields) {
            ShapePoint shape_point;
            shape_point.shape_id = csv_parser_.get_field(fields, "shape_id", std::string(""));
            shape_point.shape_pt_lat = csv_parser_.get_field(fields, "shape_pt_lat", 0.0);
            shape_point.shape_pt_lon = csv_parser_.get_field(fields, "shape_pt_lon", 0.0);
            shape_point.shape_pt_sequence = csv_parser_.get_field(fields, "shape_pt_sequence", 0u);
            shape_point.shape_dist_traveled = csv_parser_.get_field(fields, "shape_dist_traveled", 0.0);
            
            dataset.add_shape_point(shape_point);
            return true;
        });
    }
    
    /**
     * @brief Parse feed_info.txt (optional)
     */
    ParseResult parse_feed_info(const std::string& file_path, GTFSDataset& dataset) {
        if (!std::filesystem::exists(file_path)) {
            ParseResult result;
            result.success = true; // Optional file
            return result;
        }
        
        std::ifstream file(file_path);
        if (!file.is_open()) {
            ParseResult result;
            result.error_message = "Cannot open file: " + file_path;
            return result;
        }
        
        std::string line;
        if (std::getline(file, line)) {
            csv_parser_.parse_headers(line);
        }
        
        if (std::getline(file, line)) {
            auto fields = csv_parser_.parse_line(line);
            FeedInfo feed_info;
            feed_info.feed_publisher_name = csv_parser_.get_field(fields, "feed_publisher_name", std::string(""));
            feed_info.feed_publisher_url = csv_parser_.get_field(fields, "feed_publisher_url", std::string(""));
            feed_info.feed_lang = csv_parser_.get_field(fields, "feed_lang", std::string(""));
            feed_info.default_lang = csv_parser_.get_field(fields, "default_lang", std::string(""));
            feed_info.feed_start_date = csv_parser_.get_field(fields, "feed_start_date", std::string(""));
            feed_info.feed_end_date = csv_parser_.get_field(fields, "feed_end_date", std::string(""));
            feed_info.feed_version = csv_parser_.get_field(fields, "feed_version", std::string(""));
            feed_info.feed_contact_email = csv_parser_.get_field(fields, "feed_contact_email", std::string(""));
            feed_info.feed_contact_url = csv_parser_.get_field(fields, "feed_contact_url", std::string(""));
            
            dataset.set_feed_info(feed_info);
        }
        
        ParseResult result;
        result.success = true;
        result.lines_processed = 1;
        return result;
    }
    
private:
    /**
     * @brief Generic file parser template
     */
    template<typename T>
    ParseResult parse_file(const std::string& file_path, 
                          std::function<bool(const std::unordered_map<std::string, std::string>&)> processor) {
        ParseResult result;
        
        std::ifstream file(file_path);
        if (!file.is_open()) {
            result.error_message = "Cannot open file: " + file_path;
            return result;
        }
        
        std::string line;
        size_t line_number = 0;
        
        // Parse headers
        if (std::getline(file, line)) {
            line_number++;
            if (!csv_parser_.parse_headers(line)) {
                result.error_message = "Failed to parse headers in: " + file_path;
                return result;
            }
        } else {
            result.error_message = "Empty file: " + file_path;
            return result;
        }
        
        // Parse data lines
        while (std::getline(file, line)) {
            line_number++;
            
            // Skip empty lines
            if (line.empty() || std::all_of(line.begin(), line.end(), [](char c) { return std::isspace(c); })) {
                result.lines_skipped++;
                continue;
            }
            
            try {
                auto fields = csv_parser_.parse_line(line);
                if (processor(fields)) {
                    result.lines_processed++;
                } else {
                    result.lines_skipped++;
                }
            } catch (const std::exception& e) {
                result.lines_skipped++;
                // Continue processing other lines
            }
        }
        
        result.success = true;
        return result;
    }
};

} // namespace rts::core::io