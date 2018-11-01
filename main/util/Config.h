//
// Created by red on 27/10/18.
//

#ifndef AUTOPLAY_IOHANDLER_H
#define AUTOPLAY_IOHANDLER_H

#include <boost/property_tree/ptree.hpp>
#include <zupply/src/zupply.hpp>

/// Short alias for this namespace
namespace pt = boost::property_tree;

/**
 * The IOHandler class contains all functionality about handling in- and output
 */
class Config
{
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

private:
    pt::ptree          m_ptree;  ///< The ptree that holds all
    zz::log::LoggerPtr m_logger; ///< The system logger that's used everywhere
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

#endif // AUTOPLAY_IOHANDLER_H
