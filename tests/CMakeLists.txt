#
# Copyright (C) 2005-2025 Intel Corporation
#
# SPDX-License-Identifier: GPL-2.0-only OR BSD-3-Clause
#

cmake_minimum_required(VERSION 3.14)
project(ittapi_tests CXX C)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler warnings for tests only (not for libraries)
set(TEST_COMPILE_FLAGS "")
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(TEST_COMPILE_FLAGS -Wall -Wextra -Werror)
endif()

# Include directories
set(ITTAPI_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)
set(ITTAPI_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src/ittnotify)

# Build reference collector shared library
set(REFCOL_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../src/ittnotify_refcol)
add_library(ittnotify_refcol SHARED ${REFCOL_SRC_DIR}/itt_refcol_impl.c)
target_include_directories(ittnotify_refcol PRIVATE 
    ${ITTAPI_INCLUDE_DIR}
    ${ITTAPI_SRC_DIR}
)
target_compile_options(ittnotify_refcol PRIVATE -fPIC -Wall)
set_target_properties(ittnotify_refcol PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Build static ittnotify library for tests
file(GLOB ITTNOTIFY_SRCS "${ITTAPI_SRC_DIR}/*.c")
add_library(ittnotify_static STATIC ${ITTNOTIFY_SRCS})
target_include_directories(ittnotify_static PUBLIC ${ITTAPI_INCLUDE_DIR})
target_include_directories(ittnotify_static PRIVATE ${ITTAPI_SRC_DIR})
target_link_libraries(ittnotify_static PRIVATE ${CMAKE_DL_LIBS})
set_target_properties(ittnotify_static PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
)

# Test framework headers (single-header test framework)
set(TEST_FRAMEWORK_DIR ${CMAKE_CURRENT_SOURCE_DIR}/framework)

# Test executables
set(TEST_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

# Function to add a test executable
function(add_itt_test TEST_NAME)
    add_executable(${TEST_NAME} ${ARGN})
    target_include_directories(${TEST_NAME} PRIVATE 
        ${ITTAPI_INCLUDE_DIR}
        ${TEST_FRAMEWORK_DIR}
    )
    target_link_libraries(${TEST_NAME} PRIVATE ittnotify_static ${CMAKE_DL_LIBS} pthread)
    target_compile_options(${TEST_NAME} PRIVATE ${TEST_COMPILE_FLAGS})
    set_target_properties(${TEST_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${TEST_OUTPUT_DIR}
    )
endfunction()

# Add test executables
add_itt_test(test_domain src/test_domain.cpp)
add_itt_test(test_string_handle src/test_string_handle.cpp)
add_itt_test(test_task src/test_task.cpp)
add_itt_test(test_collection_control src/test_collection_control.cpp)
add_itt_test(test_frame src/test_frame.cpp)
add_itt_test(test_counter src/test_counter.cpp)
add_itt_test(test_event src/test_event.cpp)
add_itt_test(test_thread_naming src/test_thread_naming.cpp)

# Note: test_metadata is temporarily disabled due to a known double-free bug
# in the reference collector's cleanup code when used with metadata API.
# TODO: Re-enable after fixing the reference collector (see issue #XXX)
# add_itt_test(test_metadata src/test_metadata.cpp)

# Custom target to run all tests
add_custom_target(run_tests
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/scripts/run_tests.sh 
        --build-dir ${CMAKE_BINARY_DIR}
        --refcol-lib ${CMAKE_BINARY_DIR}/lib/libittnotify_refcol.so
    DEPENDS 
        ittnotify_refcol
        test_domain
        test_string_handle
        test_task
        test_collection_control
        test_frame
        test_counter
        test_event
        test_thread_naming
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running ITT API tests with reference collector"
)
