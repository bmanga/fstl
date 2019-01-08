cmake_minimum_required(VERSION 3.10)
project(fstl)

set(CMAKE_CXX_STANDARD 17)

if(FSTL_USE_STD_LIB)
  add_library(fstl INTERFACE)
  target_compile_definitions(fstl INTERFACE FSTL_USE_STD_LIB)
  target_include_directories(fstl INTERFACE include)
else()
  add_library(fstl
  src/forward_list.cpp
  src/functional.cpp
  src/vector.cpp
  src/unordered_map.cpp)

  target_include_directories(fstl PUBLIC include)
endif()


if(FSTL_BUILD_TESTS)
  add_subdirectory(test)
endif()