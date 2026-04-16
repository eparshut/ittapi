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

namespace ittapi {

class Domain;

class ScopedRegion {
public:
    ScopedRegion(const __itt_domain* domain, std::string_view name)
        : domain_(domain), id_(detail::make_null_id()), active_(true) {
        __itt_string_handle* h = detail::create_string_handle(std::string(name).c_str());
        __itt_region_begin(domain_, id_, detail::make_null_id(), h);
    }

    ScopedRegion(const __itt_domain* domain, const StringHandle& name) noexcept
        : domain_(domain), id_(detail::make_null_id()), active_(true) {
        __itt_region_begin(domain_, id_, detail::make_null_id(), name.native_handle());
    }

    ScopedRegion(const ScopedRegion&) = delete;
    ScopedRegion& operator=(const ScopedRegion&) = delete;

    ScopedRegion(ScopedRegion&& other) noexcept
        : domain_(other.domain_), id_(other.id_), active_(other.active_) {
        other.active_ = false;
    }

    ScopedRegion& operator=(ScopedRegion&& other) noexcept {
        if (this != &other) {
            end();
            domain_ = other.domain_;
            id_ = other.id_;
            active_ = other.active_;
            other.active_ = false;
        }
        return *this;
    }

    ~ScopedRegion() noexcept {
        end();
    }

    void end() noexcept {
        if (active_) {
            __itt_region_end(domain_, id_);
            active_ = false;
        }
    }

    bool active() const noexcept { return active_; }

private:
    const __itt_domain* domain_ = nullptr;
    __itt_id id_{};
    bool active_ = false;
};

}  // namespace ittapi

#endif  // ITTAPI_REGION_HPP
