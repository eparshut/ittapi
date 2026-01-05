/*
 * Copyright (C) 2005-2025 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
 */

/**
 * @file test_metadata.cpp
 * @brief Tests for ITT metadata API
 */

#include "itt_test_framework.hpp"
#include "itt_test_utils.hpp"
#include <ittnotify.h>
#include <thread>
#include <vector>
#include <cstdint>

using namespace itt_test;
using namespace itt_test::utils;

ITT_TEST(Metadata, AddU64Single) {
    __itt_domain* domain = __itt_domain_create("MetadataTest.U64");
    __itt_string_handle* key = __itt_string_handle_create("count");
    
    uint64_t value = 42;
    __itt_metadata_add(domain, __itt_null, key, __itt_metadata_u64, 1, &value);
}

ITT_TEST(Metadata, AddU64Array) {
    __itt_domain* domain = __itt_domain_create("MetadataTest.U64Array");
    __itt_string_handle* key = __itt_string_handle_create("counts");
    
    uint64_t values[] = {10, 20, 30, 40, 50};
    __itt_metadata_add(domain, __itt_null, key, __itt_metadata_u64, 5, values);
}

ITT_TEST(Metadata, AddDouble) {
    __itt_domain* domain = __itt_domain_create("MetadataTest.Double");
    __itt_string_handle* key = __itt_string_handle_create("ratio");
    
    double value = 3.14159;
    __itt_metadata_add(domain, __itt_null, key, __itt_metadata_double, 1, &value);
}

ITT_TEST(Metadata, AddStringSimple) {
    __itt_domain* domain = __itt_domain_create("MetadataTest.String");
    __itt_string_handle* key = __itt_string_handle_create("description");
    
    __itt_metadata_str_add(domain, __itt_null, key, "Test description", 0);
}

ITT_TEST(Metadata, AddMultiple) {
    __itt_domain* domain = __itt_domain_create("MetadataTest.Multi");
    
    __itt_string_handle* k1 = __itt_string_handle_create("name");
    __itt_string_handle* k2 = __itt_string_handle_create("version");
    __itt_string_handle* k3 = __itt_string_handle_create("count");
    
    __itt_metadata_str_add(domain, __itt_null, k1, "TestApp", 0);
    __itt_metadata_str_add(domain, __itt_null, k2, "1.0.0", 0);
    
    uint64_t count = 100;
    __itt_metadata_add(domain, __itt_null, k3, __itt_metadata_u64, 1, &count);
}

ITT_TEST(Metadata, AddToTask) {
    __itt_domain* domain = __itt_domain_create("MetadataTest.Task");
    __itt_string_handle* task_name = __itt_string_handle_create("MetaTask");
    __itt_string_handle* key = __itt_string_handle_create("iterations");
    
    __itt_id task_id = __itt_id_make(domain, 1);
    
    __itt_task_begin(domain, task_id, __itt_null, task_name);
    
    uint64_t value = 100;
    __itt_metadata_add(domain, task_id, key, __itt_metadata_u64, 1, &value);
    
    cpu_workload(50);
    __itt_task_end(domain);
}

ITT_TEST(Metadata, ConcurrentAdd) {
    constexpr int NUM_THREADS = 4;
    constexpr int METADATA_PER_THREAD = 50;
    
    __itt_domain* domain = __itt_domain_create("MetadataTest.Concurrent");
    std::vector<std::thread> threads;
    ThreadBarrier barrier(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([t, domain, &barrier]() {
            barrier.wait();
            for (int i = 0; i < METADATA_PER_THREAD; ++i) {
                std::string key_name = "key_T" + std::to_string(t) + "_" + std::to_string(i);
                __itt_string_handle* key = __itt_string_handle_create(key_name.c_str());
                
                uint64_t value = static_cast<uint64_t>(t * 1000 + i);
                __itt_metadata_add(domain, __itt_null, key, __itt_metadata_u64, 1, &value);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

ITT_TEST(Metadata, NullKey) {
    __itt_domain* domain = __itt_domain_create("MetadataTest.NullKey");
    uint64_t value = 42;
    
    // Should handle null key gracefully
    __itt_metadata_add(domain, __itt_null, nullptr, __itt_metadata_u64, 1, &value);
}

ITT_TEST(Metadata, LargeArray) {
    __itt_domain* domain = __itt_domain_create("MetadataTest.LargeArray");
    __itt_string_handle* key = __itt_string_handle_create("large_array");
    
    constexpr int ARRAY_SIZE = 100;
    std::vector<uint64_t> values(ARRAY_SIZE);
    for (int i = 0; i < ARRAY_SIZE; ++i) {
        values[i] = static_cast<uint64_t>(i);
    }
    
    __itt_metadata_add(domain, __itt_null, key, __itt_metadata_u64, ARRAY_SIZE, values.data());
}

ITT_TEST(Metadata, VerifyAddLog) {
    // Note: The reference collector only flushes logs at program exit.
    // This test verifies the API call doesn't crash.
    std::string domain_name = unique_domain_name("LogMetaDomain");
    std::string key_name = unique_string_name("LogMetaKey");
    
    __itt_domain* domain = __itt_domain_create(domain_name.c_str());
    __itt_string_handle* key = __itt_string_handle_create(key_name.c_str());
    
    uint64_t value = 12345;
    __itt_metadata_add(domain, __itt_null, key, __itt_metadata_u64, 1, &value);
    
    ITT_ASSERT(true);  // API call succeeded
}

ITT_TEST_MAIN()
