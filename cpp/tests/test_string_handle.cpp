/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <ittapi_string_handle.hpp>

#include <cassert>
#include <string>
#include <string_view>

static void test_construct_from_literal()
{
    ittapi::StringHandle h{"test_handle"};
    // valid() may be false if no collector is attached; just verify it compiles and runs
    (void)h.valid();
    (void)h.native_handle();
}

static void test_construct_from_string_view()
{
    std::string_view sv = "test_sv_handle";
    ittapi::StringHandle h{sv};
    (void)h.valid();
    (void)h.native_handle();
}

static void test_construct_from_std_string()
{
    std::string s = "test_string_handle";
    ittapi::StringHandle h{std::string_view(s)};
    (void)h.valid();
}

int main()
{
    test_construct_from_literal();
    test_construct_from_string_view();
    test_construct_from_std_string();
    return 0;
}
