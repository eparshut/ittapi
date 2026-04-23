/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <ittapi_domain.hpp>
#include <ittapi_task.hpp>
#include "test_helpers.hpp"

#include <utility>

static void test_scoped_task_lifecycle()
{
    ittapi::Domain d{"test.task.lifecycle"};
    ittapi::test::check_domain_name(d, "test.task.lifecycle");

    {
        auto task = d.task("lifecycle_task");
        CHECK(task.active());
    }
    // destructor should have ended the task
}

static void test_explicit_end_is_idempotent()
{
    ittapi::Domain d{"test.task.end"};
    auto task = d.task("end_task");
    CHECK(task.active());
    task.end();
    CHECK(!task.active());
    task.end();  // second call should be safe
    CHECK(!task.active());
}

static void test_move_construction()
{
    ittapi::Domain d{"test.task.move"};
    auto task1 = d.task("move_task");
    CHECK(task1.active());

    auto task2 = std::move(task1);
    CHECK(!task1.active());
    CHECK(task2.active());
}

static void test_string_handle_overload()
{
    ittapi::Domain d{"test.task.sh"};
    ittapi::StringHandle name{"sh_task"};
    ittapi::test::check_string_handle_name(name, "sh_task");

    {
        auto task = d.task(name);
        CHECK(task.active());
    }
}

static void test_manual_task_begin_end()
{
    ittapi::Domain d{"test.task.manual"};
    d.task_begin("manual_task");
    d.task_end();
}

static void test_manual_task_begin_end_string_handle()
{
    ittapi::Domain d{"test.task.manual_sh"};
    ittapi::StringHandle name{"manual_sh_task"};
    d.task_begin(name);
    d.task_end();
}

static void test_scoped_task_with_ids()
{
    ittapi::Domain d{"test.task.ids"};
    __itt_id taskid = __itt_id_make(nullptr, 1);
    __itt_id parentid = __itt_null;

    ittapi::test::check_id_is_null(parentid);

    {
        auto task = d.task("task_with_ids", taskid, parentid);
        CHECK(task.active());
    }
}

static void test_scoped_task_with_ids_string_handle()
{
    ittapi::Domain d{"test.task.ids_sh"};
    ittapi::StringHandle name{"sh_task_ids"};
    ittapi::test::check_string_handle_name(name, "sh_task_ids");
    __itt_id taskid = __itt_id_make(nullptr, 2);
    __itt_id parentid = __itt_null;

    {
        auto task = d.task(name, taskid, parentid);
        CHECK(task.active());
    }
}

static void test_manual_task_begin_end_with_ids()
{
    ittapi::Domain d{"test.task.manual_ids"};
    __itt_id taskid = __itt_id_make(nullptr, 3);
    __itt_id parentid = __itt_null;

    d.task_begin("manual_ids_task", taskid, parentid);
    d.task_end(taskid);
}

static void test_overlapped_tasks_interleaved()
{
    ittapi::Domain d{"test.task.overlapped"};
    __itt_id id1 = __itt_id_make(nullptr, 100);
    __itt_id id2 = __itt_id_make(nullptr, 200);

    // Start parent, start child, end parent, end child — only valid with overlapped
    auto parent = d.task("parent", id1, __itt_null);
    auto child = d.task("child", id2, id1);

    CHECK(parent.active());
    CHECK(child.active());

    parent.end();
    CHECK(!parent.active());
    CHECK(child.active());

    child.end();
    CHECK(!child.active());
}

static void test_overlapped_manual_interleaved()
{
    ittapi::Domain d{"test.task.overlapped_manual"};
    __itt_id id1 = __itt_id_make(nullptr, 300);
    __itt_id id2 = __itt_id_make(nullptr, 400);

    d.task_begin("first", id1, __itt_null);
    d.task_begin("second", id2, __itt_null);
    d.task_end(id1);
    d.task_end(id2);
}

static void test_overlapped_auto_id()
{
    ittapi::Domain d{"test.task.auto_id"};
    auto task = d.task("work", true);
    CHECK(task.active());
    __itt_id tid = task.id();
    CHECK(tid.d2 != 0);
}

static void test_overlapped_auto_id_parent_child()
{
    ittapi::Domain d{"test.task.auto_parent_child"};
    auto parent = d.task("parent", true);
    auto child = d.task("child", true, parent.id());

    CHECK(parent.active());
    CHECK(child.active());
    CHECK(parent.id().d2 != child.id().d2);

    parent.end();
    CHECK(!parent.active());
    CHECK(child.active());

    child.end();
    CHECK(!child.active());
}

static void test_non_overlapped_id_is_null()
{
    ittapi::Domain d{"test.task.non_overlapped_id"};
    auto task = d.task("simple");
    __itt_id tid = task.id();
    CHECK(tid.d1 == 0 && tid.d2 == 0 && tid.d3 == 0);
}

int main()
{
    test_scoped_task_lifecycle();
    test_explicit_end_is_idempotent();
    test_move_construction();
    test_string_handle_overload();
    test_manual_task_begin_end();
    test_manual_task_begin_end_string_handle();
    test_scoped_task_with_ids();
    test_scoped_task_with_ids_string_handle();
    test_manual_task_begin_end_with_ids();
    test_overlapped_tasks_interleaved();
    test_overlapped_manual_interleaved();
    test_overlapped_auto_id();
    test_overlapped_auto_id_parent_child();
    test_non_overlapped_id_is_null();
    return 0;
}
