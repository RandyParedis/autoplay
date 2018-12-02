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
 *  Created on 02/12/18
 */

#ifndef AUTOPLAY_MARKOVCHAIN_H
#define AUTOPLAY_MARKOVCHAIN_H

#include "NamedMatrix.h"

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

using namespace boost::filesystem;

namespace autoplay {
    namespace markov {
        /**
         * The MarkovChain class is the general class that handles all events w.r.t. Markov chains.
         */
        class MarkovChain
        {
        public:
            /**
             * Generate a matrix from a list of MusicXML files in a certain directory.
             * @param directory The directory to read from.
             * @param recursive When true, it continues to look for files in subdirectories.
             * @return A vector of three NamedMatrix that represent the Markov Chains.
             */
            static std::vector<NamedMatrix> generateMatrix(const path& directory, bool recursive = true);

        private:
            /**
             * Helper function for generating matrices.
             * @param filename  The filename of the MusicXML file to read.
             * @param matPitch  The pitch matrix to update.
             * @param matRhythm The rhythm matrix to update.
             * @param matChord  The chord matrix to update.
             */
            static void generateMatrix(const std::string& filename, NamedMatrix& matPitch, NamedMatrix& matRhythm,
                                       NamedMatrix& matChord);
        };
    }
}

#endif // AUTOPLAY_MARKOVCHAIN_H
