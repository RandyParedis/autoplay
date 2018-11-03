//
// Created by red on 28/10/18.
//

#include "RNEngine.h"

namespace autoplay {
    namespace util {
        void RNEngine::operator()(const std::string& type) {
            if(type == "lcg64") {
                m_engine = trng::lcg64{};
            } else if(type == "lcg64_shift") {
                m_engine = trng::lcg64_shift{};
            } else if(type == "mrg2") {
                m_engine = trng::mrg2{};
            } else if(type == "mrg3") {
                m_engine = trng::mrg3{};
            } else if(type == "mrg4") {
                m_engine = trng::mrg4{};
            } else if(type == "mrg5") {
                m_engine = trng::mrg5{};
            } else if(type == "mrg5s") {
                m_engine = trng::mrg5s{};
            } else if(type == "mt19937") {
                m_engine = trng::mt19937{};
            } else if(type == "mt19937_64") {
                m_engine = trng::mt19937_64{};
            } else if(type == "yarn2") {
                m_engine = trng::yarn2{};
            } else if(type == "yarn3") {
                m_engine = trng::yarn3{};
            } else if(type == "yarn4") {
                m_engine = trng::yarn4{};
            } else if(type == "yarn5") {
                m_engine = trng::yarn5{};
            } else if(type == "yarn5s") {
                m_engine = trng::yarn5s{};
            } else {
                throw std::invalid_argument("No such type!");
            }
        }

        void RNEngine::operator()(const std::string& type, unsigned long seed) {
            if(type == "lcg64") {
                m_engine = trng::lcg64{seed};
            } else if(type == "lcg64_shift") {
                m_engine = trng::lcg64_shift{seed};
            } else if(type == "mrg2") {
                m_engine = trng::mrg2{seed};
            } else if(type == "mrg3") {
                m_engine = trng::mrg3{seed};
            } else if(type == "mrg4") {
                m_engine = trng::mrg4{seed};
            } else if(type == "mrg5") {
                m_engine = trng::mrg5{seed};
            } else if(type == "mrg5s") {
                m_engine = trng::mrg5s{seed};
            } else if(type == "mt19937") {
                m_engine = trng::mt19937{seed};
            } else if(type == "mt19937_64") {
                m_engine = trng::mt19937_64{seed};
            } else if(type == "yarn2") {
                m_engine = trng::yarn2{seed};
            } else if(type == "yarn3") {
                m_engine = trng::yarn3{seed};
            } else if(type == "yarn4") {
                m_engine = trng::yarn4{seed};
            } else if(type == "yarn5") {
                m_engine = trng::yarn5{seed};
            } else if(type == "yarn5s") {
                m_engine = trng::yarn5s{seed};
            } else {
                throw std::invalid_argument("No such type!");
            }
        }

        void RNEngine::seed(unsigned long seed) {
            switch(m_engine.which()) {
            case 0: boost::get<trng::lcg64>(m_engine).seed(seed); break;
            case 1: boost::get<trng::lcg64_shift>(m_engine).seed(seed); break;
            case 2: boost::get<trng::mrg2>(m_engine).seed(seed); break;
            case 3: boost::get<trng::mrg3>(m_engine).seed(seed); break;
            case 4: boost::get<trng::mrg4>(m_engine).seed(seed); break;
            case 5: boost::get<trng::mrg5>(m_engine).seed(seed); break;
            case 6: boost::get<trng::mrg5s>(m_engine).seed(seed); break;
            case 7: boost::get<trng::mt19937>(m_engine).seed(seed); break;
            case 8: boost::get<trng::mt19937_64>(m_engine).seed(seed); break;
            case 9: boost::get<trng::yarn2>(m_engine).seed(seed); break;
            case 10: boost::get<trng::yarn3>(m_engine).seed(seed); break;
            case 11: boost::get<trng::yarn4>(m_engine).seed(seed); break;
            case 12: boost::get<trng::yarn5>(m_engine).seed(seed); break;
            case 13: boost::get<trng::yarn5s>(m_engine).seed(seed); break;
            default: {}
            }
        }
    }
}