/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <ittapi.hpp>

#include <cassert>

static void test_umbrella_header_compiles()
{
    ittapi::Domain d{"test.umbrella"};
    ittapi::StringHandle sh{"umbrella_handle"};

    ittapi::set_thread_name("umbrella_thread");
    ittapi::pause();
    ittapi::resume();

    {
        auto task = d.task("umbrella_task");
        assert(task.active());
    }

    {
        auto region = d.region(sh);
        assert(region.active());
    }

    {
        auto frame = d.frame();
        assert(frame.active());
    }

    {
        ittapi::ScopedPause sp;
        assert(sp.active());
        sp.resume_now();
        assert(!sp.active());
    }

    d.task_begin("manual");
    d.task_end();
}

int main()
{
    test_umbrella_header_compiles();
    return 0;
}
