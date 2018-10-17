set(autoplay_SRC
        midi/Note.cpp
        midi/Note.h
        util/Randomizer.h)

add_library(autoplay ${autoplay_SRC})

add_executable(autoplayer main.cpp)

set(EXT "so")
if(APPLE)
    set(EXT "la")
endif()

target_link_libraries(autoplayer autoplay rtmidi "${TRNG_LOCATION}/lib/libtrng4.${EXT}" ${Boost_LIBRARIES})
