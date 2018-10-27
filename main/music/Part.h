//
// Created by red on 27/10/18.
//

#ifndef AUTOPLAY_PART_H
#define AUTOPLAY_PART_H

#include <vector>
#include <memory>
#include "Measure.h"
#include "Instrument.h"

namespace music {
    /**
     * The Part class is a container that links an instrument to a set of measures.
     */
    class Part {
    public:
        /**
         * Constructor of Part
         * @param instrument    The Instrument to link.
         * @param measures      The vector of shared pointers to different Measure objects.
         */
        explicit Part(Instrument* instrument=nullptr, const MeasureList& measures={}):
                m_measures(measures), m_instrument(instrument) {}

        /**
         * Sets an Instrument
         * @param i The Instrument to set
         */
        inline void setInstrument(Instrument* i) { m_instrument = i; }

        /**
         * Sets a series of Measure pointers
         * @param m A vector of shared pointers to the Measure pointers
         */
        inline void setMeasures(const MeasureList& m) { m_measures = m; }

        /**
         * Sets a series of Measure pointers via a single (usually overflowing) Measure
         * @param m Measure that can be measurized.
         */
        inline void setMeasures(const Measure& m) { m_measures = m.measurize(); }

        /**
         * Gets the Instrument of this Part
         * @return The Instrument (as pointer)
         */
        inline Instrument* getInstrument() const { return m_instrument; }

        /**
         * Gets the Measures as a MeasureList object
         * @return The Measures
         */
        inline MeasureList getMeasures() const { return m_measures; }

        /**
         * Fetches the back of the m_measures vector
         * @return The last Measure of this Part
         */
        inline std::shared_ptr<Measure> back() const { return m_measures.back(); }
    private:
        MeasureList m_measures;   ///< The Measure pointers of this Part
        Instrument* m_instrument; ///< The Instrument of this Part
    };
}


#endif //AUTOPLAY_PART_H
