cmake_minimum_required(VERSION 3.5)

project(web-crawler LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
  "include"
)

add_subdirectory(src)
