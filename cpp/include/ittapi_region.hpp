/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#ifndef ITTAPI_REGION_HPP
#define ITTAPI_REGION_HPP

#include <string>
#include <string_view>

#include "ittapi_utils.hpp"
#include "ittapi_string_handle.hpp"

namespace ittapi
{

class Domain;

class ScopedRegion
{
public:
    ScopedRegion(const __itt_domain* domain, std::string_view name)
        : m_domain(domain)
        , m_id(detail::make_null_id())
        , m_active(true)
    {
        __itt_string_handle* h = detail::get_or_create_string_handle(name);
        __itt_region_begin(m_domain, m_id, detail::make_null_id(), h);
    }

    ScopedRegion(const __itt_domain* domain, std::string_view name,
                 __itt_id id, __itt_id parentid)
        : m_domain(domain)
        , m_id(id)
        , m_active(true)
    {
        __itt_string_handle* h = detail::get_or_create_string_handle(name);
        __itt_region_begin(m_domain, m_id, parentid, h);
    }

#if ITT_PLATFORM == ITT_PLATFORM_WIN
    ScopedRegion(const __itt_domain* domain, std::wstring_view name)
        : m_domain(domain)
        , m_id(detail::make_null_id())
        , m_active(true)
    {
        __itt_string_handle* h = detail::get_or_create_string_handle(name);
        __itt_region_begin(m_domain, m_id, detail::make_null_id(), h);
    }

    ScopedRegion(const __itt_domain* domain, std::wstring_view name,
                 __itt_id id, __itt_id parentid)
        : m_domain(domain)
        , m_id(id)
        , m_active(true)
    {
        __itt_string_handle* h = detail::get_or_create_string_handle(name);
        __itt_region_begin(m_domain, m_id, parentid, h);
    }
#endif

    ScopedRegion(const __itt_domain* domain, const StringHandle& name) noexcept
        : m_domain(domain)
        , m_id(detail::make_null_id())
        , m_active(true)
    {
        __itt_region_begin(m_domain, m_id, detail::make_null_id(), name.get());
    }

    ScopedRegion(const __itt_domain* domain, const StringHandle& name,
                 __itt_id id, __itt_id parentid) noexcept
        : m_domain(domain)
        , m_id(id)
        , m_active(true)
    {
        __itt_region_begin(m_domain, m_id, parentid, name.get());
    }

    ScopedRegion(const ScopedRegion&) = delete;
    ScopedRegion& operator=(const ScopedRegion&) = delete;
    ScopedRegion& operator=(ScopedRegion&&) = delete;

    ScopedRegion(ScopedRegion&& other) noexcept
        : m_domain(other.m_domain)
        , m_id(other.m_id)
        , m_active(other.m_active)
    {
        other.m_active = false;
    }

    ~ScopedRegion() noexcept
    {
        end();
    }

    void end() noexcept
    {
        if (m_active)
        {
            __itt_region_end(m_domain, m_id);
            m_active = false;
        }
    }

    bool active() const noexcept
    {
        return m_active;
    }

private:
    const __itt_domain* m_domain = nullptr;
    __itt_id m_id{};
    bool m_active = false;
};

}  // namespace ittapi

#endif  // ITTAPI_REGION_HPP
