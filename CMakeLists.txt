# Minimum CMake version required
cmake_minimum_required(VERSION 3.10)

# Project name and language
project(WebAssemblyProject LANGUAGES CXX)

# Set the C++ standard (optional, adjust as needed)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ensure we're using Emscripten
if(NOT CMAKE_TOOLCHAIN_FILE)
    message(FATAL_ERROR "This project requires Emscripten. Please use 'emcmake cmake' to configure.")
endif()

# Define the executable target
add_executable(main main.cpp)

# Set output directory for the final files
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# Emscripten-specific linker flags (equivalent to your Makefile)
target_link_options(main PRIVATE
    -sWASM=1                    # Enable WebAssembly output
    -sUSE_SDL=2                 # Use SDL2
    -O3                         # Optimization level
    -sASYNCIFY                  # Enable async operations
    -sNO_EXIT_RUNTIME=1         # Keep runtime alive
    -sEXPORTED_RUNTIME_METHODS=['ccall']  # Export ccall
    -sEMBIND_STD_STRING_IS_UTF8=0         # Disable UTF-8 strings in Embind
    -lembind                    # Enable Embind
    --shell-file "${CMAKE_SOURCE_DIR}/html_template/template.html"  # Shell HTML file
)

# Rename output to index.html (instead of main.html)
set_target_properties(main PROPERTIES
    OUTPUT_NAME "index"
    SUFFIX ".html"              # Ensure the output is index.html
)

# Add a custom clean target (CMake's default 'clean' usually suffices)
add_custom_target(clean-custom
    COMMAND ${CMAKE_COMMAND} -E remove
        ${CMAKE_SOURCE_DIR}/index.js
        ${CMAKE_SOURCE_DIR}/index.wasm
        ${CMAKE_SOURCE_DIR}/index.html
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Cleaning build files"
)
