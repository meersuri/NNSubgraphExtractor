cmake_minimum_required(VERSION 3.12)
project (SGEX VERSION 0.0.1)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -ggdb)

add_subdirectory(src)

