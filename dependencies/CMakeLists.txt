# Set the RtMidi static library
set(RtMidi_HDRS rtmidi/RtMidi.h)
set(RtMidi_SRC rtmidi/RtMidi.cpp)

if(UNIX)
    find_package(ALSA REQUIRED)
    set(RtMidiLibs ${ALSA_LIBRARY})
elseif(APPLE)
    set(CMAKE_STATIC_LINKER_FLAGS "-framework CoreMIDI -framework CoreAudio -framework CoreFoundation")
endif()
find_package(Threads REQUIRED)

add_library(rtmidi STATIC ${RtMidi_HDRS} ${RtMidi_SRC})
target_link_libraries(rtmidi ${RtMidiLibs} ${CMAKE_THREAD_LIBS_INIT})

