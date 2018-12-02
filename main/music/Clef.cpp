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
 *  Created on 04/11/2018
 */

#include "Clef.h"
#include "Note.h"

namespace autoplay {
    namespace music {
        std::pair<uint8_t, uint8_t> music::Clef::range() {
            auto minmax = std::make_pair(0, 0);
            if(m_sign == 'G') {
                std::vector<std::string> signs = {"E", "C", "A", "F", "D"};
                unsigned long            idx   = (unsigned long)m_line - 1;
                std::string              sign  = signs.at(idx);
                minmax.first  = music::Note::pitch(sign + std::to_string((idx > 1 ? 3 : 4) + m_clef_octave_change));
                minmax.second = music::Note::pitch(sign + std::to_string((idx > 1 ? 5 : 6) + m_clef_octave_change));
            } else if(m_sign == 'F') {
                std::vector<std::string> signs = {"B", "G", "E", "C", "A"};
                unsigned long            idx   = (unsigned long)m_line - 1;
                std::string              sign  = signs.at(idx);
                minmax.first  = music::Note::pitch(sign + std::to_string((idx > 3 ? 1 : 2) + m_clef_octave_change));
                minmax.second = music::Note::pitch(sign + std::to_string((idx > 3 ? 3 : 4) + m_clef_octave_change));
            } else /* if(m_sign == 'C') */ {
                std::vector<std::string> signs = {"G", "E", "C", "A", "F"};
                unsigned long            idx   = (unsigned long)m_line - 1;
                std::string              sign  = signs.at(idx);
                minmax.first  = music::Note::pitch(sign + std::to_string((idx > 1 ? 2 : 3) + m_clef_octave_change));
                minmax.second = music::Note::pitch(sign + std::to_string((idx > 1 ? 4 : 5) + m_clef_octave_change));
            }
            return minmax;
        }
    }
}