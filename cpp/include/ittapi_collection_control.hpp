/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#ifndef ITTAPI_COLLECTION_CONTROL_HPP
#define ITTAPI_COLLECTION_CONTROL_HPP

#include <ittnotify.h>

namespace ittapi {

inline void pause() noexcept {
    __itt_pause();
}

inline void resume() noexcept {
    __itt_resume();
}

class ScopedPause {
public:
    ScopedPause() noexcept : active_(true) {
        __itt_pause();
    }

    ScopedPause(const ScopedPause&) = delete;
    ScopedPause& operator=(const ScopedPause&) = delete;

    ScopedPause(ScopedPause&& other) noexcept : active_(other.active_) {
        other.active_ = false;
    }

    ScopedPause& operator=(ScopedPause&& other) noexcept {
        if (this != &other) {
            if (active_) {
                __itt_resume();
            }
            active_ = other.active_;
            other.active_ = false;
        }
        return *this;
    }

    ~ScopedPause() noexcept {
        if (active_) {
            __itt_resume();
        }
    }

    void resume_now() noexcept {
        if (active_) {
            __itt_resume();
            active_ = false;
        }
    }

    bool active() const noexcept { return active_; }

private:
    bool active_ = false;
};

}  // namespace ittapi

#endif  // ITTAPI_COLLECTION_CONTROL_HPP
