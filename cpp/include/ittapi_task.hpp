/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#ifndef ITTAPI_TASK_HPP
#define ITTAPI_TASK_HPP

#include <string>
#include <string_view>

#include "ittapi_utils.hpp"
#include "ittapi_string_handle.hpp"

namespace ittapi
{

class Domain;

class ScopedTask
{
public:
    ScopedTask(const __itt_domain* domain, std::string_view name)
        : m_domain(domain)
        , m_active(true)
    {
        __itt_string_handle* h = detail::get_or_create_string_handle(name);
        __itt_task_begin(m_domain, detail::make_null_id(), detail::make_null_id(), h);
    }

    ScopedTask(const __itt_domain* domain, std::string_view name,
               __itt_id taskid, __itt_id parentid)
        : m_domain(domain)
        , m_active(true)
    {
        __itt_string_handle* h = detail::get_or_create_string_handle(name);
        __itt_task_begin(m_domain, taskid, parentid, h);
    }

#if ITT_PLATFORM == ITT_PLATFORM_WIN
    ScopedTask(const __itt_domain* domain, std::wstring_view name)
        : m_domain(domain)
        , m_active(true)
    {
        __itt_string_handle* h = detail::get_or_create_string_handle(name);
        __itt_task_begin(m_domain, detail::make_null_id(), detail::make_null_id(), h);
    }

    ScopedTask(const __itt_domain* domain, std::wstring_view name,
               __itt_id taskid, __itt_id parentid)
        : m_domain(domain)
        , m_active(true)
    {
        __itt_string_handle* h = detail::get_or_create_string_handle(name);
        __itt_task_begin(m_domain, taskid, parentid, h);
    }
#endif

    ScopedTask(const __itt_domain* domain, const StringHandle& name) noexcept
        : m_domain(domain)
        , m_active(true)
    {
        __itt_task_begin(m_domain, detail::make_null_id(), detail::make_null_id(), name.get());
    }

    ScopedTask(const __itt_domain* domain, const StringHandle& name,
               __itt_id taskid, __itt_id parentid) noexcept
        : m_domain(domain)
        , m_active(true)
    {
        __itt_task_begin(m_domain, taskid, parentid, name.get());
    }

    ScopedTask(const ScopedTask&) = delete;
    ScopedTask& operator=(const ScopedTask&) = delete;
    ScopedTask& operator=(ScopedTask&&) = delete;

    ScopedTask(ScopedTask&& other) noexcept
        : m_domain(other.m_domain)
        , m_active(other.m_active)
    {
        other.m_active = false;
    }

    ~ScopedTask() noexcept
    {
        end();
    }

    void end() noexcept
    {
        if (m_active)
        {
            __itt_task_end(m_domain);
            m_active = false;
        }
    }

    bool active() const noexcept
    {
        return m_active;
    }

private:
    const __itt_domain* m_domain = nullptr;
    bool m_active = false;
};

}  // namespace ittapi

#endif  // ITTAPI_TASK_HPP
