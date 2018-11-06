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
| `-c`, `--config` | A configuration file to load. (Currently, only JSON is supported)

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
 - `generation`: This field contains all detailed information about which algorithms to use
 in generating the specific music itself. It describes the rules to follow in order to obtain
 the same result. It consists of:
    - `pitch`: The algorithm to use when creating the pitches. It can be one of the following:
       - `random-piano`: To generate completely random pitches, playable by a keyboard/piano.
       - `contain-stave`: To generate random pitches, contained within the stave.
       - Anything else will generate completely random pitches within the entire music range.
 - `export`: A small sub-tree, containing some values for exporting to MusicXML. When using
 the string `@VERSION@` in the elements of this sub-tree, the current version of autoplay
 will be used.
    - `filename`: The filename to export to.
    - `title`: The title of the piece. Not required.
    - `composer`: The composer of the piece.
    - `rights`: Additional copyright information for the piece.
 - `style`: Either a style object, or a string representing a style. See the _Styles_ page
 for more info.
 - `parts`: A list of different parts, where each element contains the following sub-tree
 (when using an `XML`-file, each part must be inside a `part` element):
    - `instrument`: A string, containing the MIDI name of an instrument.
    - `clef`: The name of a clef as a string, or a clef object (see _Clef_ page).
    - `generation`: This is the same element as the above-mentioned `generation` element, which
    allows for each stave to have an explicit algorithm set. If this element does not exist,
    the above-set `generation` element will be used.
 
For more info, please take a look at the `main/config/default.json` config file.
_**Note:** This file will be read and used as (default) input (config) for the program on
launch, so any changes to this file will change your default values._
