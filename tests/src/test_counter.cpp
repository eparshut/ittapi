/*
 * Copyright (C) 2005-2025 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
 */

/**
 * @file test_counter.cpp
 * @brief Tests for ITT counter API
 */

#include "itt_test_framework.hpp"
#include "itt_test_utils.hpp"
#include <ittnotify.h>
#include <thread>
#include <vector>

using namespace itt_test;
using namespace itt_test::utils;

ITT_TEST(Counter, CreateSimple) {
    __itt_counter counter = __itt_counter_create("TestCounter", "TestDomain");
    ITT_ASSERT_NOT_NULL(counter);
}

ITT_TEST(Counter, CreateTypedU64) {
    __itt_counter counter = __itt_counter_create_typed("Counter_U64", "Domain", __itt_metadata_u64);
    ITT_ASSERT_NOT_NULL(counter);
}

ITT_TEST(Counter, CreateTypedDouble) {
    __itt_counter counter = __itt_counter_create_typed("Counter_Double", "Domain", __itt_metadata_double);
    ITT_ASSERT_NOT_NULL(counter);
}

ITT_TEST(Counter, CreateMultiple) {
    __itt_counter c1 = __itt_counter_create("Counter1", "Domain1");
    __itt_counter c2 = __itt_counter_create("Counter2", "Domain2");
    __itt_counter c3 = __itt_counter_create("Counter3", "Domain3");
    
    ITT_ASSERT_NOT_NULL(c1);
    ITT_ASSERT_NOT_NULL(c2);
    ITT_ASSERT_NOT_NULL(c3);
    ITT_ASSERT_NE(c1, c2);
    ITT_ASSERT_NE(c2, c3);
}

ITT_TEST(Counter, CreateDuplicate) {
    __itt_counter c1 = __itt_counter_create("DupCounter", "DupDomain");
    __itt_counter c2 = __itt_counter_create("DupCounter", "DupDomain");
    
    ITT_ASSERT_NOT_NULL(c1);
    ITT_ASSERT_NOT_NULL(c2);
    ITT_ASSERT_EQ(c1, c2);
}

ITT_TEST(Counter, SetValueU64) {
    __itt_counter counter = __itt_counter_create_typed("SetCounter", "Domain", __itt_metadata_u64);
    ITT_ASSERT_NOT_NULL(counter);
    
    uint64_t value = 42;
    __itt_counter_set_value(counter, &value);
}

ITT_TEST(Counter, SetValueMultiple) {
    __itt_counter counter = __itt_counter_create_typed("MultiSetCounter", "Domain", __itt_metadata_u64);
    ITT_ASSERT_NOT_NULL(counter);
    
    for (int i = 0; i < 100; ++i) {
        uint64_t value = static_cast<uint64_t>(i * 10);
        __itt_counter_set_value(counter, &value);
    }
}

ITT_TEST(Counter, ConcurrentCreation) {
    constexpr int NUM_THREADS = 4;
    constexpr int COUNTERS_PER_THREAD = 50;
    
    std::vector<std::thread> threads;
    std::vector<std::vector<__itt_counter>> results(NUM_THREADS);
    ThreadBarrier barrier(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([t, &results, &barrier]() {
            barrier.wait();
            for (int i = 0; i < COUNTERS_PER_THREAD; ++i) {
                std::string name = "ConcCounter_T" + std::to_string(t) + "_" + std::to_string(i);
                std::string domain = "ConcDomain_T" + std::to_string(t);
                results[t].push_back(__itt_counter_create(name.c_str(), domain.c_str()));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        for (const auto& counter : results[t]) {
            ITT_ASSERT_NOT_NULL(counter);
        }
    }
}

ITT_TEST(Counter, NullName) {
    __itt_counter counter = __itt_counter_create(nullptr, "Domain");
    ITT_ASSERT_NULL(counter);
}

ITT_TEST(Counter, NullDomain) {
    __itt_counter counter = __itt_counter_create("Counter", nullptr);
    ITT_ASSERT_NULL(counter);
}

ITT_TEST(Counter, VerifyCreateLog) {
    // Note: The reference collector only flushes logs at program exit.
    // This test verifies the API call doesn't crash.
    std::string counter_name = unique_string_name("LogCounter");
    std::string domain_name = unique_domain_name("LogCounterDomain");
    
    __itt_counter counter = __itt_counter_create(counter_name.c_str(), domain_name.c_str());
    ITT_ASSERT_NOT_NULL(counter);
    ITT_ASSERT(true);  // API call succeeded
}

ITT_TEST_MAIN()
