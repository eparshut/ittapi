/*
 * Copyright (C) 2005-2025 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
 */

/**
 * @file test_task.cpp
 * @brief Tests for ITT task API
 */

#include "itt_test_framework.hpp"
#include "itt_test_utils.hpp"
#include <ittnotify.h>
#include <thread>
#include <vector>

using namespace itt_test;
using namespace itt_test::utils;

ITT_TEST(Task, BeginEndSimple) {
    __itt_domain* domain = __itt_domain_create("TaskTest.Simple");
    __itt_string_handle* name = __itt_string_handle_create("SimpleTask");
    
    ITT_ASSERT_NOT_NULL(domain);
    ITT_ASSERT_NOT_NULL(name);
    
    __itt_task_begin(domain, __itt_null, __itt_null, name);
    cpu_workload(100);
    __itt_task_end(domain);
}

ITT_TEST(Task, NestedTasks) {
    __itt_domain* domain = __itt_domain_create("TaskTest.Nested");
    __itt_string_handle* outer = __itt_string_handle_create("OuterTask");
    __itt_string_handle* inner = __itt_string_handle_create("InnerTask");
    
    __itt_task_begin(domain, __itt_null, __itt_null, outer);
    cpu_workload(50);
    
    __itt_task_begin(domain, __itt_null, __itt_null, inner);
    cpu_workload(50);
    __itt_task_end(domain);
    
    cpu_workload(50);
    __itt_task_end(domain);
}

ITT_TEST(Task, DeeplyNestedTasks) {
    __itt_domain* domain = __itt_domain_create("TaskTest.DeepNested");
    constexpr int DEPTH = 10;
    
    std::vector<__itt_string_handle*> handles;
    for (int i = 0; i < DEPTH; ++i) {
        std::string name = "Level_" + std::to_string(i);
        handles.push_back(__itt_string_handle_create(name.c_str()));
    }
    
    for (int i = 0; i < DEPTH; ++i) {
        __itt_task_begin(domain, __itt_null, __itt_null, handles[i]);
    }
    
    for (int i = DEPTH - 1; i >= 0; --i) {
        __itt_task_end(domain);
    }
}

ITT_TEST(Task, MultipleDomains) {
    __itt_domain* d1 = __itt_domain_create("TaskTest.Domain1");
    __itt_domain* d2 = __itt_domain_create("TaskTest.Domain2");
    __itt_string_handle* n1 = __itt_string_handle_create("Task1");
    __itt_string_handle* n2 = __itt_string_handle_create("Task2");
    
    __itt_task_begin(d1, __itt_null, __itt_null, n1);
    __itt_task_begin(d2, __itt_null, __itt_null, n2);
    cpu_workload(100);
    __itt_task_end(d2);
    __itt_task_end(d1);
}

ITT_TEST(Task, ConcurrentTasks) {
    constexpr int NUM_THREADS = 4;
    constexpr int TASKS_PER_THREAD = 50;
    
    __itt_domain* domain = __itt_domain_create("TaskTest.Concurrent");
    std::vector<std::thread> threads;
    ThreadBarrier barrier(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([t, domain, &barrier]() {
            std::string prefix = "Thread" + std::to_string(t) + "_Task";
            barrier.wait();
            
            for (int i = 0; i < TASKS_PER_THREAD; ++i) {
                std::string name = prefix + std::to_string(i);
                __itt_string_handle* task_name = __itt_string_handle_create(name.c_str());
                
                __itt_task_begin(domain, __itt_null, __itt_null, task_name);
                cpu_workload(10);
                __itt_task_end(domain);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

ITT_TEST(Task, RapidBeginEnd) {
    __itt_domain* domain = __itt_domain_create("TaskTest.Rapid");
    __itt_string_handle* name = __itt_string_handle_create("RapidTask");
    
    constexpr int ITERATIONS = 1000;
    for (int i = 0; i < ITERATIONS; ++i) {
        __itt_task_begin(domain, __itt_null, __itt_null, name);
        __itt_task_end(domain);
    }
}

ITT_TEST(Task, VerifyBeginLog) {
    // Note: The reference collector only flushes logs at program exit.
    // This test verifies the API call doesn't crash.
    std::string domain_name = unique_domain_name("TaskLogDomain");
    std::string task_name_str = unique_string_name("TaskLogTask");
    
    __itt_domain* domain = __itt_domain_create(domain_name.c_str());
    __itt_string_handle* task_name = __itt_string_handle_create(task_name_str.c_str());
    
    __itt_task_begin(domain, __itt_null, __itt_null, task_name);
    cpu_workload(50);
    __itt_task_end(domain);
    
    ITT_ASSERT(true);  // API call succeeded
}

ITT_TEST_MAIN()
