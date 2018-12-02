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
 *  Created on 07/10/2018
 */

#include "Note.h"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>
#include <utility>

#include <boost/algorithm/string/predicate.hpp>

namespace autoplay {
    namespace music {
        const uint8_t Note::DEFAULT_ON  = 90;
        const uint8_t Note::DEFAULT_OFF = 40;

        std::map<std::string, float> Note::DURATION = {
            {"256th", 1.0f / 256}, {"1/256", 1.0f / 256}, {"128th", 1.0f / 128}, {"1/128", 1.0f / 128},
            {"64th", 1.0f / 64},   {"1/64", 1.0f / 64},   {"32nd", 1.0f / 32},   {"1/32", 1.0f / 32},
            {"16th", 1.0f / 16},   {"1/16", 1.0f / 16},   {"8th", 1.0f / 8},     {"1/8", 1.0f / 8},
            {"eighth", 1.0f / 8},  {"4th", 1.0f / 4},     {"1/4", 1.0f / 4},     {"quarter", 1.0f / 4},
            {"half", 1.0f / 2},    {"1/2", 1.0f / 2},     {"whole", 1.0f},       {"breve", 2.0f},
            {"long", 4.0f}};

        std::vector<unsigned char> Note::getMessage(uint8_t channel, bool note_on) const {
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
                no  = 0x90;
                vel = this->m_velocity_on;
            } else {
                no  = 0x80;
                vel = this->m_velocity_off;
            }
            no += channel;

            return {no, this->m_pitch, vel};
        }

        std::vector<unsigned char> Note::getOnMessage(uint8_t channel) const { return this->getMessage(channel, true); }

        std::vector<unsigned char> Note::getOffMessage(uint8_t channel) const {
            return this->getMessage(channel, false);
        }

        uint8_t Note::pitch(const std::string& name, const Note::Semitone& s) {
            // Note range is C-1 to G9
            assert(name.length() == 2 || (name.length() == 3 && name[1] == '-'));
            std::vector<char>    notes{'C', 'D', 'E', 'F', 'G', 'A', 'B'};
            std::vector<uint8_t> next{2, 2, 1, 2, 2, 2, 1};
            Note::Semitone       q;
            std::pair<char, int> sp = splitPitch(name, q);
            assert(q == Note::Semitone::NONE);
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
            case Note::Semitone::FLAT: pitch -= 1; break;

            case Note::Semitone::SHARP: pitch += 1; break;

            default: break;
            }

            return pitch;
        }

        uint8_t Note::pitch(const std::string& name) {
            assert(name.length() > 1);
            if(name[1] == 'b' or name[1] == '#') {
                std::stringstream ss;
                ss << name[0];
                ss << name.substr(2);
                return pitch(ss.str(), name[1] == 'b' ? Note::Semitone::FLAT : Note::Semitone::SHARP);
            } else {
                return pitch(name, Note::Semitone::NONE);
            }
        }

        uint8_t Note::pitch(float hertz) {
            // The algorithm / formula is in fact a derivative from
            // the main formula on https://pages.mtu.edu/~suits/NoteFreqCalcs.html
            std::pair<float, uint8_t> base = {440.0f, 69};

            float freq = hertz / base.first;
            float a    = std::pow(2.0f, (float)1 / 12);

            uint8_t result = base.second + (uint8_t)std::round(std::log(freq) / std::log(a));
            return result;
        }

        std::string Note::pitchRepr(const uint8_t& p, Semitone& alter) {
            std::stringstream    ss;
            std::vector<char>    notes{'C', 'D', 'E', 'F', 'G', 'A', 'B'};
            std::vector<uint8_t> next{2, 2, 1, 2, 2, 2, 1};

            uint8_t pitch = p;
            uint8_t diff  = 0;
            while(pitch % 12 != 0) {
                --pitch;
                ++diff;
            }

            uint8_t note = 0;
            char    step = 'U';
            for(unsigned int idx = 0; idx < next.size(); ++idx) {
                if(diff <= note) {
                    step = notes[idx];
                    if(diff == note) {
                        alter = Semitone::NONE;
                    } else {
                        alter = Semitone::FLAT;
                    }
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

        std::string Note::pitchRepr(const uint8_t& p) {
            Semitone          s;
            std::string       repr = pitchRepr(p, s);
            std::stringstream ss;
            ss << repr[0];
            if(s == Semitone::SHARP) {
                ss << '#';
            } else if(s == Semitone::FLAT) {
                ss << 'b';
            }
            ss << repr.substr(1);
            return ss.str();
        }

        std::pair<char, int> Note::splitPitch(const std::string& name, Semitone& alter) {
            assert(name.length() > 1);
            std::vector<char> notes{'C', 'D', 'E', 'F', 'G', 'A', 'B'};
            auto              it = std::find(notes.begin(), notes.end(), name[0]);
            assert(it != notes.end());
            char step = *it;
            int  pitch;
            if(name[1] == 'b' or name[1] == '#') {
                pitch = std::stoi(name.substr(2));
                if(name[1] == 'b') {
                    alter = Semitone::FLAT;
                } else {
                    alter = Semitone::SHARP;
                }
            } else {
                alter = Semitone::NONE;
                pitch = std::stoi(name.substr(1));
            }
            assert(pitch < 10);
            return {step, pitch};
        }

        bool Note::operator==(const Note& rhs) const {
            return this->m_pitch == rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                   this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                   this->m_pause == rhs.isPause();
        }

        bool Note::operator!=(const Note& rhs) const {
            return this->m_pitch != rhs.getPitch() || this->m_duration != rhs.getDuration() ||
                   this->m_velocity_on != rhs.getVelocityOn() || this->m_velocity_off != rhs.getVelocityOff() ||
                   this->m_pause != rhs.isPause();
        }

        bool Note::operator<=(const Note& rhs) const {
            return (this->m_pitch <= rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                    this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause()) ||
                   (this->m_pitch == rhs.getPitch() && this->m_duration <= rhs.getDuration() &&
                    this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause()) ||
                   (this->m_pitch == rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                    this->m_velocity_on <= rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause()) ||
                   (this->m_pitch == rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                    this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off <= rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause());
        }

        bool Note::operator>=(const Note& rhs) const {
            return (this->m_pitch >= rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                    this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause()) ||
                   (this->m_pitch == rhs.getPitch() && this->m_duration >= rhs.getDuration() &&
                    this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause()) ||
                   (this->m_pitch == rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                    this->m_velocity_on >= rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause()) ||
                   (this->m_pitch == rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                    this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off >= rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause());
        }

        bool Note::operator<(const Note& rhs) const {
            return (this->m_pitch < rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                    this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause()) ||
                   (this->m_pitch == rhs.getPitch() && this->m_duration < rhs.getDuration() &&
                    this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause()) ||
                   (this->m_pitch == rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                    this->m_velocity_on < rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause()) ||
                   (this->m_pitch == rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                    this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off < rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause());
        }

        bool Note::operator>(const Note& rhs) const {
            return (this->m_pitch > rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                    this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause()) ||
                   (this->m_pitch == rhs.getPitch() && this->m_duration > rhs.getDuration() &&
                    this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause()) ||
                   (this->m_pitch == rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                    this->m_velocity_on > rhs.getVelocityOn() && this->m_velocity_off == rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause()) ||
                   (this->m_pitch == rhs.getPitch() && this->m_duration == rhs.getDuration() &&
                    this->m_velocity_on == rhs.getVelocityOn() && this->m_velocity_off > rhs.getVelocityOff() &&
                    this->m_pause == rhs.isPause());
        }

        std::string Note::getHeadName() const {
            std::vector<std::string> fillable = {"normal", "diamond", "triangle", "square"};
            for(const auto& begin : fillable) {
                if(boost::algorithm::starts_with(m_head, begin)) {
                    return begin;
                }
            }
            return m_head;
        }

        bool Note::canBeFilled() const {
            std::vector<std::string> fillable = {"normal", "diamond", "triangle", "square"};
            for(const auto& begin : fillable) {
                if(boost::algorithm::starts_with(m_head, begin)) {
                    return true;
                }
            }
            return false;
        }

        bool Note::getHeadFilled() const {
            std::vector<std::string> fillable = {"normal", "diamond", "triangle", "square"};
            bool                     fill     = false;
            for(const auto& begin : fillable) {
                if(boost::algorithm::starts_with(m_head, begin)) {
                    fill = true;
                    break;
                }
            }
            return (fill && !boost::algorithm::ends_with(m_head, "-empty"));
        }

        std::string Note::getType(const int& divisions) const {
            float dur   = (std::pow(2.0f, std::floor(std::log2f(m_duration))) / (float)divisions) / 4.0f;
            auto nearby = [](float val, float ref) -> bool { return val < ref + 0.0000001 && val > ref - 0.0000001; };
            if(nearby(dur, 1.0f / 256.0f)) {
                return "256th";
            } else if(nearby(dur, 1.0f / 128.0f)) {
                return "128th";
            } else if(nearby(dur, 1.0f / 64.0f)) {
                return "64th";
            } else if(nearby(dur, 1.0f / 32.0f)) {
                return "32nd";
            } else if(nearby(dur, 1.0f / 16.0f)) {
                return "16th";
            } else if(nearby(dur, 1.0f / 8.0f)) {
                return "eighth";
            } else if(nearby(dur, 1.0f / 4.0f)) {
                return "quarter";
            } else if(nearby(dur, 1.0f / 2.0f)) {
                return "half";
            } else if(nearby(dur, 1.0f)) {
                return "whole";
            } else if(nearby(dur, 2.0f)) {
                return "breve";
            } else if(nearby(dur, 4.0f)) {
                return "long";
            }
            throw std::runtime_error("Division value (" + std::to_string((int)divisions) +
                                     ") gave an unknown duration of " + std::to_string(dur) + ", which came from " +
                                     std::to_string(m_duration) + ".");
        }

        std::vector<Chord> Chord::splitByDivisions(const int& divisions, bool generatedots) const {
            std::vector<Chord> ref;

            if(getDuration() == 0) {
                throw std::runtime_error("Duration of Chord is 0!");
            }

            float rem = 0.0f;
            float T   = (float)getDuration() / (4.0f * divisions);
            int   n   = 1;

            auto nearby = [](float val, float ref) -> bool { return val < ref + 0.00000001 && val > ref - 0.00000001; };

            // Add filled notes
            while(n != 0) {
                T -= rem;
                float _n = std::log2(T) + 8;
                n        = (int)std::floor(_n);
                rem      = (float)std::pow(2.0f, n - 8);
                Chord chord{*this};
                chord.setDuration((unsigned int)(rem * divisions * 4));
                chord.setTieStart();
                chord.setTieEnd();
                ref.emplace_back(chord);

                if(nearby(_n - n, 0.0f)) {
                    break;
                }
            }

            // Add dots
            if(ref.size() > 1 && generatedots) {
                uint8_t max_dots = 2;
                uint8_t dots     = 0;
                for(auto i = ref.size() - 1; i > 0; --i) {
                    if(dots < max_dots && ref.at(i).getDuration() * 2 == ref.at(i - 1).getDuration()) {
                        ++dots;
                        ref.erase(ref.begin() + i);
                    } else {
                        ref.at(i).setDots(dots);
                        auto dur    = ref.at(i).getDuration();
                        auto newdur = dur;
                        for(uint8_t j = 1; j <= dots; ++j) {
                            newdur += dur / (int)std::pow(2, j);
                        }
                        ref.at(i).setDuration(newdur);
                        dots = 0;
                    }
                }
            }

            ref.at(0).setTieEnd(this->getTieEnd());
            ref.back().setTieStart(this->getTieStart());

            return ref;
        }

        std::shared_ptr<Note> Chord::bottom() const {
            if(empty()) {
                return nullptr;
            }
            unsigned int idx = 0;
            for(unsigned int i = 0; i < m_notes.size(); ++i) {
                if(m_notes.at(i)->getPitch() < m_notes.at(idx)->getPitch()) {
                    idx = i;
                }
            }
            return m_notes.at(idx);
        }
    }
}