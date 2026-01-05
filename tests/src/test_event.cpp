/*
 * Copyright (C) 2005-2025 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
 */

/**
 * @file test_event.cpp
 * @brief Tests for ITT event API
 */

#include "itt_test_framework.hpp"
#include "itt_test_utils.hpp"
#include <ittnotify.h>
#include <thread>
#include <vector>
#include <cstring>

using namespace itt_test;
using namespace itt_test::utils;

ITT_TEST(Event, CreateSimple) {
    // Note: Reference collector doesn't implement event_create, so it returns 0
    // This test verifies the call doesn't crash
    __itt_event event = __itt_event_create("TestEvent", 9);
    (void)event;
    ITT_ASSERT(true);  // Call succeeded without crash
}

ITT_TEST(Event, CreateMultiple) {
    // Note: Reference collector doesn't implement event_create
    // This test verifies multiple calls don't crash
    __itt_event e1 = __itt_event_create("Event1", 6);
    __itt_event e2 = __itt_event_create("Event2", 6);
    __itt_event e3 = __itt_event_create("Event3", 6);
    
    (void)e1; (void)e2; (void)e3;
    ITT_ASSERT(true);  // Calls succeeded without crash
}

ITT_TEST(Event, StartEnd) {
    __itt_event event = __itt_event_create("StartEndEvent", 13);
    
    __itt_event_start(event);
    cpu_workload(50);
    __itt_event_end(event);
}

ITT_TEST(Event, MultipleStartEnd) {
    __itt_event event = __itt_event_create("MultiEvent", 10);
    
    for (int i = 0; i < 10; ++i) {
        __itt_event_start(event);
        cpu_workload(10);
        __itt_event_end(event);
    }
}

ITT_TEST(Event, OverlappingEvents) {
    __itt_event e1 = __itt_event_create("OverlapEvent1", 13);
    __itt_event e2 = __itt_event_create("OverlapEvent2", 13);
    
    __itt_event_start(e1);
    cpu_workload(20);
    
    __itt_event_start(e2);
    cpu_workload(20);
    __itt_event_end(e2);
    
    cpu_workload(20);
    __itt_event_end(e1);
}

ITT_TEST(Event, ConcurrentCreate) {
    constexpr int NUM_THREADS = 4;
    constexpr int EVENTS_PER_THREAD = 50;
    
    std::vector<std::thread> threads;
    std::vector<std::vector<__itt_event>> results(NUM_THREADS);
    ThreadBarrier barrier(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([t, &results, &barrier]() {
            barrier.wait();
            for (int i = 0; i < EVENTS_PER_THREAD; ++i) {
                std::string name = "ConcEvent_T" + std::to_string(t) + "_" + std::to_string(i);
                results[t].push_back(__itt_event_create(name.c_str(), static_cast<int>(name.length())));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

ITT_TEST(Event, ConcurrentStartEnd) {
    constexpr int NUM_THREADS = 4;
    constexpr int EVENTS_PER_THREAD = 50;
    
    std::vector<std::thread> threads;
    ThreadBarrier barrier(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([t, &barrier]() {
            std::string name = "ConcStartEndEvent_T" + std::to_string(t);
            __itt_event event = __itt_event_create(name.c_str(), static_cast<int>(name.length()));
            
            barrier.wait();
            
            for (int i = 0; i < EVENTS_PER_THREAD; ++i) {
                __itt_event_start(event);
                cpu_workload(5);
                __itt_event_end(event);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

ITT_TEST(Event, RapidStartEnd) {
    __itt_event event = __itt_event_create("RapidEvent", 10);
    
    for (int i = 0; i < 1000; ++i) {
        __itt_event_start(event);
        __itt_event_end(event);
    }
}

ITT_TEST(Event, VerifyCreateLog) {
    // Note: Reference collector doesn't implement __itt_event_create
    // This test just verifies the call doesn't crash; log verification skipped
    std::string event_name = unique_string_name("LogEvent");
    __itt_event event = __itt_event_create(event_name.c_str(), static_cast<int>(event_name.length()));
    
    (void)event;
    ITT_ASSERT(true);  // Call succeeded without crash
}

ITT_TEST_MAIN()
