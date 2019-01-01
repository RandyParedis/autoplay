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

#include "../util/RNEngine.h"
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
            using State = std::string;

            /**
             * Constructor of a MarkovChain that is created from a CSV file.
             * @param filename  The file to read from.
             * @param engine    The random engine to use.
             * @param begin     The initial State.
             */
            explicit MarkovChain(const std::string& filename, const util::RNEngine& engine,
                                 const State& begin = "begin");

            /**
             * Constructor of a MarkovChain that is created from a NamedMatrix
             * @param namedMatrix The matrix to create the chain from.
             * @param engine    The random engine to use.
             * @param begin     The initial State.
             */
            explicit MarkovChain(const NamedMatrix& namedMatrix, const util::RNEngine& engine,
                                 const State& begin = "begin");

            /**
             * Erases a set of States from the MarkovChain
             * @param erasables The set of elements that must be erased.
             */
            void erase(const std::vector<State>& erasables);

            /**
             * Erases all States from the MarkovChain that do not appear in the set
             * @param non_erasables The set of elements that must be erased.
             */
            void keep(const std::vector<State>& non_erasables);

            /**
             * Resets the MarkovChain to its initial State.
             */
            inline void reset() { m_current = m_begin; }

            /**
             * Gets the current State of the chain.
             * @return The State of the chain.
             */
            inline State getState() const { return m_current; }

            /**
             * Go to the next State.
             * @return The new State.
             */
            State next();

        private:
            NamedMatrix    m_matrix;  ///< The transition matrix
            util::RNEngine m_engine;  ///< The random engine to use
            State          m_current; ///< The current State
            State          m_begin;   ///< The begin/start State

            /**
             * Fetches a map of all possible States to go to.
             * @return A map in the form of < State, chance to get there from current state >
             */
            std::map<State, double> fetchPossibilities() const;

            /// Special functions for machine-learning itself
        public:
            /**
             * Generate a matrix from a list of MusicXML files in a certain directory.
             * @param directory The directory to read from.
             * @param recursive When true, it continues to look for files in subdirectories.
             * @return A vector of three NamedMatrix that represent the Markov Chains.
             */
            static std::vector<NamedMatrix> generateMatrices(const path& directory, bool recursive = true);

        private:
            /**
             * Helper function for generating matrices.
             * @param filename  The filename of the MusicXML file to read.
             * @param matPitch  The pitch matrix to update.
             * @param matRhythm The rhythm matrix to update.
             * @param matChord  The chord matrix to update.
             */
            static void generateMatrices(const std::string& filename, NamedMatrix& matPitch, NamedMatrix& matRhythm,
                                         NamedMatrix& matChord);
        };
    }
}

#endif // AUTOPLAY_MARKOVCHAIN_H
