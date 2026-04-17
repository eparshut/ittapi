/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <ittapi_domain.hpp>

#include <cassert>

static void test_construct_domain()
{
    ittapi::Domain d{"test.domain"};
    (void)d.valid();
    (void)d.native_handle();
}

static void test_create_task_from_domain()
{
    ittapi::Domain d{"test.domain.task"};

    {
        auto task = d.task("my_task");
        assert(task.active());
    }
}

static void test_create_task_with_string_handle()
{
    ittapi::Domain d{"test.domain.task_sh"};
    ittapi::StringHandle name{"my_task"};

    {
        auto task = d.task(name);
        assert(task.active());
    }
}

static void test_create_region_from_domain()
{
    ittapi::Domain d{"test.domain.region"};

    {
        auto region = d.region("my_region");
        assert(region.active());
    }
}

static void test_create_frame_from_domain()
{
    ittapi::Domain d{"test.domain.frame"};

    {
        auto frame = d.frame();
        assert(frame.active());
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
