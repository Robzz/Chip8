cmake_minimum_required (VERSION 2.6)
project (Chip8)

# Version number
set (Chip8_VERSION_MAJOR 0)
set (Chip8_VERSION_MINOR 1)

# cmake_policy(SET CMP0015 OLD)

# Find the boost libraries the project depends on
include(FindBoost)
find_package(Boost
             REQUIRED             # Fail with error if Boost is not found
             COMPONENTS thread system filesystem log program_options
)

# Locate SDL2 with pkg-config
#set(PKG_CONFIG_USE_CMAKE_PREFIX_PATH ON)
find_package(PkgConfig)
pkg_check_modules(SDL2 REQUIRED NO_CMAKE_PATH NO_CMAKE_ENVIRONMENT_PATH sdl2)
include_directories()

# Generate the list of source files. Include ALL files that are commonly edited, so that they
# appear in the generated project files (e.g. Code::Blocks)
file(GLOB_RECURSE
     cpp_files
     src/*)

file(GLOB_RECURSE
     header_files
     include/*.h)

add_executable(Chip8_bin ${cpp_files} ${header_files} ".gitignore" "README.md" "LICENSE")

# Enable C++11
set_property(TARGET Chip8_bin PROPERTY CXX_STANDARD 11)
set_property(TARGET Chip8_bin PROPERTY CXX_STANDARD_REQUIRED ON)

# Silence the crapload of boost issued deprecation warnings
# (alternatively, investigate? There's so many of them I doubt the boost folks haven't thought it through.)
add_definitions("-Wno-deprecated-declarations")
if(NOT WIN32)
     add_definitions("-DBOOST_LOG_DYN_LINK")
endif()

# Include directories
include_directories("include" ${Boost_INCLUDE_DIRS} ${SDL2_INCLUDE_DIRS})

link_directories(${SDL2_LIBRARY_DIRS})
target_link_libraries(Chip8_bin ${SDL2_LDFLAGS} ${Boost_LIBRARIES})

# Output path set to build type
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# Generate the configuration header
configure_file (
  "include/Chip8_config.h.in"
  "include/Chip8_config.h"
)
