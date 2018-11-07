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
            std::function<uint8_t(RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc, pt::ptree pt)>
            getPitchAlgorithm(std::string algo = "") const;

        private:
            std::vector<uint8_t> getPitches(uint8_t min, uint8_t max, int stave) const;

        private:
            Config   m_config;   ///< The Config of the system
            RNEngine m_rnengine; ///< The Random Engine
        };
    }
}

#endif // AUTOPLAY_GENERATOR_H
