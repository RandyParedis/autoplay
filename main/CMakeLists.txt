set(autoplay_SRC
        main.cpp
        midi/Note.cpp
        midi/Note.h)

add_subdirectory(dependencies/rtmidi-3.0.0)
include_directories(dependencies/rtmidi-3.0.0)
add_executable(autoplay ${autoplay_SRC})
target_link_libraries(autoplay RtMidiLib)