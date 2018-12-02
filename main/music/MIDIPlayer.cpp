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
 *  Created on 27/10/2018
 */

#include <algorithm>

#include <zupply/src/zupply.hpp>
#include <rtmidi/RtMidi.h>
#include <zconf.h>

#include "MIDIPlayer.h"

#define SLEEP(milliseconds) usleep((unsigned long)((milliseconds)*1000.0))

namespace autoplay {
    namespace music {
        std::shared_ptr<MIDIPlayer> MIDIPlayer::instance() {
            static std::shared_ptr<MIDIPlayer> instance{new MIDIPlayer};
            return instance;
        }

        void MIDIPlayer::probe(const util::Config& config) const {
            // Create an api map.
            std::map<int, std::string> apiMap;
            apiMap[RtMidi::MACOSX_CORE]  = "OS-X CoreMidi";
            apiMap[RtMidi::WINDOWS_MM]   = "Windows MultiMedia";
            apiMap[RtMidi::UNIX_JACK]    = "Jack Client";
            apiMap[RtMidi::LINUX_ALSA]   = "Linux ALSA";
            apiMap[RtMidi::RTMIDI_DUMMY] = "RtMidi Dummy";

            std::vector<RtMidi::Api> apis;
            RtMidi::getCompiledApi(apis);

            auto logger = config.getLogger();
            logger->debug("The following APIs were found:");

            for(const auto& a : apis) {
                logger->debug("\t") << apiMap[a];
            }

            RtMidiIn*  midiin  = nullptr;
            RtMidiOut* midiout = nullptr;

            try {

                // RtMidiIn constructor ... exception possible
                midiin = new RtMidiIn();
                logger->info("Current input API: ") << apiMap[midiin->getCurrentApi()];

                // Check inputs.
                unsigned int nPorts = midiin->getPortCount();
                logger->debug("There are ") << nPorts << " MIDI input sources available:";

                for(unsigned i = 0; i < nPorts; i++) {
                    std::string portName = midiin->getPortName(i);
                    logger->debug("\tInput Port #") << (i + 1) << ": " << portName;
                }

                // RtMidiOut constructor ... exception possible
                midiout = new RtMidiOut();

                logger->info("Current output API: ") << apiMap[midiout->getCurrentApi()];

                // Check outputs.
                nPorts = midiout->getPortCount();
                logger->debug("There are ") << nPorts << " MIDI output sources available:";

                for(unsigned i = 0; i < nPorts; i++) {
                    std::string portName = midiout->getPortName(i);
                    logger->debug("\tOutput Port #") << (i + 1) << ": " << portName;
                }
            } catch(RtMidiError& error) { logger->error(error.getMessage().c_str()); }

            delete midiin;
            delete midiout;
        }

        void MIDIPlayer::play(const Score& score, const util::Config& config) const {
            auto logger = config.getLogger();
            logger->debug("Setting up RtMidi Output");

            // Create output variable
            RtMidiOut* midiout = new RtMidiOut();

            // Check available ports
            unsigned int nPorts = midiout->getPortCount();
            if(nPorts == 0) {
                logger->warn("No output ports available. Cannot play.");
            } else {
                // Open output port
                midiout->openPort(1);
                logger->debug("\tPort 1 with name '") << midiout->getPortName(1) << "' has been opened.";

                // Set all Instruments
                assert(score.getParts().size() <= 16);
                logger->debug("Setting Instruments");
                std::vector<unsigned char> msg;
                unsigned int               duration = 0; // total duration (counted in measures)
                for(uint8_t i = 0; i < score.getParts().size(); ++i) {
                    auto part = score.getParts().at(i);
                    duration  = std::max(duration, (unsigned int)part->getMeasures().size());
                    if(part->getInstruments().size() > 1 || part->getInstruments().at(0)->isPercussion()) {
                        continue;
                    }
                    auto          instrument = part->getInstruments().at(0);
                    unsigned char m1         = (char)0xc0 + (unsigned char)i;
                    auto          m2         = (unsigned char)(instrument->getProgram() - 1);
                    msg                      = {m1, m2};
                    midiout->sendMessage(&msg);
                    logger->debug("\tSet Instrument ") << instrument->getName() << " to Channel " << (int)i;
                }

                // TODO: Set time code (technically not required)
                // Set volume (control change)
                msg = {176, 7, 100};
                midiout->sendMessage(&msg);

                // Try and work around static
                msg = {0x90, 10, 0};
                midiout->sendMessage(&msg);
                SLEEP(500);
                msg = {0x80, 10, 0};
                midiout->sendMessage(&msg);

                // Collect Measures
                using message_type      = std::vector<unsigned char>;
                using message_list_type = std::vector<message_type>;
                using timestap_type     = std::vector<message_list_type>;
                unsigned int length     = 0;
                int          bpm        = 0;

                timestap_type note_list(1);
                logger->debug("Collecting {} Measure(s).", duration);
                for(unsigned measure_number = 0; measure_number < duration; ++measure_number) {
                    auto begin_measure = score.getParts().front()->getMeasures().at(measure_number);
                    length             = 4 * (unsigned)begin_measure->getDivisions() / begin_measure->getTime().second;
                    unsigned int measure_duration = length * begin_measure->getTime().first;

                    if(bpm == 0) {
                        bpm = begin_measure->getBPM();
                    }

                    unsigned int begin = (unsigned int)note_list.size() - 1;
                    note_list.resize(note_list.size() + measure_duration);

                    // Fill message list with all messages that need to be send at the current timestep
                    for(uint8_t channel = 0; channel < score.getParts().size(); ++channel) {
                        auto curr_measure = score.getParts().at(channel)->getMeasures().at(measure_number);
                        bool perc         = score.getParts().at(channel)->getInstruments().size() > 1 ||
                                    score.getParts().at(channel)->getInstruments().at(0)->isPercussion();
                        uint8_t msgch = channel;
                        if(perc) {
                            msgch = 9;
                        }

                        unsigned int time = 0;

                        for(const auto& chord : curr_measure->getNotes()) {
                            if(!chord.isPause()) {
                                for(const auto& note : chord.getNotes()) {
                                    if(!note->getTieEnd()) {
                                        auto _msg = note->getOnMessage(msgch);
                                        if(perc && note->getInstrument() != nullptr) {
                                            _msg.at(1) = note->getInstrument()->getUnpitched();
                                        }
                                        note_list.at(begin + time).emplace_back(_msg);
                                    }
                                    if(!note->getTieStart()) {
                                        auto _msg = note->getOffMessage(msgch);
                                        if(perc && note->getInstrument() != nullptr) {
                                            _msg.at(1) = note->getInstrument()->getUnpitched();
                                        }
                                        note_list.at(begin + time + note->getDuration()).emplace_back(_msg);
                                    }
                                }
                            }
                            time += chord.getDuration();
                        }
                    }
                }

                // Play Measures
                if(bpm == 0 || length == 0) {
                    logger->error("Impossible to play empty score.");
                } else {
                    zz::log::ProgBar pb{(unsigned int)note_list.size(), "Playing"};
                    for(const auto& msgs : note_list) {
                        for(const auto& m : msgs) {
                            midiout->sendMessage(&m);
                        }
                        pb.step(1);
                        SLEEP((int)(60 * 1000 / (bpm * length)));
                    }
                }

                // Control Change
                msg = {176, 7, 100};
                midiout->sendMessage(&msg);

                // SysEx
                msg = {240, 67, 4, 3, 2, 247};
                midiout->sendMessage(&msg);

                logger->debug("Finished playing. Shutting down RtMidi Output.");
            }

            delete midiout;
        }
    }
}