//
// Created by red on 27/10/18.
//

#include <version_config.h>
#include "Config.h"
#include "FileHandler.h"

Config::Config(int argc, char **argv) {
    // Setup System Logger
    zz::log::LogConfig::instance().set_format("[%datetime][%level]\t%msg");
    m_logger = zz::log::get_logger("system_logger");

    // Read default configuration file
    FileHandler fileHandler;
    std::string filename = "../config/default.json";
    fileHandler.readJSON(filename);
    m_ptree = *fileHandler.getRoot();

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
        fh.readJSON(filename);
        auto pt = fh.getRoot();
        // TODO: merge ptrees
        // m_ptree.put("verbose", pt->get<bool>("verbose"));
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

bool Config::conf_play() const {
    if(m_ptree.count("play") == 0) {
        return false;
    }
    if(m_ptree.count("play") == 1) {
        return m_ptree.get<bool>("play");
    }
    m_logger->error("Invalid amount of 'play' attributes!");
    return false;
}