# ITT API C++ Wrapper

A modern, header-only C++ wrapper for the [ITT API](https://github.com/intel/ittapi) instrumentation library. The wrapper provides RAII-based scoped helpers and type-safe C++ abstractions over the existing C API.

## Supported APIs

| API | C++ Wrapper |
|-----|------------|
| String Handle | `ittapi::StringHandle` |
| Domain | `ittapi::Domain` |
| Task | `ittapi::ScopedTask`, `Domain::task_begin()` / `Domain::task_end()` |
| Region | `ittapi::ScopedRegion` |
| Frame | `ittapi::ScopedFrame` |
| Collection Control | `ittapi::pause()`, `ittapi::resume()`, `ittapi::ScopedPause` |
| Thread Naming | `ittapi::set_thread_name()` |

## Requirements

- C++17 or later
- The existing `ittnotify` static C-library

## Including the Wrapper

Use the umbrella header to get the full API:

```cpp
#include <ittapi.hpp>
```

Or include individual headers:

```cpp
#include <ittapi_domain.hpp>
#include <ittapi_task.hpp>
#include <ittapi_collection_control.hpp>
```

## Example: Task Instrumentation

```cpp
#include <ittapi.hpp>

#include <chrono>
#include <thread>

int main() {
    ittapi::set_thread_name("main");
    ittapi::Domain domain{"example.task"};
    ittapi::StringHandle task_name{"process"};

    ittapi::pause();
    ittapi::resume();

    {
        auto task = domain.task(task_name);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    return 0;
}
```

## Linking

### CMake Consumer

```cmake
find_package(ittapi CONFIG REQUIRED)
target_link_libraries(my_app PRIVATE ittapi::cxx)
```

The `ittapi::cxx` target is an `INTERFACE` library that transitively links the existing `ittnotify` static library and adds the C++ wrapper include directories.

### Manual GCC/G++ (Linux)

```bash
g++ -std=c++17 -O2 \
    -I<ittapi-install-prefix>/include \
    app.cpp \
    <ittapi-install-prefix>/lib/libittnotify.a \
    -ldl -pthread \
    -o app
```

## Building with CMake

From the repository root:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release -DITT_API_CPP_SUPPORT=ON
cmake --build build
```

The `ITT_API_CPP_SUPPORT` option is `OFF` by default.

You can also build with the build script:

```bash
python buildall.py --cpp
```

## Running Tests

Tests are registered inside the `cpp/` subdirectory. After building:

```bash
ctest --test-dir build/cpp --output-on-failure
```

## API Reference

### Free Functions

- `ittapi::pause()` — Pause collection.
- `ittapi::resume()` — Resume collection.
- `ittapi::set_thread_name(std::string_view name)` — Set the current thread's name.

### Classes

#### `ittapi::StringHandle`

Lightweight wrapper around `__itt_string_handle*`.

```cpp
ittapi::StringHandle h{"my_handle"};
h.valid();           // true if handle was created
h.get();   // underlying __itt_string_handle*
```

#### `ittapi::Domain`

Lightweight wrapper around `__itt_domain*` with convenience factories.

```cpp
ittapi::Domain d{"my.domain"};
auto task   = d.task("task_name");     // returns ScopedTask (RAII)
auto region = d.region("region_name"); // returns ScopedRegion
auto frame  = d.frame();               // returns ScopedFrame

d.task_begin("work");                  // manual begin
d.task_end();                          // manual end
```

#### `ittapi::ScopedTask`

RAII wrapper for task begin/end.

```cpp
{
    auto task = domain.task("work");
    // ... do work ...
    task.end();     // optional early end (idempotent)
}                   // destructor ends task if still active
```

For manual (non-RAII) control:

```cpp
domain.task_begin("work");
// ... do work ...
domain.task_end();
```

#### `ittapi::ScopedRegion`

RAII wrapper for region begin/end.

#### `ittapi::ScopedFrame`

RAII wrapper for frame begin/end. Supports explicit timestamp submission.

```cpp
ittapi::ScopedFrame::submit(domain.get(), begin_ts, end_ts);
```

#### `ittapi::ScopedPause`

RAII wrapper for pause/resume. Constructor pauses, destructor resumes.

```cpp
{
    ittapi::ScopedPause sp;
    // collection is paused
    sp.resume_now();  // optional early resume
}
```
