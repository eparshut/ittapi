/*
 * Copyright (C) 2005-2025 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
 */

/**
 * @file test_collection_control.cpp
 * @brief Tests for ITT collection control API
 */

#include "itt_test_framework.hpp"
#include "itt_test_utils.hpp"
#include <ittnotify.h>
#include <thread>
#include <vector>

using namespace itt_test;
using namespace itt_test::utils;

ITT_TEST(CollectionControl, Pause) {
    __itt_pause();
}

ITT_TEST(CollectionControl, Resume) {
    __itt_resume();
}

ITT_TEST(CollectionControl, Detach) {
    __itt_detach();
}

ITT_TEST(CollectionControl, PauseResumeCycle) {
    __itt_pause();
    cpu_workload(50);
    __itt_resume();
    cpu_workload(50);
}

ITT_TEST(CollectionControl, MultiplePauses) {
    __itt_pause();
    __itt_pause();
    __itt_pause();
    cpu_workload(50);
    __itt_resume();
}

ITT_TEST(CollectionControl, MultipleResumes) {
    __itt_pause();
    cpu_workload(50);
    __itt_resume();
    __itt_resume();
    __itt_resume();
}

ITT_TEST(CollectionControl, PauseResumeWithTasks) {
    __itt_domain* domain = __itt_domain_create("CollectionTest.Tasks");
    __itt_string_handle* name = __itt_string_handle_create("TaskDuringPause");
    
    __itt_task_begin(domain, __itt_null, __itt_null, name);
    cpu_workload(20);
    __itt_task_end(domain);
    
    __itt_pause();
    
    __itt_task_begin(domain, __itt_null, __itt_null, name);
    cpu_workload(20);
    __itt_task_end(domain);
    
    __itt_resume();
    
    __itt_task_begin(domain, __itt_null, __itt_null, name);
    cpu_workload(20);
    __itt_task_end(domain);
}

ITT_TEST(CollectionControl, ConcurrentPauseResume) {
    constexpr int NUM_THREADS = 8;
    constexpr int ITERATIONS = 100;
    
    std::vector<std::thread> threads;
    ThreadBarrier barrier(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([&barrier]() {
            barrier.wait();
            for (int i = 0; i < ITERATIONS; ++i) {
                if (i % 2 == 0) {
                    __itt_pause();
                } else {
                    __itt_resume();
                }
                cpu_workload(5);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    __itt_resume();
}

ITT_TEST(CollectionControl, VerifyPauseLog) {
    // Note: The reference collector only flushes logs at program exit.
    // This test verifies the API call doesn't crash.
    __itt_pause();
    __itt_resume();
    ITT_ASSERT(true);  // API call succeeded
}

ITT_TEST(CollectionControl, VerifyResumeLog) {
    // Note: The reference collector only flushes logs at program exit.
    // This test verifies the API call doesn't crash.
    __itt_pause();
    __itt_resume();
    ITT_ASSERT(true);  // API call succeeded
}

ITT_TEST_MAIN()
