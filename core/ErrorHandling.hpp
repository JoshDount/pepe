/**
 * Error Handling Framework
 * RouteTransitSimulator - Core Error Handling
 */

#pragma once

#include <stdexcept>
#include <string>
#include <memory>
#include <vector>
#include <chrono>
#include <unordered_map>

namespace rts {
namespace core {

/**
 * @brief Error categories for the RouteTransitSimulator
 */
enum class ErrorCode {
    SUCCESS = 0,
    
    // File I/O Errors (100-199)
    FILE_NOT_FOUND = 100,
    FILE_ACCESS_DENIED = 101,
    FILE_CORRUPTED = 102,
    DIRECTORY_NOT_FOUND = 103,
    DISK_FULL = 104,
    
    // Parsing Errors (200-299)
    INVALID_FORMAT = 200,
    MISSING_REQUIRED_FIELD = 201,
    INVALID_DATA_TYPE = 202,
    GTFS_VALIDATION_FAILED = 203,
    CSV_PARSE_ERROR = 204,
    JSON_PARSE_ERROR = 205,
    
    // Memory Errors (300-399)
    OUT_OF_MEMORY = 300,
    BUFFER_OVERFLOW = 301,
    NULL_POINTER = 302,
    INVALID_MEMORY_ACCESS = 303,
    
    // Algorithm Errors (400-499)
    ALGORITHM_FAILURE = 400,
    INVALID_GRAPH = 401,
    NODE_NOT_FOUND = 402,
    NO_PATH_EXISTS = 403,
    INFINITE_LOOP_DETECTED = 404,
    INVALID_PARAMETERS = 405,
    
    // Network Errors (500-599)
    NETWORK_ERROR = 500,
    CONNECTION_TIMEOUT = 501,
    SERVER_ERROR = 502,
    INVALID_URL = 503,
    
    // System Errors (600-699)
    SYSTEM_ERROR = 600,
    PERMISSION_DENIED = 601,
    RESOURCE_EXHAUSTED = 602,
    OPERATION_TIMEOUT = 603,
    
    // Simulation Errors (700-799)
    SIMULATION_ERROR = 700,
    EVENT_PROCESSING_FAILED = 701,
    TIME_SYNCHRONIZATION_ERROR = 702,
    INVALID_EVENT = 703,
    
    // Unknown Error (999)
    UNKNOWN_ERROR = 999
};

/**
 * @brief Error severity levels
 */
enum class ErrorSeverity {
    DEBUG = 0,
    INFO = 1,
    WARNING = 2,
    ERROR = 3,
    CRITICAL = 4,
    FATAL = 5
};

/**
 * @brief Error context information
 */
struct ErrorContext {
    std::string file_name;
    int line_number;
    std::string function_name;
    std::string additional_info;
    std::chrono::system_clock::time_point timestamp;
    
    ErrorContext() : line_number(0), timestamp(std::chrono::system_clock::now()) {}
    
    ErrorContext(const std::string& file, int line, const std::string& func, const std::string& info = "")
        : file_name(file), line_number(line), function_name(func), 
          additional_info(info), timestamp(std::chrono::system_clock::now()) {}
};

/**
 * @brief Main exception class for RouteTransitSimulator
 */
class RTSException : public std::exception {
private:
    ErrorCode code_;
    ErrorSeverity severity_;
    std::string message_;
    ErrorContext context_;
    std::vector<RTSException> nested_exceptions_;
    
public:
    /**
     * @brief Constructor with full error information
     */
    RTSException(ErrorCode code, const std::string& message, 
                 ErrorSeverity severity = ErrorSeverity::ERROR,
                 const ErrorContext& context = ErrorContext())
        : code_(code), severity_(severity), message_(message), context_(context) {}
    
    /**
     * @brief Constructor with nested exception
     */
    RTSException(ErrorCode code, const std::string& message, 
                 const RTSException& nested_exception,
                 ErrorSeverity severity = ErrorSeverity::ERROR,
                 const ErrorContext& context = ErrorContext())
        : code_(code), severity_(severity), message_(message), context_(context) {
        nested_exceptions_.push_back(nested_exception);
    }
    
    /**
     * @brief Get error message
     */
    const char* what() const noexcept override {
        return message_.c_str();
    }
    
    /**
     * @brief Get error code
     */
    ErrorCode code() const { return code_; }
    
    /**
     * @brief Get error severity
     */
    ErrorSeverity severity() const { return severity_; }
    
    /**
     * @brief Get error context
     */
    const ErrorContext& context() const { return context_; }
    
    /**
     * @brief Get nested exceptions
     */
    const std::vector<RTSException>& nested_exceptions() const { return nested_exceptions_; }
    
    /**
     * @brief Add nested exception
     */
    void add_nested_exception(const RTSException& exception) {
        nested_exceptions_.push_back(exception);
    }
    
    /**
     * @brief Get formatted error message with context
     */
    std::string get_detailed_message() const {
        std::string detailed = "[" + error_code_to_string(code_) + "] " + message_;
        
        if (!context_.file_name.empty()) {
            detailed += "\n  File: " + context_.file_name;
            if (context_.line_number > 0) {
                detailed += ":" + std::to_string(context_.line_number);
            }
            if (!context_.function_name.empty()) {
                detailed += " in " + context_.function_name + "()";
            }
        }
        
        if (!context_.additional_info.empty()) {
            detailed += "\n  Info: " + context_.additional_info;
        }
        
        // Add nested exceptions
        for (size_t i = 0; i < nested_exceptions_.size(); ++i) {
            detailed += "\n  Caused by: " + nested_exceptions_[i].get_detailed_message();
        }
        
        return detailed;
    }
    
    /**
     * @brief Convert error code to string
     */
    static std::string error_code_to_string(ErrorCode code) {
        static const std::unordered_map<ErrorCode, std::string> code_map = {
            {ErrorCode::SUCCESS, "SUCCESS"},
            {ErrorCode::FILE_NOT_FOUND, "FILE_NOT_FOUND"},
            {ErrorCode::FILE_ACCESS_DENIED, "FILE_ACCESS_DENIED"},
            {ErrorCode::FILE_CORRUPTED, "FILE_CORRUPTED"},
            {ErrorCode::DIRECTORY_NOT_FOUND, "DIRECTORY_NOT_FOUND"},
            {ErrorCode::DISK_FULL, "DISK_FULL"},
            {ErrorCode::INVALID_FORMAT, "INVALID_FORMAT"},
            {ErrorCode::MISSING_REQUIRED_FIELD, "MISSING_REQUIRED_FIELD"},
            {ErrorCode::INVALID_DATA_TYPE, "INVALID_DATA_TYPE"},
            {ErrorCode::GTFS_VALIDATION_FAILED, "GTFS_VALIDATION_FAILED"},
            {ErrorCode::CSV_PARSE_ERROR, "CSV_PARSE_ERROR"},
            {ErrorCode::JSON_PARSE_ERROR, "JSON_PARSE_ERROR"},
            {ErrorCode::OUT_OF_MEMORY, "OUT_OF_MEMORY"},
            {ErrorCode::BUFFER_OVERFLOW, "BUFFER_OVERFLOW"},
            {ErrorCode::NULL_POINTER, "NULL_POINTER"},
            {ErrorCode::INVALID_MEMORY_ACCESS, "INVALID_MEMORY_ACCESS"},
            {ErrorCode::ALGORITHM_FAILURE, "ALGORITHM_FAILURE"},
            {ErrorCode::INVALID_GRAPH, "INVALID_GRAPH"},
            {ErrorCode::NODE_NOT_FOUND, "NODE_NOT_FOUND"},
            {ErrorCode::NO_PATH_EXISTS, "NO_PATH_EXISTS"},
            {ErrorCode::INFINITE_LOOP_DETECTED, "INFINITE_LOOP_DETECTED"},
            {ErrorCode::INVALID_PARAMETERS, "INVALID_PARAMETERS"},
            {ErrorCode::NETWORK_ERROR, "NETWORK_ERROR"},
            {ErrorCode::CONNECTION_TIMEOUT, "CONNECTION_TIMEOUT"},
            {ErrorCode::SERVER_ERROR, "SERVER_ERROR"},
            {ErrorCode::INVALID_URL, "INVALID_URL"},
            {ErrorCode::SYSTEM_ERROR, "SYSTEM_ERROR"},
            {ErrorCode::PERMISSION_DENIED, "PERMISSION_DENIED"},
            {ErrorCode::RESOURCE_EXHAUSTED, "RESOURCE_EXHAUSTED"},
            {ErrorCode::OPERATION_TIMEOUT, "OPERATION_TIMEOUT"},
            {ErrorCode::SIMULATION_ERROR, "SIMULATION_ERROR"},
            {ErrorCode::EVENT_PROCESSING_FAILED, "EVENT_PROCESSING_FAILED"},
            {ErrorCode::TIME_SYNCHRONIZATION_ERROR, "TIME_SYNCHRONIZATION_ERROR"},
            {ErrorCode::INVALID_EVENT, "INVALID_EVENT"},
            {ErrorCode::UNKNOWN_ERROR, "UNKNOWN_ERROR"}
        };
        
        auto it = code_map.find(code);
        return it != code_map.end() ? it->second : "UNKNOWN_ERROR_CODE";
    }
    
    /**
     * @brief Convert severity to string
     */
    static std::string severity_to_string(ErrorSeverity severity) {
        switch (severity) {
            case ErrorSeverity::DEBUG: return "DEBUG";
            case ErrorSeverity::INFO: return "INFO";
            case ErrorSeverity::WARNING: return "WARNING";
            case ErrorSeverity::ERROR: return "ERROR";
            case ErrorSeverity::CRITICAL: return "CRITICAL";
            case ErrorSeverity::FATAL: return "FATAL";
            default: return "UNKNOWN";
        }
    }
};

/**
 * @brief Error handling utilities and macros
 */
#define RTS_THROW(code, message) \
    throw rts::core::RTSException(code, message, rts::core::ErrorSeverity::ERROR, \
                                  rts::core::ErrorContext(__FILE__, __LINE__, __FUNCTION__))

#define RTS_THROW_WITH_INFO(code, message, info) \
    throw rts::core::RTSException(code, message, rts::core::ErrorSeverity::ERROR, \
                                  rts::core::ErrorContext(__FILE__, __LINE__, __FUNCTION__, info))

#define RTS_THROW_NESTED(code, message, nested) \
    throw rts::core::RTSException(code, message, nested, rts::core::ErrorSeverity::ERROR, \
                                  rts::core::ErrorContext(__FILE__, __LINE__, __FUNCTION__))

#define RTS_ASSERT(condition, code, message) \
    do { \
        if (!(condition)) { \
            RTS_THROW(code, message); \
        } \
    } while (0)

#define RTS_ASSERT_NOT_NULL(ptr, message) \
    RTS_ASSERT(ptr != nullptr, rts::core::ErrorCode::NULL_POINTER, message)

/**
 * @brief Result template for operations that can fail
 */
template<typename T>
class Result {
private:
    bool success_;
    T value_;
    RTSException error_;
    
public:
    /**
     * @brief Constructor for successful result
     */
    explicit Result(const T& value) : success_(true), value_(value), 
                                     error_(ErrorCode::SUCCESS, "") {}
    
    /**
     * @brief Constructor for failed result
     */
    explicit Result(const RTSException& error) : success_(false), value_(T{}), error_(error) {}
    
    /**
     * @brief Check if result is successful
     */
    bool is_success() const { return success_; }
    
    /**
     * @brief Check if result is failed
     */
    bool is_error() const { return !success_; }
    
    /**
     * @brief Get value (only if successful)
     */
    const T& value() const {
        if (!success_) {
            throw error_;
        }
        return value_;
    }
    
    /**
     * @brief Get value with default (safe)
     */
    T value_or(const T& default_value) const {
        return success_ ? value_ : default_value;
    }
    
    /**
     * @brief Get error (only if failed)
     */
    const RTSException& error() const {
        return error_;
    }
    
    /**
     * @brief Conversion operator for easy checking
     */
    explicit operator bool() const { return success_; }
};

/**
 * @brief Error handler interface
 */
class ErrorHandler {
public:
    virtual ~ErrorHandler() = default;
    virtual void handle_error(const RTSException& exception) = 0;
    virtual void handle_warning(const std::string& message) = 0;
    virtual void handle_info(const std::string& message) = 0;
};

/**
 * @brief Console error handler implementation
 */
class ConsoleErrorHandler : public ErrorHandler {
public:
    void handle_error(const RTSException& exception) override {
        std::cerr << "[ERROR] " << exception.get_detailed_message() << std::endl;
    }
    
    void handle_warning(const std::string& message) override {
        std::cout << "[WARNING] " << message << std::endl;
    }
    
    void handle_info(const std::string& message) override {
        std::cout << "[INFO] " << message << std::endl;
    }
};

/**
 * @brief Global error handler manager
 */
class ErrorManager {
private:
    static std::unique_ptr<ErrorHandler> handler_;
    
public:
    static void set_handler(std::unique_ptr<ErrorHandler> handler) {
        handler_ = std::move(handler);
    }
    
    static ErrorHandler* get_handler() {
        if (!handler_) {
            handler_ = std::make_unique<ConsoleErrorHandler>();
        }
        return handler_.get();
    }
    
    static void handle_error(const RTSException& exception) {
        get_handler()->handle_error(exception);
    }
    
    static void handle_warning(const std::string& message) {
        get_handler()->handle_warning(message);
    }
    
    static void handle_info(const std::string& message) {
        get_handler()->handle_info(message);
    }
};

// Static member initialization (should be in .cpp file in a real implementation)
std::unique_ptr<ErrorHandler> ErrorManager::handler_ = nullptr;

} // namespace core
} // namespace rts