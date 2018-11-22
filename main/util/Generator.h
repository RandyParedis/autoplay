//
// Created by red on 28/10/18.
//

#ifndef AUTOPLAY_GENERATOR_H
#define AUTOPLAY_GENERATOR_H

#include "../music/Score.h"
#include "Config.h"
#include "Randomizer.h"

namespace autoplay {
    namespace util {
        /**
         * The Generator class is the class that handles all generation of random music.
         */
        class Generator
        {
        public:
            /**
             * Default Constructor
             * @param config The Config that has been initialized with the system
             */
            explicit Generator(const Config& config);

            /**
             * Generates a random Score
             * @return The randomized score
             */
            music::Score generate();

        public:
            /**
             * Get the randomization algorithm for the pitch
             * @param algo  If not empty, it will use this algorithm to check, instead of the generation.pitch value
             * @return A lambda function that implements the algorithm
             */
            std::function<uint8_t(RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc, pt::ptree& pt)>
            getPitchAlgorithm(std::string algo = "") const;

            /**
             * Get the randomization algorithm for the rhythm
             * @param algo  If not empty, it will use this algorithm to check, instead of the generation.rhythm value
             * @return A lambda function that implements the algorithm
             */
            std::function<float(RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc, pt::ptree& pt)>
            getRhythmAlgorithm(std::string algo = "") const;

        private:
            /**
             * Get all the possible pitches within a range, according to the given scale.
             * @param min   The lowest possible pitch to find. When it is not part of the scale, it will take the first
             *              pitch that follows this value.
             * @param max   The highest possible pitch to find.
             * @param stave The stave to generate the pitches for (has to do with multiple instruments on same stave)
             * @return A vector, containing all the pitches.
             */
            std::vector<uint8_t> getPitches(uint8_t min, uint8_t max, int stave) const;

            /**
             * Private helper function, used to obtain the range of a specific stave.
             * @param stave     The stave to find the range for.
             * @return A range object (as a pair <min, max> )
             */
            std::pair<uint8_t, uint8_t> staveRange(int stave) const;

            /**
             * A pitch generation algorithm, based upon the movements of small particles that are randomly bombarded by
             * molecules of the surrounding medium.
             * @param gen   The generator object.
             * @param prev  The previous note that is played
             * @param conc  A vector of all concurrent notes that are being played
             * @param pt    A ptree with additional options. This algorithm expects pitch.min and pitch.max to be set
             *              in this ptree. When missing, -3 and 3 will be chosen respectively. 'stave' is the stave
             * index.
             * @return A new pitch.
             */
            uint8_t pitchBrownianMotion(RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc,
                                        const pt::ptree& pt) const;

            /**
             * A pitch generation algorithm, based upon the 1/f relationship, occuring in the strangest places of
             * nature.
             * @param gen       The generator object.
             * @param pt        A ptree with additional options. 'stave' is the stave index. "_p1fn._reinit" must be
             *                  set to (re)initialize the algorithm.
             * @return A new pitch.
             */
            uint8_t pitch1FNoise(RNEngine& gen, const pt::ptree& pt) const;

            /**
             * A rhythm generation algorithm, based upon the movements of small particles that are randomly bombarded by
             * molecules of the surrounding medium.
             * @param gen   The generator object.
             * @param prev  The previous note that is played
             * @param conc  A vector of all concurrent notes that are being played
             * @param pt    A ptree with additional options. This algorithm expects rhythm.min and rhythm.max to be set
             *              in this ptree. When missing, -3 and 3 will be chosen respectively.
             * index.
             * @return A new rhythm.
             */
            float rhythmBrownianMotion(RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc,
                                       const pt::ptree& pt) const;

        private:
            Config   m_config;   ///< The Config of the system
            RNEngine m_rnengine; ///< The Random Engine
        };
    }
}

#endif // AUTOPLAY_GENERATOR_H
