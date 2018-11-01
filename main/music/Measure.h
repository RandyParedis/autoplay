//
// Created by red on 21/10/18.
//

#ifndef AUTOPLAY_MEASURE_H
#define AUTOPLAY_MEASURE_H

#include "Clef.h"
#include "Note.h"
#include <cstdint>
#include <memory>
#include <utility>

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
    Measure() : m_staves(0), m_clef(Clef::Treble()), m_time({0, 0}), m_notes(), m_divisions(0) {}

    /**
     * Constructor with arguments
     * @param clef      The clef of the Measure
     * @param time      The time signature of the Measure
     * @param divisions The amount of divisions a quarter note takes
     * @param staves    The amount of staves
     */
    Measure(const Clef& clef, const std::pair<uint8_t, uint8_t> time, uint8_t divisions, uint8_t staves = 1)
        : m_staves(staves), m_clef(clef), m_time(time), m_notes(), m_divisions(divisions) {
        assert(m_time.first != 0 && m_time.second != 0);
        assert(m_divisions != 0);
        assert(time.second % 2 == 0);
    }

    /**
     * Checks if the Measure has its attributes set
     * @return true if staves > 0, time != {0,0} and divisions > 0
     */
    bool hasAttributes() const;

    /**
     * Set the amount of staves for the Measure
     * @param staves The new amount of staves
     */
    inline void setStaves(uint8_t staves = 1) { m_staves = staves; }

    /**
     * Get the amount of staves for this Measure
     * @return The amount of staves
     */
    inline uint8_t getStaves() const { return m_staves; }

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
    void setDivisions(const uint8_t& d) {
        assert(d != 0);
        m_divisions = d;
    }

    /**
     * Get the divisions of this Measure
     * @return the divisions of this Measure
     */
    inline uint8_t getDivisions() const { return m_divisions; }

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
     * Append a Note to the Measure. Alias of operator+
     * @param n The Note to append.
     */
    inline void append(const Note& n) { *this += n; }

    /**
     * Get the last Note of the Measure
     * @return The back of the m_notes vector
     */
    inline Note back() const { return m_notes.back(); }

    /**
     * Fetches all Notes from this Measure
     * @return A vector, containing all Notes.
     */
    inline std::vector<Note> getNotes() const { return m_notes; }

private:
    uint8_t m_staves;                   ///< The amount of staves for the measure (defaults to 1)
    Clef    m_clef;                     ///< The clef of the current measure
    std::pair<uint8_t, uint8_t> m_time; ///< The time of the measure, in the form of <beats, beat-type>

    std::vector<Note> m_notes;     ///< The Notes of this Measure
    uint8_t           m_divisions; ///< The divisions value (amount of duration per quarter
                                   /// Note) for this Measure
};
}

#endif // AUTOPLAY_MEASURE_H
