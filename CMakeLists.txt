cmake_minimum_required(VERSION 3.24)

project(structures LANGUAGES C)
set(CMAKE_C_STANDARD 17)

include(.cmake/fetch-content.cmake)

include_directories(include/)

add_library(bplustree bplustree/int_int_bplustree.c)

add_executable(bplustree_example bplustree/example.c)
target_link_libraries(bplustree_example bplustree)

enable_testing()

add_executable(TestTrue tests/test_true.c)
target_link_libraries(TestTrue cmocka)
add_test(TestTrue TestTrue)

add_executable(TestBPlusTree tests/test_bplustree.c)
target_link_libraries(TestBPlusTree cmocka bplustree)
add_test(TestBPlusTree TestBPlusTree)
