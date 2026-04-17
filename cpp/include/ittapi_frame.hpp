/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#ifndef ITTAPI_FRAME_HPP
#define ITTAPI_FRAME_HPP

#include "ittapi_utils.hpp"

namespace ittapi
{

class Domain;

class ScopedFrame
{
public:
    explicit ScopedFrame(const __itt_domain* domain) noexcept
        : m_domain(domain)
        , m_id(detail::make_null_id())
        , m_active(true)
    {
        __itt_frame_begin_v3(m_domain, &m_id);
    }

    ScopedFrame(const ScopedFrame&) = delete;
    ScopedFrame& operator=(const ScopedFrame&) = delete;

    ScopedFrame(ScopedFrame&& other) noexcept
        : m_domain(other.m_domain)
        , m_id(other.m_id)
        , m_active(other.m_active)
    {
        other.m_active = false;
    }

    ScopedFrame& operator=(ScopedFrame&& other) noexcept
    {
        if (this != &other)
        {
            end();
            m_domain = other.m_domain;
            m_id = other.m_id;
            m_active = other.m_active;
            other.m_active = false;
        }
        return *this;
    }

    ~ScopedFrame() noexcept
    {
        end();
    }

    void end() noexcept
    {
        if (m_active)
        {
            __itt_frame_end_v3(m_domain, &m_id);
            m_active = false;
        }
    }

    bool active() const noexcept
    {
        return m_active;
    }

    static void submit(const __itt_domain* domain, __itt_timestamp begin, __itt_timestamp end) noexcept
    {
        __itt_id id = detail::make_null_id();
        __itt_frame_submit_v3(domain, &id, begin, end);
    }

private:
    const __itt_domain* m_domain = nullptr;
    __itt_id m_id{};
    bool m_active = false;
};

}  // namespace ittapi

#endif  // ITTAPI_FRAME_HPP
