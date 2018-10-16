# autoplay
[![Build Status](https://travis-ci.com/RandyParedis/autoplay.svg?token=JpPMZMYvvgwpSMxJCpEi&branch=master)](https://travis-ci.com/RandyParedis/autoplay)

`autoplay` is a simple `C++` library of a combination of some music composition
algorithms. The target of this project is to create music software that can
automatically generate good melodies in a lovely rhythm with a gentle dynamic.

For more information on how this project works, please take a look at the `docs` folder.
I'm sure you'll find what you're looking for there.

## Installation
The installation of the project is rather simple. Just clone this repository and compile
via `cmake`. That's all there is to it. Oh, don't forget to clone the dependencies (
submodules) as well.

## Dependencies
#### RtMidi 3.0.0
This project makes use of the [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/) library,
current version `3.0.0`.
This library allows for _real-time_ playback of the music. Maybe this dependency will be
removed later on.

To install this dependency, you have to install and update the submodules of this repo.
The necessary files will be linked automatically during compilation.

###### Dependencies
RtMidi itself comes with a few dependencies, as can be found
[here](https://www.music.mcgill.ca/~gary/rtmidi/index.html#compiling).

#### Boost
Boost is definitely a powerful library that can handle a lot of different things. In the
scope of this project, `Boost` is used for easy `JSON` and `XML` parsing and generation.

You must have `Boost` installed on your system for this to work.

#### Google Test
The Google Testing Framework is a useful tool for writing and testing unit tests.
Of course randomization is difficult to test, but a series of algorithms can be
tested anyways.

You must have `gtest` installed.

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
Due to the random nature of the results of `autoplay`, `trng` was added. This library will
handle all random actions that happen during the music generation.

After painstakingly trying to install `trng` similarly to how `RtMidi` was installed,
the decision was made to keep it as a submodule, but describe how to install the library
into the `opt` folder.

###### TRNG Installation (Ubuntu 18.04 LTS)
For a detailed description, please read the `trng.pdf` in the `trng/doc` folder (page 96).
1. Install and Update the submodule
2. Make sure `autoconf`, `automake` and `libtool` are installed. This can be done
with `sudo apt-get install autotools-dev autoconf libtool-bin`.
3. Run `autoreconf` on the `trng` folder to make sure everything works as desired.
4. Configure `trng` in the `opt` directory with your preferred compiler (e.g. GNU C++ 7)
using the following command: `sudo CXX=g++-7 ./configure --prefix=/opt/trng`.
5. Run `sudo make` and `sudo make install`.

If you don't want to use `/opt/trng` as the folder for `trng`, you can do so, but must
compile `cmake` of this project with the flag `-DTRNG_LOC` set to the directory you chose.

**TODO:** Add scripts for this, so it will work easily on travis and possibly other OS.

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

