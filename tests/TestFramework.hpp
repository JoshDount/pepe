#pragma once

#include <iostream>
#include <string>
#include <exception>
#include <chrono>
#include <functional>
#include <cmath>

/**
 * @brief Simple testing framework for the project
 */
class TestFramework {
private:
    static int total_tests;
    static int passed_tests;
    static int failed_tests;
    
public:
    static void assert_true(bool condition, const std::string& message);
    
    static void assert_equal_int(int expected, int actual, const std::string& message);
    static void assert_equal_size_t(size_t expected, size_t actual, const std::string& message);
    static void assert_equal_string(const std::string& expected, const std::string& actual, const std::string& message);
    static void assert_equal_float(float expected, float actual, const std::string& message);
    static void assert_equal_double(double expected, double actual, const std::string& message);
    static void assert_equal_infinity(float value, const std::string& message);
    static void assert_equal_uint32_t(uint32_t expected, uint32_t actual, const std::string& message);
    
    static void assert_not_null(const void* ptr, const std::string& message);
    static void assert_null(const void* ptr, const std::string& message);
    
    static void run_test(const std::string& test_name, std::function<void()> test_func);
    static void print_summary();
    static bool all_passed();
};