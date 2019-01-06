cmake_minimum_required(VERSION 3.10)
project(fstl)

set(CMAKE_CXX_STANDARD 17)

add_library(fstl
  src/forward_list.cpp
  src/functional.cpp
  src/vector.cpp
  src/unordered_map.cpp)
target_include_directories(fstl PUBLIC include)

add_subdirectory(test)
