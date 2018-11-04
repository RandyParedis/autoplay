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
                minmax.first  = music::Note::pitch("C" + std::to_string(4 + m_clef_octave_change));
                minmax.second = music::Note::pitch("C" + std::to_string(6 + m_clef_octave_change));
            } else if(m_sign == 'F') {
                minmax.first  = music::Note::pitch("C" + std::to_string(2 + m_clef_octave_change));
                minmax.second = music::Note::pitch("C" + std::to_string(4 + m_clef_octave_change));
            } else /* if(m_sign == 'C') */ {
                minmax.first  = music::Note::pitch("C" + std::to_string(3 + m_clef_octave_change));
                minmax.second = music::Note::pitch("C" + std::to_string(5 + m_clef_octave_change));
            }
            return minmax;
        }
    }
}