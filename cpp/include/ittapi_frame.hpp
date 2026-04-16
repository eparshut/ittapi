/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#ifndef ITTAPI_FRAME_HPP
#define ITTAPI_FRAME_HPP

#include "ittapi_utils.hpp"

namespace ittapi {

class Domain;

class ScopedFrame {
public:
    explicit ScopedFrame(const __itt_domain* domain) noexcept
        : domain_(domain), id_(detail::make_null_id()), active_(true) {
        __itt_frame_begin_v3(domain_, &id_);
    }

    ScopedFrame(const ScopedFrame&) = delete;
    ScopedFrame& operator=(const ScopedFrame&) = delete;

    ScopedFrame(ScopedFrame&& other) noexcept
        : domain_(other.domain_), id_(other.id_), active_(other.active_) {
        other.active_ = false;
    }

    ScopedFrame& operator=(ScopedFrame&& other) noexcept {
        if (this != &other) {
            end();
            domain_ = other.domain_;
            id_ = other.id_;
            active_ = other.active_;
            other.active_ = false;
        }
        return *this;
    }

    ~ScopedFrame() noexcept {
        end();
    }

    void end() noexcept {
        if (active_) {
            __itt_frame_end_v3(domain_, &id_);
            active_ = false;
        }
    }

    bool active() const noexcept { return active_; }

    static void submit(const __itt_domain* domain, __itt_timestamp begin, __itt_timestamp end) noexcept {
        __itt_id id = detail::make_null_id();
        __itt_frame_submit_v3(domain, &id, begin, end);
    }

private:
    const __itt_domain* domain_ = nullptr;
    __itt_id id_{};
    bool active_ = false;
};

}  // namespace ittapi

#endif  // ITTAPI_FRAME_HPP
