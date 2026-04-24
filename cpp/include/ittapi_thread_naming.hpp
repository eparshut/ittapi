/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#ifndef ITTAPI_THREAD_NAMING_HPP
#define ITTAPI_THREAD_NAMING_HPP

#include <string>
#include <string_view>

#include "ittapi_utils.hpp"

namespace ittapi
{

inline void set_thread_name(std::string_view name)
{
    detail::thread_set_name(std::string(name).c_str());
}

#if ITT_PLATFORM == ITT_PLATFORM_WIN
inline void set_thread_name(std::wstring_view name)
{
    detail::thread_set_name(std::wstring(name).c_str());
}
#endif

}  // namespace ittapi

#endif  // ITTAPI_THREAD_NAMING_HPP
