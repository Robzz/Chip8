cmake_minimum_required (VERSION 2.6)
project (Chip8)

# Version number
set (Chip8_VERSION_MAJOR 0)
set (Chip8_VERSION_MINOR 1)

cmake_policy(SET CMP0015 OLD)

# Find libraries we depend on
include(FindBoost)
find_package(Boost
             REQUIRED             # Fail with error if Boost is not found
             COMPONENTS thread chrono system log program_options filesystem
)
set(Boost_USE_STATIC_LIBS ON)

link_directories("lib/SDL2-2.0.3/x86_64-w64-mingw32/lib")

# Generate the list of source files
file(GLOB_RECURSE
     source_files
     src/*
)

add_executable(Chip8_bin ${source_files})

# Enable C++11
set_property(TARGET Chip8_bin PROPERTY CXX_STANDARD 11)
set_property(TARGET Chip8_bin PROPERTY CXX_STANDARD_REQUIRED ON)
add_definitions("-Wno-deprecated-declarations")

# Include directories
include_directories("include" ${Boost_INCLUDE_DIRS} "lib/SDL2-2.0.3/x86_64-w64-mingw32/include")

target_link_libraries(Chip8_bin ${Boost_LIBRARIES} "mingw32" "SDL2main" "SDL2.dll")

# Output path set to build type
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# Generate the configuration header
configure_file (
  "include/Chip8_config.h.in"
  "include/Chip8_config.h"
)

include_directories("${PROJECT_BINARY_DIR}")

#add_executable(Chip8_bin ${source_files})
