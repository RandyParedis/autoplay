//
// Created by randy on 05/10/18.
//
// 'probe' function by Gary Scavone, 2003-2012.
//

#include <rtmidi/RtMidi.h>
#include <zupply/src/zupply.hpp>

#include <iostream>
#include <cstdlib>
#include <map>
#include <pthread.h>
#include <unistd.h>
#include "music/Note.h"
#include "util/Randomizer.h"
#include "music/Instrument.h"
#include "music/MIDIPlayer.h"

#include <trng/mrg2.hpp>

#define SLEEP( milliseconds ) usleep( (unsigned long) ((milliseconds) * 1000.0) )

int main()
{
    // trng::mrg2 engine;
    // engine.seed(10);
    // std::cout << Randomizer::pick_uniform(engine, 0.0f, 100.0f) << std::endl;

    // Create the Logger
    zz::log::LogConfig::instance().set_format("[%datetime][%level]\t%msg");
    auto logger = zz::log::get_logger("system_logger");
    logger->info("Started autoplayer");

    // Probe information
    std::shared_ptr<music::MIDIPlayer> midiPlayer = music::MIDIPlayer::instance();
    midiPlayer->probe(logger);

    music::Measure measure(music::Clef::Treble(), {4, 4}, 24);
    std::vector<uint8_t> pitches = {
            music::Note::pitch("E4"),
            music::Note::pitch("E4"),
            music::Note::pitch("F4"),
            music::Note::pitch("G4"),
            music::Note::pitch("G4"),
            music::Note::pitch("F4"),
            music::Note::pitch("E4"),
            music::Note::pitch("D4"),
            music::Note::pitch("C4"),
            music::Note::pitch("C4"),
            music::Note::pitch("D4"),
            music::Note::pitch("E4"),
            music::Note::pitch("E4"),
            music::Note::pitch("D4"),
            music::Note::pitch("D4"),

            music::Note::pitch("E4"),
            music::Note::pitch("E4"),
            music::Note::pitch("F4"),
            music::Note::pitch("G4"),
            music::Note::pitch("G4"),
            music::Note::pitch("F4"),
            music::Note::pitch("E4"),
            music::Note::pitch("D4"),
            music::Note::pitch("C4"),
            music::Note::pitch("C4"),
            music::Note::pitch("D4"),
            music::Note::pitch("E4"),
            music::Note::pitch("D4"),
            music::Note::pitch("C4"),
            music::Note::pitch("C4")
    };
    for(const auto& pitch: pitches) {
        music::Note n{pitch, 24};
        measure.append(n);
    }
    music::Instrument instrument = music::instruments::acoustic_grand_piano;
    std::shared_ptr<music::Part> part = std::make_shared<music::Part>(&instrument);
    part->setMeasures(measure);
    music::Score score;
    score.parts.emplace_back(part);

    midiPlayer->play(score, logger);

    /*RtMidiOut *midiout = new RtMidiOut();
    std::vector<unsigned char> message;
    // Check available ports.
    unsigned int nPorts = midiout->getPortCount();
    if ( nPorts == 0 ) {
        std::cout << "No ports available!\n";
    } else {
        // Open first available port.
        midiout->openPort( 1 );

        music::Instrument instrument = music::instruments::hapsichord;
        // Program change: 192, 5 (Channel 0, Program 5)
        message.push_back( 192 );
        message.push_back( instrument.getUnpitched() - 1 ); //< Instrument
        midiout->sendMessage( &message );

        SLEEP( 500 );

        message[0] = 0xF1; // MIDI Time Code Quarter Frame
        message[1] = 60;
        // midiout->sendMessage( &message );

        SLEEP( 500 );

        // Control Change: 176, 7, 100 (volume)
        message[0] = 176;
        message[1] = 7;
        message.push_back( 100 );
        midiout->sendMessage( &message );

        std::vector<std::pair<uint8_t, unsigned int>> pitches = {
                {music::Note::pitch("E4"), 500},
                {music::Note::pitch("E4"), 500},
                {music::Note::pitch("F4"), 500},
                // {music::Note::pitch("G4"), 500},
                // {music::Note::pitch("G4"), 500},
                // {music::Note::pitch("F4"), 500},
                // {music::Note::pitch("E4"), 500},
                // {music::Note::pitch("D4"), 500},
                // {music::Note::pitch("C4"), 500},
                // {music::Note::pitch("C4"), 500},
                // {music::Note::pitch("D4"), 500},
                // {music::Note::pitch("E4"), 500},
                // {music::Note::pitch("E4"), 750},
                // {music::Note::pitch("D4"), 250},
                // {music::Note::pitch("D4"), 1000},
                //
                // {music::Note::pitch("E4"), 500},
                // {music::Note::pitch("E4"), 500},
                // {music::Note::pitch("F4"), 500},
                // {music::Note::pitch("G4"), 500},
                // {music::Note::pitch("G4"), 500},
                // {music::Note::pitch("F4"), 500},
                // {music::Note::pitch("E4"), 500},
                // {music::Note::pitch("D4"), 500},
                // {music::Note::pitch("C4"), 500},
                // {music::Note::pitch("C4"), 500},
                // {music::Note::pitch("D4"), 500},
                // {music::Note::pitch("E4"), 500},
                // {music::Note::pitch("D4"), 750},
                // {music::Note::pitch("C4"), 250},
                // {music::Note::pitch("C4"), 1000}
        };

        // Note On: 144, 64, 90
        for(auto& p: pitches) {
            message[0] = 144;
            message[1] = p.first;
            message[2] = 90;
            midiout->sendMessage( &message );
            SLEEP( p.second );
            message[0] = 128;
            message[1] = p.first;
            message[2] = 40;
            midiout->sendMessage( &message );
        }

        SLEEP( 500 );

        // Control Change: 176, 7, 40
        message[0] = 176;
        message[1] = 7;
        message[2] = 40;
        midiout->sendMessage( &message );

        SLEEP( 500 );

        // Sysex: 240, 67, 4, 3, 2, 247
        message[0] = 240;
        message[1] = 67;
        message[2] = 4;
        message.push_back( 3 );
        message.push_back( 2 );
        message.push_back( 247 );
        midiout->sendMessage( &message );
    }

    delete midiout;
    return 0;*/
}