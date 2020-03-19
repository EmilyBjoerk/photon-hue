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

get_filename_component(BASE_PATH "${${PROJECT_NAME}_SOURCE_DIR}/" ABSOLUTE)
set(INC_PATH     "${BASE_PATH}/include")
set(SRC_PATH     "${BASE_PATH}/src")
file(GLOB SRC_FILES "${SRC_PATH}/*.cc")

add_subdirectory(hueplusplus/hueplusplus)

include_directories(${INC_PATH})
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(photon-hue hueplusplusstatic)
add_test(photon-hue photon-hue)


