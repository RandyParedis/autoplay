
set(test_SRC
        music/ClefTest.cpp
        music/InstrumentTest.cpp
        music/MeasureTest.cpp
        music/NoteTest.cpp
        music/PartTest.cpp)

find_package(GTest REQUIRED)

add_executable(tests main.cpp ${test_SRC})

target_include_directories(tests PUBLIC ${GTEST_INCLUDE_DIRS})

target_link_libraries(tests autoplay ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})