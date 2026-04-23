/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#ifndef ITTAPI_UTILS_HPP
#define ITTAPI_UTILS_HPP

#include <ittnotify.h>

#include <atomic>
#include <string_view>
#include <unordered_map>

namespace ittapi
{
namespace detail
{

inline __itt_domain* create_domain(const char* name) noexcept
{
#if ITT_PLATFORM == ITT_PLATFORM_WIN
    return __itt_domain_createA(name);
#else
    return __itt_domain_create(name);
#endif
}

inline __itt_string_handle* create_string_handle(const char* name) noexcept
{
#if ITT_PLATFORM == ITT_PLATFORM_WIN
    return __itt_string_handle_createA(name);
#else
    return __itt_string_handle_create(name);
#endif
}

inline __itt_string_handle* get_or_create_string_handle(std::string_view name)
{
    thread_local std::unordered_map<std::string_view, __itt_string_handle*> cache;
    auto it = cache.find(name);
    if (it != cache.end())
    {
        return it->second;
    }
    __itt_string_handle* h = create_string_handle(std::string(name).c_str());
    if (h != nullptr)
    {
        cache.emplace(h->strA, h);
    }
    return h;
}

inline void thread_set_name(const char* name) noexcept
{
#if ITT_PLATFORM == ITT_PLATFORM_WIN
    __itt_thread_set_nameA(name);
#else
    __itt_thread_set_name(name);
#endif
}

#if ITT_PLATFORM == ITT_PLATFORM_WIN

inline __itt_domain* create_domain(const wchar_t* name) noexcept
{
    return __itt_domain_createW(name);
}

inline __itt_string_handle* create_string_handle(const wchar_t* name) noexcept
{
    return __itt_string_handle_createW(name);
}

inline __itt_string_handle* get_or_create_string_handle(std::wstring_view name)
{
    thread_local std::unordered_map<std::wstring_view, __itt_string_handle*> cache;
    auto it = cache.find(name);
    if (it != cache.end())
    {
        return it->second;
    }
    __itt_string_handle* h = create_string_handle(std::wstring(name).c_str());
    if (h != nullptr)
    {
        cache.emplace(static_cast<const wchar_t*>(h->strW), h);
    }
    return h;
}

inline void thread_set_name(const wchar_t* name) noexcept
{
    __itt_thread_set_nameW(name);
}

#endif  // ITT_PLATFORM == ITT_PLATFORM_WIN

inline __itt_id get_null_id() noexcept
{
    __itt_id id = __itt_null;
    return id;
}

inline __itt_id gen_id() noexcept
{
    static std::atomic<unsigned long long> counter{1};
    return __itt_id_make(nullptr, counter.fetch_add(1, std::memory_order_relaxed));
}

}  // namespace detail
}  // namespace ittapi

#endif  // ITTAPI_UTILS_HPP
