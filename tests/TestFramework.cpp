#include "TestFramework.hpp"

int TestFramework::total_tests = 0;
int TestFramework::passed_tests = 0;
int TestFramework::failed_tests = 0;

void TestFramework::assert_true(bool condition, const std::string& message) {
    total_tests++;
    if (condition) {
        passed_tests++;
        std::cout << "✓ PASS: " << message << std::endl;
    } else {
        failed_tests++;
        std::cout << "✗ FAIL: " << message << std::endl;
        throw std::runtime_error("Test failed: " + message);
    }
}

void TestFramework::assert_equal_int(int expected, int actual, const std::string& message) {
    total_tests++;
    if (expected == actual) {
        passed_tests++;
        std::cout << "✓ PASS: " << message << std::endl;
    } else {
        failed_tests++;
        std::cout << "✗ FAIL: " << message 
                 << " (expected: " << expected 
                 << ", actual: " << actual << ")" << std::endl;
        throw std::runtime_error("Test failed: " + message);
    }
}

void TestFramework::assert_equal_size_t(size_t expected, size_t actual, const std::string& message) {
    total_tests++;
    if (expected == actual) {
        passed_tests++;
        std::cout << "✓ PASS: " << message << std::endl;
    } else {
        failed_tests++;
        std::cout << "✗ FAIL: " << message 
                 << " (expected: " << expected 
                 << ", actual: " << actual << ")" << std::endl;
        throw std::runtime_error("Test failed: " + message);
    }
}

void TestFramework::assert_equal_string(const std::string& expected, const std::string& actual, const std::string& message) {
    total_tests++;
    if (expected == actual) {
        passed_tests++;
        std::cout << "✓ PASS: " << message << std::endl;
    } else {
        failed_tests++;
        std::cout << "✗ FAIL: " << message 
                 << " (expected: " << expected 
                 << ", actual: " << actual << ")" << std::endl;
        throw std::runtime_error("Test failed: " + message);
    }
}

void TestFramework::assert_equal_float(float expected, float actual, const std::string& message) {
    total_tests++;
    bool are_equal = false;
    
    // Handle infinity cases
    if (std::isinf(expected) && std::isinf(actual)) {
        are_equal = (expected > 0 && actual > 0) || (expected < 0 && actual < 0);
    } else if (std::isnan(expected) && std::isnan(actual)) {
        are_equal = true;
    } else {
        are_equal = std::abs(expected - actual) < 1e-6f;
    }
    
    if (are_equal) {
        passed_tests++;
        std::cout << "✓ PASS: " << message << std::endl;
    } else {
        failed_tests++;
        std::cout << "✗ FAIL: " << message 
                 << " (expected: " << expected 
                 << ", actual: " << actual << ")" << std::endl;
        throw std::runtime_error("Test failed: " + message);
    }
}

void TestFramework::assert_equal_double(double expected, double actual, const std::string& message) {
    total_tests++;
    bool are_equal = false;
    
    // Handle infinity cases
    if (std::isinf(expected) && std::isinf(actual)) {
        are_equal = (expected > 0 && actual > 0) || (expected < 0 && actual < 0);
    } else if (std::isnan(expected) && std::isnan(actual)) {
        are_equal = true;
    } else {
        are_equal = std::abs(expected - actual) < 1e-9; // Higher precision for double
    }
    
    if (are_equal) {
        passed_tests++;
        std::cout << "✓ PASS: " << message << std::endl;
    } else {
        failed_tests++;
        std::cout << "✗ FAIL: " << message 
                 << " (expected: " << expected 
                 << ", actual: " << actual << ")" << std::endl;
        throw std::runtime_error("Test failed: " + message);
    }
}

void TestFramework::assert_equal_infinity(float value, const std::string& message) {
    total_tests++;
    if (std::isinf(value) && value > 0) {
        passed_tests++;
        std::cout << "✓ PASS: " << message << std::endl;
    } else {
        failed_tests++;
        std::cout << "✗ FAIL: " << message 
                 << " (expected: inf, actual: " << value << ")" << std::endl;
        throw std::runtime_error("Test failed: " + message);
    }
}

void TestFramework::assert_equal_uint32_t(uint32_t expected, uint32_t actual, const std::string& message) {
    total_tests++;
    if (expected == actual) {
        passed_tests++;
        std::cout << "✓ PASS: " << message << std::endl;
    } else {
        failed_tests++;
        std::cout << "✗ FAIL: " << message 
                 << " (expected: " << expected 
                 << ", actual: " << actual << ")" << std::endl;
        throw std::runtime_error("Test failed: " + message);
    }
}

void TestFramework::assert_not_null(const void* ptr, const std::string& message) {
    assert_true(ptr != nullptr, message);
}

void TestFramework::assert_null(const void* ptr, const std::string& message) {
    assert_true(ptr == nullptr, message);
}

void TestFramework::run_test(const std::string& test_name, std::function<void()> test_func) {
    std::cout << "\n--- Running Test: " << test_name << " ---" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    
    try {
        test_func();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::cout << "Test completed in " << duration.count() << " μs" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Test failed with exception: " << e.what() << std::endl;
    }
}

void TestFramework::print_summary() {
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "TEST SUMMARY" << std::endl;
    std::cout << "Total Tests: " << total_tests << std::endl;
    std::cout << "Passed: " << passed_tests << std::endl;
    std::cout << "Failed: " << failed_tests << std::endl;
    std::cout << "Success Rate: " << (total_tests > 0 ? (passed_tests * 100 / total_tests) : 0) << "%" << std::endl;
    std::cout << std::string(50, '=') << std::endl;
}

bool TestFramework::all_passed() {
    return failed_tests == 0;
}