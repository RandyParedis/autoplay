# autoplay
[![Build Status](https://travis-ci.com/RandyParedis/autoplay.svg?token=JpPMZMYvvgwpSMxJCpEi&branch=master)](https://travis-ci.com/RandyParedis/autoplay)

`autoplay` is a simple `C++` library of a combination of some music composition
algorithms (`autoplayer` is the executable for it). The target of this project is to create music software that can
automatically generate good melodies in a lovely rhythm with a gentle dynamic,
using advanced programming techniques.

For more information on how this project works, please take a look at the `wiki` pages.
I'm sure you'll find what you're looking for there.

## Functionality
By setting the right values and commandline arguments, you are currently able to generate okay-sounding
multi-part scores and get a [MusicXML](https://www.musicxml.com/) representation of the score.

Triplet-like structures lie outside the scope of this project, wherefore they are not generated.

#### What's in `autoplay`?
Because most people like lists, I've listed the possibilities of `autoplay` below.
- Customizable!
- Pseudo-random (with a wide range of possible engines)!
- A wide range of _pitch_, _rhythm_ and _chord note count_ generation algorithms!
- Exports to MusicXML, a format used in many score-writing software!
- Integratable in other projects!

## Execution
To execute `autoplay`, one simply has to execute the executable `autoplayer`.
This will allow the user to add certain flags and fetch some options. For more
info, please execute `autoplayer` with the flag `-h` or `--help`, or take a look
at the `Configuration` wiki.

## Installation
The installation of the project was never this simple! You just have to run the
following commands and you're done.
```bash
git clone https://github.com/RandyParedis/autoplay.git
git submodule update --init --recursive
install.sh
```

Please be aware that this will install all dependencies! If you already have some
dependencies and thus don't want to reinstall them, please export the following
paths to the location where these dependencies are located.
- `BOOST_ROOT`: The location where `boost` is installed. It defaults to
`/usr/include/boost`.
- `TRNG_LOC`: The location where `trng` is installed. It defaults to
`deps/trng`.

So, on my system, I have to run the following set of commands to install, because
`trng` is installed in `/opt/trng`:
```bash
export TRNG_LOC=/opt/trng
install.sh
```

## Dependencies
#### RtMidi 3.0.0
This project makes use of the [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/) library,
current version `3.0.0`.
This library allows for _real-time_ playback of the music. Maybe this dependency will be
removed/togglable later on.

To install this dependency, you have to install and update the submodules of this repo.
The necessary files will be linked automatically during compilation.

###### Dependencies
RtMidi itself comes with a few dependencies, as can be found
[here](https://www.music.mcgill.ca/~gary/rtmidi/index.html#compiling).

The installation of these should happen automatically on Linux systems when running
the `install.sh` script. On OSX systems, everything should be in order by default.

#### Boost
Boost is definitely a powerful library that can handle a lot of different things.
In the scope of this project, `Boost` is used for easy `JSON` and `XML` parsing
and generation.

#### Google Test
The Google Testing Framework is a useful tool for writing and testing unit tests.
Of course randomization is difficult to test, but a series of algorithms can be
tested anyways.

###### GTest Installation (Ubuntu 18.04 LTS)
```
sudo apt-get install libgtest-dev cmake
cd /usr/src/gtest
sudo cmake CMakeLists.txt
sudo make
sudo cp *.a /usr/lib
```

###### GTest Installation (Mac OSX)
```
sudo wget https://github.com/google/googletest/archive/release-1.7.0.tar.gz
sudo tar xf release-1.7.0.tar.gz
cd googletest-release-1.7.0
sudo cmake -DBUILD_SHARED_LIBS=ON .
sudo make
sudo cp -a include/gtest /usr/include
sudo cp -a libgtest_main.so libgtest.so /usr/lib/
```

#### TRNG (Tina's Random Number Generator)
Due to the random nature of the results of `autoplay`, `trng` was added. This
library will handle all random actions that happen during the music generation.

After painstakingly trying to install `trng` similarly to how `RtMidi` was
installed, the decision was made to keep it as a submodule.

## Progress Log
| Date | Update
|:---:|:---
| 07-11-2018 | Added `RtMidi` library for easy playback.
| 13-10-2018 | Started looking at different file formats to export to:<br>- MIDI (might have issues with the non-generality)<br>- [MusicXML](https://www.musicxml.com/) (standard open format for exchanging digital sheet music)
| 13-10-2018 | Added `TRNG` as a pseudo-random number generator.
| 14-10-2018 | Added `Boost` and started the concept of the `RPG`.
| 14-10-2018 | Did some research on different music generation algorithms
| 14-10-2018 | Added `Travis CI`
| 14-10-2018 | Created the `Randomizer` class to handle all randomization without the need to keep on using the same code over and over again.
| 15-10-2018 | Added `gtest`.
| 20-10-2018 | Added tests for `Note` class.
| 21-10-2018 | Added `Clef` and `Measure` class and added tests for the latter.
| 23-10-2018 | Added `zupply` for logging, progress bars and command line argument parsing.
| 24-10-2018 | Started work on `Instrument` class, by using a very basic implementation. Currently, only piano instruments are implemented in the `music::instruments` namespace, but this is enough for the current working version.<br>Also added a `music::instruments::findByName(const std::string&)` function to find an instrument by name (precursor to config file support).
| 27-10-2018 | Added `Part`, `Score` and `MIDIPlayer` classes.
| 27-10-2018 | Added `Config` class.
| 28-10-2018 | Added pipeline that executes `autoplayer`: It parses arguments, probes output ports, generates the music and plays it.
| 03-11-2018 | Added modularity for Instruments and Styles. 
| 04-11-2018 | Added multiple part support.<br>Also expanded Instrument set to 80 different instruments.
| 05-11-2018 | Added different/custom Clef support.
| 10-11-2018 | Fixed Rhythmic Instrument Support
| 17-11-2018 | Added **Brownian Motion** as a pitch algorithm.
| 18-11-2018 | Added `rest-ratio` and `length` parameters to config file.
| 18-11-2018 | Added **1/f Noise** and **Centralized** as a pitch algorithm.
| 22-11-2018 | Fixed MidiPlayer playback with ties, added `bpm` and `time` signature to styles.
| 22-11-2018 | Added **Brownian Motion** as a rhythm algorithm.
| 24-11-2018 | Added support for chords, **BUT** `MIDIPlayer` currently only plays bottom note of each chord and no multiple-note chords are being generated by the algorithms (yet).
| 25-11-2018 | Config file now requires `styles.json`, `instruments.json` and `clefs.json`.<br>Also added automatic beaming of notes of a type less than a quarter note.
| 28-11-2018 | Fixed MidiPlayer to a better algorithm.
| 01-12-2018 | Added better chord generation support (or rather: made it better).<br>Also added `chord-progression` as a style option.
| 02-12-2018 | Added **Accompaniment** as a pitch algorithm.
| 02-12-2018 | Started work on **Markov Chains**.
| 08-12-2018 | Made `autoplay` run from anywhere; e.g. got rid of execution folder requirement.
