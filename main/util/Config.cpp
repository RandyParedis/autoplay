//
// Created by red on 27/10/18.
//

#include <version_config.h>
#include "Config.h"
#include "FileHandler.h"

#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>

/**
 * Merge two ptrees together
 * @param pt        The root container
 * @param updates   The ptree containing updates
 *
 * @note    The good enough solution has two limitations:
 *          - the tree can only be two layers (e.g. "first.number", but not "first.again.number")
 *          - values can only be stored in leaf nodes.
 *
 * @copyright https://stackoverflow.com/questions/8154107/how-do-i-merge-update-a-boostproperty-treeptree/8175833
 */
void merge(pt::ptree& pt, const pt::ptree& updates)
{
    BOOST_FOREACH(auto& update, updates) {
        if(update.second.empty()) {
            if(!update.first.empty()) { // list
                pt.put(update.first, updates.get<std::string>(update.first));
            }
        } else {
            if(update.second.back().first.empty()) { // if list
                pt.put_child(update.first, update.second);
            } else {
                merge(pt.get_child(update.first), update.second);
            }
        }
    }
}

Config::Config(int argc, char **argv) {
    // Setup System Logger
    zz::log::LogConfig::instance().set_format("[%datetime][%level]\t%msg");
    m_logger = zz::log::get_logger("system_logger");

    // Read default configuration file
    FileHandler fileHandler;
    std::string filename = "../config/default.json";
    try {
        fileHandler.readConfig(filename);
        m_ptree = *fileHandler.getRoot();
    } catch(std::invalid_argument& e) {
        m_logger->fatal(e.what());
        exit(EXIT_FAILURE);
    }

    // Set & Parse command line arguments
    zz::cfg::ArgParser parser;
    parser.add_opt_help('h', "help");
    parser.add_opt_version(-1, "version", "autoplay version " + getAutoplayVersion() + "\n\tCreated by Randy Paredis");
    bool verbose;
    parser.add_opt_flag('v', "verbose", "how much logging should happen", &verbose);
    bool play;
    parser.add_opt_flag('p', "play", "should the music be played life", &play);
    parser.add_opt_value<std::string>('c', "config", filename, "", "load a config file", "filename");

    parser.parse(argc, argv);

    if (parser.count_error() > 0) {
        m_logger->fatal() << parser.get_error();
        std::cout << parser.get_help() << std::endl;
        exit(EXIT_FAILURE);
    }

    if(!filename.empty()) {
        FileHandler fh;
        try {
            fh.readConfig(filename);
            auto mpt = *fh.getRoot();
            merge(m_ptree, mpt);
        } catch(std::invalid_argument& e) {
            m_logger->error(e.what());
        }
    }

    if(m_ptree.count("verbose") == 1 && !verbose) {
        verbose = m_ptree.get<bool>("verbose");
    } else if(m_ptree.count("verbose") > 1) {
        m_logger->error("Invalid amount of 'verbose' attributes found!");
    }

    if(m_ptree.count("play") == 1 && !play) {
        play = m_ptree.get<bool>("play");
    } else if(m_ptree.count("play") > 1) {
        m_logger->error("Invalid amount of 'play' attributes found!");
    }

    if(!verbose) {
        m_logger->set_level_mask(0x3c);
    }

    m_logger->debug("Parsed Options:");
    m_ptree.put("verbose", verbose);
    m_logger->debug("\tVerbose: ") << (m_ptree.get<bool>("verbose") ? "yes" : "no");
    if(!filename.empty()) {
        m_logger->debug("\tConfig File: ") << filename;
    }
    m_ptree.put("play", play);
    m_logger->debug("\tPlay: ") << (m_ptree.get<bool>("play") ? "yes" : "no");
}
