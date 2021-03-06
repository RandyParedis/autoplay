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

#ifndef AUTOPLAY_MEASURE_H
#define AUTOPLAY_MEASURE_H

#include "Clef.h"
#include "Note.h"
#include <cstdint>
#include <memory>
#include <utility>

namespace autoplay {
    namespace music {
        class Measure;

        typedef std::vector<std::shared_ptr<Measure>> MeasureList;

        /**
         * The measure class helps handling groups of Note objects in a valid way
         */
        class Measure
        {
        public:
            /**
             * Default constructor
             */
            Measure() : m_fifths(0), m_clef(Clef::Treble()), m_time({0, 0}), m_notes(), m_divisions(0), m_bpm(0) {}

            /**
             * Constructor with arguments
             * @param clef      The clef of the Measure
             * @param time      The time signature of the Measure
             * @param divisions The amount of divisions a quarter note takes
             * @param fifths    The fifths of this Measure
             */
            Measure(const Clef& clef, const std::pair<uint8_t, uint8_t> time, int divisions, int fifths = 0)
                : m_fifths(fifths), m_clef(clef), m_time(time), m_notes(), m_divisions(divisions), m_bpm(80) {
                assert(m_time.first != 0 && m_time.second != 0);
                assert(m_divisions > 0);
                assert(time.second % 2 == 0);
            }

            /**
             * Checks if the Measure has its attributes set
             * @return true if time != {0,0} and divisions > 0
             */
            bool hasAttributes() const;

            /**
             * Set the amount of fifths for the Measure
             * @param fifths The new amount of staves
             */
            inline void setFifths(int fifths = 0) { m_fifths = fifths; }

            /**
             * Get the fifths of staves for this Measure
             * @return The amount of fifths
             */
            inline int getFifths() const { return m_fifths; }

            /**
             * Sets the Clef for this Measure
             * @param clef The new Clef to set
             */
            inline void setClef(const Clef& clef) { m_clef = clef; }

            /**
             * Get the Clef for this Measure
             * @return The Clef
             */
            inline Clef getClef() const { return m_clef; }

            /**
             * Sets the time for the Measure
             * @param beats     The amount of beats (numerator)
             * @param beat_type The beat-type (denominator)
             */
            void setTime(uint8_t beats, uint8_t beat_type);

            /**
             * Sets the time for the Measure
             * @param time A pair in the form of <beats, beat-type>, representing the time
             */
            void setTime(const std::pair<uint8_t, uint8_t>& time);

            /**
             * Sets the time for a Measure to the 'common' time, e.g. 4/4
             */
            void setCommonTime();

            /**
             * Sets the time for a Measure to the 'cut' time, e.g. 2/2
             */
            void setCutTime();

            /**
             * Gets the time for the Measure
             * @return The time for the Measure in the form of <beats, beat-type>
             */
            inline std::pair<uint8_t, uint8_t> getTime() const { return m_time; };

            /**
             * Set the divisions
             * @param d The new division parameter
             */
            void setDivisions(const int& d) {
                assert(d > 0);
                m_divisions = d;
            }

            /**
             * Get the divisions of this Measure
             * @return the divisions of this Measure
             */
            inline int getDivisions() const { return m_divisions; }

            /**
             * Compute the length of all Notes in this Measure
             * @return The sum of all lengths
             */
            unsigned int length() const;

            /**
             * Computes the maximal length of this Measure, based on its time and
             * divisions signature
             * @return The max length of this Measure
             */
            unsigned int max_length() const;

            /**
             * Checks if total duration of all Notes of this Measure exceeds the length of
             * one Measure
             * @return True if there are too much Notes in this Measure
             */
            bool isOverflowing() const;

            /**
             * Takes this Measure and creates a vector containing all Measures that were
             * crammed in this one.
             * e.g. if isOverflowing() returns true.
             * @return A vector of shared pointers to Measures
             */
            MeasureList measurize() const;

            /**
             * Add two Measures together, with respect to their respective time and
             * duration attributes
             * @param rhs   The Measure to add to the end of this Measure
             * @return A list of the two Measures added (with respect to overflowing
             * Measures)
             */
            MeasureList operator+(const Measure& rhs);

            /**
             * Add a Note to this Measure
             * @param rhs The Note to add
             * @return This Measure, after the note was added
             */
            Measure* operator+(const Note& rhs);

            /**
             * Add a Note respectively to this Measure
             * @param rhs The Note to add
             */
            void operator+=(const Note& rhs);

            /**
             * Add a Chord to this Measure
             * @param rhs The Chord to add
             * @return This Measure, after the note was added
             */
            Measure* operator+(const Chord& rhs);

            /**
             * Add a Chord respectively to this Measure
             * @param rhs The Chord to add
             */
            void operator+=(const Chord& rhs);

            /**
             * Append a Note to the Measure. Alias of operator+
             * @param n The Note to append.
             */
            inline void append(const Note& n) { *this += n; }

            /**
             * Append a Chord to the Measure. Alias of operator+
             * @param n The Chord to append.
             */
            inline void append(const Chord& n) { *this += n; }

            /**
             * Get the last Chord of the Measure by reference
             * @return The back of the m_notes vector
             */
            inline Chord& back() { return m_notes.back(); }

            /**
             * Fetches all Chords from this Measure
             * @return A vector, containing all Notes.
             */
            inline std::vector<Chord> getNotes() const { return m_notes; }

            /**
             * Fetches all Chords from this Measure by reference
             * @return A vector, containing all Notes.
             */
            inline std::vector<Chord>& getNotes() { return m_notes; }

            /**
             * Sets the BPM. It uses the time as reference;
             *      e.g. if beat-type is 4, bpm is expressed in quarter notes
             * @param bpm The bpm to be set
             */
            inline void setBPM(int bpm) { m_bpm = bpm; }

            /**
             * Fetches the amount of BPM for the measure.
             * @return The BPM of the measure
             */
            inline int getBPM() const { return m_bpm; }

        private:
            int  m_fifths;                      ///< The amount of fifths for the measure (defaults to 0)
            Clef m_clef;                        ///< The clef of the current measure
            std::pair<uint8_t, uint8_t> m_time; ///< The time of the measure, in the form of <beats, beat-type>

            std::vector<Chord> m_notes; ///< The Notes of this Measure

            int m_divisions; ///< The divisions value (amount of duration per quarter Note) for this Measure
            int m_bpm;       ///< The amount of beats per minute (expressed wrt the time signature)
        };
    }
}

#endif // AUTOPLAY_MEASURE_H
