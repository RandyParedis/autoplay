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
 *  Created on 27/10/2018
 */

#ifndef AUTOPLAY_SCORE_H
#define AUTOPLAY_SCORE_H

#include "Part.h"

#include <boost/property_tree/ptree.hpp>

namespace pt = boost::property_tree;

namespace autoplay {
    namespace music {
        /**
         * The Score class contains all detailed data about a certain Score.
         *
         * @note Currently, this is just a container that holds a series of Parts.
         *
         * TODO: Add (possibly randomized) data for the 'optional header data' of a
         * Score
         */
        class Score
        {
        public:
            /**
             * Score constructor
             * @param header The header data as a ptree
             */
            explicit Score(const pt::ptree& header);

            /**
             * Add a Part to a Score
             * @param part A shared pointer to a Part to add
             */
            inline void addPart(const std::shared_ptr<Part>& part) { m_parts.emplace_back(part); }

            /**
             * Get all the Parts of this Score.
             * @return A vector with shared ponters to parts.
             */
            inline std::vector<std::shared_ptr<Part>> getParts() const { return m_parts; }

            /**
             * Set the header data to
             * @param header
             */
            inline void setHeaderData(const pt::ptree& header) { m_header = header; }

            /**
             * Get the header data
             * @return The header data as a ptree
             */
            inline pt::ptree getHeaderData() const { return m_header; }

            pt::ptree getHeaderDataAsMusicXML() const;

        private:
            std::vector<std::shared_ptr<Part>> m_parts; ///< A vector of all Parts.

            pt::ptree m_header; ///< Header data
        };
    }
}

#endif // AUTOPLAY_SCORE_H
