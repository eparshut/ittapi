/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#ifndef ITTAPI_UTILS_HPP
#define ITTAPI_UTILS_HPP

#include <ittnotify.h>

namespace ittapi {
namespace detail {

inline __itt_domain* create_domain(const char* name) noexcept {
#if ITT_PLATFORM == ITT_PLATFORM_WIN
    return __itt_domain_createA(name);
#else
    return __itt_domain_create(name);
#endif
}

inline __itt_string_handle* create_string_handle(const char* name) noexcept {
#if ITT_PLATFORM == ITT_PLATFORM_WIN
    return __itt_string_handle_createA(name);
#else
    return __itt_string_handle_create(name);
#endif
}

inline void thread_set_name(const char* name) noexcept {
#if ITT_PLATFORM == ITT_PLATFORM_WIN
    __itt_thread_set_nameA(name);
#else
    __itt_thread_set_name(name);
#endif
}

inline __itt_id make_null_id() noexcept {
    __itt_id id = __itt_null;
    return id;
}

}  // namespace detail
}  // namespace ittapi

#endif  // ITTAPI_UTILS_HPP
