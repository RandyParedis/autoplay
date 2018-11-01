//
// Created by red on 19/10/18.
//

#ifndef AUTOPLAY_FILEHANDLER_H
#define AUTOPLAY_FILEHANDLER_H

#include <iostream>
#include <string>

#include "../music/Score.h"
#include <boost/property_tree/ptree.hpp>

/// Short alias for this namespace
namespace pt = boost::property_tree;

/**
 * The FileHandler class contains all functionality to read config files.
 */
class FileHandler
{
public:
    /**
     * Default FileHandler constructor
     */
    FileHandler() : m_root() {}

    /**
     * FileHandler Constructor that automatically sets a ptree.
     * @param pt The ptree to set
     */
    explicit FileHandler(pt::ptree& pt) : m_root(pt) {}

    /**
     * Sets the ptree/root/config
     * @param pt The ptree to set
     */
    void setRoot(pt::ptree& pt);

    /**
     * Clears the current root.
     */
    void clearRoot();

    /**
     * Read a JSON file as input
     * @param stream The place to read the JSON file from
     */
    void readJSON(std::istream& stream);

    /**
     * Read a JSON file as input
     * @param filename The filename to read as JSON.
     */
    void readJSON(std::string& filename);

    /**
     * Read a XML file as input
     * @param stream The place to read the XML file from
     */
    void readXML(std::istream& stream);

    /**
     * Read an XML file as input
     * @param filename  The filename to read as XML.
     * @param root      When not empty, it is the root that is stripped.
     */
    void readXML(std::string& filename, const std::string& root = "");

    /**
     * Read a config file (either using XML, or JSON), depending on the file
     * extension.
     * For XML files, it is expected to have a root element called 'config'
     * @param filename The file to read.
     *
     * @throws invalid_argument when not XML or JSON as extension.
     */
    void readConfig(std::string& filename);

    /**
     * Get the ptree that contains the new information
     * @return The ptree
     */
    inline pt::ptree* getRoot() { return &m_root; }

    /**
     * Export a score to MusicXML format
     * @param filename  The filename of the format.
     *                  Will automatically append the xml extension when not found.
     * @param score     The Score to urn into music.
     */
    static void writeMusicXML(std::string filename, const music::Score& score);

private:
    pt::ptree m_root;
};

#endif // AUTOPLAY_FILEHANDLER_H
