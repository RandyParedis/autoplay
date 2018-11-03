//
// Created by red on 27/10/18.
//

#ifndef AUTOPLAY_PART_H
#define AUTOPLAY_PART_H

#include "Instrument.h"
#include "Measure.h"
#include <memory>
#include <vector>

namespace autoplay {
    namespace music {
        /**
         * The Part class is a container that links an instrument to a set of measures.
         */
        class Part
        {
        public:
            /**
             * Constructor of Part
             * @param instrument    The Instrument to link.
             * @param measures      The vector of shared pointers to different Measure
             * objects.
             */
            explicit Part(std::shared_ptr<Instrument> instrument = nullptr, const MeasureList& measures = {})
                : m_measures(measures), m_instrument(std::move(instrument)) {}

            /**
             * Sets an Instrument
             * @param i The Instrument to set
             */
            inline void setInstrument(std::shared_ptr<Instrument> i) { m_instrument = std::move(i); }

            /**
             * Sets a series of Measure pointers
             * @param m A vector of shared pointers to the Measure pointers
             */
            inline void setMeasures(const MeasureList& m) { m_measures = m; }

            /**
             * Sets a series of Measure pointers via a single (usually overflowing)
             * Measure
             * @param m Measure that can be measurized.
             */
            inline void setMeasures(const Measure& m) { m_measures = m.measurize(); }

            /**
             * Gets the Instrument of this Part
             * @return The Instrument (as pointer)
             */
            inline std::shared_ptr<Instrument> getInstrument() const { return m_instrument; }

            /**
             * Gets the Measures as a MeasureList object
             * @return The Measures
             */
            inline MeasureList getMeasures() const { return m_measures; }

            /**
             * Fetches the back of the m_measures vector
             * @return The last Measure of this Part
             */
            inline std::shared_ptr<Measure> back() const {
                if(m_measures.empty()) {
                    return nullptr;
                }
                return m_measures.back();
            }

            /**
             * Compute the note that is playing at a certain time
             * @param timestamp The time to look
             * @return A pointer to the Note that is playing, or nullptr if no Note has
             * been found.
             */
            Note* at(unsigned int timestamp) const {
                unsigned int c  = 0;
                unsigned int mi = 0;
                auto         m  = m_measures.at(mi);
                if(!m->hasAttributes()) {
                    throw std::invalid_argument("First measure of part has no time signature.");
                }

                // Find the right Measure
                auto len = m->max_length();
                while(c + len < timestamp && mi < m_measures.size()) {
                    m = m_measures.at(mi++);
                    if(m->hasAttributes()) {
                        len = m->max_length();
                    }
                    c += len;
                }

                // Timestamp is too large
                if(mi >= m_measures.size()) {
                    return nullptr;
                }

                // Find the right Note
                for(Note& note : m_measures.at(mi)->getNotes()) {
                    if(c + note.getDuration() < timestamp) {
                        c += note.getDuration();
                    } else {
                        return &note;
                    }
                }

                return nullptr;
            }

        private:
            MeasureList                 m_measures;   ///< The Measure pointers of this Part
            std::shared_ptr<Instrument> m_instrument; ///< The Instrument of this Part
        };
    }
}

#endif // AUTOPLAY_PART_H
