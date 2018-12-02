/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, Randy Paredis
 *
 *  Created on 27/10/2018
 */

#ifndef AUTOPLAY_MIDIPLAYER_H
#define AUTOPLAY_MIDIPLAYER_H

#include "../util/Config.h"
#include "Score.h"
#include <memory>

namespace autoplay {
    namespace music {
        /**
         * The MIDIPlayer class makes use of the RtMidi library, which allows for output
         * ports to be detected and
         * sound to be played.
         *
         * @note    This class is a singleton class, e.g. it can/should only be
         * instantiated once!
         *
         * @note    It is possible some static is generated when first playing the
         * sound.
         *          Also, sometimes the sound may not be played at all.
         */
        class MIDIPlayer
        {
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
             * @param config    The Config of the system
             *
             * @note    The implementation of this function is based upon the midiprobe file from RtMidi,
             *          created by Gary Scavone, 2003-2012.
             */
            void probe(const util::Config& config) const;

            /**
             * Play a certain Score
             * @param score     The Score to play
             * @param config    The Config of the system
             */
            void play(const Score& score, const util::Config& config) const;

        private:
            /**
             * The default constructor is private, which allows this class to be
             * instantiated only once.
             */
            MIDIPlayer() = default;
        };
    }
}

#endif // AUTOPLAY_MIDIPLAYER_H
