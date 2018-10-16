cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME autoplay)
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 ")

if(UNIX)
    string(APPEND ${CMAKE_CXX_FLAGS} "-D__LINUX_ALSA__")
elseif(APPLE)
    string(APPEND ${CMAKE_CXX_FLAGS} "-D__MACOSX_CORE__")
endif()

set(TRNG_LOCATION /opt/trng CACHE FILEPATH "")

set(DEPENDENCY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

add_subdirectory(${DEPENDENCY_DIR})
find_package(Boost REQUIRED)

add_subdirectory(main)

message(STATUS "TRNG Location: ${TRNG_LOCATION}/include")
target_include_directories(autoplay PUBLIC ${TRNG_LOCATION}/include)
target_include_directories(autoplay PUBLIC ${DEPENDENCY_DIR})
target_include_directories(autoplay PUBLIC ${Boost_INCLUDE_DIRS})

add_subdirectory(test)

add_test(NAME gtester COMMAND tests)
