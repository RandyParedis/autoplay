//
// Created by randy on 05/10/18.
//
// 'probe' function by Gary Scavone, 2003-2012.
//

#include <rtmidi/RtMidi.h>

#include <iostream>
#include <cstdlib>
#include <map>
#include <pthread.h>
#include <unistd.h>
#include "music/Note.h"
#include "util/Randomizer.h"

#include <trng/mrg2.hpp>

#define SLEEP( milliseconds ) usleep( (unsigned long) ((milliseconds) * 1000.0) )

int probe()
{
    // Create an api map.
    std::map<int, std::string> apiMap;
    apiMap[RtMidi::MACOSX_CORE] = "OS-X CoreMidi";
    apiMap[RtMidi::WINDOWS_MM] = "Windows MultiMedia";
    apiMap[RtMidi::UNIX_JACK] = "Jack Client";
    apiMap[RtMidi::LINUX_ALSA] = "Linux ALSA";
    apiMap[RtMidi::RTMIDI_DUMMY] = "RtMidi Dummy";

    std::vector< RtMidi::Api > apis;
    RtMidi :: getCompiledApi( apis );

    std::cout << "\nCompiled APIs:\n";
    for ( unsigned int i=0; i<apis.size(); i++ )
        std::cout << "  " << apiMap[ apis[i] ] << std::endl;

    RtMidiIn  *midiin = 0;
    RtMidiOut *midiout = 0;

    try {

        // RtMidiIn constructor ... exception possible
        midiin = new RtMidiIn();

        std::cout << "\nCurrent input API: " << apiMap[ midiin->getCurrentApi() ] << std::endl;

        // Check inputs.
        unsigned int nPorts = midiin->getPortCount();
        std::cout << "\nThere are " << nPorts << " MIDI input sources available.\n";

        for ( unsigned i=0; i<nPorts; i++ ) {
            std::string portName = midiin->getPortName(i);
            std::cout << "  Input Port #" << i+1 << ": " << portName << '\n';
        }

        // RtMidiOut constructor ... exception possible
        midiout = new RtMidiOut();

        std::cout << "\nCurrent output API: " << apiMap[ midiout->getCurrentApi() ] << std::endl;

        // Check outputs.
        nPorts = midiout->getPortCount();
        std::cout << "\nThere are " << nPorts << " MIDI output ports available.\n";

        for ( unsigned i=0; i<nPorts; i++ ) {
            std::string portName = midiout->getPortName(i);
            std::cout << "  Output Port #" << i+1 << ": " << portName << std::endl;
        }
        std::cout << std::endl;

    } catch ( RtMidiError &error ) {
        error.printMessage();
    }

    delete midiin;
    delete midiout;

    return 0;
}

int main()
{
    trng::mrg2 engine;
    engine.seed(10);
    std::cout << Randomizer::pick_uniform(engine, 0.0f, 100.0f) << std::endl;
    /*probe();
    RtMidiOut *midiout = new RtMidiOut();
    std::vector<unsigned char> message;
    // Check available ports.
    unsigned int nPorts = midiout->getPortCount();
    if ( nPorts == 0 ) {
        std::cout << "No ports available!\n";
    } else {
        // Open first available port.
        midiout->openPort( 1 );

        // Program change: 192, 5 (Channel 0, Program 5)
        message.push_back( 192 );
        message.push_back( 5 ); //< Instrument
        midiout->sendMessage( &message );

        SLEEP( 500 );

        message[0] = 0xF1; // MIDI Time Code Quarter Frame
        message[1] = 60;
        midiout->sendMessage( &message );

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
                {music::Note::pitch("G4"), 500},
                {music::Note::pitch("G4"), 500},
                {music::Note::pitch("F4"), 500},
                {music::Note::pitch("E4"), 500},
                {music::Note::pitch("D4"), 500},
                {music::Note::pitch("C4"), 500},
                {music::Note::pitch("C4"), 500},
                {music::Note::pitch("D4"), 500},
                {music::Note::pitch("E4"), 500},
                {music::Note::pitch("E4"), 750},
                {music::Note::pitch("D4"), 250},
                {music::Note::pitch("D4"), 1000},

                {music::Note::pitch("E4"), 500},
                {music::Note::pitch("E4"), 500},
                {music::Note::pitch("F4"), 500},
                {music::Note::pitch("G4"), 500},
                {music::Note::pitch("G4"), 500},
                {music::Note::pitch("F4"), 500},
                {music::Note::pitch("E4"), 500},
                {music::Note::pitch("D4"), 500},
                {music::Note::pitch("C4"), 500},
                {music::Note::pitch("C4"), 500},
                {music::Note::pitch("D4"), 500},
                {music::Note::pitch("E4"), 500},
                {music::Note::pitch("D4"), 750},
                {music::Note::pitch("C4"), 250},
                {music::Note::pitch("C4"), 1000}
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