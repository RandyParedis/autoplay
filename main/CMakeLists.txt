set(autoplay_SRC
        main.cpp
        midi/Note.cpp
        midi/Note.h)

add_executable(autoplay ${autoplay_SRC})
target_link_libraries(autoplay rtmidi "/opt/trng/lib/libtrng4.so")
