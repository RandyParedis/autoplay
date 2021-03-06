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
 *  Created on 21/10/2018
 */

#ifndef AUTOPLAY_CLEF_H
#define AUTOPLAY_CLEF_H

#include <cassert>
#include <cstdint>
#include <utility>

namespace autoplay {
    namespace music {
        /**
         * The Clef class helps with defining a specific clef
         */
        class Clef
        {
        public:
            /**
             * Default Clef constructor
             * @param sign          The sign of the Clef
             * @param line          The line where the Clef is anchored
             * @param octave_change The change in octave if required
             */
            Clef(unsigned char sign, uint8_t line, int octave_change = 0) noexcept
                : m_sign(sign), m_line(line), m_clef_octave_change(octave_change), m_percussion(false) {
                assert(sign == 'G' || sign == 'F' || sign == 'C');
                assert(line > 0 && line < 6);
            }

            static Clef Treble() { return Clef{'G', 2}; } ///< The Treble Clef (G on the second line)
            static Clef Bass() { return Clef{'F', 4}; }   ///< The Bass Clef (F on the fourth line)
            static Clef Alto() { return Clef{'C', 3}; }   ///< The Alto Clef (C on the middle/third line)

            /**
             * Gets the sign of the Clef
             * @return sign of the Clef
             */
            inline unsigned char getSign() const { return m_sign; }

            /**
             * Gets the line of the Clef
             * @return line of the Clef
             */
            inline uint8_t getLine() const { return m_line; }

            /**
             * Gets the octave change of the Clef
             * @return octave change of the Clef
             */
            inline int getOctaveChange() const { return m_clef_octave_change; }

            /**
             * Sets the sign of the Clef
             * @param sign The new sign of the Clef
             */
            inline void setSign(const unsigned char& sign) {
                assert(sign == 'G' || sign == 'F' || sign == 'C');
                m_sign = sign;
            }

            /**
             * Sets the line of the Clef
             * @param line The new line of the Clef
             */
            inline void setLine(const uint8_t& line) {
                assert(line > 0 && line < 6);
                m_line = line;
            }

            /**
             * Set this Clef to a percussion Clef and back again.
             * @param percussion When true, change to percussion.
             */
            inline void setPercussion(const bool& percussion) { m_percussion = percussion; }

            /**
             * Check if this Clef is a percussion Clef
             * @return true if it is
             */
            inline bool isPercussion() const { return m_percussion; }

            /**
             * Sets the octave change of the Clef
             * @param octave change The new line of the Clef
             */
            inline void setOctaveChange(const int& octave_change) { m_clef_octave_change = octave_change; }

            /**
             * Clef Equality Operator
             * @param rhs The other clef to check against this one
             * @return true if the Clefs are the same
             */
            bool operator==(const Clef rhs) const {
                return m_sign == rhs.m_sign && m_line == rhs.m_line && m_clef_octave_change == rhs.m_clef_octave_change;
            }

            /**
             * Clef Inequality Operator
             * @param rhs The other clef to check against this one
             * @return false if the Clefs are the same
             */
            bool operator!=(const Clef rhs) const { return !(this->operator==(rhs)); }

            /**
             * Get the range of this Clef
             * @return A pair <min, max> containing the range of this Clef.
             */
            std::pair<uint8_t, uint8_t> range();

        private:
            unsigned char m_sign;               ///< The sign of the Clef
            uint8_t       m_line;               ///< The line on which the Clef is anchored
            int           m_clef_octave_change; ///< The change in octave if required
            bool          m_percussion;         ///< If this Clef is percussion
        };
    }
}

#endif // AUTOPLAY_CLEF_H
