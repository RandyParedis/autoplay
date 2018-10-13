cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME autoplay)
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -D__LINUX_ALSA__")

add_subdirectory(dependencies)

add_subdirectory(main)

target_include_directories(autoplay PUBLIC dependencies)
target_include_directories(autoplay PUBLIC dependencies/trng)
