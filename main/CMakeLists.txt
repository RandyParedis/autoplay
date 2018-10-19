set(autoplay_SRC
        midi/Note.cpp
        midi/Note.h
        util/Randomizer.h
        util/FileHandler.cpp
        util/FileHandler.h
)

add_library(autoplay ${autoplay_SRC})

add_executable(autoplayer main.cpp)

target_link_libraries(autoplayer autoplay rtmidi "${TRNG_LOCATION}/lib/libtrng4.a" ${Boost_LIBRARIES})
