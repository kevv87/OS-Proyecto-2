cmake_minimum_required(VERSION 3.10)

project(Test)
set(CMAKE_BUILD_TYPE Debug)

# Adding the projects to test
add_subdirectory("${PROJECT_SOURCE_DIR}/../../../common/" "${PROJECT_BINARY_DIR}/common")
add_subdirectory("${PROJECT_SOURCE_DIR}/../../../cmocka" "${PROJECT_BINARY_DIR}/cmocka")

set(SOURCES linked_list_test.c shared_memory_tests.c)

add_executable(shared_memory_tests shared_memory_tests.c)
add_executable(linked_list_on_shared_memory_test linked_list_on_shared_memory_test.c)
add_executable(metadata_table_test metadata_table_test.c)

target_link_libraries(shared_memory_tests PUBLIC Common cmocka)
target_link_libraries(linked_list_on_shared_memory_test PUBLIC Common cmocka)
target_link_libraries(metadata_table_test PUBLIC Common cmocka)

target_include_directories(shared_memory_tests PUBLIC "${PROJECT_SOURCE_DIR}/../../../")
target_include_directories(linked_list_on_shared_memory_test PUBLIC "${PROJECT_SOURCE_DIR}/../../../")
target_include_directories(metadata_table_test PUBLIC "${PROJECT_SOURCE_DIR}/../../../")
