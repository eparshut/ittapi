/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#ifndef ITTAPI_STRING_HANDLE_HPP
#define ITTAPI_STRING_HANDLE_HPP

#include <string>
#include <string_view>

#include "ittapi_utils.hpp"

namespace ittapi
{

class StringHandle
{
public:
    explicit StringHandle(std::string_view name)
        : m_handle(detail::create_string_handle(std::string(name).c_str()))
    {
    }

#if ITT_PLATFORM == ITT_PLATFORM_WIN
    explicit StringHandle(std::wstring_view name)
        : m_handle(detail::create_string_handle(std::wstring(name).c_str()))
    {
    }
#endif

    __itt_string_handle* native_handle() const noexcept
    {
        return m_handle;
    }

    bool valid() const noexcept
    {
        return m_handle != nullptr;
    }

private:
    __itt_string_handle* m_handle = nullptr;
};

}  // namespace ittapi

#endif  // ITTAPI_STRING_HANDLE_HPP
