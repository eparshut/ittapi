/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <ittapi.hpp>

#include <chrono>
#include <thread>

static void simulate_work(int ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main()
{
    // Thread naming
    ittapi::set_thread_name("main");

    // Domain and string handle creation
    ittapi::Domain domain{"example.app"};
    ittapi::StringHandle task_name{"process"};

    // Collection control — pause/resume
    ittapi::pause();
    ittapi::resume();

    // Scoped task with pre-created StringHandle (zero-overhead path)
    {
        auto task = domain.task(task_name);
        simulate_work(10);
    }

    // Scoped task with inline string (convenience path)
    {
        auto task = domain.task("startup");
        simulate_work(10);
    }

    // Scoped task with early end
    {
        auto task = domain.task("partial_work");
        simulate_work(5);
        task.end();  // end early, destructor is a no-op
    }

    // Scoped task with IDs for parent-child relationships
    {
        __itt_id parent_id = __itt_id_make(nullptr, 1);
        auto parent = domain.task("parent_task", parent_id, __itt_null);

        __itt_id child_id = __itt_id_make(nullptr, 2);
        auto child = domain.task("child_task", child_id, parent_id);
        simulate_work(5);
    }

    // Manual task begin/end (non-RAII)
    domain.task_begin("manual_work");
    simulate_work(5);
    domain.task_end();

    // Scoped region
    {
        auto region = domain.region("init_phase");
        simulate_work(10);
    }

    // Scoped frame
    {
        auto frame = domain.frame();
        simulate_work(10);
    }

    // Frame submit with explicit timestamps
    {
        __itt_timestamp begin = __itt_get_timestamp();
        simulate_work(10);
        __itt_timestamp end = __itt_get_timestamp();
        ittapi::ScopedFrame::submit(domain.get(), begin, end);
    }

    // Scoped pause — collection paused within scope
    {
        ittapi::ScopedPause sp;
        simulate_work(10);  // not collected
        sp.resume_now();    // resume early
        simulate_work(10);  // collected
    }

    return 0;
}
