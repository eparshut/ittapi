/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <ittapi_thread_naming.hpp>

static void test_set_thread_name()
{
    ittapi::set_thread_name("test_thread");
}

int main()
{
    test_set_thread_name();
    return 0;
}
