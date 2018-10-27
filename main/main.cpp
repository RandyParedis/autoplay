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
#include "util/Config.h"

#include <trng/mrg2.hpp>

#define SLEEP( milliseconds ) usleep( (unsigned long) ((milliseconds) * 1000.0) )

int main(int argc, char** argv)
{
    // trng::mrg2 engine;
    // engine.seed(10);
    // std::cout << Randomizer::pick_uniform(engine, 0.0f, 100.0f) << std::endl;

    // Create the Logger
    Config config{argc, argv};
    auto logger = config.getLogger();
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

    if(config.conf_play()) {
        midiPlayer->play(score, logger);
    }
}