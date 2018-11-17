//
// Created by red on 28/10/18.
//

#include "Generator.h"
#include "Randomizer.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>
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
            unsigned int  length     = 1; // Total amount of measures
            auto          parts      = m_config.conf_child("parts");
            unsigned long part_count = parts.size(); // Number of parts
            uint8_t       divisions  = 24;           // Amount of 'ticks' each quarter note takes
            std::pair<uint8_t, uint8_t> time = {4, 4};

            // Get Logger
            auto logger = m_config.getLogger();

            music::Score score{m_config.conf_child("export")};
            for(unsigned int i = 0; i < part_count; ++i) {
                auto pt_part = ptree_at(parts, i);

                pt::ptree options;
                options.put("stave", i);
                if(pt_part.count("generation") == 0 && m_config.conf_child("generation").count("options") == 1) {
                    BOOST_FOREACH(auto& var, m_config.conf_child("generation.options")) {
                        options.put_child(var.first, var.second);
                    }
                } else if(pt_part.get_child("generation").count("options") == 1) {
                    BOOST_FOREACH(auto& var, pt_part.get_child("generation.options")) {
                        options.put_child(var.first, var.second);
                    }
                }

                auto pitch_algo = getPitchAlgorithm(pt_part.get<std::string>("generation.pitch", ""));

                // Set Instrument(s)
                bool                                            percussion;
                std::vector<std::shared_ptr<music::Instrument>> instruments = {};
                std::map<std::string, std::string>                        repr_to_head;
                std::map<std::string, std::shared_ptr<music::Instrument>> repr_to_inst;
                if(pt_part.count("instrument") == 0) {
                    percussion = false;
                    BOOST_FOREACH(auto& inst, pt_part.get_child("instruments")) {
                        auto instrument = m_config.getInstrument(inst.second.get<std::string>("instrument"));
                        instrument->setChannel(10);
                        instruments.emplace_back(instrument);

                        auto display = inst.second.get<std::string>("display", "C4");
                        repr_to_head.insert(std::make_pair(display, inst.second.get<std::string>("symbol", "normal")));
                        repr_to_inst.insert(std::make_pair(display, instrument));
                    }
                } else {
                    auto instrument = m_config.getInstrument(pt_part.get<std::string>("instrument"));
                    percussion      = instrument->isPercussion();
                    instrument->setChannel((uint8_t)(i + 1));
                    if(percussion) {
                        instrument->setChannel(10);

                        auto display = pt_part.get<std::string>("display", "C4");
                        repr_to_head.insert(std::make_pair(display, pt_part.get<std::string>("symbol", "normal")));
                        repr_to_inst.insert(std::make_pair(display, instrument));
                    }
                    instruments.emplace_back(instrument);
                }

                // Generate Notes
                music::Clef clef = music::Clef::Treble();
                if(percussion) {
                    clef.setPercussion(true);
                } else {
                    clef = music::Clef{pt_part.get<unsigned char>("clef.sign", 'G'),
                                       (uint8_t)pt_part.get<int>("clef.line", 2),
                                       pt_part.get<int>("clef.octave-change", 0)};
                }
                music::Measure measure{clef, time, divisions, m_config.conf<int>("style.fifths")};

                std::shared_ptr<music::Part> part = std::make_shared<music::Part>(instruments);
                part->setLines((uint8_t)pt_part.get<int>("lines", 5));

                part->setInstrumentName(pt_part.get<std::string>("name", ""));

                for(unsigned int j = 0; j < length * measure.max_length();) {
                    std::shared_ptr<music::Note> prev;
                    if(j > 0) {
                        prev = std::make_shared<music::Note>(measure.back());
                    }
                    std::vector<music::Note*> conc = {};
                    for(unsigned int p = 0; p < i; ++p) {
                        music::Note* n = score.getParts().at(p)->at(j);
                        if(n) {
                            conc.emplace_back(n);
                        }
                    }
                    uint8_t pitch    = pitch_algo(m_rnengine, prev.get(), conc, options);
                    uint8_t duration = 24;
                    if(j + duration > length * measure.max_length()) {
                        duration = (uint8_t)(length * measure.max_length() - j);
                    }
                    music::Note note{pitch, duration};

                    auto prepr = music::Note::pitchRepr(pitch);
                    if(pt_part.count("instrument") == 0 || percussion) {
                        note.setHead(repr_to_head.at(prepr));
                        note.setInstrument(repr_to_inst.at(prepr));
                    }

                    measure.append(note);
                    j += duration;
                }

                part->setMeasures(measure);
                auto picked = Randomizer::pick_uniform<float>(m_rnengine, 0.0f, 1.0f);
                if(!percussion && picked <= m_config.conf<float>("style.chance")) {
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

        std::function<uint8_t(RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc, pt::ptree pt)>
        Generator::getPitchAlgorithm(std::string algo) const {
            // Get algorithm variables
            if(algo.empty()) {
                algo = m_config.conf<std::string>("generation.pitch");
            }

            m_config.getLogger()->debug("Using Pitch Algorithm '{}'", algo);

            if(algo == "random-piano") {
                return
                    [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc, pt::ptree pt) -> uint8_t {
                        auto stave = pt.get<int>("stave");
                        return (uint8_t)Randomizer::pick_uniform(gen, getPitches(21, 108, stave));
                    };
            } else if(algo == "contain-stave") {
                return
                    [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc, pt::ptree pt) -> uint8_t {
                        auto        stave  = pt.get<int>("stave");
                        auto        clef_n = ptree_at(m_config.conf_child("parts"), (uint8_t)stave).get_child("clef");
                        music::Clef clef   = music::Clef::Treble();
                        clef.setSign(clef_n.get<unsigned char>("sign", 'G'));
                        clef.setLine((uint8_t)clef_n.get<int>("line", 2));
                        clef.setOctaveChange(clef_n.get<int>("octave-change", 0));
                        auto range = clef.range();

                        return (uint8_t)Randomizer::pick_uniform(gen, getPitches(range.first, range.second, stave));
                    };
            } else if(algo == "brownian-motion") {
                return
                    [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc, pt::ptree pt) -> uint8_t {
                        auto        stave  = pt.get<int>("stave");
                        auto        clef_n = ptree_at(m_config.conf_child("parts"), (uint8_t)stave).get_child("clef");
                        music::Clef clef   = music::Clef::Treble();
                        clef.setSign(clef_n.get<unsigned char>("sign", 'G'));
                        clef.setLine((uint8_t)clef_n.get<int>("line", 2));
                        clef.setOctaveChange(clef_n.get<int>("octave-change", 0));
                        auto range = clef.range();

                        auto pitches = getPitches(range.first, range.second, stave);

                        if(prev) {
                            auto it = std::find(pitches.begin(), pitches.end(), prev->getPitch());
                            if(it == pitches.end()) {
                                throw std::invalid_argument("Note '" + std::to_string(prev->getPitch()) +
                                                            "' not in scale!");
                            }
                            auto idx = std::distance(pitches.begin(), it);
                            auto min = pt.get<long>("pitch.min", -3);
                            auto max = pt.get<long>("pitch.max", 3);
                            if(idx + min < 0) {
                                min = -idx;
                            }
                            if(idx + max >= (signed)pitches.size()) {
                                max = pitches.size() - idx - 1;
                            } else if(idx + max < 0) {
                                max = -idx;
                            }
                            idx = Randomizer::pick_uniform(gen, (int)min, (int)max);
                            return pitches.at((unsigned)idx);
                        } else {
                            return (uint8_t)Randomizer::pick_uniform(gen, pitches);
                        }
                    };
            } else {
                return
                    [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc, pt::ptree pt) -> uint8_t {
                        auto stave = pt.get<int>("stave");
                        return (uint8_t)Randomizer::pick_uniform(gen, getPitches(0, 127, stave));
                    };
            }
        }

        std::vector<uint8_t> Generator::getPitches(uint8_t min, uint8_t max, int stave) const {
            std::vector<uint8_t> ret   = {};
            auto                 scale = m_config.conf<std::string>("style.scale");
            if(scale.length() != 12) {
                m_config.getLogger()->fatal("Invalid scale '{}'! Impossible to generate pitches.", scale);
                exit(EXIT_FAILURE);
            }

            // Generate full scale
            std::stringstream ss;

            auto root = m_config.conf<char>("style.root");
            ss << root << "-1";
            auto min_root = music::Note::pitch(ss.str());

            std::string notes;

            for(uint8_t i = 0; i < min_root; ++i) {
                notes.push_back((char)scale.at(scale.length() - 1 - i));
            }
            std::reverse(notes.begin(), notes.end());
            for(uint8_t i = min_root; i < 127; ++i) {
                notes.push_back((char)scale.at((unsigned)(i - min_root) % 12));
            }

            // Check for multiple Instrument(s)
            auto part = ptree_at(m_config.conf_child("parts"), (size_t)stave);
            if(part.count("instrument") == 0 ||
               m_config.getInstrument(part.get<std::string>("instrument"))->isPercussion()) {
                boost::replace_all(notes, "1", "0");
                if(part.count("instruments") == 0) {
                    auto display = part.get<std::string>("display");
                    auto p       = music::Note::pitch(display);
                    if(0 < p && p < 127) {
                        notes.replace(p, 1, "1");
                    }
                } else {
                    BOOST_FOREACH(auto& var, part.get_child("instruments")) {
                        auto p = music::Note::pitch(var.second.get<std::string>("display"));
                        if(0 < p && p < 127) {
                            notes.replace(p, 1, "1");
                        }
                    }
                }
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