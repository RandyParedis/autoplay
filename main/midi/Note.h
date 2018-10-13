//
// Created by red on 07/10/18.
//

#ifndef AUTOPLAY_NOTE_H
#define AUTOPLAY_NOTE_H

#include <cstdint>
#include <map>
#include <string>
#include <cassert>
#include <vector>

namespace midi {
    /**
     * The Note class is a class that holds all information on any note that's played.
     */
    class Note {
    public:
        /**
         * The Semitone enum is used to pinpoint a specific pitch shift for a named Note
         */
        enum class Semitone {
            NONE, FLAT, SHARP
        };

    public:
        /**
         * Constructor for creating a pause.
         * @param duration How long the pause is held.
         */
        explicit Note(unsigned int duration): Note(0, 0, 0, duration) { this-> m_pause = true; }

        /**
         * Constructor for playing a Note at default strike velocity.
         * @param pitch     The pitch of the Note.
         * @param duration  How long the Note must be held.
         *
         * @note The default strike velocity is taken from the examples of RtMidi.
         *       A 'note on' will be struck with velocity of 90, whereas a 'note off'
         *       will have release velocity of 40.
         */
        Note(uint8_t pitch, unsigned int duration):
                Note(pitch, 90, 40, duration) {}

        /**
         * Constructor for playing a Note at constant strike velocity.
         * @param pitch     The pitch of the Note.
         * @param velocity  How forcefully the Note must be played and released.
         * @param duration  How long the Note must be held.
         */
        Note(uint8_t pitch, uint8_t velocity, unsigned int duration):
                Note(pitch, velocity, velocity, duration) {}

        /**
         * Constructor for playing a Note at constant strike velocity.
         * @param pitch         The pitch of the Note.
         * @param velocity_on   How forcefully the Note must be played.
         * @param velocity_off  How forcefully the Note must be released.
         * @param duration      How long the Note must be held.
         */
        Note(uint8_t pitch, uint8_t velocity_on, uint8_t velocity_off, unsigned int duration):
                m_pitch(pitch), m_velocity_on(velocity_on), m_velocity_off(velocity_off), m_duration(duration),
                m_pause(false) {
            assert(pitch <= 127);
        }

        /**
         * Checks whether the Note is a pause.
         * @return True if it's a pause.
         */
        inline bool isPause() { return m_pause; };

        /**
         * Checks whether the Note is not a pause.
         * @return True if it isn't a pause.
         */
        inline bool isNote() { return !m_pause; };

        /**
         * Returns the MIDI message of the Note.
         * @param channel   The channel on which the Note is played. This is in the range of [0, 15]
         * @param note_on   When true, the 'note on' message will be returned.
         * @return The MIDI message of the Note.
         */
        std::vector<unsigned char> getMessage(uint8_t channel = 0, bool note_on = true);

        /**
         * Returns the MIDI message of the Note, in its 'note on' event.
         * @param channel   The channel on which the Note is played. This is in the range of [0, 15]
         * @return The MIDI message of the Note.
         */
        std::vector<unsigned char> getOnMessage(uint8_t channel = 0);

        /**
         * Returns the MIDI message of the Note, in its 'note off' event.
         * @param channel   The channel on which the Note is played. This is in the range of [0, 15]
         * @return The MIDI message of the Note.
         */
        std::vector<unsigned char> getOffMessage(uint8_t channel = 0);

        /**
         * Get the specific pitch value, given a name and an optional Semitone.
         * @param name  The string value of a pitch, e.g. A4 (440 Hz; pitch = 69)
         * @param s     The optional Semitone associated with the name.
         * @return A MIDI representation of the given pitched name.
         */
        static uint8_t pitch(const std::string& name, Semitone s = Semitone::NONE);

        /**
         * Get the specific pitch value, given a frequency. When no pitch is found,
         * the closest pitch value is returned.
         *
         * A derivation of a very basic formula is used:
         *          fn = f0 * a^n
         *      where:
         *          f0 = frequency of a fixed note
         *          n  = number of half steps away from fixed note
         *          fn = frequency of note n steps away (here: hertz parameter)
         *          a  = 2^(1/12) = 1.05946309...
         *      implying:
         *          n  = round(log(fn/f0)/log(a))
         *
         * For generality, the fixed frequency that's used is:
         *          A4; 440 Hz; pitch 69
         *
         * @param hertz     The frequency of the pitch.
         * @return A MIDI representation of the given pitched frequency.
         */
        static uint8_t pitch(float hertz);

    private:
        uint8_t m_pitch;          ///< The note that is played, in the range of [0, 127]
        uint8_t m_velocity_on;    ///< The speed at which the Note must be struck.
        uint8_t m_velocity_off;   ///< The release speed of the Note.
        unsigned int m_duration;  ///< How long it takes for the Note to be played.

        bool m_pause;             ///< Whether the current Note is in fact a pause.
    };
}


#endif //AUTOPLAY_NOTE_H
