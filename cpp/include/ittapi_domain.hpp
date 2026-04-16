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

namespace ittapi {

class Domain {
public:
    explicit Domain(std::string_view name)
        : handle_(detail::create_domain(std::string(name).c_str())) {}

    __itt_domain* native_handle() const noexcept { return handle_; }
    bool valid() const noexcept { return handle_ != nullptr; }

    ScopedTask task(std::string_view name) const {
        return ScopedTask(handle_, name);
    }

    ScopedTask task(const StringHandle& name) const noexcept {
        return ScopedTask(handle_, name);
    }

    void task_begin(std::string_view name) const {
        __itt_string_handle* h = detail::create_string_handle(std::string(name).c_str());
        __itt_task_begin(handle_, detail::make_null_id(), detail::make_null_id(), h);
    }

    void task_begin(const StringHandle& name) const noexcept {
        __itt_task_begin(handle_, detail::make_null_id(), detail::make_null_id(), name.native_handle());
    }

    void task_end() const noexcept {
        __itt_task_end(handle_);
    }

    ScopedRegion region(std::string_view name) const {
        return ScopedRegion(handle_, name);
    }

    ScopedRegion region(const StringHandle& name) const noexcept {
        return ScopedRegion(handle_, name);
    }

    ScopedFrame frame() const noexcept {
        return ScopedFrame(handle_);
    }

private:
    __itt_domain* handle_ = nullptr;
};

}  // namespace ittapi

#endif  // ITTAPI_DOMAIN_HPP
