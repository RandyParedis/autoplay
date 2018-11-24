//
// Created by red on 07/10/18.
//

#ifndef AUTOPLAY_NOTE_H
#define AUTOPLAY_NOTE_H

#include <cassert>
#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "Instrument.h"

namespace autoplay {
    namespace music {
        /**
         * The Note class is a class that holds all information on any note that's
         * played.
         */
        class Note : public std::enable_shared_from_this<Note>
        {
        public:
            /**
             * The Semitone enum is used to pinpoint a specific pitch shift for a named
             * Note
             */
            enum class Semitone { NONE, FLAT, SHARP };

            static const uint8_t DEFAULT_ON;  ///< Default Note 'on' velocity
            static const uint8_t DEFAULT_OFF; ///< Default Note 'off' velocity

            static std::map<std::string, float> DURATION; ///< Duration map

        public:
            /**
             * Constructor for creating a pause.
             * @param duration How long the pause is held.
             */
            explicit Note(unsigned int duration) : Note(0, 0, 0, duration) { this->m_pause = true; }

            /**
             * Constructor for playing a Note at default strike velocity.
             * @param pitch     The pitch of the Note.
             * @param duration  How long the Note must be held.
             *
             * @note The default strike velocity is taken from the examples of RtMidi.
             *       A 'note on' will be struck with velocity of 90, whereas a 'note off'
             *       will have release velocity of 40.
             */
            Note(uint8_t pitch, unsigned int duration) : Note(pitch, DEFAULT_ON, DEFAULT_OFF, duration) {}

            /**
             * Constructor for playing a Note at constant strike velocity.
             * @param pitch     The pitch of the Note.
             * @param velocity  How forcefully the Note must be played and released.
             * @param duration  How long the Note must be held.
             */
            Note(uint8_t pitch, uint8_t velocity, unsigned int duration) : Note(pitch, velocity, velocity, duration) {}

            /**
             * Constructor for playing a Note at constant strike velocity.
             * @param pitch         The pitch of the Note.
             * @param velocity_on   How forcefully the Note must be played.
             * @param velocity_off  How forcefully the Note must be released.
             * @param duration      How long the Note must be held.
             */
            Note(uint8_t pitch, uint8_t velocity_on, uint8_t velocity_off, unsigned int duration)
                : m_pitch(pitch), m_velocity_on(velocity_on), m_velocity_off(velocity_off), m_duration(duration),
                  m_pause(false), m_tie_start(false), m_tie_end(false), m_dots(0), m_head(""), m_inst() {
                assert(pitch <= 127);
                assert(velocity_on <= 127);
                assert(velocity_off <= 127);
            }

            /**
             * Copy constructor
             * @param n The Note to copy
             */
            Note(const Note& n) {
                m_pitch        = n.m_pitch;
                m_velocity_on  = n.m_velocity_on;
                m_velocity_off = n.m_velocity_off;
                m_duration     = n.m_duration;
                m_pause        = n.m_pause;
                m_tie_start    = n.m_tie_start;
                m_tie_end      = n.m_tie_end;
                m_dots         = n.m_dots;
                m_head         = n.m_head;
                m_inst         = n.m_inst;
            }

            /**
             * Ties a Note to a following Note
             * @param enable    If this must be enabled.
             */
            inline void setTieStart(bool enable = true) { m_tie_start = enable; }

            /**
             * Ties a Note to a preceding Note
             * @param enable    If this must be enabled.
             */
            inline void setTieEnd(bool enable = true) { m_tie_end = enable; }

            /**
             * Get if the current Note is linked to a following Note
             * @return true if so
             */
            inline bool getTieStart() const { return m_tie_start; }

            /**
             * Get if the current Note is linked to a previous Note
             * @return true if so
             */
            inline bool getTieEnd() const { return m_tie_end; }

            /**
             * Sets the amount of dots to use
             * @param dots New amount of dots.
             */
            inline void setDots(const uint8_t& dots) { m_dots = dots; }

            /**
             * Fetches the number of dots of the Note
             * @return the number of dots
             */
            inline uint8_t getDots() const { return m_dots; }

            /**
             * Returns the duration of the Note.
             * @return The duration of the Note.
             */
            inline unsigned int getDuration() const { return m_duration; }

            /**
             * Sets a new duration for the Note
             * @param d The new duration
             */
            inline void setDuration(const unsigned int& d) { m_duration = d; }

            /**
             * Returns the pitch of the Note.
             * @return The pitch of the Note.
             */
            inline uint8_t getPitch() const { return m_pitch; }

            /**
             * Sets the new pitch for the Note
             * @param p the new pitch
             */
            inline void setPitch(const uint8_t& p) {
                assert(p <= 127);
                m_pitch = p;
            }

            /**
             * Returns the 'on' velocity of the Note.
             * @return The 'on' velocity of the Note.
             */
            inline uint8_t getVelocityOn() const { return m_velocity_on; }

            /**
             * Set the new strike velocity of the Note
             * @param v the new strike velocity
             */
            inline void setVelocityOn(const uint8_t& v) {
                assert(v <= 127);
                m_velocity_on = v;
            }

            /**
             * Returns the 'off' velocity of the Note.
             * @return The 'off' velocity of the Note.
             */
            inline uint8_t getVelocityOff() const { return m_velocity_off; }

            /**
             * Set the new release velocity of the Note
             * @param v the new release velocity
             */
            inline void setVelocityOff(const uint8_t& v) {
                assert(v <= 127);
                m_velocity_off = v;
            }

            /**
             * Checks whether the Note is a pause.
             * @return True if it's a pause.
             */
            inline bool isPause() const { return m_pause; };

            /**
             * Checks whether the Note is not a pause.
             * @return True if it isn't a pause.
             */
            inline bool isNote() const { return !m_pause; };

            /**
             * Turns the current Note into a rest/pause
             */
            inline void toPause() { m_pause = true; }

            /**
             * Returns the MIDI message of the Note.
             * @param channel   The channel on which the Note is played. This is in the
             * range of [0, 15]
             * @param note_on   When true, the 'note on' message will be returned.
             * @return The MIDI message of the Note.
             */
            std::vector<unsigned char> getMessage(uint8_t channel = 0, bool note_on = true) const;

            /**
             * Returns the MIDI message of the Note, in its 'note on' event.
             * @param channel   The channel on which the Note is played. This is in the
             * range of [0, 15]
             * @return The MIDI message of the Note.
             */
            std::vector<unsigned char> getOnMessage(uint8_t channel = 0) const;

            /**
             * Returns the MIDI message of the Note, in its 'note off' event.
             * @param channel   The channel on which the Note is played. This is in the
             * range of [0, 15]
             * @return The MIDI message of the Note.
             */
            std::vector<unsigned char> getOffMessage(uint8_t channel = 0) const;

            /**
             * Get the specific pitch value, given a name and an optional Semitone.
             * @param name  The string value of a pitch, e.g. A4 (440 Hz; pitch = 69)
             * @param s     The optional Semitone associated with the name.
             * @return A MIDI representation of the given pitched name.
             */
            static uint8_t pitch(const std::string& name, const Semitone& s);

            /**
             * Get the specific pitch value, given a name and an optional Semitone.
             * @param name  The string value of a pitch, e.g. A4 (440 Hz; pitch = 69)
             *
             * @note You can use 'b' for flats and '#' for sharps,
             *       e.g. A#4 means A sharp on fourth octave (pitch = 70)
             *
             * @return A MIDI representation of the given pitched name.
             */
            static uint8_t pitch(const std::string& name);

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

            /**
             * Given a pitch value, get the representation of the pitch
             * @param p     The pitch value
             * @param alter The alteration of the representation.
             * @return The string representation of the value
             */
            static std::string pitchRepr(const uint8_t& p, Semitone& alter);

            /**
             * Given a pitch value, get the representation of the pitch
             * @param p     The pitch value
             * @return The string representation of the value
             */
            static std::string pitchRepr(const uint8_t& p);

            /**
             * Split a pitch representation into the step and the octave
             * @param pitch The pitch value
             * @return  A pair in the form of <step, octave> of the given pitch
             */
            static std::pair<char, int> splitPitch(const std::string& pitch, Semitone& alter);

        public:
            /**
             * Set the head of the Note (for representation)
             * @param head The head name to set
             */
            inline void setHead(const std::string& head) { m_head = head; }

            /**
             * Fetches the name of the head.
             * @return The name of the head.
             */
            std::string getHeadName() const;

            /**
             * Checks if the current notehead can be filled or not
             * @return true if it can be filled.
             */
            bool canBeFilled() const;

            /**
             * Fetches if the head is filled or not
             * @return true if it must be filled
             */
            bool getHeadFilled() const;

            /**
             * Set the Note's Instrument
             * @param inst The Instrument of this Note
             */
            inline void setInstrument(const std::shared_ptr<Instrument>& inst) { m_inst = inst; }

            /**
             * Fetches the Instrument of this Note
             * @return A shared pointer to this Instrument
             */
            inline std::shared_ptr<Instrument> getInstrument() const { return m_inst; }

        private:
            uint8_t      m_pitch;        ///< The note that is played, in the range of [0, 127]
            uint8_t      m_velocity_on;  ///< The speed at which the Note must be struck.
            uint8_t      m_velocity_off; ///< The release speed of the Note.
            unsigned int m_duration;     ///< How long it takes for the Note to be played.

            bool m_pause; ///< Whether the current Note is in fact a pause.

            bool    m_tie_start; ///< Whether the current Note is tied to a sequential Note
            bool    m_tie_end;   ///< Whether the current Note is tied to a preceding Note
            uint8_t m_dots;      ///< The amount of dots of the Note

            std::string                 m_head; ///< The Note head to display in MusicXML
            std::shared_ptr<Instrument> m_inst; ///< The instrument that is used to play the current Note

        public:
            /**
             * Checks if two Notes are equal on all fronts
             * @param rhs The other Note to check
             * @return true if they are equal
             */
            bool operator==(const Note& rhs) const;

            /**
             * Checks if two Notes differ from one another
             * @param rhs The other Note to check
             * @return true if there is at least one difference between the Notes.
             */
            bool operator!=(const Note& rhs) const;

            /**
             * Checks if a Note is larger than or equal to this Note.
             * This means more specifically: if the pitch, duration, velocity_on or
             * velocity_off
             * of this Note is smaller than or equal to that of the rhs, whilst all other
             * fields are equal.
             * @param rhs The other Note to check
             * @return true if there is a field that is smaller than or equal to the field
             * of rhs
             */
            bool operator<=(const Note& rhs) const;

            /**
             * Checks if a Note is smaller than or equal to this Note.
             * This means more specifically: if the pitch, duration, velocity_on or
             * velocity_off
             * of this Note is larger than or equal to that of the rhs, whilst all other
             * fields are equal.
             * @param rhs The other Note to check
             * @return true if there is a field that is larger than or equal to the field
             * of rhs
             */
            bool operator>=(const Note& rhs) const;

            /**
             * Checks if a Note is larger than this Note.
             * This means more specifically: if the pitch, duration, velocity_on or
             * velocity_off
             * of this Note is smaller than that of the rhs, whilst all other fields are
             * equal.
             * @param rhs The other Note to check
             * @return true if there is a field that is smaller than the field of rhs
             */
            bool operator<(const Note& rhs) const;

            /**
             * Checks if a Note is smaller than this Note.
             * This means more specifically: if the pitch, duration, velocity_on or
             * velocity_off
             * of this Note is larger than that of the rhs, whilst all other fields are
             * equal.
             * @param rhs The other Note to check
             * @return true if there is a field that is larger than the field of rhs
             */
            bool operator>(const Note& rhs) const;

        public:
            std::string getType(const int& divisions) const;
        };

        /**
         * The Chord class is a class used to define a group of Notes that are played similtaneously.
         */
        class Chord
        {
        private:
            std::vector<std::shared_ptr<Note>> m_notes;

        public:
            explicit Chord(const Note& note) : m_notes() { m_notes.emplace_back(std::make_shared<Note>(note)); }

            Chord(const Chord& rhs) {
                for(const std::shared_ptr<Note>& note : rhs.m_notes) {
                    Note n = *note.get();
                    m_notes.emplace_back(std::make_shared<Note>(n));
                }
            }

            inline std::vector<std::shared_ptr<Note>> getNotes() const { return m_notes; }

            /**
             * Sets the Chord to have a specific duration.
             * @param d The new duration of all Notes in the Chord
             */
            inline void setDuration(const unsigned int& d) {
                for(const std::shared_ptr<Note>& note : m_notes) {
                    note->setDuration(d);
                }
            }

            /**
             * Ties a Note to a following Note
             * @param enable    If this must be enabled.
             */
            inline void setTieStart(bool enable = true) {
                for(const std::shared_ptr<Note>& note : m_notes) {
                    note->setTieStart(enable);
                }
            }

            /**
             * Ties a Note to a preceding Note
             * @param enable    If this must be enabled.
             */
            inline void setTieEnd(bool enable = true) {
                for(const std::shared_ptr<Note>& note : m_notes) {
                    note->setTieEnd(enable);
                }
            }

            /**
             * Get if the current Chord has a Note that is tied to a following Note
             * @return true if so
             */
            inline bool getTieStart() const { return m_notes.begin()->get()->getTieStart(); }

            /**
             * Get if the current Chord has a Note that is tied to a preceding Note
             * @return true if so
             */
            inline bool getTieEnd() const { return m_notes.begin()->get()->getTieEnd(); }

            /**
             * Returns the duration of the Chord.
             * @return The duration of the Chord.
             */
            inline unsigned int getDuration() const { return m_notes.begin()->get()->getDuration(); }

            /**
             * Sets the amount of dots to use
             * @param dots New amount of dots.
             */
            inline void setDots(const uint8_t& dots) {
                for(const std::shared_ptr<Note>& note : m_notes) {
                    note->setDots(dots);
                }
            }

            /**
             * Checks if the Chord is empty
             * @return true if it is empty
             */
            inline bool empty() const { return m_notes.empty(); }

            /**
             * Changes the Chord to a Pause
             */
            inline void toPause() { m_notes.begin()->get()->toPause(); }

            /**
             * Checks if the current Chord is a Pause
             * @return true if so
             */
            inline bool isPause() const { return m_notes.begin()->get()->isPause(); }

            /**
             * Finds the lowest Note of the Chord
             * @return a shared pointer, pointing to the bottom note (root Note) of the Chord
             */
            std::shared_ptr<Note> bottom() const;

            /**
             * Split the Chord in a series of linked notes, w.r.t. the divisions
             * @param divisions     The amount of divisions to take into a count.
             * @param generatedots  When true, dots must be used whenever possible.
             * @return A vector of linked Notes
             */
            std::vector<Chord> splitByDivisions(const int& divisions, bool generatedots) const;
        };
    }
}

#endif // AUTOPLAY_NOTE_H
