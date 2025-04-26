# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(WebAssemblyProject LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure we're using Emscripten
if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "This project requires Emscripten. Please use 'emcmake cmake' to configure.")
endif()

# Add SDL compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_SDL=2")

# Add include directories
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/drawing
    ${CMAKE_SOURCE_DIR}/include/game
)

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
