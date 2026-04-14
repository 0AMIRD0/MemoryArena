cmake_minimum_required(VERSION 3.14)
project(MemoryArena)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
  googlebenchmark
  GIT_REPOSITORY https://github.com/google/benchmark.git
  GIT_TAG        v1.8.3
)

FetchContent_MakeAvailable(googlebenchmark)

add_library(arena_lib
  linear_arena.cpp
  free_list_arena.cpp
  pool_allocator.cpp
  arena_resource.cpp
)

target_include_directories(arena_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(arena_benchmark arena_benchmark.cpp)
target_link_libraries(arena_benchmark PRIVATE arena_lib benchmark::benchmark)
