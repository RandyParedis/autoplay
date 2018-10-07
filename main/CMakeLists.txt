set(autoplay_SRC
        main.cpp)

add_subdirectory(resources/rtmidi-3.0.0)
include_directories(resources/rtmidi-3.0.0)
add_executable(autoplay ${autoplay_SRC})
target_link_libraries(autoplay RtMidiLib)