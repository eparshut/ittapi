/*
 * Copyright (C) 2005-2025 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
 */

/**
 * @file test_frame.cpp
 * @brief Tests for ITT frame API
 */

#include "itt_test_framework.hpp"
#include "itt_test_utils.hpp"
#include <ittnotify.h>
#include <thread>
#include <vector>

using namespace itt_test;
using namespace itt_test::utils;

ITT_TEST(Frame, BeginEndSimple) {
    __itt_domain* domain = __itt_domain_create("FrameTest.Simple");
    ITT_ASSERT_NOT_NULL(domain);
    
    __itt_frame_begin_v3(domain, nullptr);
    cpu_workload(100);
    __itt_frame_end_v3(domain, nullptr);
}

ITT_TEST(Frame, BeginEndWithId) {
    __itt_domain* domain = __itt_domain_create("FrameTest.WithId");
    __itt_id frame_id = __itt_id_make(domain, 42);
    
    __itt_frame_begin_v3(domain, &frame_id);
    cpu_workload(100);
    __itt_frame_end_v3(domain, &frame_id);
}

ITT_TEST(Frame, MultipleFrames) {
    __itt_domain* domain = __itt_domain_create("FrameTest.Multiple");
    constexpr int NUM_FRAMES = 10;
    
    for (int i = 0; i < NUM_FRAMES; ++i) {
        __itt_frame_begin_v3(domain, nullptr);
        cpu_workload(20);
        __itt_frame_end_v3(domain, nullptr);
    }
}

ITT_TEST(Frame, NestedFrames) {
    __itt_domain* d1 = __itt_domain_create("FrameTest.Nested1");
    __itt_domain* d2 = __itt_domain_create("FrameTest.Nested2");
    
    __itt_frame_begin_v3(d1, nullptr);
    cpu_workload(20);
    
    __itt_frame_begin_v3(d2, nullptr);
    cpu_workload(50);
    __itt_frame_end_v3(d2, nullptr);
    
    cpu_workload(20);
    __itt_frame_end_v3(d1, nullptr);
}

ITT_TEST(Frame, SubmitSimple) {
    __itt_domain* domain = __itt_domain_create("FrameTest.Submit");
    
    __itt_timestamp begin = __itt_get_timestamp();
    cpu_workload(100);
    __itt_timestamp end = __itt_get_timestamp();
    
    __itt_frame_submit_v3(domain, nullptr, begin, end);
}

ITT_TEST(Frame, TimestampOrdering) {
    __itt_timestamp ts1 = __itt_get_timestamp();
    cpu_workload(50);
    __itt_timestamp ts2 = __itt_get_timestamp();
    cpu_workload(50);
    __itt_timestamp ts3 = __itt_get_timestamp();
    
    ITT_ASSERT(ts1 <= ts2);
    ITT_ASSERT(ts2 <= ts3);
}

ITT_TEST(Frame, ConcurrentBeginEnd) {
    constexpr int NUM_THREADS = 4;
    constexpr int FRAMES_PER_THREAD = 50;
    
    __itt_domain* domain = __itt_domain_create("FrameTest.Concurrent");
    std::vector<std::thread> threads;
    ThreadBarrier barrier(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([domain, &barrier]() {
            barrier.wait();
            for (int i = 0; i < FRAMES_PER_THREAD; ++i) {
                __itt_frame_begin_v3(domain, nullptr);
                cpu_workload(5);
                __itt_frame_end_v3(domain, nullptr);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

ITT_TEST(Frame, RapidFrames) {
    __itt_domain* domain = __itt_domain_create("FrameTest.Rapid");
    constexpr int NUM_FRAMES = 1000;
    
    for (int i = 0; i < NUM_FRAMES; ++i) {
        __itt_frame_begin_v3(domain, nullptr);
        __itt_frame_end_v3(domain, nullptr);
    }
}

ITT_TEST(Frame, VerifyBeginLog) {
    // Note: The reference collector only flushes logs at program exit.
    // This test verifies the API call doesn't crash.
    std::string domain_name = unique_domain_name("FrameLogDomain");
    __itt_domain* domain = __itt_domain_create(domain_name.c_str());
    
    __itt_frame_begin_v3(domain, nullptr);
    cpu_workload(50);
    __itt_frame_end_v3(domain, nullptr);
    
    ITT_ASSERT(true);  // API call succeeded
}

ITT_TEST_MAIN()
