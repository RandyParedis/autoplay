set(RtMidi_SRC
        RtMidi.h
        RtMidi.cpp)

find_package(ALSA REQUIRED)
find_package(Threads REQUIRED)

add_library(RtMidiLib STATIC ${RtMidi_SRC})
target_link_libraries(RtMidiLib ${ALSA_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})