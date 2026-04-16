/*
  Copyright (C) 2026 Intel Corporation
  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
*/

#ifndef ITTAPI_STRING_HANDLE_HPP
#define ITTAPI_STRING_HANDLE_HPP

#include <string>
#include <string_view>

#include "ittapi_utils.hpp"

namespace ittapi {

class StringHandle {
public:
    explicit StringHandle(std::string_view name)
        : handle_(detail::create_string_handle(std::string(name).c_str())) {}

    __itt_string_handle* native_handle() const noexcept { return handle_; }
    bool valid() const noexcept { return handle_ != nullptr; }

private:
    __itt_string_handle* handle_ = nullptr;
};

}  // namespace ittapi

#endif  // ITTAPI_STRING_HANDLE_HPP
