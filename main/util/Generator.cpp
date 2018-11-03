//
// Created by red on 28/10/18.
//

#include "Generator.h"
#include "Randomizer.h"

#include <sstream>

namespace autoplay {
    namespace util {
        Generator::Generator(const Config& config) : m_config(config) {
            // Set engine
            auto engine_name = m_config.conf<std::string>("engine");
            auto seed        = m_config.conf<unsigned long>("seed");
            m_rnengine(engine_name, seed);
        }

        music::Score Generator::generate() {
            // Setup default values
            // TODO: Randomize these
            unsigned int length     = 10; // Total amount of measures
            unsigned int part_count = 1;  // Number of parts
            uint8_t      divisions  = 24; // Amount of 'ticks' each quarter note takes
            std::pair<uint8_t, uint8_t> time = {4, 4};

            // Get Algorithm
            auto pitch_algo = getPitchAlgorithm();

            auto logger = m_config.getLogger();

            music::Score score;
            for(unsigned int i = 0; i < part_count; ++i) {
                // Generate Notes
                music::Measure measure{music::Clef::Treble(), time, divisions, m_config.conf<int>("style.fifths")};

                auto instrument = m_config.getInstrument("Acoustic Grand Piano");
                instrument->setChannel((uint8_t)i);
                std::shared_ptr<music::Part> part = std::make_shared<music::Part>(instrument);

                for(unsigned int j = 0; j < length * measure.max_length();) {
                    music::Note* prev = nullptr;
                    if(j > 0) {
                        music::Note n = measure.back();
                        prev          = &n;
                    }
                    std::vector<music::Note*> conc = {};
                    for(unsigned int p = 0; p < i; ++p) {
                        music::Note* n = score.getParts().at(p)->at(j);
                        if(n) {
                            conc.emplace_back(n);
                        }
                    }
                    uint8_t pitch    = pitch_algo(m_rnengine, prev, conc);
                    uint8_t duration = 24;
                    if(j + duration > length * measure.max_length()) {
                        duration = (uint8_t)(length * measure.max_length() - j);
                    }
                    music::Note note{pitch, duration};
                    measure.append(note);
                    j += duration;
                }
                part->setMeasures(measure);
                auto picked = Randomizer::pick_uniform<float>(m_rnengine, 0.0f, 1.0f);
                if(picked <= m_config.conf<float>("style.chance")) {
                    music::Note::Semitone s;

                    auto c  = part->back()->back().getPitch();
                    auto r  = music::Note::pitchRepr(c);
                    auto p  = music::Note::splitPitch(r, s);
                    auto p1 = music::Note::pitch(m_config.conf<std::string>("style.root") + std::to_string(p.second));
                    auto p2 =
                        music::Note::pitch(m_config.conf<std::string>("style.root") + std::to_string(p.second - 1));
                    auto p3 =
                        music::Note::pitch(m_config.conf<std::string>("style.root") + std::to_string(p.second + 1));
                    auto ap1 = std::abs(c - p1);
                    auto ap2 = std::abs(c - p2);
                    auto ap3 = std::abs(c - p3);
                    if(ap1 < ap2 && ap1 < ap3) {
                        part->back()->back().setPitch(p1);
                    } else if(ap2 < ap1 && ap2 < ap3) {
                        part->back()->back().setPitch(p2);
                    } else if(ap3 < ap2 && ap3 < ap1) {
                        part->back()->back().setPitch(p3);
                    } else {
                        part->back()->back().setPitch(p1);
                    }
                }
                score.addPart(part);
            }

            return score;
        }

        std::function<uint8_t(RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc)>
        Generator::getPitchAlgorithm() const {
            // Get algorithm variables
            auto algorithm = m_config.conf<std::string>("generation.pitch");

            if(algorithm == "random-piano") {
                return [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc) -> uint8_t {
                    auto lst = getPitches(21, 108);
                    return (uint8_t)Randomizer::pick_uniform(gen, lst);
                };
            } else {
                return [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc) -> uint8_t {
                    return (uint8_t)Randomizer::pick_uniform(gen, getPitches(0, 127));
                };
            }
        }

        std::vector<uint8_t> Generator::getPitches(uint8_t min, uint8_t max) const {
            std::vector<uint8_t> ret   = {};
            auto                 scale = m_config.conf<std::string>("style.scale");
            if(scale.length() != 12) {
                m_config.getLogger()->fatal("Invalid scale '{}'! Impossible to generate pitches.", scale);
                exit(EXIT_FAILURE);
            }

            // Generate pitch sequence
            auto              root = m_config.conf<char>("style.root");
            std::stringstream ss;
            ss << root << "-1";
            auto        min_root = music::Note::pitch(ss.str());
            std::string notes    = "";
            for(uint8_t i = 0; i < min_root; ++i) {
                notes.push_back((char)scale.at(scale.length() - 1 - i));
            }
            std::reverse(notes.begin(), notes.end());
            for(uint8_t i = min_root; i < 127; ++i) {
                notes.push_back((char)scale.at((unsigned)(i - min_root) % 12));
            }

            // Generate pitches
            uint8_t cnt = 0;
            for(const char& c : notes) {
                ++cnt;
                if(cnt < min) {
                    continue;
                }
                if(cnt > max) {
                    break;
                }
                if(c == '1') {
                    ret.emplace_back(cnt - 1);
                }
            }

            return ret;
        }
    }
}