/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <ittapi.hpp>
#include "test_helpers.hpp"

static void test_umbrella_header_compiles()
{
    ittapi::Domain d{"test.umbrella"};
    ittapi::StringHandle sh{"umbrella_handle"};

    ittapi::test::check_domain_name(d, "test.umbrella");
    ittapi::test::check_string_handle_name(sh, "umbrella_handle");

    ittapi::set_thread_name("umbrella_thread");
    ittapi::pause();
    ittapi::resume();

    {
        auto task = d.task("umbrella_task");
        CHECK(task.active());
    }

    {
        auto region = d.region(sh);
        CHECK(region.active());
    }

    {
        auto frame = d.frame();
        CHECK(frame.active());
    }

    {
        ittapi::ScopedPause sp;
        CHECK(sp.active());
        sp.resume_now();
        CHECK(!sp.active());
    }

    d.task_begin("manual");
    d.task_end();
}

int main()
{
    test_umbrella_header_compiles();
    return 0;
}
