# Set the RtMidi static library
set(RtMidi_HDRS rtmidi/RtMidi.h)
set(RtMidi_SRC rtmidi/RtMidi.cpp)

find_package(ALSA REQUIRED)
find_package(Threads REQUIRED)

add_library(rtmidi STATIC ${RtMidi_HDRS} ${RtMidi_SRC})
target_link_libraries(rtmidi ${ALSA_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})

# Set the TRNG static library
file(GLOB trng_HDRS trng/src/*.hpp)
file(GLOB trng_SRC trng/src/*.cc)

add_library(trng ${trng_SRC} ${trng_HDRS})
target_include_directories(trng PUBLIC trng/src)