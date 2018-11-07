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
            explicit Part(std::shared_ptr<Instrument> instrument, const MeasureList& measures = {})
                : m_measures(measures), m_instruments({std::move(instrument)}), m_instrument_name(""), m_lines(5) {}

            /**
             * Constructor of Part
             * @param instruments   The list of Instrument pointers to link.
             * @param measures      The vector of shared pointers to different Measure
             * objects.
             */
            explicit Part(const std::vector<std::shared_ptr<Instrument>>& instruments, const MeasureList& measures = {})
                : m_measures(measures), m_instruments(instruments), m_instrument_name("") {}

            /**
             * Adds an Instrument
             * @param i The Instrument to set
             */
            inline void addInstrument(std::shared_ptr<Instrument> i) { m_instruments.emplace_back(std::move(i)); }

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
             * Gets the Instrument vector of this Part
             * @return The Instrument vector (as a vector of pointers)
             */
            inline std::vector<std::shared_ptr<Instrument>> getInstruments() const { return m_instruments; }

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
             * Sets the name of the Instrument explicitly
             * @param name The name of the Instrument
             */
            inline void setInstrumentName(const std::string& name = "") { m_instrument_name = name; }

            /**
             * Gets the name of the Instrument, if set
             * @return The name of the Instrument
             */
            inline std::string getInstrumentName() const {
                if(m_instrument_name.empty()) {
                    if(m_instruments.empty()) {
                        return "";
                    } else {
                        return m_instruments.at(0)->getName();
                    }
                }
                return m_instrument_name;
            }

            /**
             * Set the amount of lines for this Part
             * @param lines The new amount of lines
             */
            inline void setLines(uint8_t lines) {
                assert(lines > 0 && lines < 10);
                m_lines = lines;
            }

            /**
             * Fetches the amount of lines for this Part
             * @return The amount of lines
             */
            inline uint8_t getLines() const { return m_lines; }

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
            MeasureList                              m_measures;        ///< The Measure pointers of this Part
            std::vector<std::shared_ptr<Instrument>> m_instruments;     ///< The Instrument vector of this Part
            std::string                              m_instrument_name; ///< The Instrument Name
            uint8_t                                  m_lines;           ///< The amount of lines for a stave
        };
    }
}

#endif // AUTOPLAY_PART_H
