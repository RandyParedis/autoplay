//
// Created by red on 27/10/18.
//

#ifndef AUTOPLAY_IOHANDLER_H
#define AUTOPLAY_IOHANDLER_H

#include <zupply/src/zupply.hpp>
#include <boost/property_tree/ptree.hpp>

/// Short alias for this namespace
namespace pt = boost::property_tree;


/**
 * The IOHandler class contains all functionality about handling in- and output
 */
class Config {
public:
    /**
     * Default constructor. Should be initialized with command line arguments
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
     */
    inline pt::ptree getPtree() const { return m_ptree; }

public:
    /**
     * Check if the configuration says 'play' is true
     * @return True if 'play' is true
     */
    bool conf_play() const;
private:
    pt::ptree m_ptree; ///< The ptree that holds all
    zz::log::LoggerPtr m_logger; ///< The system logger that's used everywhere
};


#endif //AUTOPLAY_IOHANDLER_H
