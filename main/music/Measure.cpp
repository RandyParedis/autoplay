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

#include "Measure.h"
#include <iostream>

namespace autoplay {
    namespace music {
        bool Measure::hasAttributes() const {
            return m_time.first != 0 && m_time.second != 0 && m_divisions > 0 && m_bpm != 0;
        }

        void Measure::setTime(uint8_t beats, uint8_t beat_type) { this->setTime({beats, beat_type}); }

        void Measure::setTime(const std::pair<uint8_t, uint8_t>& time) {
            assert(time.first != 0 && time.second != 0);
            assert(time.second % 2 == 0);
            m_time.first  = time.first;
            m_time.second = time.second;
        }

        void Measure::setCommonTime() { this->setTime({4, 4}); }

        void Measure::setCutTime() { this->setTime({2, 2}); }

        unsigned int Measure::length() const {
            unsigned int length = 0;
            for(const Chord& c : m_notes) {
                if(!c.empty()) {
                    length += c.getDuration();
                }
            }
            return length;
        }

        unsigned int Measure::max_length() const {
            if(hasAttributes()) {
                return (unsigned int)(4 * m_time.first * m_divisions / m_time.second);
            }
            return 0;
        }

        bool Measure::isOverflowing() const {
            unsigned int l = length();
            unsigned int m = max_length();
            return l > m || m == 0;
        }

        MeasureList Measure::measurize() const {
            std::shared_ptr<Measure> me = std::make_shared<Measure>(m_clef, m_time, m_divisions, m_fifths);
            me->setBPM(m_bpm);
            MeasureList res = {me};
            if(isOverflowing()) {
                for(const Chord& c : m_notes) {
                    std::shared_ptr<Measure> m     = res.back();
                    unsigned int             m_len = m->length();
                    if(m_len + c.getDuration() <= m->max_length()) {
                        m->m_notes.emplace_back(c);
                    } else if(m_len == m->max_length() && c.getDuration() < m->max_length()) {
                        res.emplace_back(std::make_shared<Measure>(m_clef, m_time, m_divisions, m_fifths));
                        res.back()->setBPM(m_bpm);
                        res.back()->m_notes.emplace_back(c);
                    } else {
                        // Create new notes
                        std::vector<Chord> v = {};

                        unsigned int da = m->max_length() - m_len;

                        Chord a{c};
                        a.setDuration(da);
                        v.emplace_back(a);

                        unsigned int db = c.getDuration() - da;
                        while(db > m->max_length()) {
                            Chord b{c};
                            b.setDuration(m->max_length());
                            v.emplace_back(b);
                            db -= m->max_length();
                        }
                        Chord b{c};
                        b.setDuration(db);
                        v.emplace_back(b);

                        for(unsigned int i = 0; i < v.size(); ++i) {
                            // Link/tie all Notes
                            if(i < v.size() - 1) {
                                v[i].setTieStart();
                                v[i + 1].setTieEnd();
                            }

                            // Append to measures
                            m->m_notes.emplace_back(v[i]);
                            if(i < v.size() - 1) {
                                res.emplace_back(std::make_shared<Measure>(m_clef, m_time, m_divisions, m_fifths));
                                res.back()->setBPM(m_bpm);
                                m = res.back();
                            }
                        }
                    }
                }
            } else {
                res.back()->m_notes = m_notes;
            }
            return res;
        }

        MeasureList Measure::operator+(const Measure& rhs) {
            for(const Chord& c : rhs.m_notes) {
                *this += c;
            }
            return this->measurize();
        }

        Measure* Measure::operator+(const Note& rhs) {
            Chord chord{rhs};
            this->m_notes.emplace_back(chord);
            return this;
        }

        Measure* Measure::operator+(const Chord& rhs) {
            this->m_notes.emplace_back(rhs);
            return this;
        }

        void Measure::operator+=(const Note& rhs) { *this + rhs; }

        void Measure::operator+=(const Chord& rhs) { *this + rhs; }
    }
}