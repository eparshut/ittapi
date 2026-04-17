/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#include <ittapi_domain.hpp>
#include <ittapi_region.hpp>
#include "test_helpers.hpp"

#include <utility>

static void test_scoped_region_lifecycle()
{
    ittapi::Domain d{"test.region.lifecycle"};
    ittapi::test::check_domain_name(d, "test.region.lifecycle");

    {
        auto region = d.region("lifecycle_region");
        ITT_CHECK(region.active());
    }
}

static void test_explicit_end_is_idempotent()
{
    ittapi::Domain d{"test.region.end"};
    auto region = d.region("end_region");
    ITT_CHECK(region.active());
    region.end();
    ITT_CHECK(!region.active());
    region.end();
    ITT_CHECK(!region.active());
}

static void test_move_construction()
{
    ittapi::Domain d{"test.region.move"};
    auto r1 = d.region("move_region");
    ITT_CHECK(r1.active());

    auto r2 = std::move(r1);
    ITT_CHECK(!r1.active());
    ITT_CHECK(r2.active());
}

static void test_string_handle_overload()
{
    ittapi::Domain d{"test.region.sh"};
    ittapi::StringHandle name{"sh_region"};
    ittapi::test::check_string_handle_name(name, "sh_region");

    {
        auto region = d.region(name);
        ITT_CHECK(region.active());
    }
}

static void test_scoped_region_with_ids()
{
    ittapi::Domain d{"test.region.ids"};
    ittapi::test::check_domain_name(d, "test.region.ids");
    __itt_id id = __itt_id_make(nullptr, 10);
    __itt_id parentid = __itt_null;

    ittapi::test::check_id_is_null(parentid);

    {
        auto region = d.region("region_with_ids", id, parentid);
        ITT_CHECK(region.active());
    }
}

static void test_scoped_region_with_ids_string_handle()
{
    ittapi::Domain d{"test.region.ids_sh"};
    ittapi::StringHandle name{"sh_region_ids"};
    ittapi::test::check_string_handle_name(name, "sh_region_ids");
    __itt_id id = __itt_id_make(nullptr, 11);
    __itt_id parentid = __itt_null;

    {
        auto region = d.region(name, id, parentid);
        ITT_CHECK(region.active());
    }
}

int main()
{
    test_scoped_region_lifecycle();
    test_explicit_end_is_idempotent();
    test_move_construction();
    test_string_handle_overload();
    test_scoped_region_with_ids();
    test_scoped_region_with_ids_string_handle();
    return 0;
}
