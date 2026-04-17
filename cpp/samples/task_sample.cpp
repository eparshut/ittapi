/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <ittapi.hpp>

#include <chrono>
#include <thread>

int main()
{
    ittapi::set_thread_name("main");
    ittapi::Domain domain{"example.task"};
    ittapi::StringHandle task_name{"process"};

    ittapi::pause();
    ittapi::resume();


    {
        auto task = domain.task(task_name);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }


    {
        auto task = domain.task("startup");
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
