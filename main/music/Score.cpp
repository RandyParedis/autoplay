/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, Randy Paredis
 *
 *  Created on 06/11/2018
 */

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