set(autoplay_SRC
        music/Note.cpp
        music/Note.h
        util/Randomizer.h
        util/FileHandler.cpp
        util/FileHandler.h
        music/Measure.cpp
        music/Measure.h
        music/Clef.h
        music/Instrument.h
        music/Instrument.cpp
        music/Part.h
        music/Score.h
        music/MIDIPlayer.cpp
        music/MIDIPlayer.h)

add_library(autoplay ${autoplay_SRC})

add_executable(autoplayer main.cpp)

target_link_libraries(autoplayer autoplay rtmidi zupply "${TRNG_LOCATION}/lib/libtrng4.a" ${Boost_LIBRARIES})
