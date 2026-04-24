/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <ittapi_domain.hpp>
#include "test_helpers.hpp"

static void test_construct_domain()
{
    ittapi::Domain d{"test.domain"};
    ittapi::test::check_domain_name(d, "test.domain");
}

static void test_create_task_from_domain()
{
    ittapi::Domain d{"test.domain.task"};
    ittapi::test::check_domain_name(d, "test.domain.task");

    {
        auto task = d.task("my_task");
        CHECK(task.active());
    }
}

static void test_create_task_with_string_handle()
{
    ittapi::Domain d{"test.domain.task_sh"};
    ittapi::StringHandle name{"my_task"};
    ittapi::test::check_string_handle_name(name, "my_task");

    {
        auto task = d.task(name);
        CHECK(task.active());
    }
}

static void test_create_region_from_domain()
{
    ittapi::Domain d{"test.domain.region"};
    ittapi::test::check_domain_name(d, "test.domain.region");

    {
        auto region = d.region("my_region");
        CHECK(region.active());
    }
}

static void test_create_frame_from_domain()
{
    ittapi::Domain d{"test.domain.frame"};
    ittapi::test::check_domain_name(d, "test.domain.frame");

    {
        auto frame = d.frame();
        CHECK(frame.active());
    }
}

int main()
{
    test_construct_domain();
    test_create_task_from_domain();
    test_create_task_with_string_handle();
    test_create_region_from_domain();
    test_create_frame_from_domain();
    return 0;
}
