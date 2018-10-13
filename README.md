# autoplay
`autoplay` is a simple c++ implementation of a combination of some music composition algorithms.
The target of this project is to create music software that can automatically generate good melodies in a lovely rhythm.

## Dependencies
#### RtMidi 3.0.0
This project makes use of the [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/) library, current version `3.0.0`.
This library allows for _real-time_ playback of the music. Maybe this dependency will be
removed later on.

#### TRNG 

## Progress Log
| Date | Update
|:---:|:---
| 07-11-2018 | Added `RtMidi` library for easy playback.
| 13-10-2018 | Started looking at different file formats to export to:<br>- MIDI (might have issues with the non-generality)<br>- [MusicXML](https://www.musicxml.com/) (standard open format for exchanging digital sheet music)
| 13-10-2018 | Added `TRNG` as a pseudo-random number generator. 

