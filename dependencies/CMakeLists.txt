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

# Set the zupply library
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w" )
add_library(zupply zupply/src/zupply.cpp zupply/src/zupply.hpp)
#set_target_properties(zupply PROPERTIES CMAKE_CXX_FLAGS "-std=c++11")
