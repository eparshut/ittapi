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

    // Overlapped tasks with IDs — parent and child can end in any order
    {
        auto parent = domain.task("parent_task", true);
        simulate_work(5);

        auto child = domain.task("child_task", true, parent.id());
        simulate_work(5);

        parent.end();   // end parent while child is still running
        simulate_work(5);
    }                   // child ends here via destructor

    // Manual task begin/end (non-RAII, simple task)
    domain.task_begin("manual_work");
    simulate_work(5);
    domain.task_end();

    // Manual overlapped task begin/end (ID-based)
    {
        __itt_id id = __itt_id_make(nullptr, 3);
        domain.task_begin("overlapped_manual", id, __itt_null);
        simulate_work(5);
        domain.task_end(id);
    }

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
