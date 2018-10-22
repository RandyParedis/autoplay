# Set the RtMidi static library
set(RtMidi_HDRS rtmidi/RtMidi.h)
set(RtMidi_SRC rtmidi/RtMidi.cpp)

add_library(rtmidi STATIC ${RtMidi_HDRS} ${RtMidi_SRC})
find_package(Threads REQUIRED)

if(APPLE)
    target_link_libraries(rtmidi "-framework CoreMIDI" "-framework CoreAudio" "-framework CoreFoundation")
    target_link_libraries(rtmidi ${CMAKE_THREAD_LIBS_INIT})
elseif(UNIX)
    find_package(ALSA REQUIRED)
    target_link_libraries(rtmidi ${ALSA_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
endif()

