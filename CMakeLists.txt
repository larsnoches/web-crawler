cmake_minimum_required(VERSION 3.8)

project(web-crawler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(DESIRED_WARNINGS "-Wall -Wextra -Wconversion -Wunreachable-code -Wuninitialized -pedantic-errors -Wold-style-cast -Wno-error=unused-variable -Wshadow -Wfloat-equal")

include_directories(
  "include"
  ${ZLIB_DEPS}
)

add_subdirectory(src)
add_subdirectory(deps/zlib-1.2.11)
