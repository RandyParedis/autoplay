//
// Created by red on 19/10/18.
//

#include "FileHandler.h"
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

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

            // Populize Ptree
            file_tree.put_child("score-partwise", pt::ptree());
            file_tree.put("score-partwise.<xmlattr>.version", "3.0");
            file_tree.put_child("score-partwise.part-list", pt::ptree());
            unsigned int part_idx = 0;
            for(const auto& part : score.getParts()) {
                ++part_idx;

                // Add part to part-list
                pt::ptree plt;
                plt.put("<xmlattr>.id", "P" + std::to_string(part_idx));
                plt.put("part-name", "Music");

                // Add part to score-partwise
                pt::ptree part_tree;
                part_tree.put("<xmlattr>.id", "P" + std::to_string(part_idx));

                // Set Instrument data
                auto instrument = part->getInstrument();
                plt.put("score-instrument.instrument-name", instrument->getName());
                plt.put("score-instrument.<xmlattr>.id",
                        "P" + std::to_string(part_idx) + "-X" + std::to_string(instrument->getUnpitched()));
                plt.put("midi-instrument.midi-channel", instrument->getChannel());
                plt.put("midi-instrument.midi-program", instrument->getProgram());
                if(instrument->getUnpitched() != 0) {
                    plt.put("midi-instrument.midi-unpitched", instrument->getUnpitched());
                }
                plt.put("midi-instrument.<xmlattr>.id",
                        "P" + std::to_string(part_idx) + "-X" + std::to_string(instrument->getUnpitched()));

                // Create storage for Measure & Note attributes
                std::shared_ptr<music::Measure>         prev(nullptr);
                std::list<std::shared_ptr<music::Note>> links = {};

                // Add measures
                unsigned int measure_idx = 0;
                for(const auto& measure : part->getMeasures()) {
                    ++measure_idx;
                    pt::ptree measure_tree;

                    if(prev == nullptr) {
                        prev = std::make_shared<music::Measure>();
                    }

                    // Set attributes if required
                    if(measure->hasAttributes()) {
                        uint8_t divisions = measure->getDivisions();
                        if(divisions != prev->getDivisions()) {
                            prev->setDivisions(divisions);
                            measure_tree.put("attributes.divisions", (int)divisions);
                        }

                        int fifths = measure->getFifths();
                        if(fifths != prev->getFifths()) {
                            prev->setFifths(fifths);
                            measure_tree.put("attributes.key.fifths", fifths);
                        }

                        auto time = measure->getTime();
                        if(time != prev->getTime()) {
                            prev->setTime(time);
                            measure_tree.put("attributes.time.beats", time.first);
                            measure_tree.put("attributes.time.beat-type", time.second);
                        }

                        auto clef = measure->getClef();
                        if(clef.getLine() != prev->getClef().getLine() || clef.getSign() != prev->getClef().getSign()) {
                            prev->setClef(clef);
                            measure_tree.put("attributes.clef.sign", (char)clef.getSign());
                            measure_tree.put("attributes.clef.line", clef.getLine());
                        }
                    }
                    measure_tree.put("<xmlattr>.number", measure_idx);

                    // Add Notes
                    for(const auto& note : measure->getNotes()) {
                        pt::ptree note_tree;

                        music::Note::Semitone s;

                        auto repr = music::Note::splitPitch(music::Note::pitchRepr(note.getPitch()), s);

                        if(note.isPause()) {
                            note_tree.put("rest", "");
                            continue;
                        }

                        note_tree.put("pitch.step", repr.first);
                        if(s == music::Note::Semitone::SHARP) {
                            note_tree.put("pitch.alter", 1);
                        } else if(s == music::Note::Semitone::FLAT) {
                            note_tree.put("pitch.alter", -1);
                        }
                        note_tree.put("pitch.octave", repr.second);
                        note_tree.put("duration", note.getDuration());

                        auto it = std::find(links.begin(), links.end(), std::make_shared<music::Note>(note));
                        if(it != links.end()) {
                            note_tree.put("tie.<xmlattr>.type", "stop");
                            links.erase(it);
                        }
                        for(const auto& p : note.getLinks()) {
                            links.push_back(p);
                        }
                        if(!note.getLinks().empty()) {
                            note_tree.put("tie.<xmlattr>.type", "start");
                        }
                        note_tree.put("voice", 1);
                        note_tree.put("type", "quarter");

                        measure_tree.add_child("note", note_tree);
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