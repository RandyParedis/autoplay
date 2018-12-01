# Styles
All music can be defined by a certain _style_. Although this _style_ might be subject to
objectivity, it is good to figure out a few predefined rules that make up a general idea of a
style.

It is possible for the user to create, adapt and expand the existing styles by setting some
very simple variables in the `main/config/styles.json` file. To keep this documentation
compact and informational, I refer to this file for more details on the specific values for
a certain style. See below for a list of possible styles.

## Styles Object
In the configuration file, it is possible to describe a certain `style`. This can be done by
either setting it as a string (see below), or by setting it as an object. The same tree
structure can be used in both the `configuration` and the `style` files, but for the latter,
it must be contained within the `styles`-object and must have the style name as key.
It has the following fields:
- `scale`: Either a string, describing one of the types listed below, or a sequence of 12
boolean values, representing the pitches on a `chromatic` tone scale.
- `root`: Any note (without its octave number) that has a certain `chance` of being the last 
note in a given part. The `scale` will take this value as the representation of the first
boolean value in the corresponding sequence. It basically allows you to say `C major` (`scale`
= `diatonic.major`, `root`=`C`)
- `chance`: A value between 0 and 1 that corresponds to a certain chance that any given part
will change its last note to the `root` note. Usually, this value is rather large.
- `fifths`: This represents the key signature of the piece. When `0`, no special signature is
used. If this value is strictly positive, the first `n` values (`n` is the value to which
`fifths` is set) of the below-mentioned `sharp list` are used (as sharps) for the signature.
When strictly negative, we take a look at the first `abs(n)` values (`abs(n)` is the absolute
value of the `fifths` value) of the `flat list`.
  - `sharp list`: `F`, `C`, `G`, `D`, `A`, `E`, `B`
  - `flat list`: `B`, `E`, `A`, `D`, `G`, `C`, `F`
- `bpm`: The amount of beats per minute of the score. This determines the tempo for the
defined beat-type.
- `time`: The time signature of the score. Contains two fields:
  - `beats`: The amount of beats per measure.
  - `type`: The beat-type of the score.
- `chord-progression`: A string, containing the chord progression per measure, separated
by a `-` sign.
- `from`: This helper field allows a user to simply identify with which style to merge. 

None of the above fields are required. If they are not set, the value of the default style
is used.

## Style Types
As said above, there is a possibility to set a certain `scale` to a string, which can be one
of:
- `monotonic`: All notes are the `root`, but the octave may differ (modern music).
- `chromatic` or `dodecatonic`: All 12 notes/pitches of a certain octave are used.
- `diatonic.major`: The general `major` scale.
- `diatonic.minor`: The general `minor` scale.
- `whole-tone` or `hexatonic`: There    - Add `chord-progression` option, containing a sequence of chords
    (as list of chords, joined to a string, using the `-`) are only whole tones in between each step in the scale.
- `heptatonic.melodic`: The melodic minor scale.
- `heptatonic.harmonic`: The harmonic minor scale, commonly used from the 17th century to the
20th century.
- `heptatonic.aeolian`: Natural minor scale.
- `heptatonic.locrian`: 1st permutation of the Aeolian mode. 
- `heptatonic.ionian`: Natural major scale.
- `heptatonic.dorian`: 3th permutation of the Aeolian mode.
- `heptatonic.phrygian`: 4th permutation of the Aeolian mode.
- `heptatonic.lydian`: 5th permutation of the Aeolian mode.
- `heptatonic.mixolydian`: 6th permutation of the Aeolian mode.
- `octatonic`, `diminished` or `korsakov`: 8-note symmetric scale, usually used in jazz
music.

## Possible Styles
Here, all possible strings for the `style`-value in the config file(s) are listed. It's
important to note that these strings correspond to the exact key names of the `styles`-object,
described above.
- `default`: A `chromatic` tone scale, with its `root` being the `C`; and both `chance` and
`fifths` are `0`.
- `white-keys`: Only use the white keys of a piano.
- `black-keys`: Only use the black keys of a piano, which is common practice in Oriental
music.
- `<X>-major`: Any major key, where `<X>` can be one of the following: `C`, `F`, `B`, `Bb`,
`Eb`, `Ab`, `Db`, `Gb`, `Cb`, `G`, `D`, `A`, `E`, `B`, `F#`, `C#`
- `<X>-minor`: Any minor key, where `<X>` can be one of the following: `A`, `D`, `G`, `C`,
`F`, `Bb`, `Eb`, `Ab`, `E`, `B`, `F#`, `C#`, `G#`, `D#`, `A#`
