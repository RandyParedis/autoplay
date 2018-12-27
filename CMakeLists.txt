cmake_minimum_required(VERSION 3.5)
set(PROJECT_NAME autoplay)
set(AUTOPLAY_VERSION "0.1.5")
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++14 -DBOOST_NO_CXX11_SCOPED_ENUMS ")
add_definitions(-DAUTOPLAY_VERSION=${AUTOPLAY_VERSION})

if(APPLE)
    string(APPEND CMAKE_CXX_FLAGS "-D__MACOSX_CORE__ ")
    message(STATUS "MACOSX CORE DETECTED")
elseif(UNIX)
    string(APPEND CMAKE_CXX_FLAGS "-D__LINUX_ALSA__ ")
    message(STATUS "UNIX ALSA DETECTED")
endif()

set(DEPENDENCY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/dependencies)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/installed)
set(BIN_INSTALL_LOCATION     bin)
set(CONFIG_INSTALL_LOCATION  config)

add_subdirectory(${DEPENDENCY_DIR})
find_package(Boost REQUIRED COMPONENTS system filesystem)

set(TRNG_LOCATION /opt/trng CACHE FILEPATH "")
message(STATUS "TRNG location: ${TRNG_LOCATION}")

add_subdirectory(main)

target_include_directories(autoplay SYSTEM PUBLIC ${TRNG_LOCATION}/include)
target_include_directories(autoplay SYSTEM PUBLIC ${TRNG_LOCATION}/lib)
target_include_directories(autoplay SYSTEM PUBLIC ${DEPENDENCY_DIR})
#target_include_directories(autoplay PUBLIC ${DEPENDENCY_DIR}/zupply/src)
target_include_directories(autoplay SYSTEM PUBLIC ${Boost_INCLUDE_DIRS})

add_subdirectory(test)

add_test(NAME gtester COMMAND tests)
