cmake_minimum_required(VERSION 2.8)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

project(photon-hue C CXX)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra -pedantic -Wno-attributes")

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")

add_subdirectory(hueplusplus)
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(photon-hue hueplusplusstatic)


