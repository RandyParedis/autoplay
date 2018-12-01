//
// Created by red on 27/10/18.
//

#include "Config.h"
#include "FileHandler.h"
#include <version_config.h>

#include <boost/foreach.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

namespace autoplay {
    namespace util {
        bool check_binary(const std::string& b) {
            for(const auto& c : b) {
                if(c != '0' && c != '1') {
                    return false;
                }
            }
            return true;
        }

        Config::Config(int argc, char** argv) {
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
            parser.add_opt_version(-1, "version",
                                   "autoplay version " + getAutoplayVersion() + "\n\tCreated by Randy Paredis");
            bool verbose;
            parser.add_opt_flag('v', "verbose", "how much logging should happen", &verbose);
            bool play;
            parser.add_opt_flag('p', "play", "should the music be played life", &play);
            parser.add_opt_value<std::string>('c', "config", filename, "", "load a config file", "filename");
            std::string instrument_file;
            parser.add_opt_value<std::string>(
                'i', "instruments", instrument_file, "",
                "load an instrument config file; when omitted, the default instruments.json is used", "filename");
            std::string styles_file;
            parser.add_opt_value<std::string>(
                's', "styles", styles_file, "",
                "load a styles config file; when omitted, the default styles.json is used", "filename");
            std::string clefs_file;
            parser.add_opt_value<std::string>('k', "clefs", clefs_file, "",
                                              "load a clefs config file; when omitted, the default clefs.json is used",
                                              "filename");

            parser.parse(argc, argv);

            if(parser.count_error() > 0) {
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
                } catch(std::invalid_argument& e) { m_logger->error(e.what()); }
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

            if(m_ptree.count("instruments") == 1) {
                instrument_file = m_ptree.get<std::string>("instruments");
            } else if(m_ptree.count("instruments") > 1) {
                m_logger->error("Invalid amount of 'instruments' attributes found!");
            }

            if(m_ptree.count("styles") == 1) {
                styles_file = m_ptree.get<std::string>("styles");
            } else if(m_ptree.count("styles") > 1) {
                m_logger->error("Invalid amount of 'styles' attributes found!");
            }

            if(m_ptree.count("clefs") == 1) {
                clefs_file = m_ptree.get<std::string>("clefs");
            } else if(m_ptree.count("clefs") > 1) {
                m_logger->error("Invalid amount of 'clefs' attributes found!");
            }

            if(!verbose) {
                m_logger->set_level_mask(0x3c);
            }

            m_ptree.put("verbose", verbose);
            m_ptree.put("play", play);

            if(instrument_file.empty()) {
                instrument_file = "../config/instruments.json";
            }
            loadInstruments(instrument_file);

            if(styles_file.empty()) {
                styles_file = "../config/styles.json";
            }
            loadStyles(styles_file);

            if(clefs_file.empty()) {
                clefs_file = "../config/clefs.json";
            }
            loadClefs(clefs_file);

            m_logger->debug("Parsed Options:");
            if(!filename.empty()) {
                m_logger->debug("\tConfig File: ") << filename;
            }
            print_options(m_ptree, m_logger);
        }

        pt::ptree Config::conf_child(const std::string& path) const {
            try {
                return m_ptree.get_child(path);
            } catch(pt::ptree_error& pte) {
                m_logger->error("Invalid path for '{}'. Not found in config.", path);
                return pt::ptree();
            }
        }

        bool Config::isLeaf(const std::string& path) const {
            auto pt = m_ptree.get_child(path);
            return pt.empty();
        }

        void Config::loadInstruments(const std::string& filename) {
            FileHandler fileHandler;
            try {
                fileHandler.readConfig(filename);
                m_instruments = *fileHandler.getRoot();

                // Check validity
                std::vector<std::string> to_rem;
                BOOST_FOREACH(const auto& var, m_instruments) {
                    if(var.second.size() != 3) {
                        m_logger->warn("Instrument '{}' has {} children; expected 3.", var.first, var.second.size());
                    }
                    if(var.second.count("channel") + var.second.count("program") + var.second.count("unpitched") != 3) {
                        m_logger->warn("Invalid Instrument '{}'. Discarding...", var.first);
                        to_rem.emplace_back(var.first);
                    }
                }
                for(const auto& rem : to_rem) {
                    m_instruments.erase(rem);
                }
                if(m_instruments.empty()) {
                    m_logger->fatal("The instrument config file '{}' is invalid.", filename);
                    exit(EXIT_FAILURE);
                }

                m_logger->debug("Loaded {} instrument(s).", m_instruments.size());
            } catch(std::invalid_argument& e) {
                m_logger->fatal(e.what());
                exit(EXIT_FAILURE);
            }
        }

        void Config::loadStyles(const std::string& filename) {
            FileHandler fileHandler;
            try {
                fileHandler.readConfig(filename);
                m_styles = *fileHandler.getRoot();

                // Check validity
                if(m_styles.count("types") != 1) {
                    m_logger->fatal("The styles config file '{}' has an invalid amount of 'types' keys.", filename);
                    exit(EXIT_FAILURE);
                }
                if(m_styles.count("styles") != 1) {
                    m_logger->fatal("The styles config file '{}' has an invalid amount of 'styles' keys.", filename);
                    exit(EXIT_FAILURE);
                }

                m_logger->debug("Loaded {} style(s).", m_styles.get_child("styles").size());
            } catch(std::invalid_argument& e) {
                m_logger->fatal(e.what());
                exit(EXIT_FAILURE);
            }

            // Set current style
            if(m_ptree.get_child("style").empty()) {
                auto style_name = m_ptree.get<std::string>("style");
                if(m_styles.get_child("styles").count(style_name) == 0) {
                    m_logger->warn("Invalid style '{}'. Using 'default' instead.", style_name);
                    style_name = "default";
                }
                auto sty = m_styles.get_child("styles." + style_name);
                if(style_name != "default") {
                    auto from = sty.get<std::string>("from", "default");

                    // For loop to prevent circular dependencies to link to one another
                    for(unsigned int i = 0; i < m_styles.get_child("styles").size() && from != "default"; ++i) {
                        merge(sty, m_styles.get_child("styles." + from), true);
                        from = m_styles.get_child("styles." + from).get<std::string>("from", "default");

                        if(i == m_styles.get_child("styles").size() - 1) {
                            m_logger->warn("Circular Style dependency suspected for '{}' and '{}'.", from, style_name);
                        }
                    }
                    merge(sty, m_styles.get_child("styles.default"), true);
                }
                auto g = sty.get<std::string>("scale", "chromatic");
                if(!check_binary(g)) {
                    sty.put("scale", m_styles.get<std::string>("types." + g, "111111111111"));
                }
                m_ptree.put_child("style", sty);
            } else {
                auto sty  = m_ptree.get_child("style");
                auto from = sty.get<std::string>("from", "default");

                // For loop to prevent circular dependencies to link to one another
                for(unsigned int i = 0; i < m_styles.get_child("styles").size() && from != "default"; ++i) {
                    merge(sty, m_styles.get_child("styles." + from), true);
                    from = m_styles.get_child("styles." + from).get<std::string>("from", "default");

                    if(i == m_styles.get_child("styles").size() - 1) {
                        m_logger->warn("Circular Style dependency suspected for '{}' and '{}'.", from,
                                       sty.get<std::string>("from", ""));
                    }
                }
                merge(sty, m_styles.get_child("styles.default"), true);
                auto g = sty.get<std::string>("scale", "chromatic");
                if(!check_binary(g)) {
                    sty.put("scale", m_styles.get<std::string>("types." + g, "111111111111"));
                }
                m_ptree.put_child("style", sty);
            }
        }

        void Config::loadClefs(const std::string& filename) {
            FileHandler fileHandler;
            try {
                fileHandler.readConfig(filename);
                m_clefs = *fileHandler.getRoot();

                // Check validity
                std::vector<std::string> to_rem;
                BOOST_FOREACH(const auto& var, m_clefs) {
                    if(var.second.size() != 3) {
                        m_logger->warn("Clef '{}' has {} children; expected 3.", var.first, var.second.size());
                    }
                    if(var.second.count("sign") + var.second.count("line") + var.second.count("octave-change") != 3) {
                        m_logger->warn("Invalid Clef '{}'. Discarding...", var.first);
                        to_rem.emplace_back(var.first);
                    }
                }
                for(const auto& rem : to_rem) {
                    m_clefs.erase(rem);
                }
                if(m_clefs.empty()) {
                    m_logger->fatal("The clef config file '{}' is invalid.", filename);
                    exit(EXIT_FAILURE);
                }

                m_logger->debug("Loaded {} clef(s).", m_clefs.size());
            } catch(std::invalid_argument& e) {
                m_logger->fatal(e.what());
                exit(EXIT_FAILURE);
            }

            auto& parts = m_ptree.get_child("parts");
            for(size_t i = 0; i < parts.size(); ++i) {
                pt::ptree   clef;
                auto&       part = ptree_at_ref(parts, i);
                music::Clef c    = getClef(part.get<std::string>("clef", "Treble"));

                clef.put("sign", (char)c.getSign());
                clef.put("line", (int)c.getLine());
                clef.put("octave-change", c.getOctaveChange());
                part.put_child("clef", clef);
            }
        }

        std::shared_ptr<music::Instrument> Config::getInstrument(const std::string& name) const {
            if(m_instruments.count(name) == 0) {
                m_logger->warn("Unknown instrument '{}'", name);
            }
            auto channel   = m_instruments.get<int>(name + ".channel", 0);
            auto program   = m_instruments.get<int>(name + ".program", 0);
            auto unpitched = m_instruments.get<int>(name + ".unpitched", 0);

            return std::make_shared<music::Instrument>(name, channel, program, unpitched);
        }

        pt::ptree Config::getStyle(const std::string& name) const { return m_styles.get_child(name); }

        music::Clef Config::getClef(const std::string& name) const {
            pt::ptree clef = m_clefs.get_child(name);

            if(m_clefs.count(name) == 0) {
                m_logger->warn("Unknown clef '{}'", name);
            }

            auto sign   = clef.get<char>("sign", 'G');
            auto line   = (uint8_t)clef.get<int>("line", 2);
            auto octave = clef.get<int>("octave-change", 0);

            return music::Clef{(unsigned char)sign, line, octave};
        }

        void merge(pt::ptree& pt, const pt::ptree& updates, bool overwrite) {
            BOOST_FOREACH(auto& update, updates) {
                if(update.second.empty()) {
                    if(!update.first.empty()) {                                          // if leaf
                        if(overwrite && pt.get<std::string>(update.first, "").empty()) { // if must add
                            pt.put(update.first, updates.get<std::string>(update.first));
                        }
                    }
                } else {
                    if(update.second.back().first.empty()) { // if list
                        pt.put_child(update.first, update.second);
                    } else if(pt.count(update.first) != 0) {
                        merge(pt.get_child(update.first), update.second, overwrite);
                    } else {
                        pt.put_child(update.first, update.second);
                    }
                }
            }
        }

        void print_options(const pt::ptree& pt, zz::log::LoggerPtr& logger, const std::string& prepend) {
            for(const auto& o : pt) {
                if(o.second.empty()) {
                    logger->debug("\t") << prepend << o.first << ": " << pt.get<std::string>(o.first);
                } else {
                    print_options(o.second, logger, prepend + o.first + ".");
                }
            }
        }

        pt::ptree& ptree_at_ref(pt::ptree& pt, size_t n) { return std::next(pt.find(""), n)->second; }

        pt::ptree ptree_at(const pt::ptree& pt, size_t n) { return std::next(pt.find(""), n)->second; }
    }
}