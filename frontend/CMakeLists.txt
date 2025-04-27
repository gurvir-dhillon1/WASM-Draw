# Minimum CMake version required
cmake_minimum_required(VERSION 3.14) # Increased version for FetchContent

# Project name and language
project(WebAssemblyProject LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Option to build tests instead of WebAssembly app
option(BUILD_TESTS "Build tests instead of WebAssembly application" OFF)

# Common include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/drawing
    ${CMAKE_SOURCE_DIR}/include/game
)

if(BUILD_TESTS)
    # Test build configuration
    message(STATUS "Configuring test build")
    
    # Enable testing
    enable_testing()

     # Find SDL2 package for native testing
     # find_package(SDL2 REQUIRED)
     # include_directories(${SDL2_INCLUDE_DIRS})   

    # Include FetchContent module
    include(FetchContent)
    
    # Fetch GoogleTest
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )
    
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    
    # Make GoogleTest available
    FetchContent_MakeAvailable(googletest)
    
    # Include source files (excluding bindings)
    set(COMMON_SOURCES
        src/drawing/DrawingCommands.cpp
    )
    
    # Test sources
    set(TEST_SOURCES
        tests/drawing/DrawingCommandsTest.cpp
        # Add other test files as needed
        # tests/drawing/ShapesTest.cpp
        # tests/game/GameStateTest.cpp
    )
    
    # Create test executable
    add_executable(run_tests ${TEST_SOURCES} ${COMMON_SOURCES})
    
    # Link against Google Test
    target_link_libraries(run_tests gtest gtest_main gmock)
    
    # Add tests to CTest
    add_test(NAME AllTests COMMAND run_tests)
    
else()
    # WebAssembly build configuration
    message(STATUS "Configuring WebAssembly build")
    
    # Ensure we're using Emscripten
    if(NOT CMAKE_TOOLCHAIN_FILE)
        message(FATAL_ERROR "This project requires Emscripten. Please use 'emcmake cmake' to configure.")
    endif()
    
    # Add SDL compiler flags
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2")
    
    # Gather all source files
    set(SOURCES
        src/main.cpp
        src/drawing/DrawingCommands.cpp
        src/drawing/DrawingCommandsBindings.cpp
        src/drawing/Shapes.cpp
        src/game/GameLoop.cpp
        src/game/GameState.cpp
    )
    
    # Define the executable target
    add_executable(main ${SOURCES})
    
    # Set output directory
    set_target_properties(main PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
    
    # Emscripten-specific linker flags
    target_link_options(main PRIVATE
        -sWASM=1
        -sUSE_SDL=2
        -O3
        -sASYNCIFY
        -sNO_EXIT_RUNTIME=1
        -sEXPORTED_RUNTIME_METHODS=['ccall']
        -sEMBIND_STD_STRING_IS_UTF8=0
        -lembind
        --shell-file "${CMAKE_SOURCE_DIR}/html_template/template.html"
    )
    
    # Rename output to index.html
    set_target_properties(main PROPERTIES
        OUTPUT_NAME "index"
        SUFFIX ".html"
    )
    
    # Custom clean target
    add_custom_target(clean-custom
        COMMAND ${CMAKE_COMMAND} -E remove
            ${CMAKE_SOURCE_DIR}/index.js
            ${CMAKE_SOURCE_DIR}/index.wasm
            ${CMAKE_SOURCE_DIR}/index.html
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        COMMENT "Cleaning build files"
    )
endif()
