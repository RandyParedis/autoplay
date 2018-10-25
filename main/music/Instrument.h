//
// Created by red on 25/10/18.
//

#ifndef AUTOPLAY_INSTRUMENT_H
#define AUTOPLAY_INSTRUMENT_H

#include <string>

namespace music {
    class Instrument {
    public:
        Instrument(const std::string& name, uint8_t channel, uint8_t program, uint8_t unpitched) :
                m_name(name), m_channel(channel), m_program(program), m_unpitched(unpitched) {}

        inline std::string getName() const { return m_name; }
        inline uint8_t getChannel() const { return m_channel; }
        inline uint8_t getProgram() const { return m_program; }
        inline uint8_t getUnpitched() const { return m_unpitched; }

        inline void setName(const std::string& name) { m_name = name; }
        inline void setChannel(const uint8_t& channel) { m_channel = channel; }
        inline void setProgram(const uint8_t& program) { m_program = program; }
        inline void setUnpitched(const uint8_t& unpitched) { m_unpitched = unpitched; }

    private:
        std::string m_name;
        uint8_t m_channel;
        uint8_t m_program;
        uint8_t m_unpitched; ///< The tone that must be played. In the MusicXML, we start counting from 1!
    };

    namespace instruments {
        /// Instruments

        // Pianos
        Instrument acoustic_grand_piano{"Acoustic Grand Piano", 1, 1, 1};
        Instrument bright_acoustic_piano{"Bright Acoustic Piano", 1, 1, 2};
        Instrument electric_grand_piano{"Electric Grand Piano", 1, 1, 3};
        Instrument honky_tonk_piano{"Honky-tonk Piano", 1, 1, 4};
        Instrument electric_piano1{"Electric Piano 1", 1, 1, 5};
        Instrument electric_piano2{"Electric Piano 2", 1, 1, 6};
        Instrument hapsichord{"Hapsichord", 1, 1, 7};
        Instrument clavinet{"Clavinet", 1, 1, 8};

        std::vector<Instrument*> all = {
                &acoustic_grand_piano, &bright_acoustic_piano, &electric_grand_piano, &honky_tonk_piano,
                &electric_piano1, &electric_piano2, &hapsichord, &clavinet
        };

        /**
         * Find an Instrument by full name
         * @param name The name of the Instrument to find
         * @return nullptr if no Instrument was found, otherwise a pointer to the Instrument
         */
        Instrument* findByName(const std::string& name);
    }
}


#endif //AUTOPLAY_INSTRUMENT_H
