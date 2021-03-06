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
 *  Created on 19/10/2018
 */

#include "FileHandler.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <boost/foreach.hpp>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <list>

namespace autoplay {
    namespace util {
        void FileHandler::setRoot(pt::ptree& pt) { m_root = pt; }

        void FileHandler::clearRoot() { m_root.clear(); }

        void FileHandler::readJSON(std::istream& stream) { pt::read_json(stream, m_root); }

        void FileHandler::readJSON(const std::string& filename) { pt::read_json(filename, m_root); }

        void FileHandler::readXML(std::istream& stream) { pt::read_xml(stream, m_root); }

        void FileHandler::readXML(const std::string& filename, const std::string& root) {
            pt::read_xml(filename, m_root);
            if(!root.empty()) {
                m_root = m_root.get_child(root);
            }
            // parse parts into list
            pt::ptree tmp;
            BOOST_FOREACH(auto update, m_root.get_child("parts")) {
                if(update.first == "part") {
                    tmp.push_back(std::make_pair("", update.second));
                    // m_root.add_child("parts.", update.second);
                }
            }
            m_root.erase("parts");
            m_root.add_child("parts", tmp);
        }

        void FileHandler::readConfig(const std::string& filename) {
            std::string ext = filename.substr(filename.find_last_of('.') + 1);
            std::transform(ext.begin(), ext.end(), ext.begin(), ::toupper);
            if(ext == "JSON") {
                readJSON(filename);
            } else if(ext == "XML") {
                readXML(filename, "config");
            } else {
                std::string error = "Unknown file extension '" + ext + "'. Please use 'JSON', or 'XML'.";
                throw std::invalid_argument(error);
            }
        }

        void FileHandler::writeMusicXML(std::string filename, const music::Score& score) {
            // Set the valid extension
            auto        lio = filename.find_last_of('.');
            std::string ext = filename.substr(lio + 1);
            if(lio == std::string::npos || ext != "xml") {
                ext = "xml";
                filename += "." + ext;
            }
            if(ext.empty()) {
                ext = "xml";
                filename += ext;
            }

            pt::ptree file_tree;

            boost::property_tree::xml_writer_settings<std::string> settings('\t', 1);

            /// Populize Ptree
            // Header data
            auto header = score.getHeaderDataAsMusicXML();
            file_tree.put_child("score-partwise", header);

            // Score
            file_tree.put("score-partwise.<xmlattr>.version", "3.0");
            file_tree.put_child("score-partwise.part-list", pt::ptree());
            unsigned int part_idx = 0;
            for(const auto& part : score.getParts()) {
                ++part_idx;

                // Add part to part-list
                pt::ptree plt;
                plt.put("<xmlattr>.id", "P" + std::to_string(part_idx));
                plt.put("part-name", part->getInstrumentName());

                // Add part to score-partwise
                pt::ptree part_tree;
                part_tree.put("<xmlattr>.id", "P" + std::to_string(part_idx));

                // Set Instrument data
                std::map<std::string, std::string> instr_ids;
                for(const auto& instrument : part->getInstruments()) {
                    pt::ptree scin;
                    scin.add("instrument-name", instrument->getName());
                    instr_ids.insert(std::make_pair(instrument->getName(),
                                                    "P" + std::to_string(part_idx) + "-I" +
                                                        std::to_string((int)instrument->getUnpitched())));
                    scin.add("<xmlattr>.id", instr_ids.at(instrument->getName()));
                    plt.add_child("score-instrument", scin);
                }
                for(const auto& instrument : part->getInstruments()) {
                    pt::ptree mdin;
                    mdin.add("midi-channel", instrument->getChannel());
                    mdin.add("midi-program", instrument->getProgram());
                    if(instrument->getUnpitched() != 0) {
                        mdin.add("midi-unpitched", instrument->getUnpitched() + 1);
                    }
                    mdin.add("<xmlattr>.id", instr_ids.at(instrument->getName()));

                    plt.add_child("midi-instrument", mdin);
                }

                // Create storage for Measure attributes
                std::shared_ptr<music::Measure> prev(nullptr);

                // Add measures
                unsigned int measure_idx = 0;
                auto         new_lines   = part->getLines();
                uint8_t      prev_lines  = 5;
                for(const auto& measure : part->getMeasures()) {
                    ++measure_idx;
                    pt::ptree measure_tree;
                    bool      set_clef = false;

                    if(prev == nullptr) {
                        prev     = std::make_shared<music::Measure>();
                        set_clef = true;
                    }

                    // Set attributes if required
                    if(measure->hasAttributes()) {
                        auto time = measure->getTime();
                        int  bpm  = measure->getBPM();
                        if(bpm != prev->getBPM()) {
                            prev->setBPM(bpm);
                            music::Note nt{(unsigned)(4 * measure->getDivisions() / time.second)};
                            measure_tree.put("direction.direction-type.metronome.beat-unit",
                                             nt.getType(measure->getDivisions()));
                            measure_tree.put("direction.direction-type.metronome.per-minute", measure->getBPM());
                            measure_tree.put("direction.direction-type.metronome.<xmlattr>.parentheses", "no");
                            measure_tree.put("direction.sound.<xmlattr>.tempo", measure->getBPM());
                            measure_tree.put("direction.<xmlattr>.placement", "above");
                        }

                        int divisions = measure->getDivisions();
                        if(divisions != prev->getDivisions()) {
                            prev->setDivisions(divisions);
                            measure_tree.put("attributes.divisions", divisions);
                        }

                        int fifths = measure->getFifths();
                        if(fifths != prev->getFifths()) {
                            prev->setFifths(fifths);
                            measure_tree.put("attributes.key.fifths", fifths);
                        }

                        if(time != prev->getTime()) {
                            prev->setTime(time);
                            measure_tree.put("attributes.time.beats", time.first);
                            measure_tree.put("attributes.time.beat-type", time.second);
                        }

                        auto clef = measure->getClef();
                        if(set_clef || clef.getLine() != prev->getClef().getLine() ||
                           clef.getSign() != prev->getClef().getSign() ||
                           clef.isPercussion() != prev->getClef().isPercussion()) {
                            prev->setClef(clef);

                            if(clef.isPercussion()) {
                                measure_tree.put("attributes.clef.sign", "percussion");
                                measure_tree.put("attributes.clef.line", 2);
                            } else {
                                std::string sign;
                                sign += (char)clef.getSign();
                                measure_tree.put("attributes.clef.sign", sign);
                                measure_tree.put("attributes.clef.line", (int)clef.getLine());
                            }
                        }

                        if(new_lines != prev_lines) {
                            prev_lines = new_lines;
                            measure_tree.put("attributes.staff-details.staff-lines", (int)new_lines);
                        }
                    }
                    measure_tree.put("<xmlattr>.number", measure_idx);

                    // Add Notes
                    for(unsigned int _c_ = 0; _c_ < measure->getNotes().size(); ++_c_) {
                        auto vec = measure->getNotes().at(_c_).splitByDivisions(prev->getDivisions(), true);
                        for(unsigned int _v_ = 0; _v_ < vec.size(); ++_v_) {
                            auto chord = vec.at(_v_);
                            bool c_    = false;
                            if(chord.isPause()) {
                                pt::ptree note_tree;
                                note_tree.put("rest", "");
                                note_tree.put("duration", chord.getDuration());
                                measure_tree.add_child("note", note_tree);
                                continue;
                            }

                            for(const auto& note : chord.getNotes()) {
                                pt::ptree note_tree;
                                pt::ptree notation_tree;

                                music::Note::Semitone s;

                                auto repr = music::Note::splitPitch(music::Note::pitchRepr(note->getPitch()), s);

                                if(!c_) {
                                    c_ = true;
                                } else {
                                    note_tree.put("chord", "");
                                }

                                if(measure->getClef().isPercussion()) {
                                    note_tree.put("unpitched.display-step", repr.first);
                                    note_tree.put("unpitched.display-octave", repr.second);
                                } else {
                                    note_tree.put("pitch.step", repr.first);
                                    if(s == music::Note::Semitone::SHARP) {
                                        note_tree.put("pitch.alter", 1);
                                    } else if(s == music::Note::Semitone::FLAT) {
                                        note_tree.put("pitch.alter", -1);
                                    }
                                    note_tree.put("pitch.octave", repr.second);
                                }
                                note_tree.put("duration", note->getDuration());

                                if(note->getInstrument() != nullptr) {
                                    note_tree.put("instrument.<xmlattr>.id",
                                                  instr_ids.at(note->getInstrument()->getName()));
                                }

                                if(note->getTieEnd()) {
                                    pt::ptree tmp;
                                    tmp.put("<xmlattr>.type", "stop");
                                    note_tree.add_child("tie", tmp);
                                    notation_tree.add_child("tied", tmp);
                                }

                                if(note->getTieStart()) {
                                    pt::ptree tmp;
                                    tmp.put("<xmlattr>.type", "start");
                                    note_tree.add_child("tie", tmp);
                                    notation_tree.add_child("tied", tmp);
                                }

                                note_tree.put("voice", 1);
                                note_tree.put("type", note->getType(prev->getDivisions()));

                                for(uint8_t i = 0; i < note->getDots(); ++i) {
                                    note_tree.add("dot", "");
                                }

                                if(!note->getHeadName().empty()) {
                                    note_tree.put("notehead", note->getHeadName());
                                    if(note->canBeFilled()) {
                                        note_tree.put("notehead.<xmlattr>.filled",
                                                      note->getHeadFilled() ? "yes" : "no");
                                    }
                                }

                                // Set beams
                                int curdur  = note->getDuration();
                                int prevdur = -1;
                                if(_v_ == 0) {
                                    if(_c_ != 0) {
                                        auto x = measure->getNotes()
                                                     .at(_c_ - 1)
                                                     .splitByDivisions(prev->getDivisions(), true)
                                                     .back();
                                        if(!x.isPause()) {
                                            prevdur = x.getDuration();
                                        }
                                    }
                                } else {
                                    if(!vec.at(_v_ - 1).isPause()) {
                                        prevdur = vec.at(_v_ - 1).getDuration();
                                    }
                                }

                                int nextdur = -1;
                                if(_v_ == vec.size() - 1) {
                                    if(_c_ != measure->getNotes().size() - 1) {
                                        auto x = measure->getNotes()
                                                     .at(_c_ + 1)
                                                     .splitByDivisions(prev->getDivisions(), true)
                                                     .front();
                                        if(!x.isPause()) {
                                            nextdur = x.getDuration();
                                        }
                                    }
                                } else {
                                    if(!vec.at(_v_ + 1).isPause()) {
                                        nextdur = vec.at(_v_ + 1).getDuration();
                                    }
                                }
                                for(unsigned int b = 1; b < 6; ++b) {
                                    int ref = measure->getDivisions() / (int)std::pow(2, b - 1);

                                    if(curdur < ref && !(prevdur == -1 && nextdur == -1)) {
                                        pt::ptree beam;
                                        if(prevdur == -1) {
                                            beam.add("beam", "begin");
                                            beam.add("beam.<xmlattr>.number", b);
                                        } else if(nextdur == -1) {
                                            beam.add("beam", "end");
                                            beam.add("beam.<xmlattr>.number", b);
                                        } else {
                                            if(prevdur < ref && nextdur < ref) {
                                                beam.add("beam", "continue");
                                                beam.add("beam.<xmlattr>.number", b);
                                            } else if(prevdur < ref) {
                                                beam.add("beam", "end");
                                                beam.add("beam.<xmlattr>.number", b);
                                            } else if(nextdur < ref) {
                                                beam.add("beam", "begin");
                                                beam.add("beam.<xmlattr>.number", b);
                                            }
                                        }
                                        if(!beam.empty()) {
                                            note_tree.add_child("beam", beam.get_child("beam"));
                                        }
                                    }
                                }

                                note_tree.put_child("notations", notation_tree);

                                measure_tree.add_child("note", note_tree);
                            }
                        }
                    }

                    if(measure_idx == part->getMeasures().size()) {
                        measure_tree.put("barline.bar-style", "light-heavy");
                        measure_tree.put("barline.<xmlattr>.location", "right");
                    }

                    part_tree.add_child("measure", measure_tree);
                }

                file_tree.add_child("score-partwise.part-list.score-part", plt);
                file_tree.add_child("score-partwise.part", part_tree);
            }

            // Open filestream & write Score as MusicXML
            std::ofstream file;
            file.open(filename);

            file << "<?xml version=\"1.0\" encoding=\"";
            file << settings.encoding;
            file << "\" standalone=\"no\"?>\n";
            file << "<!DOCTYPE score-partwise PUBLIC \"-//Recordare//DTD MusicXML 3.0 "
                    "Partwise//EN\" "
                    "\"http://www.musicxml.org/dtds/partwise.dtd\">\n";

            write_xml_element(file, std::basic_string<pt::ptree::key_type::value_type>(), file_tree, -1, settings);

            // Finalize
            file.close();
        }
    }
}