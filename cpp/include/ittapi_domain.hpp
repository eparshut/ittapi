/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#ifndef ITTAPI_DOMAIN_HPP
#define ITTAPI_DOMAIN_HPP

#include <string>
#include <string_view>

#include "ittapi_utils.hpp"
#include "ittapi_string_handle.hpp"
#include "ittapi_task.hpp"
#include "ittapi_region.hpp"
#include "ittapi_frame.hpp"

namespace ittapi
{

class Domain
{
public:
    explicit Domain(std::string_view name)
        : m_domain(detail::create_domain(std::string(name).c_str()))
    {
    }

#if ITT_PLATFORM == ITT_PLATFORM_WIN
    explicit Domain(std::wstring_view name)
        : m_domain(detail::create_domain(std::wstring(name).c_str()))
    {
    }
#endif

    __itt_domain* get() const noexcept
    {
        return m_domain;
    }

    bool valid() const noexcept
    {
        return m_domain != nullptr;
    }

    Domain(const Domain&) = default;
    Domain& operator=(const Domain&) = default;

    Domain(Domain&& other) noexcept
        : m_domain(other.m_domain)
    {
        other.m_domain = nullptr;
    }

    Domain& operator=(Domain&& other) noexcept
    {
        m_domain = other.m_domain;
        other.m_domain = nullptr;
        return *this;
    }

    [[nodiscard]] ScopedTask task(std::string_view name) const
    {
        return ScopedTask(m_domain, name);
    }

    [[nodiscard]] ScopedTask task(std::string_view name, __itt_id taskid, __itt_id parentid) const
    {
        return ScopedTask(m_domain, name, taskid, parentid);
    }

#if ITT_PLATFORM == ITT_PLATFORM_WIN
    [[nodiscard]] ScopedTask task(std::wstring_view name) const
    {
        return ScopedTask(m_domain, name);
    }

    [[nodiscard]] ScopedTask task(std::wstring_view name, __itt_id taskid, __itt_id parentid) const
    {
        return ScopedTask(m_domain, name, taskid, parentid);
    }
#endif

    [[nodiscard]] ScopedTask task(const StringHandle& name) const noexcept
    {
        return ScopedTask(m_domain, name);
    }

    [[nodiscard]] ScopedTask task(const StringHandle& name, __itt_id taskid, __itt_id parentid) const noexcept
    {
        return ScopedTask(m_domain, name, taskid, parentid);
    }

    void task_begin(std::string_view name) const
    {
        __itt_string_handle* h = detail::get_or_create_string_handle(name);
        __itt_task_begin(m_domain, detail::make_null_id(), detail::make_null_id(), h);
    }

    void task_begin(std::string_view name, __itt_id taskid, __itt_id parentid) const
    {
        __itt_string_handle* h = detail::get_or_create_string_handle(name);
        __itt_task_begin_overlapped(m_domain, taskid, parentid, h);
    }

#if ITT_PLATFORM == ITT_PLATFORM_WIN
    void task_begin(std::wstring_view name) const
    {
        __itt_string_handle* h = detail::get_or_create_string_handle(name);
        __itt_task_begin(m_domain, detail::make_null_id(), detail::make_null_id(), h);
    }

    void task_begin(std::wstring_view name, __itt_id taskid, __itt_id parentid) const
    {
        __itt_string_handle* h = detail::get_or_create_string_handle(name);
        __itt_task_begin_overlapped(m_domain, taskid, parentid, h);
    }
#endif

    void task_begin(const StringHandle& name) const noexcept
    {
        __itt_task_begin(m_domain, detail::make_null_id(), detail::make_null_id(), name.get());
    }

    void task_begin(const StringHandle& name, __itt_id taskid, __itt_id parentid) const noexcept
    {
        __itt_task_begin_overlapped(m_domain, taskid, parentid, name.get());
    }

    void task_end() const noexcept
    {
        __itt_task_end(m_domain);
    }

    void task_end(__itt_id taskid) const noexcept
    {
        __itt_task_end_overlapped(m_domain, taskid);
    }

    [[nodiscard]] ScopedRegion region(std::string_view name) const
    {
        return ScopedRegion(m_domain, name);
    }

    [[nodiscard]] ScopedRegion region(std::string_view name, __itt_id id, __itt_id parentid) const
    {
        return ScopedRegion(m_domain, name, id, parentid);
    }

#if ITT_PLATFORM == ITT_PLATFORM_WIN
    [[nodiscard]] ScopedRegion region(std::wstring_view name) const
    {
        return ScopedRegion(m_domain, name);
    }

    [[nodiscard]] ScopedRegion region(std::wstring_view name, __itt_id id, __itt_id parentid) const
    {
        return ScopedRegion(m_domain, name, id, parentid);
    }
#endif

    [[nodiscard]] ScopedRegion region(const StringHandle& name) const noexcept
    {
        return ScopedRegion(m_domain, name);
    }

    [[nodiscard]] ScopedRegion region(const StringHandle& name, __itt_id id, __itt_id parentid) const noexcept
    {
        return ScopedRegion(m_domain, name, id, parentid);
    }

    [[nodiscard]] ScopedFrame frame() const noexcept
    {
        return ScopedFrame(m_domain);
    }

private:
    __itt_domain* m_domain = nullptr;
};

}  // namespace ittapi

#endif  // ITTAPI_DOMAIN_HPP
