//
// Created by red on 28/10/18.
//

#include "Generator.h"
#include "Randomizer.h"

Generator::Generator(const Config &config): m_config(config) {
    // Set engine
    auto engine_name = m_config.conf<std::string>("engine");
    auto seed = m_config.conf<unsigned long>("seed");
    m_rnengine(engine_name, seed);
}

music::Score Generator::generate() {
    // Setup default values
    // TODO: Randomize these
    unsigned int length = 10;       // Total amount of measures
    unsigned int part_count = 1;    // Number of parts
    uint8_t divisions = 24;         // Amount of 'ticks' each quarter note takes
    std::pair<uint8_t, uint8_t> time = {4, 4};

    // Get Algorithm
    auto pitch_algo = getPitchAlgorithm();

    music::Score score;
    for(unsigned int i = 0; i < part_count; ++i) {
        // Generate Notes
        music::Measure measure{ music::Clef::Treble(), time, divisions };
        auto instrument = music::instruments::acoustic_grand_piano;
        std::shared_ptr<music::Part> part = std::make_shared<music::Part>(instrument);
        for(unsigned int j = 0; j < length*measure.max_length();) {
            music::Note* prev = nullptr;
            if(j > 0) {
                music::Note n = measure.back();
                prev = &n;
            }
            std::vector<music::Note*> conc = {};
            for(unsigned int p = 0; p < i; ++p) {
                music::Note* n = score.parts.at(p)->at(j);
                if(n) {
                    conc.emplace_back(n);
                }
            }
            uint8_t pitch = pitch_algo(m_rnengine, prev, conc);
            uint8_t duration = 24;
            music::Note note{pitch, duration};
            measure.append(note);
            j += duration;
        }
        part->setMeasures(measure);
        score.parts.emplace_back(part);
    }

    return score;
}

std::function<uint8_t(RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc)>
        Generator::getPitchAlgorithm() const {
    // Get algorithm variables
    auto algorithm = m_config.conf<std::string>("generation.pitch");

    if(algorithm == "random-piano") {
        return [](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc)-> uint8_t {
            return (uint8_t)Randomizer::pick_uniform(gen, 21, 108);
        };
    } else {
        return [](RNEngine& gen, music::Note* prev, std::vector<music::Note*>& conc)-> uint8_t {
            return (uint8_t)Randomizer::pick_uniform(gen, 0, 127);
        };
    }
}