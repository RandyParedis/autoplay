//
// Created by red on 07/10/18.
//

#include <algorithm>
#include <numeric>
#include <cmath>
#include <utility>
#include <sstream>
#include "Note.h"

namespace music {
    const uint8_t Note::DEFAULT_ON = 90;
    const uint8_t Note::DEFAULT_OFF = 40;

    std::vector<unsigned char> Note::getMessage(uint8_t channel, bool note_on) {
        // There are but 16 channels
        assert(channel < 16);

        unsigned char no, vel;

        /**
         * First byte:
         *      [Note On]   1001 nnnn
         *      [Note Off]  1000 nnnn
         *
         * With nnnn the Channel Number (0-15)
         *
         * Second byte:
         *      0kkk kkkk   <-  Key / Pitch
         *
         * Third byte:
         *      0vvv vvvv   <-  Velocity
         */

        if(note_on) {
            no = 0x90;
            vel = this->m_velocity_on;
        } else {
            no = 0x80;
            vel = this->m_velocity_off;
        }
        no += channel;

        return {no, this->m_pitch, vel};
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
        std::pair<char, int> sp = splitPitch(name);
        auto it = std::find(notes.begin(), notes.end(), sp.first);
        assert(it != notes.end());

        // Find octave
        auto pitch = (uint8_t)(sp.second + 1);
        assert(pitch < 10);
        pitch = (uint8_t)(pitch * 12);

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

    std::string Note::pitchRepr(const uint8_t& p, Semitone& alter) {
        std::stringstream ss;
        std::vector<char>   notes {'C', 'D', 'E', 'F', 'G', 'A', 'B'};
        std::vector<uint8_t> next { 2,   2,   1,   2,   2,   2,   1 };

        uint8_t pitch = p;
        uint8_t diff = 0;
        while(pitch % 12 != 0) {
            --pitch;
            ++diff;
        }

        uint8_t note = 0;
        char step = 'U';
        for(unsigned int idx = 0; idx < next.size(); ++idx) {
            if(diff <= note) {
                step = notes[idx];
                if(diff == note) { alter = Semitone::NONE; }
                else { alter = Semitone::FLAT; }
                break;
            }
            note += next[idx];
        }
        assert(step != 'U');
        ss << step;

        pitch = (uint8_t)((pitch / 12) - 1);
        assert(pitch < 10);
        ss << (int)pitch;

        return ss.str();
    }

    std::pair<char, int> Note::splitPitch(const std::string &name) {
        assert(name.length() == 2 || (name.length() == 3 && name[1] == '-'));
        std::vector<char> notes {'C', 'D', 'E', 'F', 'G', 'A', 'B'};
        auto it = std::find(notes.begin(), notes.end(), name[0]);
        assert(it != notes.end());
        char step = *it;
        int pitch = std::stoi(name.substr(1));
        assert(pitch < 10);
        return {step, pitch};
    }

    bool Note::operator==(const Note &rhs) const {
        return this->m_pitch == rhs.getPitch() &&
               this->m_duration == rhs.getDuration() &&
               this->m_velocity_on == rhs.getVelocityOn() &&
               this->m_velocity_off == rhs.getVelocityOff() &&
               this->m_pause == rhs.isPause();
    }

    bool Note::operator!=(const Note &rhs) const {
        return this->m_pitch != rhs.getPitch() ||
               this->m_duration != rhs.getDuration() ||
               this->m_velocity_on != rhs.getVelocityOn() ||
               this->m_velocity_off != rhs.getVelocityOff() ||
               this->m_pause != rhs.isPause();
    }

    bool Note::operator<=(const Note &rhs) const {
        return (this->m_pitch <= rhs.getPitch() &&
               this->m_duration == rhs.getDuration() &&
               this->m_velocity_on == rhs.getVelocityOn() &&
               this->m_velocity_off == rhs.getVelocityOff() &&
               this->m_pause == rhs.isPause()) ||
               (this->m_pitch == rhs.getPitch() &&
                this->m_duration <= rhs.getDuration() &&
                this->m_velocity_on == rhs.getVelocityOn() &&
                this->m_velocity_off == rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause()) ||
               (this->m_pitch == rhs.getPitch() &&
                 this->m_duration == rhs.getDuration() &&
                 this->m_velocity_on <= rhs.getVelocityOn() &&
                 this->m_velocity_off == rhs.getVelocityOff() &&
                 this->m_pause == rhs.isPause()) ||
               (this->m_pitch == rhs.getPitch() &&
                this->m_duration == rhs.getDuration() &&
                this->m_velocity_on == rhs.getVelocityOn() &&
                this->m_velocity_off <= rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause());
    }

    bool Note::operator>=(const Note &rhs) const {
        return (this->m_pitch >= rhs.getPitch() &&
                this->m_duration == rhs.getDuration() &&
                this->m_velocity_on == rhs.getVelocityOn() &&
                this->m_velocity_off == rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause()) ||
               (this->m_pitch == rhs.getPitch() &&
                this->m_duration >= rhs.getDuration() &&
                this->m_velocity_on == rhs.getVelocityOn() &&
                this->m_velocity_off == rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause()) ||
               (this->m_pitch == rhs.getPitch() &&
                this->m_duration == rhs.getDuration() &&
                this->m_velocity_on >= rhs.getVelocityOn() &&
                this->m_velocity_off == rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause()) ||
               (this->m_pitch == rhs.getPitch() &&
                this->m_duration == rhs.getDuration() &&
                this->m_velocity_on == rhs.getVelocityOn() &&
                this->m_velocity_off >= rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause());
    }

    bool Note::operator<(const Note &rhs) const {
        return (this->m_pitch < rhs.getPitch() &&
                this->m_duration == rhs.getDuration() &&
                this->m_velocity_on == rhs.getVelocityOn() &&
                this->m_velocity_off == rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause()) ||
               (this->m_pitch == rhs.getPitch() &&
                this->m_duration < rhs.getDuration() &&
                this->m_velocity_on == rhs.getVelocityOn() &&
                this->m_velocity_off == rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause()) ||
               (this->m_pitch == rhs.getPitch() &&
                this->m_duration == rhs.getDuration() &&
                this->m_velocity_on < rhs.getVelocityOn() &&
                this->m_velocity_off == rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause()) ||
               (this->m_pitch == rhs.getPitch() &&
                this->m_duration == rhs.getDuration() &&
                this->m_velocity_on == rhs.getVelocityOn() &&
                this->m_velocity_off < rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause());
    }

    bool Note::operator>(const Note &rhs) const {
        return (this->m_pitch > rhs.getPitch() &&
                this->m_duration == rhs.getDuration() &&
                this->m_velocity_on == rhs.getVelocityOn() &&
                this->m_velocity_off == rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause()) ||
               (this->m_pitch == rhs.getPitch() &&
                this->m_duration > rhs.getDuration() &&
                this->m_velocity_on == rhs.getVelocityOn() &&
                this->m_velocity_off == rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause()) ||
               (this->m_pitch == rhs.getPitch() &&
                this->m_duration == rhs.getDuration() &&
                this->m_velocity_on > rhs.getVelocityOn() &&
                this->m_velocity_off == rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause()) ||
               (this->m_pitch == rhs.getPitch() &&
                this->m_duration == rhs.getDuration() &&
                this->m_velocity_on == rhs.getVelocityOn() &&
                this->m_velocity_off > rhs.getVelocityOff() &&
                this->m_pause == rhs.isPause());
    }
}