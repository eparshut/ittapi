/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <ittapi_collection_control.hpp>

#include <cassert>
#include <utility>

static void test_pause_resume()
{
    ittapi::pause();
    ittapi::resume();
}

static void test_scoped_pause_resumes_on_destruction()
{

    {
        ittapi::ScopedPause sp;
        assert(sp.active());
    }
    // destructor should have called resume()
}

static void test_resume_now_disables_destructor()
{
    ittapi::ScopedPause sp;
    assert(sp.active());
    sp.resume_now();
    assert(!sp.active());
    // destructor should not call resume() again
}

static void test_move_construction()
{
    ittapi::ScopedPause sp1;
    assert(sp1.active());

    auto sp2 = std::move(sp1);
    assert(!sp1.active());
    assert(sp2.active());
}

int main()
{
    test_pause_resume();
    test_scoped_pause_resumes_on_destruction();
    test_resume_now_disables_destructor();
    test_move_construction();
    return 0;
}
