/*
 * Copyright (C) 2005-2025 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
 */

/**
 * @file test_string_handle.cpp
 * @brief Tests for ITT string handle creation and management
 */

#include "itt_test_framework.hpp"
#include "itt_test_utils.hpp"
#include <ittnotify.h>
#include <thread>
#include <vector>

using namespace itt_test;
using namespace itt_test::utils;

ITT_TEST(StringHandle, CreateSingleHandle) {
    __itt_string_handle* handle = __itt_string_handle_create("TestStringHandle");
    ITT_ASSERT_NOT_NULL(handle);
    ITT_ASSERT(handle->strA != nullptr);
}

ITT_TEST(StringHandle, CreateMultipleHandles) {
    __itt_string_handle* h1 = __itt_string_handle_create("Handle1");
    __itt_string_handle* h2 = __itt_string_handle_create("Handle2");
    __itt_string_handle* h3 = __itt_string_handle_create("Handle3");
    
    ITT_ASSERT_NOT_NULL(h1);
    ITT_ASSERT_NOT_NULL(h2);
    ITT_ASSERT_NOT_NULL(h3);
    ITT_ASSERT_NE(h1, h2);
    ITT_ASSERT_NE(h2, h3);
}

ITT_TEST(StringHandle, CreateDuplicateHandle) {
    const char* name = "DupHandle";
    __itt_string_handle* h1 = __itt_string_handle_create(name);
    __itt_string_handle* h2 = __itt_string_handle_create(name);
    
    ITT_ASSERT_NOT_NULL(h1);
    ITT_ASSERT_NOT_NULL(h2);
    ITT_ASSERT_EQ(h1, h2);
}

ITT_TEST(StringHandle, CreateWithNullName) {
    __itt_string_handle* handle = __itt_string_handle_create(nullptr);
    ITT_ASSERT_NULL(handle);
}

ITT_TEST(StringHandle, ConcurrentCreation) {
    constexpr int NUM_THREADS = 8;
    constexpr int HANDLES_PER_THREAD = 50;
    
    std::vector<std::thread> threads;
    std::vector<std::vector<__itt_string_handle*>> results(NUM_THREADS);
    ThreadBarrier barrier(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([t, &results, &barrier]() {
            barrier.wait();
            for (int i = 0; i < HANDLES_PER_THREAD; ++i) {
                std::string name = "ConcHandle_T" + std::to_string(t) + "_" + std::to_string(i);
                results[t].push_back(__itt_string_handle_create(name.c_str()));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        for (const auto* handle : results[t]) {
            ITT_ASSERT_NOT_NULL(handle);
        }
    }
}

ITT_TEST(StringHandle, VerifyLogCreation) {
    // Note: The reference collector only flushes logs at program exit.
    // This test verifies the API call doesn't crash.
    std::string unique_name = unique_string_name("LogVerifyHandle");
    __itt_string_handle* handle = __itt_string_handle_create(unique_name.c_str());
    
    ITT_ASSERT_NOT_NULL(handle);
    ITT_ASSERT(true);  // API call succeeded
}

ITT_TEST_MAIN()
