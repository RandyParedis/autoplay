# Clefs
In melodic music, we only know 3 clefs (`G`, `F` and `C`). These three are used in a lot of
different ways and can be combined in many fashions. This is why the user of `autoplay` is
able to set a custom clef.

## Clef Object
The clef object is merely described by three attributes:
- `sign`: Either `G`, `F` or `C`
- `line`: Each clef is anchored at a different line, enabling the user to shift this clef
up and down through the octave itself.
- `octave-change`: The amount with which the octave can be changed. (allows for _8va_ and
_8va Bassa_). This value is in intervals of octaves, meaning a normal _8va_ is obtained
by setting this value to `1`.