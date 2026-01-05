/*
 * Copyright (C) 2005-2025 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
 */

/**
 * @file itt_test_utils.hpp
 * @brief Utility functions and fixtures for ITT API tests
 */

#ifndef ITT_TEST_UTILS_HPP
#define ITT_TEST_UTILS_HPP

#include "itt_test_framework.hpp"
#include <ittnotify.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace itt_test {
namespace utils {

// =============================================================================
// Environment Setup
// =============================================================================

/**
 * @brief RAII class for setting environment variables during tests
 */
class ScopedEnvVar {
public:
    ScopedEnvVar(const std::string& name, const std::string& value)
        : name_(name) {
        const char* old = std::getenv(name.c_str());
        if (old) {
            had_value_ = true;
            old_value_ = old;
        }
        setenv(name.c_str(), value.c_str(), 1);
    }

    ~ScopedEnvVar() {
        if (had_value_) {
            setenv(name_.c_str(), old_value_.c_str(), 1);
        } else {
            unsetenv(name_.c_str());
        }
    }

    ScopedEnvVar(const ScopedEnvVar&) = delete;
    ScopedEnvVar& operator=(const ScopedEnvVar&) = delete;

private:
    std::string name_;
    std::string old_value_;
    bool had_value_{false};
};

// =============================================================================
// Thread Synchronization Utilities
// =============================================================================

/**
 * @brief Barrier for synchronizing multiple threads
 */
class ThreadBarrier {
public:
    explicit ThreadBarrier(size_t count) : count_(count), waiting_(0), generation_(0) {}

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        size_t gen = generation_;
        if (++waiting_ == count_) {
            ++generation_;
            waiting_ = 0;
            cv_.notify_all();
        } else {
            cv_.wait(lock, [this, gen] { return gen != generation_; });
        }
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    size_t count_;
    size_t waiting_;
    size_t generation_;
};

// =============================================================================
// Workload Generators
// =============================================================================

/**
 * @brief Generate CPU workload for testing
 */
inline void cpu_workload(int iterations = 1000) {
    volatile double result = 0.0;
    for (int i = 0; i < iterations; ++i) {
        result += static_cast<double>(i) * 0.001;
    }
    (void)result;
}

/**
 * @brief Sleep-based workload for timing tests
 */
inline void timed_workload(std::chrono::milliseconds duration) {
    std::this_thread::sleep_for(duration);
}

// =============================================================================
// ITT API Helper Functions
// =============================================================================

/**
 * @brief Create a unique domain name for testing
 */
inline std::string unique_domain_name(const std::string& prefix = "TestDomain") {
    static std::atomic<int> counter{0};
    return prefix + "_" + std::to_string(counter.fetch_add(1));
}

/**
 * @brief Create a unique string handle name for testing
 */
inline std::string unique_string_name(const std::string& prefix = "TestString") {
    static std::atomic<int> counter{0};
    return prefix + "_" + std::to_string(counter.fetch_add(1));
}

// =============================================================================
// Test Fixtures
// =============================================================================

/**
 * @brief Base fixture for reference collector tests
 */
class RefCollectorFixture {
public:
    static void SetUp() {
        // Get the log directory from environment or use default
        const char* log_dir = std::getenv("INTEL_LIBITTNOTIFY_LOG_DIR");
        if (log_dir) {
            log_verifier_ = std::make_unique<LogVerifier>(log_dir);
        } else {
            log_verifier_ = std::make_unique<LogVerifier>();
        }
        
        // Record the time before test starts (for finding log files)
        test_start_time_ = std::chrono::system_clock::now();
    }

    static void TearDown() {
        log_verifier_.reset();
    }

    static LogVerifier& log_verifier() {
        return *log_verifier_;
    }

    static std::chrono::system_clock::time_point test_start_time() {
        return test_start_time_;
    }

protected:
    static inline std::unique_ptr<LogVerifier> log_verifier_;
    static inline std::chrono::system_clock::time_point test_start_time_;
};

/**
 * @brief Fixture that creates a fresh domain for each test
 */
class DomainFixture : public RefCollectorFixture {
public:
    static void SetUp() {
        RefCollectorFixture::SetUp();
        domain_name_ = unique_domain_name();
        domain_ = __itt_domain_create(domain_name_.c_str());
    }

    static void TearDown() {
        RefCollectorFixture::TearDown();
        domain_ = nullptr;
        domain_name_.clear();
    }

    static __itt_domain* domain() { return domain_; }
    static const std::string& domain_name() { return domain_name_; }

protected:
    static inline __itt_domain* domain_{nullptr};
    static inline std::string domain_name_;
};

// =============================================================================
// Log Verification Helpers
// =============================================================================

/**
 * @brief Wait for log file to be written and contain expected content
 */
inline bool wait_for_log_content(const LogVerifier& verifier, 
                                  const std::string& expected,
                                  std::chrono::milliseconds timeout = std::chrono::milliseconds(1000)) {
    auto start = std::chrono::steady_clock::now();
    while (std::chrono::steady_clock::now() - start < timeout) {
        std::string log_file = verifier.find_latest_log();
        if (!log_file.empty()) {
            std::string content = verifier.read_log(log_file);
            if (verifier.contains(content, expected)) {
                return true;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    return false;
}

/**
 * @brief Assert that the log contains expected pattern
 */
#define ITT_ASSERT_LOG_CONTAINS(verifier, pattern)                             \
    do {                                                                       \
        std::string log_file = (verifier).find_latest_log();                   \
        ITT_ASSERT_MSG(!log_file.empty(), "Log file not found");               \
        std::string content = (verifier).read_log(log_file);                   \
        ITT_ASSERT_MSG((verifier).contains(content, pattern),                  \
            std::string("Log does not contain pattern: ") + pattern);          \
    } while (0)

/**
 * @brief Assert that the log matches a regex pattern
 */
#define ITT_ASSERT_LOG_MATCHES(verifier, regex_pattern)                        \
    do {                                                                       \
        std::string log_file = (verifier).find_latest_log();                   \
        ITT_ASSERT_MSG(!log_file.empty(), "Log file not found");               \
        std::string content = (verifier).read_log(log_file);                   \
        ITT_ASSERT_MSG((verifier).matches_regex(content, regex_pattern),       \
            std::string("Log does not match regex: ") + regex_pattern);        \
    } while (0)

} // namespace utils
} // namespace itt_test

#endif // ITT_TEST_UTILS_HPP
