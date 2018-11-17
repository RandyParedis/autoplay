# Algorithms
Random Music Generation technically consists of three parts (with some exceptions when talking
about _chords_, _staccatos_...), which I've called **Random Pitch Generation (RPG)**,
**Random Rythm Generation (RRG)** and **Random Dynamics Generation (RDG)**.

I've decided to take a look at all three of these types of algorithms separately to get
the best result.

## Random Pitch Generation (RPG)
The general idea behind RPG is basically an extension on the normal RNGs. After doing
some research, I've come up with the following algorithms for this generation.
For simplicity's sake, we'll suppose there are the following functions, assuming `[...]` is a sequence of numbers (array,
sequential pointers, vector...) and `{...}` depicts a map or a dictionary of some sorts
(which maps a key uniquely to a value):

| Function Name | Description
|:---:|:---
| `choose([...])` | Pick a random value uniformly from the given list.
| `choose({...})` | Pick a random element from the given map, where an element is a key in the map and the corresponding value the chance (between 0 and 1) that this value is chosen. (e.g. weighted random selection)
| `choose([...], dist, a, b, c=false)` | Pick a random value from the given list where the chance that a certain value is picked depends on the `dist` distribution function. `a` is the lowest possible value to be mapped and `b` the highest. The probability of the value at index `x` is `a + (x - min)*(b - a)/(max - min)`, with `min` and `max` the minimum (index) and maximum (index) of the list. When `c` is `true`, we want to center the 0 in our list to the center of our distribution.
| `gaussian([...], a=-3, b=3, c=false)` | Shorthand for `choose([...], f_g, a, b, c)`, where `f_g` stands for the Gaussian distribution function. 
| `range(a, b)` | Create a sequential range between `a` and `b`.

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

Before the algorithms are discussed, please be aware that rests are not taken into a
count at the moment. This could be a 80/20 rule between note and rest.

The algorithms I've come up with (with `pn` the new pitch and `po` the
old/previous one) are:

#### Totally Random
This is maybe one of the easiest algorithms that we could use (with the exception of a
semitone musical piece). Basically it generates a random note somewhere in the musical
spectrum.

`pn = choose(range(0, 127))`

#### Brownian Motion
Most music pieces don't make a lot of jumps. This algorithm, based upon the movement
of small particles that are randomly bombarded by molecules of the surrounding medium,
basically forces the jumps to be somewhere between three pitches above or below the
currently played note. [More info here.](https://quod.lib.umich.edu/s/spobooks/bbv9810.0001.001/1:18/--algorithmic-composition-a-gentle-introduction-to-music?rgn=div1;view=fulltext#18.5)

`pn = choose(range(-3, 3)) + po`

Without loss of generality, the range of `-3` to `3` can be customizable.

#### 1/f Noise
As discussed [here](https://quod.lib.umich.edu/s/spobooks/bbv9810.0001.001/1:18/--algorithmic-composition-a-gentle-introduction-to-music?rgn=div1;view=fulltext#18.7),
1/f Noise is a very special class of noise and recurs often in nature. Besides the
strangely easy algorithm created by Richard F. Voss, there are a few issues: a good
selection of the sides of the dice, the mappings and the table to use.

We can say (for instance) that we will use 5-sided "dice", as discussed on the site,
but will we only stay in the same octave, only move up, or shift the result over, so we
get a similar range to the **Brownian Motion**?

#### Centralized
Invented as a stepping-stone to the **Gaussian Voicing** implementation, this algorithm
came to be. It basically says that the chance for each note chosen is the highest in the
middle of our pitch range, playing the piece mostly around the central octave.

It uses the Gauss-curve of a standard normal distribution.

`pn = gaussian(range(0, 127))`

#### Gaussian Voicing
**Voicing** generally is a technique most experienced composers and pianists use when
writing a piece. It is the idea that the next note that will be played will most likely
be close to the one you just played. This is commonly used in the two inversions of a 
simple chord.

**Gaussian Voicing** uses the Gauss-curve of a standard normal distribution to focus its
center around the justly played note.

`pn = gaussian(range(-po, 127-po), -3, 3, true) + po`

(_Note that in the algorithm two shifts of the pitch are required. The first is to centralize
it around `po`, the second to bring it back to the valid range._)

#### Markov Chains
`pn` will be the following pitch according to a machine-learned markov chain.

## Random Rhythm Generation (RRG)
RRG differs from normal RNGs because instead of picking a random value from 0 to 1 and
shifting it to fit a range, RRG will have to pick a value from the list below:
- 256th
- 128th
- 64th
- 32nd
- 16th
- eighth
- quarter
- half
- whole
- breve
- long

On top of that, rhythm in music is also determined by the beats per minute,
which can change throughout the piece. Even though this might seem to complicate things,
it can actually be separated into two parts:
1. Pick a rhythm for each note.
2. Change the speed of the music.

Let's discuss both options separately.

### 1. Random Note Rhythms (RNR)
Each note is basically a fraction, respectively `1/256`, `1/128`, `1/64`, `1/32`,
`1/16`, `1/8`, `1/4`, `1/2`, `1/1`, `2/1`, `4/1`; or `2^(-n)` where `n` goes from
`8` to `-2`; or even `2^(n-8)` for `n` going from `0` to `10`. This concept allows for
easy RNR, we basically get `rhythm = pow(2, choose(range(0, 10))-8)`, with respect to
the above mentioned functions and `pow` the power function.

Again, for the actual implementation of this algorithm, there are a few possibilities,
with the easiest being the **Totally Random** that was described above (of course,
with another range) and the most natural the **Markov Chain**.

Another way of implementing this would be to make a list of the most common and best
sounding note rhythms and picking a random one from that list.

The only issue with this method is that it does not allow for triplet-like structures
(unless you make it an edge-case, or you pick randomly if the structure should be a
triplet). We could of course take a look at how MusicXML and MIDI do this by looking
at _division_.

##### RNR with Division
In the beginning of the piece, we choose a certain amount of _divisions_. This number
represents how long a quarter note takes. For this, we have to determine the smallest
duration in our piece (we could use the above function on a range of `[0, 6]`). Now we
divide `1/4` by the fraction representing the smallest duration, so we can determine
the length of a quarter note in our piece.

Allowing triplet-like structures (e.g. tuplets) becomes quite easy. We just multiply 
the length by 3 for a triplet, 5 for a quintuplet, 6 for a sextuplet and so on.
Multiply by half of the factorial of the max tuplet size will allow for all
possibilities less than the max tuplet size.

All and all, `24` is a common value (it allows up to 32nd notes and triplets).

This could give an algorithm that picks a random value between 1 and four times
(represents a long note) the division. But then there is the issue of prime numbers.

##### TL;DR (Too Long; Didn't Read)
Picking a precise RNR algorithm that makes the music sound good has some unwanted
difficulties with any algorithm. The best choice would be to pick a value between 0
and 10 to represent the rhythm (`rhythm = pow(2, choose(range(0, 10))-8)`). Next we
could choose an augmentation `a` of the rhythm (value between 3 and 9, without 4 and
with a bigger chance of choosing 3 than any other value). We would remember `a` for `a`
sequential notes and than choose again if the odds say so.

### 2. Random BPM (RBPM)
We could choose a random BPM every note, but that would be extreme. A better way to
do is would be to choose a BPM every `n` measures where `n` is also chosen randomly.

Yet, depending on the choice for `n` the musical piece will be a disaster, or a lovely
harmony. A simple way to do this is to do this after all notes are generated on the
measures where the style of the music seemingly has changed (using some classification
algorithm).

For the scope of this project, I've decided not to bother with RBPM outside of the
first measure.