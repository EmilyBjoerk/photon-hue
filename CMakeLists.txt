cmake_minimum_required(VERSION 2.8)

set(CXXSTANDARD "-std=c++17")
set(COPT        "-O0 -ggdb3 -pthread")
set(CWARN       "-Werror -Wall -Wextra -pedantic -Wno-attributes")
set(CTUNING     "-ftrapv -fsanitize=undefined -fsanitize=unsigned-integer-overflow -fsanitize=implicit-conversion -fprofile-instr-generate -fcoverage-mapping")

project(photon-hue C CXX)
set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CXXSTANDARD} ${CFAKE_AVR} ${COPT} ${CWARN} ${CTUNING}")

get_filename_component(BASE_PATH "${${PROJECT_NAME}_SOURCE_DIR}/" ABSOLUTE)
set(INC_PATH     "${BASE_PATH}/include")
set(SRC_PATH     "${BASE_PATH}/src")
file(GLOB SRC_FILES "${SRC_PATH}/*.cc")

include_directories(${INC_PATH})
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(photon-hue)
add_test(photon-hue photon-hue)


