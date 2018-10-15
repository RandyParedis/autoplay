# Algorithms
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

The algorithms I've come up with (with `pn` the new pitch and `po` the
old/previous one) are:

##### Totally Random 
This is maybe one of the easiest algorithms that we could use (with the exception of a
semitone musical piece). Basically it generates a random note somewhere in the musical
spectrum.

`pn = choose(range(0, 127))`

##### Brownian Motion
Most music pieces don't make a lot of jumps. This algorithm, based upon the movement
of small particles that are randomly bombarded by molecules of the surrounding medium,
basically forces the jumps to be somewhere between three pitches above or below the
currently played note. [More info here.](https://quod.lib.umich.edu/s/spobooks/bbv9810.0001.001/1:18/--algorithmic-composition-a-gentle-introduction-to-music?rgn=div1;view=fulltext#18.5)

`pn = choose(range(-3, 3)) + po`

##### 1/f Noise
As discussed [here](https://quod.lib.umich.edu/s/spobooks/bbv9810.0001.001/1:18/--algorithmic-composition-a-gentle-introduction-to-music?rgn=div1;view=fulltext#18.7),
1/f Noise is a very special class of noise and recurs often in nature. Besides the
strangely easy algorithm created by Richard F. Voss, there are a few issues: a good
selection of the sides of the dice, the mappings and the table to use.

We can say (for instance) that we will use 5-sided "dice", as discussed on the site,
but will we only stay in the same octave, only move up, or shift the result over, so we
get a similar range to the **Brownian Motion**?

Maybe we can add some user-definable options?

##### Centralized
Invented as a stepping-stone to the **Gaussian Voicing** implementation, this algorithm
came to be. It basically says that the chance for each note chosen is the highest in the
middle of our pitch range, playing the piece mostly around the central octave.

It uses the Gauss-curve of a standard normal distribution.

`pn = gaussian(range(0, 127))`

##### Gaussian Voicing
**Voicing** generally is a technique most experienced composers and pianists use when
writing a piece. It is the idea that the next note that will be played will most likely
be close to the one you just played. This is commonly used in the two inversions of a 
simple chord.

**Gaussian Voicing** uses the Gauss-curve of a standard normal distribution to focus its
center around the justly played note.

`pn = gaussian(range(-po, 127-po), -3, 3, true) + po`

(_Note that in the algorithm two shifts of the pitch are required. The first to centralize
it around `po`, the second to bring it back to the valid range._)

##### Markov Chains
`pn` will be the following pitch according to a machine-learned markov chain.