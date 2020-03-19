cmake_minimum_required(VERSION 3.7)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

project(photon-hue CXX)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "-Werror -Wall -Wextra -pedantic -Wno-attributes")
file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")

add_subdirectory(hueplusplus EXCLUDE_FROM_ALL)
add_executable(${PROJECT_NAME} ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} hueplusplusstatic)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)

set(SERVICE_LAUNCHER ${PROJECT_NAME}-service)
configure_file("install/launcher.in" ${SERVICE_LAUNCHER})
install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${SERVICE_LAUNCHER} DESTINATION bin)

set(SERVICE_SYSTEMD ${PROJECT_NAME}.service)
configure_file("install/service.in" ${SERVICE_SYSTEMD})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${SERVICE_SYSTEMD} DESTINATION "/etc/systemd/system/")


