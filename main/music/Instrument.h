//
// Created by red on 25/10/18.
//

#ifndef AUTOPLAY_INSTRUMENT_H
#define AUTOPLAY_INSTRUMENT_H

#include <memory>
#include <string>
#include <vector>

namespace music {
    /**
     * The Instrument class handles all simple actions concerned to instruments and the way certain things sound.
     */
    class Instrument {
    public:
        /**
         * Default Constructor
         * @param name          The name (as string) of an Instrument
         * @param channel       The channel of the Instrument
         * @param program       The program of the Instrument
         * @param unpitched     The index of the MIDI sound corresponding to the Instrument
         */
        Instrument(std::string name, uint8_t channel, uint8_t program, uint8_t unpitched) :
                m_name(std::move(name)), m_channel(channel), m_program(program), m_unpitched(unpitched) {}

        /**
         * Gets the name of the Instrument
         * @return The name
         */
        inline std::string getName() const { return m_name; }

        /**
         * Gets the channel of the Instrument
         * @return The channel
         */
        inline uint8_t getChannel() const { return m_channel; }

        /**
         * Gets the program of the Instrument
         * @return The program
         */
        inline uint8_t getProgram() const { return m_program; }

        /**
         * Gets the unpitched value of the Instrument
         * @return The unpitched value (index)
         */
        inline uint8_t getUnpitched() const { return m_unpitched; }

        /**
         * Sets the name of the Instrument
         * @param name New name to set
         */
        inline void setName(const std::string& name) { m_name = name; }

        /**
         * Sets the channel of the Instrument
         * @param channel New channel to set
         */
        inline void setChannel(const uint8_t& channel) { m_channel = channel; }

        /**
         * Sets the program of the Instrument
         * @param program New program to set
         */
        inline void setProgram(const uint8_t& program) { m_program = program; }

        /**
         * Sets the unpitched of the Instrument
         * @param unpitched New unpitched to set
         */
        inline void setUnpitched(const uint8_t& unpitched) { m_unpitched = unpitched; }

    private:
        std::string m_name;     ///< The name of the Instrument
        uint8_t m_channel;      ///< The channel of the Instrument
        uint8_t m_program;      ///< The program of the Instrument
        uint8_t m_unpitched;    ///< The tone that must be played. In the MusicXML, we start counting from 1!
    };

    /**
     * The instruments namespace contains all existing music instruments in MIDI as Instrument objects.
     *
     * @note Currently, merely the piano-like Instruments are implemented
     *
     * TODO: Implement all Instruments
     */
    namespace instruments {

        /// Pianos

        static std::shared_ptr<Instrument> acoustic_grand_piano = std::make_shared<Instrument>("Acoustic Grand Piano", 1, 1, 1);
        static std::shared_ptr<Instrument> bright_acoustic_piano = std::make_shared<Instrument>("Bright Acoustic Piano", 1, 1, 2);
        static std::shared_ptr<Instrument> electric_grand_piano = std::make_shared<Instrument>("Electric Grand Piano", 1, 1, 3);
        static std::shared_ptr<Instrument> honky_tonk_piano = std::make_shared<Instrument>("Honky-tonk Piano", 1, 1, 4);
        static std::shared_ptr<Instrument> electric_piano1 = std::make_shared<Instrument>("Electric Piano 1", 1, 1, 5);
        static std::shared_ptr<Instrument> electric_piano2 = std::make_shared<Instrument>("Electric Piano 2", 1, 1, 6);
        static std::shared_ptr<Instrument> hapsichord = std::make_shared<Instrument>("Hapsichord", 1, 1, 7);
        static std::shared_ptr<Instrument> clavinet = std::make_shared<Instrument>("Clavinet", 1, 1, 8);

        /**
         * The all vector is a list of all the above instruments, indexable by 0, or by name,
         * using the findByName function below.
         */
        static std::vector<std::shared_ptr<Instrument>> all = {
                acoustic_grand_piano, bright_acoustic_piano, electric_grand_piano, honky_tonk_piano,
                electric_piano1, electric_piano2, hapsichord, clavinet
        };

        /**
         * Find an Instrument by full name
         * @param name The name of the Instrument to find
         * @return nullptr if no Instrument was found, otherwise a pointer to the Instrument
         */
        std::shared_ptr<Instrument> findByName(const std::string& name);
    }
}


#endif //AUTOPLAY_INSTRUMENT_H
