//
// Created by red on 04/11/18.
//

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