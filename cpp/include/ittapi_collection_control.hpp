/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#ifndef ITTAPI_COLLECTION_CONTROL_HPP
#define ITTAPI_COLLECTION_CONTROL_HPP

#include <ittnotify.h>

namespace ittapi
{

inline void pause() noexcept
{
    __itt_pause();
}

inline void resume() noexcept
{
    __itt_resume();
}

class ScopedPause
{
public:
    ScopedPause() noexcept
        : m_active(true)
    {
        __itt_pause();
    }

    ScopedPause(const ScopedPause&) = delete;
    ScopedPause& operator=(const ScopedPause&) = delete;
    ScopedPause& operator=(ScopedPause&&) = delete;

    ScopedPause(ScopedPause&& other) noexcept
        : m_active(other.m_active)
    {
        other.m_active = false;
    }

    ~ScopedPause() noexcept
    {
        if (m_active)
        {
            __itt_resume();
        }
    }

    void resume_now() noexcept
    {
        if (m_active)
        {
            __itt_resume();
            m_active = false;
        }
    }

    bool active() const noexcept
    {
        return m_active;
    }

private:
    bool m_active = false;
};

}  // namespace ittapi

#endif  // ITTAPI_COLLECTION_CONTROL_HPP
