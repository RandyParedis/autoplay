# autoplay
`autoplay` is a simple c++ implementation of a combination of some music composition algorithms.
The target of this project is to create music software that can automatically generate good melodies in a lovely rhythm.

## Algorithms
Random Music Generation technically consists of three parts (with some exceptions when talking
about _chords_, _staccatos_...), which I've called **Random Pitch Generation (RPG)**,
**Random Rythm Generation (RRG)** and **Random Dynamics Generation (RDG)**.

I've decided to take a look at all three of these types of algorithms separately to get
the best result.

#### Random Pitch Generation (RPG)
The general idea behind RPG is basically an extension on the normal RNGs. After doing
some research, I've come up with the following algorithms for this generation.
For simplicity's sake, we'll suppose there are the following functions (based upon the
corresponding functions in `GML`), assuming `[...]` is a sequence of numbers (array,
sequential pointers, vector...) and `{...}` depicts a map or a dictionary of some sorts
(which maps a key uniquely to a value):

| Function Name | Description
|:---:|:---
| `choose([...])` | Pick a random value uniformly from the given list.
| `choose({...})` | Pick a random element from the given map, where an element is a key in the map and the corresponding value the chance (between 0 and 1) that this value is chosen. (e.g. weighted random selection)
| `choose([...], dist, a, b)` | Pick a random value from the given list where the chance that a certain value is picked depends on the `dist` distribution function. `a` is the lowest possible value to be mapped and `b` the highest. The probability of the value at index `x` is <br>`a + (x - min)*(b - a)/(max - min)`, with `min` and `max` the minimum (index) and maximum (index) of the list.
| `gaussian([...], a=-3, b=3)` | Shorthand for `choose([...], f_g, a, b)`, where `f_g` stands for the Gaussian distribution function. 
| `range(a, b)` | Create a sequential range between `a` and `b`. 

The algorithms I've come up with (with `pn` the new pitch and `po` the old/previous one) are:
- **Totally Random:** `pn = choose(range(0, 127))`
- **Brownian Motion:** `pn = choose(range(-3, 3)) + po`
- **Noise:** See [this page](https://quod.lib.umich.edu/s/spobooks/bbv9810.0001.001/1:18/--algorithmic-composition-a-gentle-introduction-to-music?rgn=div1;view=fulltext#18.5),
which also discusses the above mentioned `Brownian Motion`.
- **Voicing:** `pn = gaussian(range(0, 127))`
- **Markov:** `pn` will be the following pitch according to a machine-learned markov
chain.

###### Weighted Random Selection
This algorithm that is used in the main bulk of the functions described above has the
following pseudocode, according to [this website](https://medium.com/@peterkellyonline/weighted-random-selection-3ff222917eb6).
```
randomWeight = choose(range(1, sumOfWeights))
for each item in array do
    randomWeight = randomWeight - item.weight
    if randomWeight <= 0 then
        pick this item
    fi 
done
```

## Dependencies
#### RtMidi 3.0.0
This project makes use of the [RtMidi](https://www.music.mcgill.ca/~gary/rtmidi/) library,
current version `3.0.0`.
This library allows for _real-time_ playback of the music. Maybe this dependency will be
removed later on.

To install this dependency, you have to install and update the submodules of this repo.
The necessary files will be linked automatically during compilation.

#### Boost
Boost is definitely a powerful library that can handle a lot of different things. In the
scope of this project, `Boost` is used for easy `JSON` and `XML` parsing and generation.

You must have `Boost` installed on your system for this to work.

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
3. Run `autoreconf` on the `trng` folder to make sure everything works as desired.
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
| 14-10-2018 | Added `Boost` and started the concept of the `RPG`.
| 14-10-2018 | Did some research on different music generation algorithms
| 14-10-2018 | Started adding `Travis CI`

