cmake_minimum_required(VERSION 3.5)

project(web-crawler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
  "include"
  ${ZLIB_DEPS}
)

add_subdirectory(src)
add_subdirectory(deps/zlib-1.2.11)
