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
        music/Part.h
        music/Score.h
        music/MIDIPlayer.cpp
        music/MIDIPlayer.h
        util/Config.cpp
        util/Config.h
        util/Generator.cpp
        util/Generator.h
        util/RNEngine.cpp
        util/RNEngine.h
        music/Clef.cpp
        music/Score.cpp

        markov/NamedMatrix.cpp
        markov/NamedMatrix.h
        markov/SpecialQueue.h
        markov/MarkovChain.cpp
        markov/MarkovChain.h)

add_library(autoplay ${autoplay_SRC})

configure_file(version_config.h.in ${CMAKE_BINARY_DIR}/generated/version_config.h)
target_include_directories(autoplay PUBLIC ${CMAKE_BINARY_DIR}/generated/)
message(STATUS "Autoplay version: ${AUTOPLAY_VERSION}")

add_executable(autoplayer main.cpp)


target_link_libraries(autoplayer autoplay rtmidi zupply "${TRNG_LOCATION}/lib/libtrng4.a" ${Boost_LIBRARIES})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND NOT APPLE)
    target_link_libraries(autoplayer atomic)
endif()

install(TARGETS autoplayer DESTINATION ${BIN_INSTALL_LOCATION})

# Copy config files to ${CONFIG_INSTALL_LOCATION}
install(DIRECTORY config/ DESTINATION ${CONFIG_INSTALL_LOCATION})
