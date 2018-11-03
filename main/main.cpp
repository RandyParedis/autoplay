//
// Created by randy on 05/10/18.
//
// 'probe' function by Gary Scavone, 2003-2012.
//

#include "music/Instrument.h"
#include "music/MIDIPlayer.h"
#include "music/Note.h"
#include "util/Config.h"
#include "util/FileHandler.h"
#include "util/Generator.h"
#include "util/Randomizer.h"
#include <trng/lcg64.hpp>
#include <zupply/src/zupply.hpp>
#include <rtmidi/RtMidi.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include <pthread.h>
#include <unistd.h>

#define SLEEP(milliseconds) usleep((unsigned long)((milliseconds)*1000.0))

using namespace autoplay;

int main(int argc, char** argv) {
    // Create the Logger
    util::Config config{argc, argv};
    auto         logger = config.getLogger();
    logger->info("Started autoplayer");

    // Probe information
    std::shared_ptr<music::MIDIPlayer> midiPlayer = music::MIDIPlayer::instance();
    midiPlayer->probe(config);

    util::Generator generator{config};
    music::Score    score = generator.generate();

    if(config.conf<bool>("play")) {
        midiPlayer->play(score, config);
    }

    if(!config.isLeaf("export")) {
        util::FileHandler::writeMusicXML(config.conf<std::string>("export.filename"), score);
    }

    logger->info("Finished autoplayer");
}
