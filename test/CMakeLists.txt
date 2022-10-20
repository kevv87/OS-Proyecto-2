cmake_minimum_required(VERSION 3.10)

project(Test)
set(CMAKE_BUILD_TYPE Debug)

# Adding the projects to test

#[[
add_subdirectory("${PROJECT_SOURCE_DIR}/../common" "${PROJECT_BINARY_DIR}/common")
]]

add_subdirectory("${PROJECT_SOURCE_DIR}/../cmocka" "${PROJECT_BINARY_DIR}/cmocka")
add_subdirectory("${PROJECT_SOURCE_DIR}/../hw_iface" "${PROJECT_BINARY_DIR}/hw_iface")

add_executable(test_serial_comm test_hw_iface/test_serial_comm.c)

target_link_libraries(test_serial_comm PUBLIC Common cmocka hardware_iface)

target_include_directories(test_serial_comm PUBLIC "${PROJECT_SOURCE_DIR}/../")
