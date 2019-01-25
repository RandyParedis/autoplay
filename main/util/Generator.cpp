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
 *  Created on 28/10/2018
 */

#include "Generator.h"
#include "../markov/MarkovChain.h"
#include "Randomizer.h"

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>
#include <bitset>
#include <sstream>

namespace autoplay {
    namespace util {
        Generator::Generator(const Config& config, const zz::log::LoggerPtr& logger)
            : m_config(config), m_logger(logger) {
            // Set engine
            auto engine_name = m_config.conf<std::string>("engine");
            auto seed        = m_config.conf<unsigned long>("seed", 0);
            m_rnengine(engine_name, seed);
        }

        music::Score Generator::generate() {
            // Setup default values
            auto          length     = (unsigned)m_config.conf<int>("length", 10); // Total amount of measures
            auto          parts      = m_config.conf_child("parts");
            unsigned long part_count = parts.size(); // Number of parts
            int           divisions  = 64;           // Amount of 'ticks' each quarter note takes

            std::pair<uint8_t, uint8_t> time = {(uint8_t)m_config.conf<int>("style.time.beats", 4),
                                                (uint8_t)m_config.conf<int>("style.time.type", 4)};

            // Get Logger
            auto logger = m_config.getLogger();

            std::vector<std::string> chord_progression;
            auto chord_progression_string = m_config.conf<std::string>("style.chord-progression", "");
            if(!chord_progression_string.empty()) {
                chord_progression = markov::split_on(chord_progression_string, '-');
            }

            music::Score score{m_config.conf_child("export")};
            for(unsigned int i = 0; i < part_count; ++i) {
                auto pt_part = ptree_at(parts, i);
                if(pt_part.count("generation") == 0) {
                    pt_part.put_child("generation", m_config.conf_child("generation"));
                } else {
                    merge(pt_part.get_child("generation"), m_config.conf_child("generation"));
                }

                pt::ptree options;
                options.put("stave", i);
                options.put("_reinit", true);
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
                auto chord_algo  = getChordNoteCountAlgorithm(pt_part.get<std::string>("generation.chord", ""));

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
                    clef = music::Clef{(unsigned char)pt_part.get<char>("clef.sign", 'G'),
                                       (uint8_t)pt_part.get<int>("clef.line", 2),
                                       pt_part.get<int>("clef.octave-change", 0)};
                }
                music::Measure measure{clef, time, divisions, m_config.conf<int>("style.fifths")};

                measure.setBPM(m_config.conf<int>("style.bpm", 80));

                options.put("rhythm._divs", divisions);

                std::shared_ptr<music::Part> part = std::make_shared<music::Part>(instruments);
                part->setLines((uint8_t)pt_part.get<int>("lines", 5));

                part->setInstrumentName(pt_part.get<std::string>("name", ""));

                std::shared_ptr<music::Chord> prev;
                auto                          mlen = measure.max_length();
                for(unsigned int j = 0; j < length * mlen;) {
                    std::vector<music::Chord*> conc = {};
                    for(unsigned int p = 0; p < i; ++p) {
                        music::Chord* n = score.getParts().at(p)->at(j);
                        if(n) {
                            conc.emplace_back(n);
                        }
                    }

                    // Set Rhythm
                    auto rh       = rhythm_algo(m_rnengine, prev.get(), conc, options);
                    auto duration = (unsigned)(divisions * 4 * rh);

                    // Prevent overflowing over final measure
                    if(j + duration > length * mlen) {
                        duration = length * mlen - j;
                    }

                    // Ability to do chords
                    int num_notes = chord_algo(m_rnengine, prev.get(), conc, options);

                    music::Chord chord;
                    if(!chord_progression.empty()) {
                        options.put("_currchord", chord_progression.at((j / mlen) % chord_progression.size()));
                    }
                    options.put("j", j);
                    options.put("mlen", mlen);
                    for(int nn = 0; nn < num_notes; ++nn) {
                        uint8_t pitch = pitch_algo(m_rnengine, prev.get(), conc, options);

                        // Remap the percussion depending on the chord progression
                        if(!percussion && !chord_progression.empty() && j % mlen == 0) {
                            std::string value = chord_progression.at((j / mlen) % chord_progression.size());
                            if(value.at(value.length() - 1) == 'm') {
                                value = value.substr(0, value.length() - 1);
                            }
                            pitch = remapPitch(pitch, value, clef.range());
                        }

                        if(chord.in(pitch)) {
                            // --nn; ///< Infinite loop possible!
                            continue;
                        }

                        music::Note note{pitch, duration};

                        if(options.get<bool>("pitch._rest", false)) {
                            note.toPause();
                            options.put("pitch._rest", false);
                        }

                        auto prepr = music::Note::pitchRepr(pitch);
                        if(pt_part.count("instrument") == 0 || percussion) {
                            note.setHead(repr_to_head.at(prepr));
                            note.setInstrument(repr_to_inst.at(prepr));
                        }

                        chord.append(note);
                    }

                    prev = std::make_shared<music::Chord>(chord);

                    measure.append(chord);
                    j += duration;

                    options.put("_reinit", false);
                }

                part->setMeasures(measure);

                // Change the last Note to the root note with a chance of style.chance
                if(!percussion) {
                    auto c = part->back()->back().bottom()->getPitch();
                    if(!part->back()->back().bottom()->getTieEnd()) {
                        part->back()->back().bottom()->setPitch(
                            remapPitch(c, m_config.conf<std::string>("style.root"), clef.range()));
                    }
                }

                // Generate random rests (for this part)
                auto rests = pt_part.get<float>("generation.rest-ratio", 0.0f);
                // if(pt_part.count("generation.rest-ratio") != 0) {
                //     rests = pt_part.get<float>("generation.rest-ratio", 0.0f);
                // }
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
                    if(part->noteAt(idx)->getTieStart() || part->noteAt(idx)->getTieEnd()) {
                        continue;
                    }
                    part->toPause(idx);
                }
                score.addPart(part);
            }

            return score;
        }

        std::function<uint8_t(RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc, pt::ptree& pt)>
        Generator::getPitchAlgorithm(std::string algo) const {
            // Get algorithm variables
            if(algo.empty()) {
                algo = m_config.conf<std::string>("generation.pitch", "random");
            }

            m_config.getLogger()->debug("Using Pitch Algorithm '{}'", algo);

            if(algo == "random-piano") {
                return [this](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    auto stave = pt.get<int>("stave");
                    return (uint8_t)Randomizer::pick_uniform(gen, getPitches(21, 108, stave));
                };
            } else if(algo == "contain-stave") {
                return [this](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    auto stave = pt.get<int>("stave");
                    auto range = staveRange(stave);

                    return (uint8_t)Randomizer::pick_uniform(gen, getPitches(range.first, range.second, stave));
                };
            } else if(algo == "brownian-motion") {
                return [this](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc,
                              pt::ptree& pt) -> uint8_t { return pitchBrownianMotion(gen, prev, conc, pt); };
            } else if(algo == "1/f-noise") {
                return [this](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    auto res = pitch1FNoise(gen, pt);
                    return res;
                };
            } else if(algo == "centralized") {
                return [this](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc,
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
            } else if(algo == "accompaniment") {
                return [this](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    auto schematic = pt.get<std::string>("pitch.schematic", "");
                    auto stave     = pt.get<int>("stave");
                    auto range     = staveRange(stave);
                    if(schematic.empty()) {
                        auto chord = m_config.conf<std::string>("style.root", "C");
                        if(!conc.empty()) {
                            // TODO: Use *all* simultaneous chords instead of a random one.
                            chord =
                                conc.at((unsigned long)Randomizer::pick_uniform(gen, 0, (int)conc.size()))->getName();
                        }
                        return pitchAccompanimentSchematic(
                            std::string("ABC").substr((unsigned long)Randomizer::pick_uniform(gen, 0, 3), 1), chord, 0,
                            1, range, stave);
                    } else {
                        auto j     = pt.get<unsigned int>("j");
                        auto mlen  = pt.get<unsigned int>("mlen");
                        auto chord = pt.get<std::string>("_currchord", m_config.conf<std::string>("style.root", "C"));

                        return pitchAccompanimentSchematic(schematic, chord, j, mlen, range, stave);
                    }
                };
            } else if(algo == "markov-chain") {
                return [this](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    static std::shared_ptr<markov::MarkovChain> mc;
                    if(!mc) {
                        mc = std::make_shared<markov::MarkovChain>(pt.get<std::string>("pitch.chain"), gen);

                        // Remove all states that cannot be reached in our algorithm
                        auto stave = pt.get<int>("stave");
                        auto range = staveRange(stave);
                        auto p     = getPitches(range.first, range.second, stave);

                        std::vector<std::string> non_erasables;
                        for(const auto& v : p) {
                            non_erasables.emplace_back(music::Note::pitchRepr(v));
                        }

                        mc->keep(non_erasables);
                    }
                    if(pt.get<bool>("_reinit", false)) {
                        mc->reset();
                    }
                    markov::MarkovChain::State next = mc->next();
                    if(next == "rest") {
                        pt.put("pitch._rest", true);
                        next = "C-1";
                    }
                    return music::Note::pitch(next);
                };
            } else if(algo == "gaussian-voicing") {
                return [this](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    auto stave = pt.get<int>("stave");
                    auto range = staveRange(stave);
                    auto p     = getPitches(range.first, range.second, stave);

                    std::vector<float> p2;
                    for(const auto& v : p) {
                        p2.push_back((float)((int)v));
                    }
                    return (uint8_t)Randomizer::gaussian(gen, p2, -3.0f, 3.0f, true);
                };
            } else {
                return [this](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc,
                              pt::ptree& pt) -> uint8_t {
                    auto stave = pt.get<int>("stave");
                    return (uint8_t)Randomizer::pick_uniform(gen, getPitches(0, 128, stave));
                };
            }
        }

        std::function<float(RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc, pt::ptree& pt)>
        Generator::getRhythmAlgorithm(std::string algo) const {
            // Get algorithm variables
            if(algo.empty()) {
                algo = m_config.conf<std::string>("generation.rhythm", "constant");
            }

            m_config.getLogger()->debug("Using Rhythm Algorithm '{}'", algo);

            if(algo == "random") {
                return [](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc, pt::ptree& pt) -> float {
                    auto smallest =
                        (int)std::log2(music::Note::DURATION.at(pt.get<std::string>("rhythm.smallest", "256th")));
                    auto largest =
                        (int)std::log2(music::Note::DURATION.at(pt.get<std::string>("rhythm.largest", "long")));
                    smallest += 8;
                    largest += 8;
                    auto rh = (float)std::pow(2.0f, Randomizer::pick_uniform(gen, smallest, largest) - 8);
                    return rh;
                };
            } else if(algo == "brownian-motion") {
                return [this](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc,
                              pt::ptree& pt) -> float { return rhythmBrownianMotion(gen, prev, conc, pt); };
            } else if(algo == "1/f-noise") {
                return [this](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc,
                              pt::ptree& pt) -> float { return rhythm1FNoise(gen, pt); };
            } else if(algo == "markov-chain") {
                return [](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc, pt::ptree& pt) -> float {
                    static std::shared_ptr<markov::MarkovChain> mc;
                    if(!mc) {
                        mc = std::make_shared<markov::MarkovChain>(pt.get<std::string>("rhythm.chain"), gen);
                    }
                    if(pt.get<bool>("_reinit", false)) {
                        mc->reset();
                    }
                    markov::MarkovChain::State next = mc->next();
                    return std::stof(next) / (4 * 64);
                };
            } else {
                return [](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc, pt::ptree& pt) -> float {
                    auto rh = pt.get<std::string>("rhythm.duration", "quarter");
                    try {
                        return music::Note::DURATION.at(rh);
                    } catch(std::exception& e) { return 0.25f; }
                };
            }
        }

        bool is_number(const std::string& s) {
            return !s.empty() && std::find_if(s.begin(), s.end(), [](char c) { return !std::isdigit(c); }) == s.end();
        }

        std::function<int(RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc, pt::ptree& pt)>
        Generator::getChordNoteCountAlgorithm(std::string algo) const {
            // Get algorithm variables
            if(algo.empty()) {
                algo = m_config.conf<std::string>("generation.chord", "constant");
            }

            m_config.getLogger()->debug("Using Chord Note Count Algorithm '{}'", algo);

            if(algo == "random") {
                return [](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc, pt::ptree& pt) -> int {
                    auto min = pt.get<int>("chord.min", 1);
                    auto max = pt.get<int>("chord.max", 1);
                    return Randomizer::pick_uniform(gen, min, max + 1);
                };
            } else if(algo == "weighted") {
                return [this](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc,
                              pt::ptree& pt) -> int {
                    static std::map<int, float> mapping;
                    if(pt.get<bool>("_reinit", false)) {
                        mapping.clear();
                    }
                    if(mapping.empty()) {
                        float sum = 0.0f;
                        BOOST_FOREACH(const auto& var, pt.get_child("chord")) {
                            if(is_number(var.first)) {
                                auto s = var.second.get<float>("");
                                sum += s;
                                mapping.insert({std::stoi(var.first), s});
                            }
                        }

                        int min = mapping.begin()->first;

                        // Fix chances if required
                        if(sum > 1.0f) { // Normalize
                            m_logger->warn("Sum of all weighted elements exceeds 1! Normalizing the values...");
                            for(auto& kv : mapping) {
                                kv.second /= sum;
                            }
                            m_logger->warn("New values:");
                            for(const auto& kv : mapping) {
                                m_logger->warn("\t{} --> {}", kv.first, kv.second);
                            }
                        } else if(sum < 1.0f) {
                            float one = 1.0f - sum;
                            if(mapping.count(1) == 1) {
                                one += mapping.at(1);
                                m_logger->warn(
                                    "Invalid sum of {}. Changing chance that a single note occurs from {} to {}.", sum,
                                    mapping.at(1), one);
                            }
                            mapping[1] = one;
                            if(min != 1) {
                                for(int j = 2; j < min; ++j) {
                                    mapping[j] = 0.0f;
                                }
                            }
                        }
                    }

                    std::map<int, float> map_ref{mapping};

                    std::function<float(const int&)> func = [&map_ref](const int& val) -> float {
                        if(map_ref.count(val) == 1) {
                            return map_ref.at(val);
                        }
                        return 0.0f;
                    };
                    return Randomizer::pick_weighted(gen, mapping.begin()->first, mapping.rbegin()->first + 1, 1, func);
                };
            } else if(algo == "markov-chain") {
                return [](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc, pt::ptree& pt) -> int {
                    static std::shared_ptr<markov::MarkovChain> mc;
                    if(!mc) {
                        mc = std::make_shared<markov::MarkovChain>(pt.get<std::string>("chord.chain"), gen);
                    }
                    if(pt.get<bool>("_reinit", false)) {
                        mc->reset();
                    }
                    markov::MarkovChain::State next = mc->next();
                    return std::stoi(next);
                };
            } else {
                return [](RNEngine& gen, music::Chord* prev, std::vector<music::Chord*>& conc, pt::ptree& pt) -> int {
                    return pt.get<int>("chord.amount", 1);
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
            auto        clef_n = ptree_at(m_config.conf_child("parts"), (size_t)stave).get_child("clef");
            music::Clef clef   = music::Clef::Treble();
            clef.setSign((unsigned char)clef_n.get<char>("sign", 'G'));
            clef.setLine((uint8_t)clef_n.get<int>("line", 2));
            clef.setOctaveChange(clef_n.get<int>("octave-change", 0));
            return clef.range();
        }

        uint8_t Generator::remapPitch(uint8_t pitch, const std::string& to, const std::pair<uint8_t, uint8_t>& range) {
            auto picked = Randomizer::pick_uniform<float>(m_rnengine, 0.0f, 1.0f);

            if(picked <= m_config.conf<float>("style.chance")) {
                music::Note::Semitone s;

                auto r = music::Note::pitchRepr(pitch);
                auto p = music::Note::splitPitch(r, s);

                // Find nearest "to" pitch.
                //  To do this, there are 3 possibilities: same octave, or one octave up or down.
                //  But, we have to be aware of the range constraint that has been set on the remapping!
                std::vector<uint8_t> pitches;
                for(const auto& i : {-1, 0, 1}) {
                    auto o = music::Note::pitch(to + std::to_string(p.second + i));
                    if(o >= range.first && o <= range.second) {
                        pitches.emplace_back(o);
                    }
                }
                if(!pitches.empty()) {
                    uint8_t smallest = pitches.at(0);
                    for(const auto& o : pitches) {
                        if(std::abs(pitch - o) < std::abs(pitch - smallest)) {
                            smallest = o;
                        }
                    }
                    pitch = smallest;
                }
            }

            return pitch;
        }

        uint8_t Generator::pitchBrownianMotion(autoplay::util::RNEngine& gen, autoplay::music::Chord* prev,
                                               std::vector<autoplay::music::Chord*>& conc, const pt::ptree& pt) const {
            auto stave = pt.get<int>("stave", 0);
            auto range = staveRange(stave);

            auto pitches = getPitches(range.first, range.second, stave);

            if(prev) {
                uint8_t pitch = prev->getNotes()
                                    .at((unsigned long)Randomizer::pick_uniform(gen, 0, (int)prev->getNotes().size()))
                                    ->getPitch();
                auto it = std::find(pitches.begin(), pitches.end(), pitch);

                /// Don't test if the pitch is out of range!
                /// It shouldn't be, but the chord does not take the pitches into a count.
                // TODO: chord algorithm should take pitches into a count.
                //  -> Should be solved by default!

                // if(it == pitches.end()) {
                //     throw std::invalid_argument("Note '" + std::to_string(pitch) + "' not in scale!");
                // }

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

            if(pt.get<bool>("_reinit", false)) {
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

        uint8_t Generator::pitchAccompanimentSchematic(const std::string& schematic, std::string chordname,
                                                       unsigned int timestep, unsigned int measure_length,
                                                       const std::pair<uint8_t, uint8_t>& range, int stave) const {
            bool minor = false;
            if(chordname.at(chordname.length() - 1) == 'm') {
                minor     = true;
                chordname = chordname.substr(0, chordname.length() - 1);
            }

            // Check the length of the schematic
            auto  sl = (unsigned int)schematic.length();
            float sm = std::log2((float)sl);
            if(std::floor(sm) - sm < 0.0f) {
                throw std::invalid_argument(
                    "The schematic for the accompaniment has an invalid length. A power of 2 was expected, " +
                    std::to_string(sl) + " was obtained.");
            }

            // Find the separation/group point of each new "note"
            unsigned int group = measure_length / sl;
            timestep %= measure_length;
            auto idx = (unsigned int)std::floor((float)timestep / group);

            auto pitches = getPitches(range.first, range.second, stave);

            uint8_t min   = pitches.at(0);
            bool    found = false;
            for(const auto& p : pitches) {
                music::Note::Semitone s;

                std::string repr;
                repr += music::Note::splitPitch(music::Note::pitchRepr(p), s).first;
                if(s == music::Note::Semitone::SHARP) {
                    repr += '#';
                } else if(s == music::Note::Semitone::FLAT) {
                    repr += 'b';
                }

                // Cast to a pitch in the first octave to check comparison between flats and sharps
                if(music::Note::pitch(repr + std::to_string(1)) == music::Note::pitch(chordname + std::to_string(1))) {
                    min   = p;
                    found = true;
                    break;
                }
            }
            if(!found) {
                throw std::invalid_argument("Impossible to find the chord " + chordname + " in the range.");
            }

            char    letter = schematic.at(idx);
            uint8_t nxt    = min;
            switch(letter) {
            case 'A': return min;

            case 'B':
                if(minor) {
                    nxt += (uint8_t)3;
                } else {
                    nxt += (uint8_t)4;
                }
                if(std::find(pitches.begin(), pitches.end(), nxt) == pitches.end()) {
                    std::string err =
                        "The letter 'B' in the schematic falls outside of the possibilities. Returning as 'A' in '";
                    err += chordname;
                    if(minor) {
                        err += "m";
                    }
                    err += "'.";
                    m_logger->warn() << err;
                    return min;
                }
                return nxt;

            case 'C':
                nxt += (uint8_t)7;
                if(std::find(pitches.begin(), pitches.end(), nxt) == pitches.end()) {
                    std::string err =
                        "The letter 'C' in the schematic falls outside of the possibilities. Returning as 'A' in '";
                    err += chordname;
                    if(minor) {
                        err += "m";
                    }
                    err += "'.";
                    m_logger->warn() << err;
                    return min;
                }
                return nxt;

            default:
                throw std::invalid_argument("The schematic contains an invalid letter '" + std::to_string(letter) +
                                            "'. Only A, B and C are allowed!");
            }
        }

        float Generator::rhythmBrownianMotion(autoplay::util::RNEngine& gen, autoplay::music::Chord* prev,
                                              std::vector<autoplay::music::Chord*>& conc, const pt::ptree& pt) const {
            auto divisions = pt.get<unsigned int>("rhythm._divs");
            auto smallest  = (int)std::log2(music::Note::DURATION.at(pt.get<std::string>("rhythm.smallest", "256th")));
            auto largest   = (int)std::log2(music::Note::DURATION.at(pt.get<std::string>("rhythm.largest", "long")));
            smallest += 8;
            largest += 8;
            if(prev) {
                float prev_type = (float)prev->getDuration() / (4.0f * divisions);
                auto  prev_n    = (int)(std::log2(prev_type) + 8);
                auto  min       = pt.get<int>("rhythm.min", -3);
                auto  max       = pt.get<int>("rhythm.max", 3);
                if(prev_n + min < smallest) {
                    min = prev_n - smallest;
                }
                if(prev_n + max >= largest) {
                    max = largest - prev_n;
                } else if(prev_n + max < smallest) {
                    max = prev_n - smallest;
                }
                return (float)std::pow(2.0f, Randomizer::pick_uniform(gen, prev_n + min, prev_n + max) - 8);
            } else {
                return (float)std::pow(2.0f, Randomizer::pick_uniform(gen, smallest, largest) - 8);
            }
        }

        float Generator::rhythm1FNoise(autoplay::util::RNEngine& gen, const pt::ptree& pt) const {
            // Constants through function
            uint8_t num_dice   = 3;
            auto    num_states = (uint8_t)std::pow((int)2, (int)num_dice);

            auto smallest = (int)std::log2(music::Note::DURATION.at(pt.get<std::string>("rhythm.smallest", "256th")));
            auto largest  = (int)std::log2(music::Note::DURATION.at(pt.get<std::string>("rhythm.largest", "long")));

            // (Re)init table
            static uint8_t state = 0;
            static std::vector<std::pair<int, int>> dice{num_dice, {0, 0}};

            if(pt.get<bool>("_reinit", false)) {
                state = 0;
                dice.assign(num_dice, {0, 0});

                // Fill dice_ranges uniformly
                int all = (largest - smallest) / num_dice;
                for(auto& die : dice) {
                    die.second = all;
                }
                all = (largest - smallest) - (all * num_dice);
                for(unsigned int i = 0; i < (unsigned)all; ++i) {
                    dice.at(i % num_dice).second += 1;
                }
            }

            // Roll dice (depending on the state)
            int sum = 0;
            for(unsigned int i = 0; i < dice.size(); ++i) {
                if(state == 0) {
                    dice.at(i).first = Randomizer::pick_uniform(gen, 0, dice.at(i).second + 1);
                    sum += dice.at(i).first;
                } else {
                    std::bitset<16> bs{(unsigned long long)((state - 1) ^ state)};
                    if(bs.test(i)) {
                        dice.at(i).first = Randomizer::pick_uniform(gen, 0, dice.at(i).second + 1);
                        sum += dice.at(i).first;
                    }
                }
            }

            state = (uint8_t)((state + 1) % num_states);

            auto r = (float)std::pow(2.0f, smallest + sum);
            return r;
        }
    }
}