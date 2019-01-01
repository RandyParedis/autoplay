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

#ifndef AUTOPLAY_NAMEDMATRIX_H
#define AUTOPLAY_NAMEDMATRIX_H

#include <map>
#include <string>
#include <vector>

namespace autoplay {
    namespace markov {

        /**
         * The NamedMatrix class handles all actions concerning matrices (or as much as are used)
         * It is different from a normal matrix in that each row and column (externally) need to be accessed
         * by a name.
         */
        class NamedMatrix
        {
        public:
            /**
             * Default constructor
             */
            NamedMatrix();

            /**
             * Constructor that automatically creates a filled matrix
             * @param rownames  The names for each row
             * @param colnames  The names for each column
             * @param value     The value to fill the matrix with
             */
            NamedMatrix(const std::vector<std::string>& rownames, const std::vector<std::string>& colnames,
                        double value = 0.0f);

            /**
             * Access an element of the matrix.
             * @param row       The row name to access.
             * @param column    The column name to access
             * @return A reference to the accessed element.
             */
            double& operator()(const std::string& row, const std::string& column);

            /**
             * Access an element of the matrix.
             * @param row       The row name to access.
             * @param column    The column name to access
             * @return A reference to the accessed element.
             */
            const double& operator()(const std::string& row, const std::string& column) const;

            /**
             * Access an element of the matrix.
             * @param row       The row name to access.
             * @param column    The column name to access
             * @return A reference to the accessed element.
             */
            double& at(const std::string& row, const std::string& column);

            /**
             * Access an element of the matrix.
             * @param row       The row name to access.
             * @param column    The column name to access
             * @return A reference to the accessed element.
             */
            const double& at(const std::string& row, const std::string& column) const;

            /**
             * Fetch a row of the matrix.
             * @param row       The row name to access.
             * @return A reference to the row.
             */
            std::vector<double>& operator[](const std::string& row);

            /**
             * Fetch a row of the matrix.
             * @param row       The row name to access.
             * @return A reference to the row.
             */
            const std::vector<double>& operator[](const std::string& row) const;

            /**
             * Fetch a row of the matrix.
             * @param row       The row name to access.
             * @return A reference to the row.
             */
            std::vector<double>& at(const std::string& row);

            /**
             * Fetch a row of the matrix.
             * @param row       The row name to access.
             * @return A reference to the row.
             */
            const std::vector<double>& at(const std::string& row) const;

            /**
             * Fetch a row of the matrix.
             * @param row       The row name to access.
             * @return A row, by a pair, e.g. <column, row-element>
             */
            std::vector<std::pair<std::string, double>> get(const std::string& row);

            /**
             * Fetch a row of the matrix.
             * @param row       The row name to access.
             * @return A row, by a pair, e.g. <column, row-element>
             */
            const std::vector<std::pair<std::string, double>> get(const std::string& row) const;

            /**
             * Compute the sum of a row
             * @param row   The name of the row to compute the sum for.
             * @return The sum of the row.
             */
            double rowSum(const std::string& row) const;

            /**
             * Normalize all rows, e.g. divide all elements in each row by the sum of the row
             */
            void normalizeRows();

            /**
             * Checks if the matrix is empty
             * @return True if it's empty
             */
            bool empty() const;

            /**
             * Checks if a certain row exists.
             * @param row   The row name to check
             * @return True if it exists.
             */
            bool isRow(const std::string& row) const;

            /**
             * Checks if a certain column exists.
             * @param column   The row name to check
             * @return True if it exists.
             */
            bool isColumn(const std::string& column) const;

            /**
             * Adds a row to the matrix
             * @param row       The row name of the new row.
             * @param value     The value to fill all elements with.
             */
            void addRow(const std::string& row, double value = 0.0f);

            /**
             * Adds a column to the matrix
             * @param column    The column name of the new column.
             * @param value     The value to fill all elements with.
             */
            void addColumn(const std::string& column, double value = 0.0f);

            /**
             * Drops a column from the matrix
             * @param column The column to drop.
             * @return True if the columns could be dropped, false otherwise.
             */
            bool dropColumn(const std::string& column);

            /**
             * Drops a row from the matrix
             * @param row The row to drop.
             * @return True if the rows could be dropped, false otherwise.
             */
            bool dropRow(const std::string& row);

            /**
             * Fetch all the column headers
             * @return A list of all the columns
             */
            std::vector<std::string> getColumns() const;

            /**
             * Fetch all the row headers
             * @return A list of all the rows
             */
            std::vector<std::string> getRows() const;

            /**
             * Write the matrix to a CSV-file
             * @param filename  The filename of the CSV.
             * @param sep       The separator in the file
             */
            void toCSV(const std::string& filename, char sep = ',') const;

            /**
             * Generate a NamedMatrix from a CSV file.
             * @param filename  The filename to generate the CSV from
             * @param sep       The separator that's been used in the file
             * @return A NamedMatrix, containing the data of the CSV.
             */
            static NamedMatrix fromCSV(const std::string& filename, char sep = ',');

        private:
            /// Allow for internal use of indexes

            /**
             * Access an element of the matrix.
             * @param row       The row index to access.
             * @param column    The column index to access
             * @return A reference to the accessed element.
             */
            double& operator()(unsigned long row, unsigned long column);

            /**
             * Access an element of the matrix.
             * @param row       The row index to access.
             * @param column    The column index to access
             * @return A reference to the accessed element.
             */
            const double& operator()(unsigned long row, unsigned long column) const;

            /**
             * Access an element of the matrix.
             * @param row       The row index to access.
             * @param column    The column index to access
             * @return A reference to the accessed element.
             */
            double& at(unsigned long row, unsigned long column);

            /**
             * Access an element of the matrix.
             * @param row       The row index to access.
             * @param column    The column index to access
             * @return A reference to the accessed element.
             */
            const double& at(unsigned long row, unsigned long column) const;

            /**
             * Fetch a row of the matrix.
             * @param row       The row index to access.
             * @return A reference to the row.
             */
            std::vector<double>& operator[](unsigned long row);

            /**
             * Fetch a row of the matrix.
             * @param row       The row index to access.
             * @return A reference to the row.
             */
            const std::vector<double>& operator[](unsigned long row) const;

            /**
             * Fetch a row of the matrix.
             * @param row       The row index to access.
             * @return A reference to the row.
             */
            std::vector<double>& at(unsigned long row);
            /**
             * Fetch a row of the matrix.
             * @param row       The row index to access.
             * @return A reference to the row.
             */
            const std::vector<double>& at(unsigned long row) const;

            /**
             * Compute the sum of a row
             * @param row   The index of the row to compute the sum for.
             * @return The sum of the row.
             */
            double rowSum(unsigned long row) const;

            /**
             * Checks if a certain row exists.
             * @param row   The row index to check
             * @return True if it exists.
             */
            bool isRow(unsigned long row) const;

            /**
             * Checks if a certain column exists.
             * @param column   The column index to check
             * @return True if it exists.
             */
            bool isColumn(unsigned long column) const;

        private:
            std::vector<std::vector<double>> m_matrix;     ///< The actual matrix
            std::map<std::string, unsigned long> m_rowmap; ///< A map for efficient name -> index mapping for the rows
            std::map<std::string, unsigned long>
                m_colmap; ///< A map for efficient name -> index mapping for the columns
        };

        /**
         * Splits a string into a vector of substrings on a character.
         * @param s     The string to split.
         * @param c     The character to split on.
         * @return A vector, containing the splitted string.
         */
        std::vector<std::string> split_on(const std::string& s, const char& c = ',');

        /**
         * Code taken from StackOverflow in order to support MAC-line endings (\r),
         * as well as UNIX-line endings (\n) when getting a line from a file.
         * @param is    The input stream to read from.
         * @param t     The string where the line will be stored.
         * @return  The inputstream to read from will be returned, after being read.
         */
        std::istream& getline_safe(std::istream& is, std::string& t);
    }
}

#endif // AUTOPLAY_NAMEDMATRIX_H
