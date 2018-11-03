//
// Created by red on 27/10/18.
//

#ifndef AUTOPLAY_IOHANDLER_H
#define AUTOPLAY_IOHANDLER_H

#include <boost/property_tree/ptree.hpp>
#include <zupply/src/zupply.hpp>

#include "../music/Instrument.h"

/// Short alias for this namespace
namespace pt = boost::property_tree;

namespace autoplay {
    namespace util {
        /**
        * The Config class contains all functionality about handling configuration files
        */
        class Config
        {
        public:
            /**
             * Default constructor. Should be initialized with command line arguments.
             * @param argc The amount of arguments
             * @param argv The argument array
             */
            Config(int argc, char** argv);

            /**
             * Fetches the system logger
             * @return A logger, called 'system_logger'
             */
            inline zz::log::LoggerPtr getLogger() const { return m_logger; }

            /**
             * Get the entire ptree
             * @return A pointer to the ptree
             *
             * @deprecated Use the conf function instead
             */
            [[deprecated]] inline pt::ptree getPtree() const { return m_ptree; }

        public:
            /**
             * Get the specified value from the config pt
             * @tparam T    The return type
             * @param path  The name of the config value to get
             * @return The value of what was gotten
             */
            template <typename T>
            T conf(const std::string& path) const;

            /**
             * Check if a ptree element at a certain path is a leaf.
             * @param path The path to check
             * @return True if it's a leaf.
             */
            bool isLeaf(const std::string& path) const;

        private:
            /**
             * Load the instruments into this Config.
             * @param filename The file to load the Instruments from.
             */
            void loadInstruments(const std::string& filename);

            /**
             * Load the styles into this Config.
             * @param filename
             */
            void loadStyles(const std::string& filename);

        public:
            /**
             * Fetches a shared pointer to an Instrument with a given name, as it was set
             * in the instruments config
             * @param name The name of the instrument (key in config)
             * @return A shared pointer to this instrument
             */
            std::shared_ptr<music::Instrument> getInstrument(const std::string& name) const;

            /**
             * Fetches the style with a given name from the config
             * @param name The name of the style
             * @return A ptree, containing all fields for this style.
             */
            pt::ptree getStyle(const std::string& name) const;

        private:
            pt::ptree          m_ptree;       ///< The ptree that holds all configuration data
            pt::ptree          m_instruments; ///< The ptree that holds all Instruments
            pt::ptree          m_styles;      ///< The ptree that holds all styles
            zz::log::LoggerPtr m_logger;      ///< The system logger that's used everywhere
        };

        template <typename T>
        T Config::conf(const std::string& path) const {
            try {
                return m_ptree.get<T>(path);
            } catch(pt::ptree_error& pte) {
                m_logger->error("Invalid path for '{}'. Not found in config.", path);
                return T();
            }
        }

        /**
         * Merge two ptrees together
         * @param pt        The root container
         * @param updates   The ptree containing updates
         *
         * @note    The good enough solution has two limitations:
         *          - the tree can only be two layers (e.g. "first.number", but not
         * "first.again.number")
         *          - values can only be stored in leaf nodes.
         *
         * @copyright
         * https://stackoverflow.com/questions/8154107/how-do-i-merge-update-a-boostproperty-treeptree/8175833
         */
        void merge(pt::ptree& pt, const pt::ptree& updates);

        /**
         * Print the contents of a ptree
         * @param pt        The ptree to print
         * @param logger    The logger to print to
         * @param prepend   A path that must be prepended to all values
         */
        void print_options(const pt::ptree& pt, zz::log::LoggerPtr& logger, const std::string& prepend = "");
    }
}

#endif // AUTOPLAY_IOHANDLER_H
