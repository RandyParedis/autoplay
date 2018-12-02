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
 *  Created on 05/10/2018
 */

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

    util::Generator generator{config, logger};

    try {
        music::Score score = generator.generate();

        if(!config.isLeaf("export")) {
            auto fname = config.conf<std::string>("export.filename");
            logger->debug("Exporting Score to '{}'.", fname);
            util::FileHandler::writeMusicXML(fname, score);
        }

        if(config.conf<bool>("play")) {
            midiPlayer->play(score, config);
        }

        logger->info("Finished autoplayer");

    } catch(std::exception& e) {
        logger->fatal(e.what());
        exit(EXIT_FAILURE);
    }
}
