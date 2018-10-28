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
#include "util/Generator.h"

#include <trng/lcg64.hpp>

#define SLEEP( milliseconds ) usleep( (unsigned long) ((milliseconds) * 1000.0) )

int main(int argc, char** argv)
{
    // Create the Logger
    Config config{argc, argv};
    auto logger = config.getLogger();
    logger->info("Started autoplayer");

    // Probe information
    std::shared_ptr<music::MIDIPlayer> midiPlayer = music::MIDIPlayer::instance();
    midiPlayer->probe(config);

    Generator generator{config};
    music::Score score = generator.generate();

    if(config.conf<bool>("play")) {
        midiPlayer->play(score, config);
    }

    logger->info("Finished autoplayer");
}