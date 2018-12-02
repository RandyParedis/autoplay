# Arguments
When `autoplayer` is launched, there are a few options/possibilities for the user to teak and
play with. Below, I've listed all possible ways to configure the program from the commandline.

_**Note:** These flags can be combined in any way, but the last option that has been given
suppresses all others._

| Option | Meaning
|:---:|:---
| `-h`, `--help` | Show the `help`-menu (which gives the same information as this table). Afterwards, the program ends.
| `--version` | Display the current version of `autoplay`.
| `-v`, `--verbose` | Show debug information, even in _Release_ mode. (overwrites config file)
| `-p`, `--play` | When this flag is set, the music will be played upon execution. (overwrites config file)
| `-c`, `--config` | A configuration file to load.
| `-i`, `--instruments` | An instruments file to load.
| `-s`, `--styles` | A styles file to load.
| `-k`, `--clefs` | A clefs file to load.

# Config File
It is also possible to load a config file into `autoplayer`, to allow for further and
more detailed descriptions of options. When using an `XML` config file, you must wrap
all options inside a root element called `config`.


The file contains of the following tree structure and fields:
 
 - `verbose`: When set to `true`, it will show all debug information as well as info,
 warning, error and fatal messages. Therefore, it does the same as the `-v` or `--verbose`
 flag.<br>_**Note:** Please be aware this field will be discarded if the corresponding flags
 have been set._
 - `play`: When set to `true`, it will play the music after generation, simulating the
 `-p` or `--play` flags.<br>_**Note:** Please be aware this field will be discarded if
 the corresponding flags have been set._
 - `engine`: This field corresponds to the specific _Random Engine_ has been used, or more
 specifically: which algorithm will be used for generating the random numbers.<br>This field
 can be any of the following:
    - `lcg64`: Use a linear congruential generator with modulus 2^64
    - `lcg64_shift`: Use a linear congruential generator with modulus 2^64 with additional
                     bit-shift transformation.
    - `mrg2`: Use a multiple recurrence generator based on a linear feed-back shift register
              sequence over F_{2^31-1} of depth 2.
    - `mrg3`: Use a multiple recurrence generator based on a linear feed-back shift register
              sequence over F_{2^31-1} of depth 3.
    - `mrg4`: Use a multiple recurrence generator based on a linear feed-back shift register
              sequence over F_{2^31-1} of depth 4.
    - `mrg5`: Use a multiple recurrence generator based on a linear feed-back shift register
              sequence over F_{2^31-1} of depth 5.
    - `mrg5s`: Use a multiple recurrence generator based on a linear feed-back shift register
               sequence over F_m of depth 5, with `m` being a Sophie-Germain Prime.
    - `mt19937`: Use a Mersenne twister, generating 32 random bits.
    - `mt19937_64`: Use a Mersenne twister, generating 64 random bits.
    - `yarn2`: Use a multiple recursive generator with 2 feedback taps.
    - `yarn3`: Use a multiple recursive generator with 3 feedback taps.
    - `yarn4`: Use a multiple recursive generator with 4 feedback taps.
    - `yarn5`: Use a multiple recursive generator with 5 feedback taps.
    - `yarn5s`: Use a multiple recursive generator with 5 feedback taps.
 - `seed`: The seed to give the engine. This way, the same seed returns the same music.
 Obviously, when more options are added and more things are being randomized, the same seed
 will yield different results.
 - `length`: The duration of the score to generate, expressed in measures. Defaults to
 10 when this field is not set.
 - `instruments`: The instruments file to use.<br>_**Note:**
 Please be aware this field will be discarded if the corresponding flags have been
 set._
 - `styles`: The styles file to use.<br>_**Note:** Please be aware this field will
 be discarded if the corresponding flags have been set._
 - `clefs`: The clefs file to use.<br>_**Note:** Please be aware this field will be
 discarded if the corresponding flags have been set._
 - `generation`: This field contains all detailed information about which algorithms to use
 in generating the specific music itself. It describes the rules to follow in order to obtain
 the same result. (See the Algorithms page for details on individual algorithms.)
 It consists of:
    - `pitch`: The algorithm to use when creating the pitches. It can be one of the following:
       - `random-piano`: To generate completely random pitches, playable by a keyboard/piano.
       - `contain-stave`: To generate random pitches, contained within the stave.
       - `brownian-motion`: An implementation of the Brownian Motion algorithm that is
       contained within the stave. Options for this algorithm are `pitch.min` and `pitch.max`,
       representing the ranges of the jumps. 
       - `1/f-noise`: An implementation of the "white noise" algorithm (based upon the
       one invented by Richard F. Voss), using 3 dice and respecting the stave.
       - `centralized`: Using the gaussian distribution, mapped on the stave range of
       the current stave, generate notes whose chance of being chosen corresponds with
       its value on the gauss-curve.
       - `accompaniment`: Uses a very simple algorithm to generate good-sounding
       music, based upon the `options.pitch.schematic` schema and the `style.chord-progression`
       values. The schematic can be created/read as follows (it's based upon [this site](https://chordchord.com/)).
       <br>It's a string of a power-2 length (e.g. `1`, `2`, `4`, `8`...) containing a
       sequence of `A`s, `B`s and/or `C`s. An `A` means the bottom note needs to be
       used, a `B` says it'll use the chord's middle and `C` uses the top note.
       The `B` takes minor chords into a count, meaning that any chord-progression
       may contain a minor chord (e.g. `Em`).<br>_**Note:** This algorithm actually
       does not use any randomization._
       - Anything else will generate completely random pitches within the entire music range.
    - `rhythm`: The algorithm to use for rhythms. The `options.rhythm.smallest` and 
    `options.rhythm.largest` options allow the user to specify the minimal and maximal
    duration of a note. This is respected by all algorithm.<br>Possible algorithms are:
       - `random`: A random rhythm out of the possible range.
       - `brownian-motion`: Implementation of the Brownian motion as a rhythmic algorithm.
       - Anything else will create a constant tempo of `options.rhythm.duration` notes
       (representations according to the MusicXML note types, or the corresponding fractions
       when less than one). If this option cannot be found, `quarter` will be used instead.
    - `chord`: This is the algorithm that is being used in deciding how many notes to
    play in a chord at a given time. The possible values are:
       - `random`: Choose a random value in the inclusive range from `options.chord.min`
       to `options.chord.max`.
       - `weighted`: Give each value a certain percentage of occurring (by setting the
       valid elements in `options.chord`). For instance:
       ```
       "chord": {
           "2": 0.2,
           "3": 0.1
       }
       ```
       This can be read as: a two-note chord has a 20% chance of occurring, whilst a 3-
       note chord has a 10% chance to appear. The missing 70% is appointed towards single
       notes.<br>It is possible to set the value for a single note (key is `"1"`), but if the
       percentages don't add up to 1, this value will be changed and a warning
       will be shown.<br>If the sum of all these chances is greater than 1, a warning will
       also be issued and all values will be normalized (divided by the sum).<br>
       Missing values in this list will be mapped to `0`, except for a single note, as
       mentioned above.
       - All other values are mapped to generate chords of `options.chord.amount` notes.
    - `options`: An object, representing the additional options of the above-mentioned
    algorithms.
    - `rest-ratio`: A floating point number, representing the approximated percentage of
      notes that should be turned into rests.<br>_(**Note:** This percentage is merely an 
      approximation and thus is not a precise representation of the exact percentage.)_
 - `export`: A small sub-tree, containing some values for exporting to MusicXML. When using
 the string `@VERSION@` in the elements of this sub-tree, the current version of autoplay
 will be used.
    - `filename`: The filename to export to.
    - `title`: The title of the piece. Not required.
    - `composer`: The composer of the piece.
    - `rights`: Additional copyright information for the piece.
 - `style`: Either a style object, or a string representing a style. See the _Styles_ page
 for more info on these objects.
 - `parts`: A list of different parts, where each element contains the following sub-tree
 (when using an `XML`-file, each part must be inside a `part` element):
    - `instrument`: A string, containing the MIDI name of an instrument.
    - `instruments`: When the `instrument` element does not exist, it is required that this
    element does exist. It represents another list of instrument object (like this one),
    but with the difference that all elements are rhythmic. This allows for displaying a
    set of rhythmic instruments that belong together on a corresponding stave. E.g. a
    drum set is usually represented on a single stave, where each note has a different
    meaning.
    - `clef`: The name of a clef as a string, or a clef object (see _Clef_ page).
    - `generation`: This is the same element as the above-mentioned `generation` element, which
    allows for each stave to have an explicit algorithm set. If this element does not exist,
    the above-set `generation` element will be used.
    - `lines`: When exporting the score to `MusicXML`-format, this variable tells how many
    lines are needed for the instrument. Defaults to `5`, which is the normal value.
    - `display`: For rhythmic instruments, this field tells the software where to display
    the notes for this instrument. It is usually combined with the `instruments` field.
    - `symbol`: Rhythmic instruments can have a series of possible representations. For
    user friendlyness and modularity, that is what this field does. Possible values are:
    `slash`, `triangle`, `diamond`, `square`, `cross`, `x`, `circle-x`, `normal`, `cluster`,
    `inverted triangle`, `arrow down`, `arrow up`, `slashed`, `back slashed`, `do`, `re`,
     `mi`, `fa`, `so`, `la`, `ti`, `none`, `normal-empty`, `diamond-empty`, `triangle-empty`
     and `square-empty`
     - `name`: The name for the instrument can be set explicitly with this field.
 
For more info, please take a look at the `main/config/default.json` config file.

_**Note:** This file will be read and used as (default) input (config) for the program on
launch, so any changes to this file will change your default values._
