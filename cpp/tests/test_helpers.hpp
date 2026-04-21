/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#pragma once

#include <ittapi_domain.hpp>
#include <ittapi_string_handle.hpp>
#include <ittnotify.h>

#include <cstdio>
#include <cstdlib>
#include <cstring>

// ---------------------------------------------------------------------------
// Assertion macros with file/line diagnostics
// ---------------------------------------------------------------------------

#define CHECK(expr)                                                    \
    do                                                                     \
    {                                                                      \
        if (!(expr))                                                       \
        {                                                                  \
            std::fprintf(stderr, "FAIL %s:%d: %s\n",                      \
                         __FILE__, __LINE__, #expr);                       \
            std::abort();                                                  \
        }                                                                  \
    } while (0)

#define CHECK_STR_EQ(actual, expected)                                 \
    do                                                                     \
    {                                                                      \
        const char* a_ = (actual);                                         \
        const char* e_ = (expected);                                       \
        if (std::strcmp(a_, e_) != 0)                                      \
        {                                                                  \
            std::fprintf(stderr, "FAIL %s:%d: expected \"%s\", got \"%s\"\n", \
                         __FILE__, __LINE__, e_, a_);                      \
            std::abort();                                                  \
        }                                                                  \
    } while (0)

// ---------------------------------------------------------------------------
// Domain / StringHandle / ID verification helpers
// ---------------------------------------------------------------------------

namespace ittapi
{
namespace test
{

inline void check_domain_name(const Domain& domain, const char* expected)
{
    if (domain.get() == nullptr)
    {
        return; // no collector attached — nothing to verify
    }
    CHECK_STR_EQ(domain.get()->nameA, expected);
}

inline void check_string_handle_name(const StringHandle& handle, const char* expected)
{
    if (handle.get() == nullptr)
    {
        return; // no collector attached — nothing to verify
    }
    CHECK_STR_EQ(handle.get()->strA, expected);
}

inline void check_id_fields(const __itt_id& id,
                             unsigned long long d1,
                             unsigned long long d2,
                             unsigned long long d3)
{
    if (id.d1 != d1 || id.d2 != d2 || id.d3 != d3)
    {
        std::fprintf(stderr,
                     "FAIL: id {%llu,%llu,%llu} != expected {%llu,%llu,%llu}\n",
                     id.d1, id.d2, id.d3, d1, d2, d3);
        std::abort();
    }
}

inline void check_id_is_null(const __itt_id& id)
{
    check_id_fields(id, 0, 0, 0);
}

} // namespace test
} // namespace ittapi
