//
// Created by red on 27/10/18.
//

#include <algorithm>

#include <rtmidi/RtMidi.h>
#include <zupply/src/zupply.hpp>
#include <zconf.h>

#include "MIDIPlayer.h"

#define SLEEP( milliseconds ) usleep( (unsigned long) ((milliseconds) * 1000.0) )

namespace music {
    std::shared_ptr<MIDIPlayer> MIDIPlayer::instance() {
        static std::shared_ptr<MIDIPlayer> instance{new MIDIPlayer};
        return instance;
    }

    void MIDIPlayer::probe(zz::log::LoggerPtr &logger) const {
        // Create an api map.
        std::map<int, std::string> apiMap;
        apiMap[RtMidi::MACOSX_CORE] = "OS-X CoreMidi";
        apiMap[RtMidi::WINDOWS_MM] = "Windows MultiMedia";
        apiMap[RtMidi::UNIX_JACK] = "Jack Client";
        apiMap[RtMidi::LINUX_ALSA] = "Linux ALSA";
        apiMap[RtMidi::RTMIDI_DUMMY] = "RtMidi Dummy";

        std::vector<RtMidi::Api> apis;
        RtMidi::getCompiledApi(apis);

        logger->debug("The following APIs were found:");

        for(const auto& a: apis) {
            logger->debug("\t") << apiMap[a];
        }

        RtMidiIn  *midiin = nullptr;
        RtMidiOut *midiout = nullptr;

        try {

            // RtMidiIn constructor ... exception possible
            midiin = new RtMidiIn();
            logger->info("Current input API: ") << apiMap[midiin->getCurrentApi()];

            // Check inputs.
            unsigned int nPorts = midiin->getPortCount();
            logger->info("There are ") << nPorts << " MIDI input sources available:";

            for ( unsigned i=0; i<nPorts; i++ ) {
                std::string portName = midiin->getPortName(i);
                logger->info("\tInput Port #") << (i+1) << ": " << portName;
            }

            // RtMidiOut constructor ... exception possible
            midiout = new RtMidiOut();

            logger->info("Current output API: ") << apiMap[midiout->getCurrentApi()];

            // Check outputs.
            nPorts = midiout->getPortCount();
            logger->info("There are ") << nPorts << " MIDI output sources available:";

            for ( unsigned i=0; i<nPorts; i++ ) {
                std::string portName = midiout->getPortName(i);
                logger->info("\tOutput Port #") << (i+1) << ": " << portName;
            }
        } catch ( RtMidiError &error ) {
            logger->error(error.getMessage().c_str());
        }

        delete midiin;
        delete midiout;
    }

    void MIDIPlayer::play(const Score& score, zz::log::LoggerPtr &logger) const {
        logger->debug("Setting up RtMidi Output");

        // Create output variable
        RtMidiOut *midiout = new RtMidiOut();

        // Check available ports
        unsigned int nPorts = midiout->getPortCount();
        if ( nPorts == 0 ) {
            logger->warn("No output ports available. Cannot play.");
        } else {
            // Open output port
            midiout->openPort(1);
            logger->debug("\tPort 1 with name '") << midiout->getPortName(1) << "' has been opened.";

            // Set all Instruments
            assert(score.parts.size() <= 16);
            logger->debug("Setting Instruments");
            std::vector<unsigned char> msg;
            unsigned int duration = 0; // total duration (counted in measures)
            for(uint8_t i = 0; i < score.parts.size(); ++i) {
                auto part = score.parts.at(i);
                auto* instrument = part->getInstrument();
                unsigned char f = (char)0xc0 + (unsigned char)i;
                msg = { f, instrument->getUnpitched() - 1 };
                midiout->sendMessage(&msg);
                logger->debug("\tSet Instrument ") << instrument->getName() << " to Channel " << (int)i;

                duration = std::max(duration, (unsigned int)part->getMeasures().size());
            }

            // TODO: Set time code (technically not required)
            // Set volume (control change)
            msg = {176, 7, 100};
            midiout->sendMessage(&msg);

            // Try and work around static
            msg = { 0x90, 10, 0 };
            midiout->sendMessage(&msg);
            SLEEP(500);
            msg = { 0x80, 10, 0 };
            midiout->sendMessage(&msg);

            // Collect & Play Measures
            logger->debug("Collecting and Playing ") << duration << " Measure(s)";
            zz::log::ProgBar pb{duration, "Playing"};
            for(unsigned measure_number = 0; measure_number < duration; ++measure_number) {
                std::vector<std::vector<std::vector<unsigned char>>> note_list;
                for(uint8_t channel = 0; channel < score.parts.size(); ++channel) {
                    auto curr_measure = score.parts.at(channel)->getMeasures().at(measure_number);
                    if(curr_measure->isOverflowing()) {
                        logger->error("Measure ") << measure_number << " overflowing for Part " << (int)channel;
                    }
                    std::vector<std::vector<unsigned char>> nl = {};

                    for(const auto& note: curr_measure->getNotes()) {
                        nl.emplace_back(note.getOnMessage(channel));
                        for(uint8_t len = 0; len < note.getDuration()-2; ++len) { // duration - strike - release
                            msg = {};
                            nl.emplace_back(msg);
                        }
                        nl.emplace_back(note.getOffMessage(channel));
                    }
                    if(!note_list.empty() && note_list.back().size() != nl.size()) {
                        logger->fatal("Number of messages for Measure ") << (int)(measure_number-1)
                                                                         << " do not equal those for Measure "
                                                                         << (int)measure_number;
                    }
                    note_list.emplace_back(nl);
                }
                if(note_list.empty()) {
                    logger->warn("No Parts found to play.");
                    break;
                }
                auto ptcnt = note_list.size();
                auto d = note_list.back().size();
                logger->trace("Playing ") << ptcnt << " Part(s) of size " << d;
                for(unsigned int i = 0; i < d; ++i) {
                    for(unsigned int j = 0; j < ptcnt; ++j) {
                        msg = note_list.at(j).at(i);
                        if(!msg.empty()) {
                            logger->trace("\tSending message w/ pitch ") << (int)msg.at(1) << " on channel " << j;
                            midiout->sendMessage(&msg);
                        }
                    }
                    SLEEP(24);
                }
                pb.step(1);
            }

            // Control Change
            msg = {176, 7, 100};
            midiout->sendMessage(&msg);

            // SysEx
            msg = { 240, 67, 4, 3, 2, 247 };
            midiout->sendMessage(&msg);

            logger->debug("Finished playing. Shutting down RtMidi Output.");
        }

        delete midiout;
    }
}