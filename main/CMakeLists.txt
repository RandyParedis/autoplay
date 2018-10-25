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
        music/Instrument.cpp)

add_library(autoplay ${autoplay_SRC})

add_executable(autoplayer main.cpp)

target_link_libraries(autoplayer autoplay rtmidi zupply "${TRNG_LOCATION}/lib/libtrng4.a" ${Boost_LIBRARIES})
