set(autoplay_SRC
        midi/Note.cpp
        midi/Note.h
        util/Randomizer.h)

add_library(autoplay ${autoplay_SRC})

add_executable(autoplayer main.cpp)

target_link_libraries(autoplayer autoplay rtmidi "/opt/trng/lib/libtrng4.so" ${Boost_LIBRARIES})
