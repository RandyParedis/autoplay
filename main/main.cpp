//
// Created by randy on 05/10/18.
//
// 'probe' function by Gary Scavone, 2003-2012.
//

#include <RtMidi.h>

#include <iostream>
#include <cstdlib>
#include <map>
#include <pthread.h>
#include <unistd.h>

#define SLEEP( milliseconds ) usleep( (unsigned long) (milliseconds * 1000.0) )

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
    probe();
    RtMidiOut *midiout = new RtMidiOut();
    std::vector<unsigned char> message;
    // Check available ports.
    unsigned int nPorts = midiout->getPortCount();
    if ( nPorts == 0 ) {
        std::cout << "No ports available!\n";
        goto cleanup;
    }
    // Open first available port.
    midiout->openPort( 1 );
    // Send out a series of MIDI messages.
    // Program change: 192, 5
    message.push_back( 192 );
    message.push_back( 5 );
    midiout->sendMessage( &message );
    // Control Change: 176, 7, 100 (volume)
    message[0] = 176;
    message[1] = 7;
    message.push_back( 100 );
    midiout->sendMessage( &message );
    // Note On: 144, 64, 90
    message[0] = 144;
    message[1] = 64;
    message[2] = 90;
    midiout->sendMessage( &message );
    SLEEP( 500 ); // Platform-dependent ... see example in tests directory.
    // Note Off: 128, 64, 40
    message[0] = 128;
    message[1] = 64;
    message[2] = 40;
    midiout->sendMessage( &message );
    // Clean up
    cleanup:
    delete midiout;
    return 0;
}