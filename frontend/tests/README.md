# WASM Draw Testing Guide

This document explains how to build and run tests for the WASM-Draw project.

## Prerequisites

Before running the tests, make sure you have the following installed:

- CMake (version 3.14 or higher)
- A C++ compiler with C++14 support
- SDL2 library (NOTE: This is NOT currently required, but may be required in future test builds)

### Installing SDL2

#### macOS
```bash
brew install sdl2
```

#### Ubuntu/Debian
```bash
sudo apt-get install libsdl2-dev
```

#### Windows
Install SDL2 development libraries from [SDL's website](https://www.libsdl.org/download-2.0.php) or use vcpkg:
```
vcpkg install sdl2
```

## Building and Running Tests

Follow these steps to build and run the tests:

### 1. Clean previous build files (if any)

```bash
rm -rf build_tests
```

### 2. Configure the project in test mode

```bash
cmake -B build_tests -DBUILD_TESTS=ON
```

### 3. Build the tests

```bash
cmake --build build_tests
```

### 4. Run the tests

```bash
cd build_tests && ctest
```

Or run the test executable directly for more detailed output:

```bash
./build_tests/run_tests
```

## Switching Between Test and WebAssembly Builds

### WebAssembly Build (Normal Build)

```bash
emcmake cmake -B build
cmake --build build
```

### Test Build

```bash
cmake -B build_tests -DBUILD_TESTS=ON
cmake --build build_tests
```

## Adding New Tests

Add new test files to the `tests/` directory structure, mirroring the `src/` structure. Then update the `TEST_SOURCES` variable in CMakeLists.txt if needed.

Example:
```cmake
set(TEST_SOURCES
    tests/drawing/DrawingCommandsTest.cpp
    tests/drawing/ShapesTest.cpp
    tests/game/GameStateTest.cpp
)
```

## Troubleshooting

If you encounter issues with SDL2 not being found, make sure it's installed and properly set in your PATH. You might need to specify the SDL2 location explicitly:

```bash
cmake -B build_tests -DBUILD_TESTS=ON -DSDL2_DIR=/path/to/sdl2
```
