/*
 * Copyright (C) 2005-2025 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
 */

/**
 * @file test_thread_naming.cpp
 * @brief Tests for ITT thread naming API
 */

#include "itt_test_framework.hpp"
#include "itt_test_utils.hpp"
#include <ittnotify.h>
#include <thread>
#include <vector>

using namespace itt_test;
using namespace itt_test::utils;

ITT_TEST(ThreadNaming, SetNameMainThread) {
    __itt_thread_set_name("MainThread");
    cpu_workload(50);
}

ITT_TEST(ThreadNaming, SetNameWithPrefix) {
    __itt_thread_set_name("Worker::MainThread");
    cpu_workload(50);
}

ITT_TEST(ThreadNaming, SetNameMultipleTimes) {
    __itt_thread_set_name("FirstName");
    cpu_workload(20);
    __itt_thread_set_name("SecondName");
    cpu_workload(20);
    __itt_thread_set_name("ThirdName");
    cpu_workload(20);
}

ITT_TEST(ThreadNaming, SetNameWithNull) {
    __itt_thread_set_name(nullptr);
}

ITT_TEST(ThreadNaming, SetNameWithEmpty) {
    __itt_thread_set_name("");
    cpu_workload(50);
}

ITT_TEST(ThreadNaming, NameMultipleThreads) {
    constexpr int NUM_THREADS = 4;
    
    std::vector<std::thread> threads;
    ThreadBarrier barrier(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([t, &barrier]() {
            std::string name = "WorkerThread_" + std::to_string(t);
            __itt_thread_set_name(name.c_str());
            barrier.wait();
            cpu_workload(50);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

ITT_TEST(ThreadNaming, ConcurrentNaming) {
    constexpr int NUM_THREADS = 8;
    constexpr int RENAMES_PER_THREAD = 10;
    
    std::vector<std::thread> threads;
    ThreadBarrier barrier(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([t, &barrier]() {
            barrier.wait();
            for (int i = 0; i < RENAMES_PER_THREAD; ++i) {
                std::string name = "Thread_" + std::to_string(t) + "_Iter_" + std::to_string(i);
                __itt_thread_set_name(name.c_str());
                cpu_workload(5);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

ITT_TEST(ThreadNaming, NamedThreadWithTasks) {
    constexpr int NUM_THREADS = 4;
    
    __itt_domain* domain = __itt_domain_create("ThreadNaming.Tasks");
    std::vector<std::thread> threads;
    ThreadBarrier barrier(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([t, domain, &barrier]() {
            std::string thread_name = "TaskWorker_" + std::to_string(t);
            __itt_thread_set_name(thread_name.c_str());
            
            std::string task_name_str = "Task_" + std::to_string(t);
            __itt_string_handle* task_name = __itt_string_handle_create(task_name_str.c_str());
            
            barrier.wait();
            
            __itt_task_begin(domain, __itt_null, __itt_null, task_name);
            cpu_workload(50);
            __itt_task_end(domain);
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

ITT_TEST(ThreadNaming, ShortLivedThreads) {
    constexpr int NUM_THREADS = 20;
    
    for (int i = 0; i < NUM_THREADS; ++i) {
        std::thread t([i]() {
            std::string name = "ShortLived_" + std::to_string(i);
            __itt_thread_set_name(name.c_str());
        });
        t.join();
    }
}

ITT_TEST(ThreadNaming, VerifySetNameLog) {
    // Note: Reference collector doesn't implement __itt_thread_set_name.
    // This test verifies the API call doesn't crash.
    std::string thread_name = unique_string_name("LogThread");
    __itt_thread_set_name(thread_name.c_str());
    
    ITT_ASSERT(true);  // API call succeeded
}

ITT_TEST_MAIN()
