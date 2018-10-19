//
// Created by red on 19/10/18.
//

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include "FileHandler.h"

#include <iostream>

void FileHandler::setRoot(pt::ptree &pt) { m_root = pt; }

void FileHandler::clearRoot() { m_root.clear(); }

void FileHandler::readJSON(std::istream &stream) {
    pt::read_json(stream, m_root);
}

void FileHandler::readJSON(std::string &filename) {
    pt::read_json(filename, m_root);
}

void FileHandler::readXML(std::istream &stream) {
    pt::read_xml(stream, m_root);
}

void FileHandler::readXML(std::string &filename) {
    pt::read_xml(filename, m_root);
}