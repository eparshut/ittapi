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
        __itt_string_handle* h = detail::create_string_handle(std::string(name).c_str());
        __itt_task_begin(m_domain, detail::make_null_id(), detail::make_null_id(), h);
    }

    ScopedTask(const __itt_domain* domain, const StringHandle& name) noexcept
        : m_domain(domain)
        , m_active(true)
    {
        __itt_task_begin(m_domain, detail::make_null_id(), detail::make_null_id(), name.native_handle());
    }

    ScopedTask(const ScopedTask&) = delete;
    ScopedTask& operator=(const ScopedTask&) = delete;

    ScopedTask(ScopedTask&& other) noexcept
        : m_domain(other.m_domain)
        , m_active(other.m_active)
    {
        other.m_active = false;
    }

    ScopedTask& operator=(ScopedTask&& other) noexcept
    {
        if (this != &other)
        {
            end();
            m_domain = other.m_domain;
            m_active = other.m_active;
            other.m_active = false;
        }
        return *this;
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
