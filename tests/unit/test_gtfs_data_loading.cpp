#include "../../core/io/GTFSData.hpp"
#include "../../core/io/GTFSParser.hpp"
#include "../../core/io/GTFSConverter.hpp"
#include "../TestFramework.hpp"
#include <filesystem>
#include <fstream>

void test_gtfs_data_loading() {
    using namespace rts::core::io;
    using namespace rts::core::graph;
    
    // Test 1: Basic GTFS data structures
    {
        // Test Agency
        Agency agency("AGENCY1", "Test Transit", "http://test.com", "America/New_York");
        TestFramework::assert_equal_string("AGENCY1", agency.agency_id, "GTFS: Agency ID should match");
        TestFramework::assert_equal_string("Test Transit", agency.agency_name, "GTFS: Agency name should match");
        TestFramework::assert_equal_string("http://test.com", agency.agency_url, "GTFS: Agency URL should match");
        TestFramework::assert_equal_string("America/New_York", agency.agency_timezone, "GTFS: Agency timezone should match");
        
        // Test Stop
        Stop stop("STOP1", "Test Station", 40.7128, -74.0060);
        TestFramework::assert_equal_string("STOP1", stop.stop_id, "GTFS: Stop ID should match");
        TestFramework::assert_equal_string("Test Station", stop.stop_name, "GTFS: Stop name should match");
        TestFramework::assert_equal_double(40.7128, stop.stop_lat, "GTFS: Stop latitude should match");
        TestFramework::assert_equal_double(-74.0060, stop.stop_lon, "GTFS: Stop longitude should match");
        
        // Test Route
        Route route("ROUTE1", "AGENCY1", "R1", "Express Line", RouteType::SUBWAY);
        TestFramework::assert_equal_string("ROUTE1", route.route_id, "GTFS: Route ID should match");
        TestFramework::assert_equal_string("AGENCY1", route.agency_id, "GTFS: Route agency ID should match");
        TestFramework::assert_equal_string("R1", route.route_short_name, "GTFS: Route short name should match");
        TestFramework::assert_equal_string("Express Line", route.route_long_name, "GTFS: Route long name should match");
        TestFramework::assert_equal_int(static_cast<int>(RouteType::SUBWAY), static_cast<int>(route.route_type), "GTFS: Route type should match");
        
        // Test Trip
        Trip trip("ROUTE1", "SERVICE1", "TRIP1", "Downtown");
        TestFramework::assert_equal_string("ROUTE1", trip.route_id, "GTFS: Trip route ID should match");
        TestFramework::assert_equal_string("SERVICE1", trip.service_id, "GTFS: Trip service ID should match");
        TestFramework::assert_equal_string("TRIP1", trip.trip_id, "GTFS: Trip ID should match");
        TestFramework::assert_equal_string("Downtown", trip.trip_headsign, "GTFS: Trip headsign should match");
        
        // Test StopTime
        StopTime stop_time("TRIP1", "08:00:00", "08:00:00", "STOP1", 1);
        TestFramework::assert_equal_string("TRIP1", stop_time.trip_id, "GTFS: StopTime trip ID should match");
        TestFramework::assert_equal_string("08:00:00", stop_time.arrival_time, "GTFS: StopTime arrival time should match");
        TestFramework::assert_equal_string("08:00:00", stop_time.departure_time, "GTFS: StopTime departure time should match");
        TestFramework::assert_equal_string("STOP1", stop_time.stop_id, "GTFS: StopTime stop ID should match");
        TestFramework::assert_equal_uint32_t(1u, stop_time.stop_sequence, "GTFS: StopTime sequence should match");
        
        // Test Calendar
        Calendar calendar("SERVICE1", true, true, true, true, true, false, false, "20240101", "20241231");
        TestFramework::assert_equal_string("SERVICE1", calendar.service_id, "GTFS: Calendar service ID should match");
        TestFramework::assert_true(calendar.monday, "GTFS: Calendar should have Monday service");
        TestFramework::assert_true(calendar.friday, "GTFS: Calendar should have Friday service");
        TestFramework::assert_true(!calendar.saturday, "GTFS: Calendar should not have Saturday service");
        TestFramework::assert_equal_string("20240101", calendar.start_date, "GTFS: Calendar start date should match");
        TestFramework::assert_equal_string("20241231", calendar.end_date, "GTFS: Calendar end date should match");
        
        // Test CalendarDate
        CalendarDate calendar_date("SERVICE1", "20240704", 2);
        TestFramework::assert_equal_string("SERVICE1", calendar_date.service_id, "GTFS: CalendarDate service ID should match");
        TestFramework::assert_equal_string("20240704", calendar_date.date, "GTFS: CalendarDate date should match");
        TestFramework::assert_equal_int(2, calendar_date.exception_type, "GTFS: CalendarDate exception type should match");
        
        // Test ShapePoint
        ShapePoint shape_point("SHAPE1", 40.7128, -74.0060, 1, 0.0);
        TestFramework::assert_equal_string("SHAPE1", shape_point.shape_id, "GTFS: ShapePoint shape ID should match");
        TestFramework::assert_equal_double(40.7128, shape_point.shape_pt_lat, "GTFS: ShapePoint latitude should match");
        TestFramework::assert_equal_double(-74.0060, shape_point.shape_pt_lon, "GTFS: ShapePoint longitude should match");
        TestFramework::assert_equal_uint32_t(1u, shape_point.shape_pt_sequence, "GTFS: ShapePoint sequence should match");
        
        // Test FeedInfo
        FeedInfo feed_info("Test Publisher", "http://test.com", "en", "1.0");
        TestFramework::assert_equal_string("Test Publisher", feed_info.feed_publisher_name, "GTFS: FeedInfo publisher name should match");
        TestFramework::assert_equal_string("http://test.com", feed_info.feed_publisher_url, "GTFS: FeedInfo publisher URL should match");
        TestFramework::assert_equal_string("en", feed_info.feed_lang, "GTFS: FeedInfo language should match");
        TestFramework::assert_equal_string("1.0", feed_info.feed_version, "GTFS: FeedInfo version should match");
    }
    
    // Test 2: GTFSDataset basic operations
    {
        GTFSDataset dataset;
        
        // Add data
        Agency agency("AGENCY1", "Test Transit", "http://test.com", "America/New_York");
        dataset.add_agency(agency);
        
        Stop stop1("STOP1", "Station A", 40.7128, -74.0060);
        Stop stop2("STOP2", "Station B", 40.7589, -73.9851);
        dataset.add_stop(stop1);
        dataset.add_stop(stop2);
        
        Route route("ROUTE1", "AGENCY1", "R1", "Express Line", RouteType::SUBWAY);
        dataset.add_route(route);
        
        Trip trip("ROUTE1", "SERVICE1", "TRIP1", "Downtown");
        dataset.add_trip(trip);
        
        StopTime stop_time1("TRIP1", "08:00:00", "08:00:00", "STOP1", 1);
        StopTime stop_time2("TRIP1", "08:05:00", "08:05:00", "STOP2", 2);
        dataset.add_stop_time(stop_time1);
        dataset.add_stop_time(stop_time2);
        
        Calendar calendar("SERVICE1", true, true, true, true, true, false, false, "20240101", "20241231");
        dataset.add_calendar(calendar);
        
        // Test retrieval
        const Agency* retrieved_agency = dataset.get_agency("AGENCY1");
        TestFramework::assert_true(retrieved_agency != nullptr, "GTFS: Should retrieve agency");
        TestFramework::assert_equal_string("Test Transit", retrieved_agency->agency_name, "GTFS: Retrieved agency name should match");
        
        const Stop* retrieved_stop = dataset.get_stop("STOP1");
        TestFramework::assert_true(retrieved_stop != nullptr, "GTFS: Should retrieve stop");
        TestFramework::assert_equal_string("Station A", retrieved_stop->stop_name, "GTFS: Retrieved stop name should match");
        
        const Route* retrieved_route = dataset.get_route("ROUTE1");
        TestFramework::assert_true(retrieved_route != nullptr, "GTFS: Should retrieve route");
        TestFramework::assert_equal_string("Express Line", retrieved_route->route_long_name, "GTFS: Retrieved route name should match");
        
        const Trip* retrieved_trip = dataset.get_trip("TRIP1");
        TestFramework::assert_true(retrieved_trip != nullptr, "GTFS: Should retrieve trip");
        TestFramework::assert_equal_string("Downtown", retrieved_trip->trip_headsign, "GTFS: Retrieved trip headsign should match");
        
        auto trip_stop_times = dataset.get_stop_times_for_trip("TRIP1");
        TestFramework::assert_equal_size_t(size_t(2), trip_stop_times.size(), "GTFS: Should have 2 stop times for trip");
        TestFramework::assert_equal_string("STOP1", trip_stop_times[0].stop_id, "GTFS: First stop time should be for STOP1");
        TestFramework::assert_equal_string("STOP2", trip_stop_times[1].stop_id, "GTFS: Second stop time should be for STOP2");
        
        auto route_trips = dataset.get_trips_for_route("ROUTE1");
        TestFramework::assert_equal_size_t(size_t(1), route_trips.size(), "GTFS: Should have 1 trip for route");
        TestFramework::assert_equal_string("TRIP1", route_trips[0], "GTFS: Route trip should be TRIP1");
        
        // Test statistics
        TestFramework::assert_equal_size_t(size_t(1), dataset.get_agency_count(), "GTFS: Should have 1 agency");
        TestFramework::assert_equal_size_t(size_t(2), dataset.get_stop_count(), "GTFS: Should have 2 stops");
        TestFramework::assert_equal_size_t(size_t(1), dataset.get_route_count(), "GTFS: Should have 1 route");
        TestFramework::assert_equal_size_t(size_t(1), dataset.get_trip_count(), "GTFS: Should have 1 trip");
        TestFramework::assert_equal_size_t(size_t(2), dataset.get_stop_time_count(), "GTFS: Should have 2 stop times");
        TestFramework::assert_equal_size_t(size_t(1), dataset.get_calendar_count(), "GTFS: Should have 1 calendar");
        
        // Test validation
        TestFramework::assert_true(dataset.validate(), "GTFS: Dataset should be valid");
        
        // Test clear
        dataset.clear();
        TestFramework::assert_equal_size_t(size_t(0), dataset.get_agency_count(), "GTFS: Should have 0 agencies after clear");
        TestFramework::assert_equal_size_t(size_t(0), dataset.get_stop_count(), "GTFS: Should have 0 stops after clear");
    }
    
    // Test 3: CSV Parser functionality
    {
        CSVParser parser;
        
        // Test header parsing
        std::string header_line = "stop_id,stop_name,stop_lat,stop_lon";
        TestFramework::assert_true(parser.parse_headers(header_line), "CSV: Should parse headers successfully");
        
        auto headers = parser.get_headers();
        TestFramework::assert_equal_size_t(size_t(4), headers.size(), "CSV: Should have 4 headers");
        TestFramework::assert_equal_string("stop_id", headers[0], "CSV: First header should be stop_id");
        TestFramework::assert_equal_string("stop_name", headers[1], "CSV: Second header should be stop_name");
        TestFramework::assert_equal_string("stop_lat", headers[2], "CSV: Third header should be stop_lat");
        TestFramework::assert_equal_string("stop_lon", headers[3], "CSV: Fourth header should be stop_lon");
        
        // Test line parsing
        std::string data_line = "STOP1,\"Test Station\",40.7128,-74.0060";
        auto fields = parser.parse_line(data_line);
        TestFramework::assert_equal_size_t(size_t(4), fields.size(), "CSV: Should parse 4 fields");
        TestFramework::assert_equal_string("STOP1", fields["stop_id"], "CSV: stop_id field should match");
        TestFramework::assert_equal_string("Test Station", fields["stop_name"], "CSV: stop_name field should match");
        TestFramework::assert_equal_string("40.7128", fields["stop_lat"], "CSV: stop_lat field should match");
        TestFramework::assert_equal_string("-74.0060", fields["stop_lon"], "CSV: stop_lon field should match");
        
        // Test field conversion
        TestFramework::assert_equal_string("STOP1", parser.get_field(fields, "stop_id", std::string("")), "CSV: String field conversion should work");
        TestFramework::assert_equal_double(40.7128, parser.get_field(fields, "stop_lat", 0.0), "CSV: Double field conversion should work");
        TestFramework::assert_equal_int(1, parser.get_field(fields, "nonexistent", 1), "CSV: Default value should be returned for missing field");
    }
    
    // Test 4: GTFS file parsing
    {
        // Create temporary test directory
        std::string test_dir = "./test_gtfs_temp";
        std::filesystem::create_directories(test_dir);
        
        // Generate sample GTFS files
        TestFramework::assert_true(GTFSGraphConverter::generate_sample_gtfs_files(test_dir), "GTFS: Should generate sample files");
        
        // Test file existence
        TestFramework::assert_true(std::filesystem::exists(test_dir + "/agency.txt"), "GTFS: agency.txt should exist");
        TestFramework::assert_true(std::filesystem::exists(test_dir + "/stops.txt"), "GTFS: stops.txt should exist");
        TestFramework::assert_true(std::filesystem::exists(test_dir + "/routes.txt"), "GTFS: routes.txt should exist");
        TestFramework::assert_true(std::filesystem::exists(test_dir + "/trips.txt"), "GTFS: trips.txt should exist");
        TestFramework::assert_true(std::filesystem::exists(test_dir + "/stop_times.txt"), "GTFS: stop_times.txt should exist");
        TestFramework::assert_true(std::filesystem::exists(test_dir + "/calendar.txt"), "GTFS: calendar.txt should exist");
        
        // Parse GTFS directory
        GTFSParser parser;
        GTFSDataset dataset;
        auto result = parser.parse_gtfs_directory(test_dir, dataset);
        
        TestFramework::assert_true(result.success, "GTFS: Parsing should succeed");
        TestFramework::assert_true(result.error_message.empty(), "GTFS: Should have no error message");
        TestFramework::assert_true(result.lines_processed > 0, "GTFS: Should process some lines");
        TestFramework::assert_true(result.parse_time_ms >= 0.0, "GTFS: Parse time should be non-negative");
        
        // Verify parsed data
        TestFramework::assert_equal_size_t(size_t(1), dataset.get_agency_count(), "GTFS: Should parse 1 agency");
        TestFramework::assert_equal_size_t(size_t(4), dataset.get_stop_count(), "GTFS: Should parse 4 stops");
        TestFramework::assert_equal_size_t(size_t(2), dataset.get_route_count(), "GTFS: Should parse 2 routes");
        TestFramework::assert_equal_size_t(size_t(3), dataset.get_trip_count(), "GTFS: Should parse 3 trips");
        TestFramework::assert_equal_size_t(size_t(8), dataset.get_stop_time_count(), "GTFS: Should parse 8 stop times");
        TestFramework::assert_equal_size_t(size_t(1), dataset.get_calendar_count(), "GTFS: Should parse 1 calendar");
        
        // Test specific data
        const Agency* agency = dataset.get_agency("SAMPLE");
        TestFramework::assert_true(agency != nullptr, "GTFS: Should find SAMPLE agency");
        TestFramework::assert_equal_string("Sample Transit Agency", agency->agency_name, "GTFS: Agency name should match");
        
        const Stop* stop = dataset.get_stop("STOP1");
        TestFramework::assert_true(stop != nullptr, "GTFS: Should find STOP1");
        TestFramework::assert_equal_string("Downtown Station", stop->stop_name, "GTFS: Stop name should match");
        
        const Route* route = dataset.get_route("ROUTE1");
        TestFramework::assert_true(route != nullptr, "GTFS: Should find ROUTE1");
        TestFramework::assert_equal_string("Downtown Express", route->route_long_name, "GTFS: Route name should match");
        TestFramework::assert_equal_int(static_cast<int>(RouteType::SUBWAY), static_cast<int>(route->route_type), "GTFS: Route type should be subway");
        
        // Cleanup
        std::filesystem::remove_all(test_dir);
    }
    
    // Test 5: GTFS to Graph conversion
    {
        // Create sample dataset
        auto dataset = GTFSGraphConverter::create_sample_dataset();
        
        // Test dataset validity
        TestFramework::assert_true(dataset.validate(), "GTFS: Sample dataset should be valid");
        TestFramework::assert_equal_size_t(size_t(1), dataset.get_agency_count(), "GTFS: Sample should have 1 agency");
        TestFramework::assert_equal_size_t(size_t(4), dataset.get_stop_count(), "GTFS: Sample should have 4 stops");
        TestFramework::assert_equal_size_t(size_t(1), dataset.get_route_count(), "GTFS: Sample should have 1 route");
        TestFramework::assert_equal_size_t(size_t(2), dataset.get_trip_count(), "GTFS: Sample should have 2 trips");
        
        // Convert to graph
        GTFSGraphConverter::ConversionOptions options;
        options.strategy = GTFSGraphConverter::ConversionStrategy::STOP_TO_STOP;
        options.include_walking_edges = true;
        options.max_transfer_distance_m = 10000.0; // Increase to 10km for NYC stops
        
        GTFSGraphConverter converter(options);
        WeightedGraph graph;
        auto conv_result = converter.convert(dataset, graph);
        
        TestFramework::assert_true(conv_result.success, "GTFS: Conversion should succeed");
        TestFramework::assert_true(conv_result.error_message.empty(), "GTFS: Should have no conversion error");
        TestFramework::assert_true(conv_result.nodes_created > 0, "GTFS: Should create nodes");
        TestFramework::assert_true(conv_result.edges_created > 0, "GTFS: Should create edges");
        TestFramework::assert_equal_size_t(size_t(4), conv_result.unique_stops, "GTFS: Should have 4 unique stops");
        TestFramework::assert_equal_size_t(size_t(1), conv_result.unique_routes, "GTFS: Should have 1 unique route");
        TestFramework::assert_equal_size_t(size_t(2), conv_result.unique_trips, "GTFS: Should have 2 unique trips");
        TestFramework::assert_true(conv_result.conversion_time_ms >= 0.0, "GTFS: Conversion time should be non-negative");
        
        // Verify graph structure
        TestFramework::assert_equal_size_t(conv_result.nodes_created, graph.num_nodes(), "GTFS: Graph should have correct number of nodes");
        // Note: For undirected graphs, the WeightedGraph.num_edges() counts directed edges,
        // but the converter counts each bidirectional edge as 2 edges
        TestFramework::assert_true(conv_result.edges_created >= graph.num_edges(), "GTFS: Conversion should create at least as many edges as graph reports");
        
        // Test that both transit and transfer edges were created
        TestFramework::assert_true(conv_result.transit_edges > 0, "GTFS: Should create transit edges");
        TestFramework::assert_true(conv_result.transfer_edges > 0, "GTFS: Should create transfer edges");
        TestFramework::assert_equal_size_t(conv_result.transit_edges + conv_result.transfer_edges, conv_result.edges_created, "GTFS: Edge counts should add up");
    }
    
    // Test 6: Error handling and edge cases
    {
        GTFSParser parser;
        GTFSDataset dataset;
        
        // Test non-existent directory
        auto result = parser.parse_gtfs_directory("./nonexistent_directory", dataset);
        TestFramework::assert_true(!result.success, "GTFS: Should fail for non-existent directory");
        TestFramework::assert_true(!result.error_message.empty(), "GTFS: Should have error message for non-existent directory");
        
        // Test empty dataset validation
        GTFSDataset empty_dataset;
        TestFramework::assert_true(!empty_dataset.validate(), "GTFS: Empty dataset should not be valid");
        
        // Test dataset with missing references
        GTFSDataset invalid_dataset;
        Agency agency("AGENCY1", "Test", "http://test.com", "UTC");
        Route route("ROUTE1", "MISSING_AGENCY", "R1", "Test Route", RouteType::BUS);
        invalid_dataset.add_agency(agency);
        invalid_dataset.add_route(route);
        TestFramework::assert_true(!invalid_dataset.validate(), "GTFS: Dataset with missing agency reference should not be valid");
        
        // Test conversion with empty dataset
        GTFSGraphConverter converter;
        WeightedGraph empty_graph;
        auto conv_result = converter.convert(empty_dataset, empty_graph);
        TestFramework::assert_true(!conv_result.success, "GTFS: Conversion should fail for invalid dataset");
        TestFramework::assert_true(!conv_result.error_message.empty(), "GTFS: Should have conversion error message");
    }
    
    // Test 7: Performance and large dataset handling
    {
        // Create a larger dataset for performance testing
        GTFSDataset large_dataset;
        
        // Add agency
        Agency agency("LARGE_AGENCY", "Large Transit System", "http://large.transit.com", "America/New_York");
        large_dataset.add_agency(agency);
        
        // Add many stops
        const size_t num_stops = 100;
        for (size_t i = 0; i < num_stops; ++i) {
            std::string stop_id = "STOP" + std::to_string(i);
            std::string stop_name = "Station " + std::to_string(i);
            double lat = 40.7 + (i % 10) * 0.01;
            double lon = -74.0 + (i / 10) * 0.01;
            large_dataset.add_stop(Stop(stop_id, stop_name, lat, lon));
        }
        
        // Add route
        Route route("LARGE_ROUTE", "LARGE_AGENCY", "LR", "Large Route", RouteType::BUS);
        large_dataset.add_route(route);
        
        // Add trip
        Trip trip("LARGE_ROUTE", "SERVICE1", "LARGE_TRIP", "Express");
        large_dataset.add_trip(trip);
        
        // Add stop times for the trip
        for (size_t i = 0; i < num_stops; ++i) {
            std::string stop_id = "STOP" + std::to_string(i);
            int hours = 8 + (i / 60);
            int minutes = i % 60;
            std::string time = (hours < 10 ? "0" : "") + std::to_string(hours) + ":" +
                              (minutes < 10 ? "0" : "") + std::to_string(minutes) + ":00";
            large_dataset.add_stop_time(StopTime("LARGE_TRIP", time, time, stop_id, i + 1));
        }
        
        // Add calendar
        Calendar calendar("SERVICE1", true, true, true, true, true, false, false, "20240101", "20241231");
        large_dataset.add_calendar(calendar);
        
        // Test dataset statistics
        TestFramework::assert_equal_size_t(size_t(1), large_dataset.get_agency_count(), "GTFS: Large dataset should have 1 agency");
        TestFramework::assert_equal_size_t(num_stops, large_dataset.get_stop_count(), "GTFS: Large dataset should have correct number of stops");
        TestFramework::assert_equal_size_t(size_t(1), large_dataset.get_route_count(), "GTFS: Large dataset should have 1 route");
        TestFramework::assert_equal_size_t(size_t(1), large_dataset.get_trip_count(), "GTFS: Large dataset should have 1 trip");
        TestFramework::assert_equal_size_t(num_stops, large_dataset.get_stop_time_count(), "GTFS: Large dataset should have correct number of stop times");
        
        // Test validation performance
        TestFramework::assert_true(large_dataset.validate(), "GTFS: Large dataset should be valid");
        
        // Test conversion performance
        GTFSGraphConverter::ConversionOptions options;
        options.include_walking_edges = false; // Disable for performance
        GTFSGraphConverter converter(options);
        WeightedGraph large_graph;
        
        auto conv_result = converter.convert(large_dataset, large_graph);
        TestFramework::assert_true(conv_result.success, "GTFS: Large dataset conversion should succeed");
        TestFramework::assert_equal_size_t(num_stops, conv_result.nodes_created, "GTFS: Should create correct number of nodes");
        TestFramework::assert_true(conv_result.edges_created > 0, "GTFS: Should create edges for large dataset");
        TestFramework::assert_true(conv_result.conversion_time_ms < 5000.0, "GTFS: Conversion should complete in reasonable time");
    }
}