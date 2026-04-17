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
        : m_handle(detail::create_domain(std::string(name).c_str()))
    {
    }

#if ITT_PLATFORM == ITT_PLATFORM_WIN
    explicit Domain(std::wstring_view name)
        : m_handle(detail::create_domain(std::wstring(name).c_str()))
    {
    }
#endif

    __itt_domain* native_handle() const noexcept
    {
        return m_handle;
    }

    bool valid() const noexcept
    {
        return m_handle != nullptr;
    }

    ScopedTask task(std::string_view name) const
    {
        return ScopedTask(m_handle, name);
    }

    ScopedTask task(std::string_view name, __itt_id taskid, __itt_id parentid) const
    {
        return ScopedTask(m_handle, name, taskid, parentid);
    }

#if ITT_PLATFORM == ITT_PLATFORM_WIN
    ScopedTask task(std::wstring_view name) const
    {
        return ScopedTask(m_handle, name);
    }

    ScopedTask task(std::wstring_view name, __itt_id taskid, __itt_id parentid) const
    {
        return ScopedTask(m_handle, name, taskid, parentid);
    }
#endif

    ScopedTask task(const StringHandle& name) const noexcept
    {
        return ScopedTask(m_handle, name);
    }

    ScopedTask task(const StringHandle& name, __itt_id taskid, __itt_id parentid) const noexcept
    {
        return ScopedTask(m_handle, name, taskid, parentid);
    }

    void task_begin(std::string_view name) const
    {
        __itt_string_handle* h = detail::create_string_handle(std::string(name).c_str());
        __itt_task_begin(m_handle, detail::make_null_id(), detail::make_null_id(), h);
    }

    void task_begin(std::string_view name, __itt_id taskid, __itt_id parentid) const
    {
        __itt_string_handle* h = detail::create_string_handle(std::string(name).c_str());
        __itt_task_begin(m_handle, taskid, parentid, h);
    }

#if ITT_PLATFORM == ITT_PLATFORM_WIN
    void task_begin(std::wstring_view name) const
    {
        __itt_string_handle* h = detail::create_string_handle(std::wstring(name).c_str());
        __itt_task_begin(m_handle, detail::make_null_id(), detail::make_null_id(), h);
    }

    void task_begin(std::wstring_view name, __itt_id taskid, __itt_id parentid) const
    {
        __itt_string_handle* h = detail::create_string_handle(std::wstring(name).c_str());
        __itt_task_begin(m_handle, taskid, parentid, h);
    }
#endif

    void task_begin(const StringHandle& name) const noexcept
    {
        __itt_task_begin(m_handle, detail::make_null_id(), detail::make_null_id(), name.native_handle());
    }

    void task_begin(const StringHandle& name, __itt_id taskid, __itt_id parentid) const noexcept
    {
        __itt_task_begin(m_handle, taskid, parentid, name.native_handle());
    }

    void task_end() const noexcept
    {
        __itt_task_end(m_handle);
    }

    ScopedRegion region(std::string_view name) const
    {
        return ScopedRegion(m_handle, name);
    }

    ScopedRegion region(std::string_view name, __itt_id id, __itt_id parentid) const
    {
        return ScopedRegion(m_handle, name, id, parentid);
    }

#if ITT_PLATFORM == ITT_PLATFORM_WIN
    ScopedRegion region(std::wstring_view name) const
    {
        return ScopedRegion(m_handle, name);
    }

    ScopedRegion region(std::wstring_view name, __itt_id id, __itt_id parentid) const
    {
        return ScopedRegion(m_handle, name, id, parentid);
    }
#endif

    ScopedRegion region(const StringHandle& name) const noexcept
    {
        return ScopedRegion(m_handle, name);
    }

    ScopedRegion region(const StringHandle& name, __itt_id id, __itt_id parentid) const noexcept
    {
        return ScopedRegion(m_handle, name, id, parentid);
    }

    ScopedFrame frame() const noexcept
    {
        return ScopedFrame(m_handle);
    }

private:
    __itt_domain* m_handle = nullptr;
};

}  // namespace ittapi

#endif  // ITTAPI_DOMAIN_HPP
