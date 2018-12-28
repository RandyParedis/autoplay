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
 *  Created on 02/12/18
 */

#include "MarkovChain.h"
#include "../util/FileHandler.h"
#include "../util/Randomizer.h"
#include "SpecialQueue.h"

#include <boost/property_tree/exceptions.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <queue>

namespace autoplay {
    namespace markov {

        MarkovChain::MarkovChain(const std::string& filename, const util::RNEngine& engine,
                                 const MarkovChain::State& begin)
            : m_engine(engine), m_current(begin), m_begin(begin) {
            m_matrix = NamedMatrix::fromCSV(filename);
        }

        MarkovChain::MarkovChain(const markov::NamedMatrix& namedMatrix, const util::RNEngine& engine,
                                 const MarkovChain::State& begin)
            : m_matrix(namedMatrix), m_engine(engine), m_current(begin), m_begin(begin) {}

        MarkovChain::State MarkovChain::next() {
            auto poss = fetchPossibilities();

            std::vector<MarkovChain::State> states;
            for(const auto& p : poss) {
                states.emplace_back(p.first);
            }

            std::function<float(const State&)> func = [&poss](const State& state) -> float { return poss.at(state); };

            State s = util::Randomizer::pick_weighted(m_engine, states, func);

            m_current = s;
            return s;
        }

        std::map<MarkovChain::State, float> MarkovChain::fetchPossibilities() const {
            auto vec = m_matrix.get(m_current);
            std::map<MarkovChain::State, float> poss;
            for(const auto& pr : vec) {
                poss.insert(pr);
            }
            return poss;
        }

        std::vector<NamedMatrix> MarkovChain::generateMatrices(const path& directory, bool recursive) {
            if(!is_directory(directory)) {
                throw std::runtime_error("The given path is not a directory.");
            }
            NamedMatrix matPitch;
            NamedMatrix matRhythm;
            NamedMatrix matChord;

            std::queue<path> q;
            q.push(directory);
            while(!q.empty()) {
                path p = q.front();
                if(!is_directory(p)) {
                    std::cout << p.string() << " is not a directory!" << std::endl;
                    q.pop();
                    continue;
                }
                std::cout << "QUEUE: " << q.size() << ", PATH: " << p.string() << std::endl;
                for(auto& entry : boost::make_iterator_range(directory_iterator(p), {})) {
                    if(is_directory(entry.path())) {
                        if(recursive) {
                            std::cout << "APPENDING " << entry.path().string() << std::endl;
                            q.push(entry.path());
                        }
                    } else if(entry.path().extension().string() == ".xml") {
                        generateMatrices(entry.path().string(), matPitch, matRhythm, matChord);
                    }
                }
                q.pop();
            }
            return {matPitch, matRhythm, matChord};
        }

        void MarkovChain::generateMatrices(const std::string& filename, NamedMatrix& matPitch, NamedMatrix& matRhythm,
                                           NamedMatrix& matChord) {
            std::cout << "PATH: " << filename << std::endl;

            pt::ptree score;
            try {
                pt::read_xml(filename, score);
            } catch(const boost::property_tree::xml_parser_error& ex) {
                std::cerr << "error in " << ex.filename() << ":" << ex.line() << "\n\t=> " << ex.what() << std::endl;
                return;
            }
            if(score.count("score-partwise") != 1 ||
               score.get<std::string>("score-partwise.<xmlattr>.version", "1.0") != "3.0") {
                return;
            }

            std::string row = "begin";
            if(!matPitch.isRow(row)) {
                matPitch.addRow(row);
            }
            int new_divisions = 64;
            if(!matRhythm.isRow(row)) {
                matRhythm.addRow(row);
            }
            if(!matChord.isRow(row)) {
                matChord.addRow(row);
            }
            SpecialQueue<std::string> history_pitch;
            SpecialQueue<std::string> history_rhythm;
            SpecialQueue<std::string> history_chord;
            history_pitch.enqueue(row);
            history_rhythm.enqueue(row);
            history_chord.enqueue(row);
            for(const auto& o : score.get_child("score-partwise")) {
                if(o.first == "part") {
                    int divisions = 64;
                    for(const auto& p : o.second) {
                        if(p.first == "measure") {
                            divisions      = p.second.get<int>("attributes.divisions", divisions);
                            int chord_size = 1;
                            for(const auto& m : p.second) {
                                if(m.first == "note") {
                                    if(m.second.count("rest") == 0) {
                                        /// Pitch
                                        auto prepr = m.second.get<std::string>("pitch.step");
                                        if(m.second.get<int>("pitch.alter", 0) == -1) {
                                            prepr += "b";
                                        } else if(m.second.get<int>("pitch.alter", 0) == 1) {
                                            prepr += "#";
                                        }
                                        prepr += m.second.get<std::string>("pitch.octave");
                                        // auto pitch = (int)music::Note::pitch(prepr);
                                        if(!matPitch.isColumn(prepr)) {
                                            matPitch.addColumn(prepr);
                                        }
                                        if(!matPitch.isRow(prepr)) {
                                            matPitch.addRow(prepr);
                                        }

                                        for(const auto& note : history_pitch.front()) {
                                            matPitch.at(note, prepr) += 1;
                                        }

                                        /// Rhythm
                                        std::string new_length =
                                            std::to_string(new_divisions * m.second.get<int>("duration") / divisions);
                                        if(!matRhythm.isColumn(new_length)) {
                                            matRhythm.addColumn(new_length);
                                        }
                                        if(!matRhythm.isRow(new_length)) {
                                            matRhythm.addRow(new_length);
                                        }

                                        for(const auto& note : history_rhythm.front()) {
                                            matRhythm.at(note, new_length) += 1;
                                        }

                                        /// Control History
                                        if(m.second.count("chord") == 1) {
                                            chord_size += 1;
                                            history_pitch.enqueue(prepr, true);
                                        } else {
                                            history_pitch.enqueue(prepr);
                                            history_pitch.dequeue();

                                            history_rhythm.enqueue(new_length);
                                            history_rhythm.dequeue();

                                            /// Chord Count
                                            std::string chrd = std::to_string(chord_size);
                                            if(!matChord.isColumn(chrd)) {
                                                matChord.addColumn(chrd);
                                            }
                                            if(!matChord.isRow(chrd)) {
                                                matChord.addRow(chrd);
                                            }

                                            for(const auto& note : history_chord.front()) {
                                                matChord.at(note, chrd) += 1;
                                            }

                                            history_chord.enqueue(chrd);
                                            history_chord.dequeue();

                                            chord_size = 1;
                                        }
                                    } else { // rest
                                        /// Pitch
                                        std::string prepr = "rest";
                                        if(!matPitch.isColumn(prepr)) {
                                            matPitch.addColumn(prepr);
                                        }
                                        if(!matPitch.isRow(prepr)) {
                                            matPitch.addRow(prepr);
                                        }

                                        for(const auto& note : history_pitch.front()) {
                                            matPitch.at(note, prepr) += 1;
                                        }
                                        history_pitch.enqueue(prepr);
                                        history_pitch.dequeue();

                                        /// Rhythm
                                        std::string new_length =
                                            std::to_string(new_divisions * m.second.get<int>("duration") / divisions);
                                        if(!matRhythm.isColumn(new_length)) {
                                            matRhythm.addColumn(new_length);
                                        }
                                        if(!matRhythm.isRow(new_length)) {
                                            matRhythm.addRow(new_length);
                                        }

                                        for(const auto& note : history_rhythm.front()) {
                                            matRhythm.at(note, new_length) += 1;
                                        }
                                        history_rhythm.enqueue(new_length);
                                        history_rhythm.dequeue();
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}