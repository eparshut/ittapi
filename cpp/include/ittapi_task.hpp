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

namespace ittapi {

class Domain;

class ScopedTask {
public:
    ScopedTask(const __itt_domain* domain, std::string_view name)
        : domain_(domain), active_(true) {
        __itt_string_handle* h = detail::create_string_handle(std::string(name).c_str());
        __itt_task_begin(domain_, detail::make_null_id(), detail::make_null_id(), h);
    }

    ScopedTask(const __itt_domain* domain, const StringHandle& name) noexcept
        : domain_(domain), active_(true) {
        __itt_task_begin(domain_, detail::make_null_id(), detail::make_null_id(), name.native_handle());
    }

    ScopedTask(const ScopedTask&) = delete;
    ScopedTask& operator=(const ScopedTask&) = delete;

    ScopedTask(ScopedTask&& other) noexcept
        : domain_(other.domain_), active_(other.active_) {
        other.active_ = false;
    }

    ScopedTask& operator=(ScopedTask&& other) noexcept {
        if (this != &other) {
            end();
            domain_ = other.domain_;
            active_ = other.active_;
            other.active_ = false;
        }
        return *this;
    }

    ~ScopedTask() noexcept {
        end();
    }

    void end() noexcept {
        if (active_) {
            __itt_task_end(domain_);
            active_ = false;
        }
    }

    bool active() const noexcept { return active_; }

private:
    const __itt_domain* domain_ = nullptr;
    bool active_ = false;
};

}  // namespace ittapi

#endif  // ITTAPI_TASK_HPP
