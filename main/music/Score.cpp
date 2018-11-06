//
// Created by red on 06/11/18.
//

#include "Score.h"
#include <boost/algorithm/string/replace.hpp>
#include <boost/foreach.hpp>
#include <zupply/src/zupply.hpp>
#include <version_config.h>

namespace autoplay {
    namespace music {
        Score::Score(const pt::ptree& header) : m_header(header) {
            BOOST_FOREACH(auto& elm, m_header) {
                std::string val = elm.second.get<std::string>("");
                boost::replace_all(val, "@VERSION@", getAutoplayVersion());
                m_header.put(elm.first, val);
            }
        }

        pt::ptree Score::getHeaderDataAsMusicXML() const {
            pt::ptree result;

            // Set title
            result.put("work.work-title", m_header.get<std::string>("title", ""));

            // Set composer
            pt::ptree composer;
            composer.put("<xmlattr>.type", "composer");
            composer.put("", m_header.get<std::string>("composer", ""));
            result.add_child("identification.creator", composer);

            // Set rights
            result.put("identification.rights", m_header.get<std::string>("rights", ""));

            // Set encoding details
            zz::time::DateTime date;
            date.to_utc_time();
            result.put("identification.encoding.encoding-date", date.to_string("%Y-%m-%d"));
            result.put("identification.encoding.encoder", "Randy Paredis");
            result.put("identification.encoding.software", "autoplay v" + getAutoplayVersion());
            result.put("identification.encoding.encoding-description",
                       "This file has been encoded using the autoplay software.");

            return result;
        }
    }
}