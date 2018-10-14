cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME autoplay)
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -D__LINUX_ALSA__")

add_subdirectory(dependencies)
find_package(Boost REQUIRED)

add_subdirectory(main)

target_include_directories(autoplay PUBLIC /opt/trng/include)
target_include_directories(autoplay PUBLIC dependencies)
target_include_directories(autoplay PUBLIC ${Boost_INCLUDE_DIRS})