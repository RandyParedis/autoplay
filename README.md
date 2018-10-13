# autoplay
`autoplay` is a simple c++ implementation of a combination of some music composition algorithms.
The target of this project is to create music software that can automatically generate good melodies in a lovely rhythm.

## Dependencies
#### RtMidi 3.0.0
This project makes use of the [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/) library,
current version `3.0.0`.
This library allows for _real-time_ playback of the music. Maybe this dependency will be
removed later on.

To install this dependency, you have to install and update the submodules of this repo.
The necessary files will be linked automatically during compilation.

#### TRNG (Tina's Random Number Generator)
Due to the random nature of the results of `autoplay`, `trng` was added. This library will
handle all random actions that happen during the music generation.

After painstakingly trying to install `trng` similarly to how `RtMidi` was installed,
the decision was made to keep it as a submodule, but describe how to install the library
into the `opt` folder.

##### Installation (Ubuntu 18.04 LTS)
For a detailed description, please read the `trng.pdf` in the `trng/doc` folder (page 96).
1. Install and Update the submodule
2. Make sure `autoconf`, `automake` and `libtool` are installed. This can be done
with `sudo apt-get install autotools-dev autoconf libtool-bin`.
3. Run `autoreconf` to make sure everything works as desired.
4. Configure `trng` in the `opt` directory with your preferred compiler (e.g. GNU C++ 7)
using the following command: `sudo CXX=g++-7 ./configure --prefix=/opt/trng`.
5. Run `sudo make` and `sudo make install`.

**TODO:** Add scripts for this, so it will work easily on travis and possibly other OS.

## Progress Log
| Date | Update
|:---:|:---
| 07-11-2018 | Added `RtMidi` library for easy playback.
| 13-10-2018 | Started looking at different file formats to export to:<br>- MIDI (might have issues with the non-generality)<br>- [MusicXML](https://www.musicxml.com/) (standard open format for exchanging digital sheet music)
| 13-10-2018 | Added `TRNG` as a pseudo-random number generator. 

