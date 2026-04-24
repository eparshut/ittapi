#
#  Copyright (C) 2026 Intel Corporation
#  SPDX-License-Identifier: (BSD-3-Clause OR GPL-2.0-only)
#

add_library(ittapi-cpp INTERFACE)
add_library(ittapi::cxx ALIAS ittapi-cpp)

target_include_directories(ittapi-cpp
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(ittapi-cpp INTERFACE ittnotify)
target_compile_features(ittapi-cpp INTERFACE cxx_std_17)

if(NOT WIN32)
    find_package(Threads)
endif()

# Sample
add_executable(task_sample samples/task_sample.cpp)
target_link_libraries(task_sample PRIVATE ittapi-cpp)
if(NOT WIN32)
    target_link_libraries(task_sample PRIVATE ${CMAKE_DL_LIBS})
    if(Threads_FOUND)
        target_link_libraries(task_sample PRIVATE Threads::Threads)
    endif()
endif()

# Tests
enable_testing()

set(CPP_TESTS
    test_string_handle
    test_domain
    test_task
    test_region
    test_frame
    test_collection_control
    test_thread_naming
    test_ittapi
)

foreach(test_name ${CPP_TESTS})
    add_executable(${test_name} tests/${test_name}.cpp)
    target_link_libraries(${test_name} PRIVATE ittapi-cpp)
    if(NOT WIN32)
        target_link_libraries(${test_name} PRIVATE ${CMAKE_DL_LIBS})
        if(Threads_FOUND)
            target_link_libraries(${test_name} PRIVATE Threads::Threads)
        endif()
    endif()
    add_test(NAME cpp_${test_name} COMMAND ${test_name})
endforeach()
