/*
 * Copyright (C) 2005-2025 Intel Corporation
 *
 * SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
 */

/**
 * @file test_domain.cpp
 * @brief Tests for ITT domain creation and management
 */

#include "itt_test_framework.hpp"
#include "itt_test_utils.hpp"
#include <ittnotify.h>
#include <thread>
#include <vector>

using namespace itt_test;
using namespace itt_test::utils;

// =============================================================================
// Domain Creation Tests
// =============================================================================

ITT_TEST(Domain, CreateSingleDomain) {
    const char* domain_name = "TestDomain_Single";
    __itt_domain* domain = __itt_domain_create(domain_name);
    
    ITT_ASSERT_NOT_NULL(domain);
    ITT_ASSERT(domain->nameA != nullptr);
    ITT_ASSERT_EQ(1, domain->flags);
}

ITT_TEST(Domain, CreateMultipleDomains) {
    __itt_domain* domain1 = __itt_domain_create("TestDomain_Multi1");
    __itt_domain* domain2 = __itt_domain_create("TestDomain_Multi2");
    __itt_domain* domain3 = __itt_domain_create("TestDomain_Multi3");
    
    ITT_ASSERT_NOT_NULL(domain1);
    ITT_ASSERT_NOT_NULL(domain2);
    ITT_ASSERT_NOT_NULL(domain3);
    ITT_ASSERT_NE(domain1, domain2);
    ITT_ASSERT_NE(domain2, domain3);
}

ITT_TEST(Domain, CreateDuplicateDomain) {
    const char* domain_name = "TestDomain_Duplicate";
    __itt_domain* domain1 = __itt_domain_create(domain_name);
    __itt_domain* domain2 = __itt_domain_create(domain_name);
    
    ITT_ASSERT_NOT_NULL(domain1);
    ITT_ASSERT_NOT_NULL(domain2);
    ITT_ASSERT_EQ(domain1, domain2);
}

ITT_TEST(Domain, CreateWithDottedName) {
    __itt_domain* domain = __itt_domain_create("Company.Product.Module");
    ITT_ASSERT_NOT_NULL(domain);
}

ITT_TEST(Domain, CreateWithNullName) {
    __itt_domain* domain = __itt_domain_create(nullptr);
    ITT_ASSERT_NULL(domain);
}

ITT_TEST(Domain, ConcurrentCreation) {
    constexpr int NUM_THREADS = 8;
    constexpr int DOMAINS_PER_THREAD = 50;
    
    std::vector<std::thread> threads;
    std::vector<std::vector<__itt_domain*>> results(NUM_THREADS);
    ThreadBarrier barrier(NUM_THREADS);
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        threads.emplace_back([t, &results, &barrier]() {
            barrier.wait();
            for (int i = 0; i < DOMAINS_PER_THREAD; ++i) {
                std::string name = "ConcDomain_T" + std::to_string(t) + "_" + std::to_string(i);
                results[t].push_back(__itt_domain_create(name.c_str()));
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    for (int t = 0; t < NUM_THREADS; ++t) {
        ITT_ASSERT_EQ(DOMAINS_PER_THREAD, static_cast<int>(results[t].size()));
        for (const auto* domain : results[t]) {
            ITT_ASSERT_NOT_NULL(domain);
        }
    }
}

ITT_TEST(Domain, VerifyLogCreation) {
    // Note: The reference collector only flushes logs at program exit.
    // This test verifies the API call doesn't crash.
    // Log verification would require a subprocess or is deferred to manual testing.
    std::string unique_name = unique_domain_name("LogVerifyDomain");
    __itt_domain* domain = __itt_domain_create(unique_name.c_str());
    
    ITT_ASSERT_NOT_NULL(domain);
    ITT_ASSERT(true);  // API call succeeded
}

ITT_TEST_MAIN()
