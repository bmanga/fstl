cmake_minimum_required(VERSION 3.10)
project(tests)

set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_CURRENT_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")

add_executable(tests
  main.cpp
  forward_list.cpp
  unordered_map.cpp
  vector.cpp)
target_link_libraries(tests PRIVATE fstl CONAN_PKG::catch2)
target_include_directories(tests PRIVATE ../include)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
  set_property(TARGET tests PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

