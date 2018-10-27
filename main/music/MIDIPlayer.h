//
// Created by red on 27/10/18.
//

#ifndef AUTOPLAY_MIDIPLAYER_H
#define AUTOPLAY_MIDIPLAYER_H

#include <memory>
#include <zupply/src/zupply.hpp>
#include "Score.h"

namespace music {
    /**
     * The MIDIPlayer class makes use of the RtMidi library, which allows for output ports to be detected and
     * sound to be played.
     *
     * @note    This class is a singleton class, e.g. it can/should only be instantiated once!
     *
     * @note    It is possible some static is generated when first playing the sound.
     *          Also, sometimes the sound may not be played at all.
     *
     * TODO: Fetch logger via IOHandler class
     */
    class MIDIPlayer {
    public:
        /**
         * Deleted copy constructor
         */
        MIDIPlayer(const MIDIPlayer&) = delete;

        /**
         * Deleted copy assignment
         */
        MIDIPlayer& operator=(const MIDIPlayer&) = delete;

    public:
        /**
         * Gets the instance of the MIDIPlayer in a thread-safe way.
         * @return The singleton instance.
         */
        static std::shared_ptr<MIDIPlayer> instance();

        /**
         * Log all output port information.
         * @param logger The logger to log this info to.
         *
         * @note    The implementation of this function is based upon the midiprobe file from RtMidi,
         *          created by Gary Scavone, 2003-2012.
         */
        void probe(zz::log::LoggerPtr& logger) const;

        /**
         * Play a certain Score
         * @param score     The Score to play
         * @param logger    The logger to log all info to.
         */
        void play(const Score& score, zz::log::LoggerPtr &logger) const;

    private:
        /**
         * The default constructor is private, which allows this class to be instantiated only once.
         */
        MIDIPlayer() = default;
    };
}


#endif //AUTOPLAY_MIDIPLAYER_H