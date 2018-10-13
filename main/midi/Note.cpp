//
// Created by red on 07/10/18.
//

#include <algorithm>
#include <numeric>
#include <cmath>
#include "Note.h"

namespace midi {
    std::vector<unsigned char> Note::getMessage(uint8_t channel, bool note_on) {
        // There are but 16 channels
        assert(channel < 16);

        std::vector<unsigned char> message{3, 0};

        /**
         * First byte:
         *      [Note On]   1001 nnnn
         *      [Note Off]  1000 nnnn
         *
         * With nnnn the Channel Number (0-15)
         *
         *
         * Third byte:
         *      0vvv vvvv   <-  Velocity
         */
        if(note_on) {
            message[0] = 144;
            message[2] = this->m_velocity_on;
        } else {
            message[0] = 128;
            message[2] = this->m_velocity_off;
        }
        message[0] += channel;

        /**
         * Second byte:
         *      0kkk kkkk   <-  Key / Pitch
         */
        message[1] = this->m_pitch;

        return message;
    }

    std::vector<unsigned char> Note::getOnMessage(uint8_t channel) {
        return this->getMessage(channel, true);
    }

    std::vector<unsigned char> Note::getOffMessage(uint8_t channel) {
        return this->getMessage(channel, false);
    }

    uint8_t Note::pitch(const std::string &name, Note::Semitone s) {
        // Note range is C-1 to G9
        assert(name.length() == 2 || (name.length() == 3 && name[1] == '-'));
        std::vector<char>   notes {'C', 'D', 'E', 'F', 'G', 'A', 'B'};
        std::vector<uint8_t> next { 2,   2,   1,   2,   2,   2,   1 };
        auto it = std::find(notes.begin(), notes.end(), name[0]);
        assert(it != notes.end());

        // Find octave
        uint8_t pitch = (uint8_t)(std::stoi(name.substr(1)));
        assert(pitch < 10);
        pitch = (uint8_t)((pitch + 1) * 12);

        // Find tone
        pitch += std::accumulate(next.begin(), next.begin() + (it - notes.begin()), 0);

        // Apply Semitone
        switch(s) {
            case Note::Semitone::FLAT:
                pitch -= 1;
                break;

            case Note::Semitone::SHARP:
                pitch += 1;
                break;

            default:
                break;
        }

        return pitch;
    }

    uint8_t Note::pitch(float hertz) {
        // The algorithm / formula is in fact a derivative from
        // the main formula on https://pages.mtu.edu/~suits/NoteFreqCalcs.html
        std::pair<float, uint8_t> base = {440.0f, 69};

        float freq = hertz / base.first;
        float a = std::pow(2.0f, (float)1/12);

        uint8_t result = base.second + (uint8_t)std::round(std::log(freq) / std::log(a));
        return result;
    }
}