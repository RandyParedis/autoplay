
set(test_SRC
        main.cpp)

find_package(GTest REQUIRED)

add_executable(tests ${test_SRC})

target_include_directories(tests PUBLIC ${GTEST_INCLUDE_DIRS})

target_link_libraries(tests autoplay ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})