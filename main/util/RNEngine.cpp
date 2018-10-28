//
// Created by red on 28/10/18.
//

#include "RNEngine.h"

void RNEngine::operator()(const std::string &type) {
    if(type == "lcg64") {
        m_engine = trng::lcg64{};
    } else if(type == "mrg5") {
        m_engine = trng::mrg5{};
    } else {
        throw std::invalid_argument("No such type!");
    }
}

void RNEngine::operator()(const std::string &type, unsigned long seed) {
    if(type == "lcg64") {
        m_engine = trng::lcg64{seed};
    } else if(type == "mrg5") {
        m_engine = trng::mrg5{seed};
    } else {
        throw std::invalid_argument("No such type!");
    }
}

void RNEngine::seed(unsigned long seed) {
    switch(m_engine.which()) {
        case 0: boost::get<trng::lcg64>(m_engine).seed(seed); break;
        case 1: boost::get<trng::mrg5>(m_engine).seed(seed); break;
        default: {}
    }
}
