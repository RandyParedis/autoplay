//
// Created by red on 28/10/18.
//

#include "Generator.h"
#include "Randomizer.h"

#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>
#include <bitset>
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
            auto          length     = (unsigned)m_config.conf<int>("length", 10); // Total amount of measures
            auto          parts      = m_config.conf_child("parts");
            unsigned long part_count = parts.size(); // Number of parts
            int           divisions  = 64;           // Amount of 'ticks' each quarter note takes

            std::pair<uint8_t, uint8_t> time = {(uint8_t)m_config.conf<int>("style.time.beats", 4),
                                                (uint8_t)m_config.conf<int>("style.time.type", 4)};

            // Get Logger
            auto logger = m_config.getLogger();

            music::Score score{m_config.conf_child("export")};
            for(unsigned int i = 0; i < part_count; ++i) {
                auto pt_part = ptree_at(parts, i);

                pt::ptree options;
                options.put("stave", i);
                options.put("_p1fn._reinit", true);
                if(pt_part.count("generation") == 0) {
                    if(m_config.conf_child("generation").count("options") == 1) {
                        BOOST_FOREACH(auto& var, m_config.conf_child("generation.options")) {
                            options.put_child(var.first, var.second);
                        }
                    }
                } else if(pt_part.get_child("generation").count("options") == 1) {
                    BOOST_FOREACH(auto& var, pt_part.get_child("generation.options")) {
                        options.put_child(var.first, var.second);
                    }
                }

                auto pitch_algo  = getPitchAlgorithm(pt_part.get<std::string>("generation.pitch", ""));
                auto rhythm_algo = getRhythmAlgorithm(pt_part.get<std::string>("generation.rhythm", ""));

                // Set Instrument(s)
                bool percussion;

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

                measure.setBPM(m_config.conf<int>("style.bpm", 80));

                std::shared_ptr<music::Part> part = std::make_shared<music::Part>(instruments);
                part->setLines((uint8_t)pt_part.get<int>("lines", 5));

                part->setInstrumentName(pt_part.get<std::string>("name", ""));

                std::shared_ptr<music::Note> prev;
                for(unsigned int j = 0; j < length * measure.max_length();) {
                    std::vector<music::Note*> conc = {};
                    for(unsigned int p = 0; p < i; ++p) {
                        music::Note* n = score.getParts().at(p)->at(j);
                        if(n) {
                            conc.emplace_back(n);
                        }
                    }
                    uint8_t pitch    = pitch_algo(m_rnengine, prev.get(), conc, options);
                    auto    rh       = rhythm_algo(m_rnengine, prev.get(), conc, options);
                    auto    duration = (unsigned)(divisions * 4 * rh);

                    // Prevent overflowing over final measure
                    if(j + duration > length * measure.max_length()) {
                        duration = length * measure.max_length() - j;
                    }
                    music::Note note{pitch, duration};

                    auto prepr = music::Note::pitchRepr(pitch);
                    if(pt_part.count("instrument") == 0 || percussion) {
                        note.setHead(repr_to_head.at(prepr));
                        note.setInstrument(repr_to_inst.at(prepr));
                    }

                    prev = std::make_shared<music::Note>(note);
                    measure.append(note);
                    j += duration;
                }

                part->setMeasures(measure);
                auto picked = Randomizer::pick_uniform<float>(m_rnengine, 0.0f, 1.0f);

                // Change the last Note to the root note with a chance of style.chance
                if(!percussion && picked <= m_config.conf<float>("style.chance")) {
                    music::Note::Semitone s;

                    auto c = part->back()->back().getPitch();
                    auto r = music::Note::pitchRepr(c);
                    auto p = music::Note::splitPitch(r, s);

                    // Find nearest root note.
                    //  To do this, there are 3 possibilities: same octave, or one octave up or down.
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

                // Generate random rests (for this part)
                auto rests = m_config.conf<float>("generation.rest-ratio", 0.0f);
                if(pt_part.count("generation.rest-ratio") != 0) {
                    rests = pt_part.get<float>("generation.rest-ratio", 0.0f);
                }
                if(rests < 0.0f) {
                    logger->warn("The rest-ratio is less than 0. Changing it to 0.");
                    rests = std::fabs(rests);
                }
                unsigned int ntcnt = 0;
                for(const auto& _m : part->getMeasures()) {
                    ntcnt += _m->getNotes().size();
                }
                auto amount = (unsigned int)std::round(ntcnt * rests);
                for(unsigned int w = 0; w < amount; ++w) {
                    auto idx = (unsigned)Randomizer::pick_uniform(m_rnengine, 0, ntcnt - 1);
                    part->toPause(idx);
                }
                score.addPart(part);
            }

            return score;
        }

        std::function<uint8_t(RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc, pt::ptree& pt)>
        Generator::getPitchAlgorithm(std::string algo) const {
            // Get algorithm variables
            if(algo.empty()) {
                algo = m_config.conf<std::string>("generation.pitch");
            }

            m_config.getLogger()->debug("Using Pitch Algorithm '{}'", algo);

            if(algo == "random-piano") {
                return [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    auto stave = pt.get<int>("stave");
                    return (uint8_t)Randomizer::pick_uniform(gen, getPitches(21, 108, stave));
                };
            } else if(algo == "contain-stave") {
                return [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    auto stave = pt.get<int>("stave");
                    auto range = staveRange(stave);

                    return (uint8_t)Randomizer::pick_uniform(gen, getPitches(range.first, range.second, stave));
                };
            } else if(algo == "brownian-motion") {
                return [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc,
                              pt::ptree& pt) -> uint8_t { return pitchBrownianMotion(gen, prev, conc, pt); };
            } else if(algo == "1/f-noise") {
                return [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    auto res = pitch1FNoise(gen, pt);
                    pt.put("_p1fn._reinit", false);
                    return res;
                };
            } else if(algo == "centralized") {
                return [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    auto stave = pt.get<int>("stave");
                    auto range = staveRange(stave);
                    auto p     = getPitches(range.first, range.second, stave);

                    std::vector<float> p2;
                    for(const auto& v : p) {
                        p2.push_back((int)v);
                    }
                    return (uint8_t)Randomizer::gaussian(gen, p2);
                };
            } /*else if(algo == "gaussian-voicing") {
                return [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    auto stave = pt.get<int>("stave");
                    auto range = staveRange(stave);
                    auto p     = getPitches(range.first, range.second, stave);

                    std::vector<float> p2;
                    for(const auto& v : p) {
                        p2.push_back((int)v);
                    }
                    return (uint8_t)Randomizer::gaussian(gen, p2, -3.0f, 3.0f, true);
                };
            }*/ else {
                return [this](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    auto stave = pt.get<int>("stave");
                    return (uint8_t)Randomizer::pick_uniform(gen, getPitches(0, 128, stave));
                };
            }
        }

        std::function<float(RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc, pt::ptree& pt)>
        Generator::getRhythmAlgorithm(std::string algo) const {
            // Get algorithm variables
            if(algo.empty()) {
                algo = m_config.conf<std::string>("generation.rhythm");
            }

            m_config.getLogger()->debug("Using Rhythm Algorithm '{}'", algo);

            if(algo == "random") {
                return [](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc, pt::ptree& pt) -> float {
                    auto rh = (float)std::pow(2.0f, Randomizer::pick_uniform(gen, 1, 10) - 8);
                    return rh;
                };
            } else {
                return [](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc, pt::ptree& pt) -> float {
                    auto rh = pt.get<std::string>("rhythm.duration", "quarter");
                    try {
                        return music::Note::DURATION.at(rh);
                    } catch(std::exception& e) { return 0.25f; }
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

        std::pair<uint8_t, uint8_t> Generator::staveRange(int stave) const {
            auto        clef_n = ptree_at(m_config.conf_child("parts"), (uint8_t)stave).get_child("clef");
            music::Clef clef   = music::Clef::Treble();
            clef.setSign(clef_n.get<unsigned char>("sign", 'G'));
            clef.setLine((uint8_t)clef_n.get<int>("line", 2));
            clef.setOctaveChange(clef_n.get<int>("octave-change", 0));
            return clef.range();
        }

        uint8_t Generator::pitchBrownianMotion(autoplay::util::RNEngine& gen, autoplay::music::Note* prev,
                                               std::vector<autoplay::music::Note*>& conc, const pt::ptree& pt) const {
            auto stave = pt.get<int>("stave", 0);
            auto range = staveRange(stave);

            auto pitches = getPitches(range.first, range.second, stave);

            if(prev) {
                auto it = std::find(pitches.begin(), pitches.end(), prev->getPitch());
                if(it == pitches.end()) {
                    throw std::invalid_argument("Note '" + std::to_string(prev->getPitch()) + "' not in scale!");
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
                idx = Randomizer::pick_uniform(gen, (int)(min + idx), (int)(max + idx));
                return pitches.at((unsigned)idx);
            } else {
                return (uint8_t)Randomizer::pick_uniform(gen, pitches);
            }
        }

        uint8_t Generator::pitch1FNoise(autoplay::util::RNEngine& gen, const pt::ptree& pt) const {
            // Constants through function
            uint8_t num_dice   = 3;
            auto    num_states = (uint8_t)std::pow((int)2, (int)num_dice);

            auto stave = pt.get<int>("stave", 0);
            auto range = staveRange(stave);

            auto pitches = getPitches(range.first, range.second, stave);

            // (Re)init table
            static uint8_t state = 0;
            static std::vector<std::pair<uint8_t, uint8_t>> dice{num_dice, {0, 0}};

            if(pt.get<bool>("_p1fn._reinit", false)) {
                state = 0;
                dice.assign(num_dice, {0, 0});

                // Fill dice_ranges uniformly
                auto all = (uint8_t)((int)(pitches.size() - 1) / num_dice);
                for(auto& die : dice) {
                    die.second = all;
                }
                all = (uint8_t)((pitches.size() - 1) - (all * num_dice));
                for(unsigned int i = 0; i < all; ++i) {
                    dice.at(i % num_dice).second += 1;
                }
            }

            // Roll dice (depending on the state)
            unsigned int sum = 0;
            for(unsigned int i = 0; i < dice.size(); ++i) {
                if(state == 0) {
                    dice.at(i).first = (uint8_t)Randomizer::pick_uniform(gen, 0, dice.at(i).second + 1);
                    sum += dice.at(i).first;
                } else {
                    std::bitset<16> bs{(unsigned long long)((state - 1) ^ state)};
                    if(bs.test(i)) {
                        dice.at(i).first = (uint8_t)Randomizer::pick_uniform(gen, 0, dice.at(i).second + 1);
                        sum += dice.at(i).first;
                    }
                }
            }

            state = (uint8_t)((state + 1) % num_states);

            return pitches.at(sum);
        }
    }
}