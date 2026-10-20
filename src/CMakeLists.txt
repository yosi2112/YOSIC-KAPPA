cmake_minimum_required(VERSION 3.25)
project(yosic_kappa VERSION 0.1.0 LANGUAGES CXX)

if(NOT WIN32 OR NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    message(FATAL_ERROR "Use the Visual Studio 18 x64 Developer Command Prompt with MSVC.")
endif()

if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "Use the x64 MSVC toolchain; the portable package targets Windows x64.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

option(YOSIC_ENABLE_ANALYSIS "Enable MSVC static analysis" OFF)

add_library(yosic_build_options INTERFACE)
target_compile_options(yosic_build_options INTERFACE /W4 /WX /permissive- /utf-8)
target_compile_definitions(yosic_build_options INTERFACE WIN32_LEAN_AND_MEAN NOMINMAX)
if(YOSIC_ENABLE_ANALYSIS)
    target_compile_options(yosic_build_options INTERFACE /analyze)
endif()

add_library(yosic-core cpu/cpu.cpp memory/memory.cpp bus/mmio.cpp media/media.cpp devices/fdc/fdc.cpp)
target_include_directories(yosic-core PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(yosic-core PUBLIC yosic_build_options)
add_executable(yosic-sim main.cpp cli/run.cpp cli/disks.cpp cli/debugger.cpp cli/monitor.cpp)
target_link_libraries(yosic-sim PRIVATE yosic-core)
target_compile_definitions(yosic-sim PRIVATE YOSIC_VERSION="${PROJECT_VERSION}")
add_library(yosic-assembler assembler/assembler.cpp)
target_include_directories(yosic-assembler PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(yosic-assembler PUBLIC yosic_build_options)
add_executable(yosic-as assembler/main.cpp)
target_link_libraries(yosic-as PRIVATE yosic-assembler)


add_subdirectory(tools/mkimage)
include(tools/distribution/install.cmake)
