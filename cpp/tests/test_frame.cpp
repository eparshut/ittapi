/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <ittapi_domain.hpp>
#include <ittapi_frame.hpp>

#include <cassert>
#include <utility>

static void test_scoped_frame_lifecycle()
{
    ittapi::Domain d{"test.frame.lifecycle"};

    {
        auto frame = d.frame();
        assert(frame.active());
    }
}

static void test_explicit_end_is_idempotent()
{
    ittapi::Domain d{"test.frame.end"};
    auto frame = d.frame();
    assert(frame.active());
    frame.end();
    assert(!frame.active());
    frame.end();
    assert(!frame.active());
}

static void test_move_construction()
{
    ittapi::Domain d{"test.frame.move"};
    auto f1 = d.frame();
    assert(f1.active());

    auto f2 = std::move(f1);
    assert(!f1.active());
    assert(f2.active());
}

static void test_submit()
{
    ittapi::Domain d{"test.frame.submit"};
    // Just verify it compiles and runs without a collector
    ittapi::ScopedFrame::submit(d.native_handle(), 0, 100);
}

int main()
{
    test_scoped_frame_lifecycle();
    test_explicit_end_is_idempotent();
    test_move_construction();
    test_submit();
    return 0;
}
