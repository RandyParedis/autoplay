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
 *  Created on 25/10/2018
 */

#ifndef AUTOPLAY_INSTRUMENT_H
#define AUTOPLAY_INSTRUMENT_H

#include <cassert>
#include <memory>
#include <string>
#include <vector>

namespace autoplay {
    namespace music {
        /**
         * The Instrument class handles all simple actions concerned to instruments and
         * the way certain things sound.
         */
        class Instrument
        {
        public:
            /**
             * Default Constructor
             * @param name          The name (as string) of an Instrument
             * @param channel       The channel of the Instrument
             * @param program       The program of the Instrument
             * @param unpitched     The index of the MIDI sound corresponding to the
             * Instrument
             */
            Instrument(std::string name, uint8_t channel, uint8_t program, uint8_t unpitched)
                : m_name(std::move(name)), m_channel(channel), m_program(program), m_unpitched(unpitched) {
                assert(channel < 16);
            }

            /**
             * Gets the name of the Instrument
             * @return The name
             */
            inline std::string getName() const { return m_name; }

            /**
             * Gets the channel of the Instrument
             * @return The channel
             */
            inline uint8_t getChannel() const { return m_channel; }

            /**
             * Gets the program of the Instrument
             * @return The program
             */
            inline uint8_t getProgram() const { return m_program; }

            /**
             * Gets the unpitched value of the Instrument
             * @return The unpitched value (index)
             */
            inline uint8_t getUnpitched() const { return m_unpitched; }

            /**
             * Sets the name of the Instrument
             * @param name New name to set
             */
            inline void setName(const std::string& name) { m_name = name; }

            /**
             * Sets the channel of the Instrument
             * @param channel New channel to set
             */
            inline void setChannel(const uint8_t& channel) {
                assert(channel < 16);
                m_channel = channel;
            }

            /**
             * Sets the program of the Instrument
             * @param program New program to set
             */
            inline void setProgram(const uint8_t& program) { m_program = program; }

            /**
             * Sets the unpitched of the Instrument
             * @param unpitched New unpitched to set
             */
            inline void setUnpitched(const uint8_t& unpitched) { m_unpitched = unpitched; }

            /**
             * Check if the Instrument is a percussion Instrument
             * @return true if the Instrument is of type 'percussion'
             */
            inline bool isPercussion() const { return m_unpitched > 0; }

        private:
            std::string m_name;      ///< The name of the Instrument
            uint8_t     m_channel;   ///< The channel of the Instrument
            uint8_t     m_program;   ///< The program of the Instrument
            uint8_t     m_unpitched; ///< The tone that must be played. In the MusicXML, we
                                     /// start counting from 1!
        };
    }
}

#endif // AUTOPLAY_INSTRUMENT_H
