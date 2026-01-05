/*
 * Copyright (C) 2005-2025 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
 */

/**
 * @file itt_test_framework.hpp
 * @brief Lightweight single-header C++ test framework for ITT API testing
 * 
 * This framework provides:
 * - Test case registration and execution
 * - Assertions with detailed failure messages
 * - Reference collector log file verification
 * - Test fixtures for setup/teardown
 * - Colorized console output
 * - Test timing and statistics
 */

#ifndef ITT_TEST_FRAMEWORK_HPP
#define ITT_TEST_FRAMEWORK_HPP

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <regex>
#include <memory>
#include <algorithm>
#include <filesystem>
#include <iomanip>

namespace itt_test {

// =============================================================================
// Console Colors
// =============================================================================

namespace colors {
    const char* const RED     = "\033[1;31m";
    const char* const GREEN   = "\033[1;32m";
    const char* const YELLOW  = "\033[1;33m";
    const char* const BLUE    = "\033[1;34m";
    const char* const MAGENTA = "\033[1;35m";
    const char* const CYAN    = "\033[1;36m";
    const char* const RESET   = "\033[0m";
    const char* const BOLD    = "\033[1m";
}

// =============================================================================
// Test Result
// =============================================================================

enum class TestStatus {
    PASSED,
    FAILED,
    SKIPPED
};

struct TestResult {
    std::string name;
    TestStatus status{TestStatus::PASSED};
    std::string message;
    double duration_ms{0.0};
    std::vector<std::string> failures;
};

// =============================================================================
// Assertion Context
// =============================================================================

class AssertionContext {
public:
    std::string file;
    int line{0};
    std::vector<std::string> failures;
    bool current_test_failed{false};

    void reset() {
        failures.clear();
        current_test_failed = false;
    }

    void add_failure(const std::string& msg) {
        failures.push_back(msg);
        current_test_failed = true;
    }
};

// Thread-local assertion context
inline AssertionContext& get_assertion_context() {
    static thread_local AssertionContext ctx;
    return ctx;
}

// =============================================================================
// Assertion Macros
// =============================================================================

#define ITT_TEST_STRINGIFY(x) #x
#define ITT_TEST_TOSTRING(x) ITT_TEST_STRINGIFY(x)

#define ITT_ASSERT(condition)                                                  \
    do {                                                                       \
        if (!(condition)) {                                                    \
            std::ostringstream oss;                                            \
            oss << __FILE__ << ":" << __LINE__ << ": Assertion failed: "       \
                << ITT_TEST_STRINGIFY(condition);                              \
            itt_test::get_assertion_context().add_failure(oss.str());          \
        }                                                                      \
    } while (0)

#define ITT_ASSERT_MSG(condition, msg)                                         \
    do {                                                                       \
        if (!(condition)) {                                                    \
            std::ostringstream oss;                                            \
            oss << __FILE__ << ":" << __LINE__ << ": " << msg;                 \
            itt_test::get_assertion_context().add_failure(oss.str());          \
        }                                                                      \
    } while (0)

#define ITT_ASSERT_EQ(expected, actual)                                        \
    do {                                                                       \
        auto _expected = (expected);                                           \
        auto _actual = (actual);                                               \
        if (_expected != _actual) {                                            \
            std::ostringstream oss;                                            \
            oss << __FILE__ << ":" << __LINE__ << ": Expected '"               \
                << ITT_TEST_STRINGIFY(expected) << "' == '"                    \
                << ITT_TEST_STRINGIFY(actual) << "', got "                     \
                << _expected << " != " << _actual;                             \
            itt_test::get_assertion_context().add_failure(oss.str());          \
        }                                                                      \
    } while (0)

#define ITT_ASSERT_NE(val1, val2)                                              \
    do {                                                                       \
        auto _val1 = (val1);                                                   \
        auto _val2 = (val2);                                                   \
        if (_val1 == _val2) {                                                  \
            std::ostringstream oss;                                            \
            oss << __FILE__ << ":" << __LINE__ << ": Expected '"               \
                << ITT_TEST_STRINGIFY(val1) << "' != '"                        \
                << ITT_TEST_STRINGIFY(val2) << "', but both equal " << _val1;  \
            itt_test::get_assertion_context().add_failure(oss.str());          \
        }                                                                      \
    } while (0)

#define ITT_ASSERT_NULL(ptr)                                                   \
    do {                                                                       \
        if ((ptr) != nullptr) {                                                \
            std::ostringstream oss;                                            \
            oss << __FILE__ << ":" << __LINE__ << ": Expected '"               \
                << ITT_TEST_STRINGIFY(ptr) << "' to be null, but it wasn't";   \
            itt_test::get_assertion_context().add_failure(oss.str());          \
        }                                                                      \
    } while (0)

#define ITT_ASSERT_NOT_NULL(ptr)                                               \
    do {                                                                       \
        if ((ptr) == nullptr) {                                                \
            std::ostringstream oss;                                            \
            oss << __FILE__ << ":" << __LINE__ << ": Expected '"               \
                << ITT_TEST_STRINGIFY(ptr) << "' to be non-null";              \
            itt_test::get_assertion_context().add_failure(oss.str());          \
        }                                                                      \
    } while (0)

#define ITT_ASSERT_TRUE(condition) ITT_ASSERT(condition)
#define ITT_ASSERT_FALSE(condition) ITT_ASSERT(!(condition))

#define ITT_ASSERT_THROWS(expr, exception_type)                                \
    do {                                                                       \
        bool caught = false;                                                   \
        try {                                                                  \
            expr;                                                              \
        } catch (const exception_type&) {                                      \
            caught = true;                                                     \
        } catch (...) {                                                        \
        }                                                                      \
        if (!caught) {                                                         \
            std::ostringstream oss;                                            \
            oss << __FILE__ << ":" << __LINE__ << ": Expected '"               \
                << ITT_TEST_STRINGIFY(expr) << "' to throw "                   \
                << ITT_TEST_STRINGIFY(exception_type);                         \
            itt_test::get_assertion_context().add_failure(oss.str());          \
        }                                                                      \
    } while (0)

// =============================================================================
// Test Case
// =============================================================================

struct TestCase {
    std::string name;
    std::string suite;
    std::function<void()> test_func;
    std::function<void()> setup;
    std::function<void()> teardown;
    bool skip{false};
    std::string skip_reason;
};

// =============================================================================
// Test Registry (Singleton)
// =============================================================================

class TestRegistry {
public:
    static TestRegistry& instance() {
        static TestRegistry registry;
        return registry;
    }

    void register_test(const TestCase& tc) {
        tests_.push_back(tc);
    }

    const std::vector<TestCase>& tests() const {
        return tests_;
    }

    void clear() {
        tests_.clear();
    }

private:
    TestRegistry() = default;
    std::vector<TestCase> tests_;
};

// =============================================================================
// Test Registration Helper
// =============================================================================

class TestRegistrar {
public:
    TestRegistrar(const std::string& suite, const std::string& name,
                  std::function<void()> func,
                  std::function<void()> setup = nullptr,
                  std::function<void()> teardown = nullptr) {
        TestCase tc;
        tc.suite = suite;
        tc.name = name;
        tc.test_func = func;
        tc.setup = setup;
        tc.teardown = teardown;
        TestRegistry::instance().register_test(tc);
    }
};

// =============================================================================
// Test Macros
// =============================================================================

#define ITT_TEST_SUITE(suite_name)                                             \
    namespace suite_name##_suite

#define ITT_TEST(suite_name, test_name)                                        \
    static void suite_name##_##test_name##_impl();                             \
    static itt_test::TestRegistrar suite_name##_##test_name##_registrar(       \
        #suite_name, #test_name, suite_name##_##test_name##_impl);             \
    static void suite_name##_##test_name##_impl()

#define ITT_TEST_F(suite_name, test_name, fixture)                             \
    static void suite_name##_##test_name##_impl();                             \
    static itt_test::TestRegistrar suite_name##_##test_name##_registrar(       \
        #suite_name, #test_name, suite_name##_##test_name##_impl,              \
        []() { fixture::SetUp(); },                                            \
        []() { fixture::TearDown(); });                                        \
    static void suite_name##_##test_name##_impl()

// =============================================================================
// Log File Verifier
// =============================================================================

class LogVerifier {
public:
    explicit LogVerifier(const std::string& log_dir = "")
        : log_dir_(log_dir.empty() ? get_default_log_dir() : log_dir) {}

    /**
     * @brief Find the most recent log file in the log directory
     */
    std::string find_latest_log() const {
        std::filesystem::path dir_path(log_dir_);
        if (!std::filesystem::exists(dir_path)) {
            return "";
        }

        std::string latest_file;
        std::filesystem::file_time_type latest_time;
        bool found = false;

        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.find("libittnotify_refcol_") != std::string::npos &&
                    filename.find(".log") != std::string::npos) {
                    if (!found || entry.last_write_time() > latest_time) {
                        latest_time = entry.last_write_time();
                        latest_file = entry.path().string();
                        found = true;
                    }
                }
            }
        }
        return latest_file;
    }

    /**
     * @brief Read log file contents
     */
    std::string read_log(const std::string& log_path) const {
        std::ifstream file(log_path);
        if (!file.is_open()) {
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    /**
     * @brief Check if log contains a specific pattern
     */
    bool contains(const std::string& log_content, const std::string& pattern) const {
        return log_content.find(pattern) != std::string::npos;
    }

    /**
     * @brief Check if log matches a regex pattern
     */
    bool matches_regex(const std::string& log_content, const std::string& pattern) const {
        std::regex re(pattern);
        return std::regex_search(log_content, re);
    }

    /**
     * @brief Count occurrences of a pattern
     */
    size_t count_occurrences(const std::string& log_content, const std::string& pattern) const {
        size_t count = 0;
        size_t pos = 0;
        while ((pos = log_content.find(pattern, pos)) != std::string::npos) {
            ++count;
            pos += pattern.length();
        }
        return count;
    }

    /**
     * @brief Get all lines matching a pattern
     */
    std::vector<std::string> get_matching_lines(const std::string& log_content,
                                                 const std::string& pattern) const {
        std::vector<std::string> result;
        std::istringstream stream(log_content);
        std::string line;
        while (std::getline(stream, line)) {
            if (line.find(pattern) != std::string::npos) {
                result.push_back(line);
            }
        }
        return result;
    }

    /**
     * @brief Clear all log files in the directory
     */
    void clear_logs() const {
        std::filesystem::path dir_path(log_dir_);
        if (!std::filesystem::exists(dir_path)) {
            return;
        }

        for (const auto& entry : std::filesystem::directory_iterator(dir_path)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.find("libittnotify_refcol_") != std::string::npos) {
                    std::filesystem::remove(entry.path());
                }
            }
        }
    }

    const std::string& log_dir() const { return log_dir_; }

private:
    std::string log_dir_;

    static std::string get_default_log_dir() {
        const char* env_dir = std::getenv("INTEL_LIBITTNOTIFY_LOG_DIR");
        if (env_dir && env_dir[0] != '\0') {
            return env_dir;
        }
        return "/tmp";
    }
};

// =============================================================================
// Test Runner
// =============================================================================

class TestRunner {
public:
    struct Options {
        bool verbose = false;
        bool color = true;
        std::string filter;
        std::string log_dir;
    };

    explicit TestRunner(const Options& opts) : opts_(opts) {}

    int run() {
        const auto& tests = TestRegistry::instance().tests();
        
        print_header();

        int passed = 0;
        int failed = 0;
        int skipped = 0;
        std::vector<TestResult> results;

        for (const auto& tc : tests) {
            if (!filter_matches(tc)) {
                continue;
            }

            TestResult result = run_test(tc);
            results.push_back(result);

            switch (result.status) {
                case TestStatus::PASSED:
                    ++passed;
                    break;
                case TestStatus::FAILED:
                    ++failed;
                    break;
                case TestStatus::SKIPPED:
                    ++skipped;
                    break;
            }

            print_result(result);
        }

        print_summary(passed, failed, skipped, results);

        return failed > 0 ? 1 : 0;
    }

private:
    Options opts_;

    bool filter_matches(const TestCase& tc) const {
        if (opts_.filter.empty()) {
            return true;
        }
        std::string full_name = tc.suite + "." + tc.name;
        return full_name.find(opts_.filter) != std::string::npos;
    }

    TestResult run_test(const TestCase& tc) {
        TestResult result;
        result.name = tc.suite + "." + tc.name;

        if (tc.skip) {
            result.status = TestStatus::SKIPPED;
            result.message = tc.skip_reason;
            return result;
        }

        get_assertion_context().reset();

        auto start = std::chrono::high_resolution_clock::now();

        try {
            if (tc.setup) {
                tc.setup();
            }

            tc.test_func();

            if (tc.teardown) {
                tc.teardown();
            }
        } catch (const std::exception& e) {
            get_assertion_context().add_failure(
                std::string("Exception: ") + e.what());
        } catch (...) {
            get_assertion_context().add_failure("Unknown exception");
        }

        auto end = std::chrono::high_resolution_clock::now();
        result.duration_ms = std::chrono::duration<double, std::milli>(end - start).count();

        if (get_assertion_context().current_test_failed) {
            result.status = TestStatus::FAILED;
            result.failures = get_assertion_context().failures;
        } else {
            result.status = TestStatus::PASSED;
        }

        return result;
    }

    void print_header() const {
        std::cout << "\n";
        std::cout << color(colors::CYAN) << "═══════════════════════════════════════════════════════════════\n";
        std::cout << "                    ITT API Test Suite                         \n";
        std::cout << "═══════════════════════════════════════════════════════════════\n";
        std::cout << color(colors::RESET) << "\n";
    }

    void print_result(const TestResult& result) const {
        std::cout << "  ";
        
        switch (result.status) {
            case TestStatus::PASSED:
                std::cout << color(colors::GREEN) << "✓ PASS" << color(colors::RESET);
                break;
            case TestStatus::FAILED:
                std::cout << color(colors::RED) << "✗ FAIL" << color(colors::RESET);
                break;
            case TestStatus::SKIPPED:
                std::cout << color(colors::YELLOW) << "○ SKIP" << color(colors::RESET);
                break;
        }

        std::cout << "  " << result.name;
        std::cout << color(colors::BLUE) << " (" << std::fixed << std::setprecision(2) 
                  << result.duration_ms << " ms)" << color(colors::RESET);
        std::cout << "\n";

        if (result.status == TestStatus::FAILED && opts_.verbose) {
            for (const auto& failure : result.failures) {
                std::cout << "       " << color(colors::RED) << failure 
                          << color(colors::RESET) << "\n";
            }
        }

        if (result.status == TestStatus::SKIPPED && !result.message.empty()) {
            std::cout << "       " << color(colors::YELLOW) << "Reason: " << result.message
                      << color(colors::RESET) << "\n";
        }
    }

    void print_summary(int passed, int failed, int skipped,
                       const std::vector<TestResult>& results) const {
        std::cout << "\n";
        std::cout << color(colors::CYAN) << "───────────────────────────────────────────────────────────────\n";
        std::cout << color(colors::RESET);
        
        std::cout << "  Summary: ";
        std::cout << color(colors::GREEN) << passed << " passed" << color(colors::RESET) << ", ";
        std::cout << color(colors::RED) << failed << " failed" << color(colors::RESET) << ", ";
        std::cout << color(colors::YELLOW) << skipped << " skipped" << color(colors::RESET);
        std::cout << "\n";

        if (failed > 0) {
            std::cout << "\n  " << color(colors::RED) << "Failed tests:" << color(colors::RESET) << "\n";
            for (const auto& result : results) {
                if (result.status == TestStatus::FAILED) {
                    std::cout << "    • " << result.name << "\n";
                    for (const auto& failure : result.failures) {
                        std::cout << "      " << color(colors::RED) << failure 
                                  << color(colors::RESET) << "\n";
                    }
                }
            }
        }

        std::cout << "\n";
        if (failed == 0) {
            std::cout << color(colors::GREEN) << "  ✓ All tests passed!" << color(colors::RESET) << "\n";
        } else {
            std::cout << color(colors::RED) << "  ✗ Some tests failed!" << color(colors::RESET) << "\n";
        }
        std::cout << "\n";
    }

    std::string color(const char* c) const {
        return opts_.color ? c : "";
    }
};

// =============================================================================
// Main Function Helper
// =============================================================================

inline int run_tests(int argc, char* argv[]) {
    TestRunner::Options opts;
    opts.verbose = true;
    opts.color = true;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--no-color") {
            opts.color = false;
        } else if (arg == "--filter" && i + 1 < argc) {
            opts.filter = argv[++i];
        } else if (arg == "--verbose" || arg == "-v") {
            opts.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            opts.verbose = false;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Usage: " << argv[0] << " [options]\n"
                      << "Options:\n"
                      << "  --no-color     Disable colored output\n"
                      << "  --filter NAME  Run only tests matching NAME\n"
                      << "  --verbose, -v  Show detailed output\n"
                      << "  --quiet, -q    Show minimal output\n"
                      << "  --help, -h     Show this help\n";
            return 0;
        }
    }

    TestRunner runner(opts);
    return runner.run();
}

#define ITT_TEST_MAIN()                                                        \
    int main(int argc, char* argv[]) {                                         \
        return itt_test::run_tests(argc, argv);                                \
    }

} // namespace itt_test

#endif // ITT_TEST_FRAMEWORK_HPP
