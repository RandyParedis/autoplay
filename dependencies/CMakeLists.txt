# Set the RtMidi static library
set(RtMidi_HDRS rtmidi/RtMidi.h)
set(RtMidi_SRC rtmidi/RtMidi.cpp)

add_library(rtmidi STATIC ${RtMidi_HDRS} ${RtMidi_SRC})

if(APPLE)
    target_link_libraries(rtmidi "-framework CoreMIDI" "-framework CoreAudio" "-framework CoreFoundation")
elseif(UNIX)
    find_package(ALSA REQUIRED)
    target_link_libraries(rtmidi ${ALSA_LIBRARY})
endif()

find_package(Threads REQUIRED)

target_link_libraries(rtmidi ${CMAKE_THREAD_LIBS_INIT})

