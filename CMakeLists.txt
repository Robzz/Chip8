cmake_minimum_required (VERSION 2.6)
project (Chip8)

# Version number
set (Chip8_VERSION_MAJOR 0)
set (Chip8_VERSION_MINOR 1)

cmake_policy(SET CMP0015 OLD)

# Find the boost libraries the project depends on
include(FindBoost)
find_package(Boost
             REQUIRED             # Fail with error if Boost is not found
             COMPONENTS thread system filesystem log program_options
)
set(Boost_USE_STATIC_LIBS ON)

# TODO : this is a quick and dirty workaround. Fix it. pkg-config?
# Also, this forces to specify the cmake_policy CMP0015.
link_directories("lib/SDL2-2.0.3/x86_64-w64-mingw32/lib")

# Generate the list of source files. Include ALL files that are commonly edited, so that they
# appear in the generated project files (e.g. Code::Blocks)
file(GLOB_RECURSE
     cpp_files
     src/*)

file(GLOB_RECURSE
     header_files
     include/*.h)

add_executable(Chip8_bin ${cpp_files} ${header_files} ".gitignore")

# Enable C++11
set_property(TARGET Chip8_bin PROPERTY CXX_STANDARD 11)
set_property(TARGET Chip8_bin PROPERTY CXX_STANDARD_REQUIRED ON)

# Silence the crapload of boost issued deprecation warnings
# (alternatively, investigate? There's so many of them I doubt the boost folks haven't thought it through.)
add_definitions("-Wno-deprecated-declarations")

# Include directories
# Again, quick and dirty fix...
include_directories("include" ${Boost_INCLUDE_DIRS} "lib/SDL2-2.0.3/x86_64-w64-mingw32/include")

# Not portable to linux...
target_link_libraries(Chip8_bin ${Boost_LIBRARIES} "mingw32" "SDL2main" "SDL2.dll")

# Output path set to build type
set(EXECUTABLE_OUTPUT_PATH bin/${CMAKE_BUILD_TYPE})

# Generate the configuration header
configure_file (
  "include/Chip8_config.h.in"
  "include/Chip8_config.h"
)

#add_executable(Chip8_bin ${source_files})
