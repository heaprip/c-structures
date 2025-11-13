cmake_minimum_required(VERSION 3.24)

project(structures LANGUAGES C)
set(CMAKE_C_STANDARD 17)

include_directories(include/)

add_library(bplustree bplustree/int_int_bplustree.c)

add_executable(bplustree_example bplustree/example.c)
target_link_libraries(bplustree_example bplustree)
