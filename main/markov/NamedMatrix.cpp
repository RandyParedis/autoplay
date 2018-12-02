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

#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fstream>

#include "NamedMatrix.h"

namespace autoplay {
    namespace markov {

        NamedMatrix::NamedMatrix() : m_matrix(), m_rowmap(), m_colmap() {}

        NamedMatrix::NamedMatrix(const std::vector<std::string>& rownames, const std::vector<std::string>& colnames,
                                 float value)
            : NamedMatrix() {

            // First do the columns, because that's a little bit more efficient.
            for(const auto& column : colnames) {
                addColumn(column, value);
            }
            for(const auto& row : rownames) {
                addRow(row, value);
            }
        }

        float& NamedMatrix::operator()(unsigned long row, unsigned long column) { return m_matrix[row][column]; }

        float& NamedMatrix::operator()(const std::string& row, const std::string& column) {
            return m_matrix[m_rowmap.at(row)][m_colmap.at(column)];
        }

        const float& NamedMatrix::operator()(unsigned long row, unsigned long column) const {
            return m_matrix[row][column];
        }

        const float& NamedMatrix::operator()(const std::string& row, const std::string& column) const {
            return m_matrix[m_rowmap.at(row)][m_colmap.at(column)];
        }

        float& NamedMatrix::at(unsigned long row, unsigned long column) { return m_matrix.at(row).at(column); }

        float& NamedMatrix::at(const std::string& row, const std::string& column) {
            return m_matrix.at(m_rowmap.at(row)).at(m_colmap.at(column));
        }

        const float& NamedMatrix::at(unsigned long row, unsigned long column) const {
            return m_matrix.at(row).at(column);
        }

        const float& NamedMatrix::at(const std::string& row, const std::string& column) const {
            return m_matrix.at(m_rowmap.at(row)).at(m_colmap.at(column));
        }

        std::vector<float>& NamedMatrix::operator[](unsigned long row) { return m_matrix[row]; }

        const std::vector<float>& NamedMatrix::operator[](unsigned long row) const { return m_matrix[row]; }

        std::vector<float>& NamedMatrix::operator[](const std::string& row) { return m_matrix[m_rowmap.at(row)]; }

        const std::vector<float>& NamedMatrix::operator[](const std::string& row) const {
            return m_matrix[m_rowmap.at(row)];
        }

        std::vector<float>& NamedMatrix::at(unsigned long row) { return m_matrix[row]; }

        const std::vector<float>& NamedMatrix::at(unsigned long row) const { return m_matrix[row]; }

        std::vector<float>& NamedMatrix::at(const std::string& row) { return m_matrix[m_rowmap.at(row)]; }

        const std::vector<float>& NamedMatrix::at(const std::string& row) const { return m_matrix[m_rowmap.at(row)]; }

        float NamedMatrix::rowSum(unsigned long row) const {
            float sum = 0.0f;
            auto& r   = at(row);
            for(const auto& elm : r) {
                sum += elm;
            }
            return sum;
        }

        float NamedMatrix::rowSum(const std::string& row) const {
            float sum = 0.0f;
            auto& r   = at(row);
            for(const auto& elm : r) {
                sum += elm;
            }
            return sum;
        }

        void NamedMatrix::normalizeRows() {
            for(unsigned long r = 0; r < m_matrix.size(); ++r) {
                float sum = rowSum(r);
                for(unsigned long c = 0; c < at(r).size(); ++c) {
                    at(r, c) /= sum;
                }
            }
        }

        bool NamedMatrix::empty() const {
            bool r = true;
            for(const auto& row : m_matrix) {
                if(!row.empty()) {
                    r = false;
                    break;
                }
            }
            return r;
        }

        bool NamedMatrix::isRow(unsigned long row) const { return row < m_rowmap.size(); }

        bool NamedMatrix::isRow(const std::string& row) const { return m_rowmap.find(row) != m_rowmap.end(); }

        bool NamedMatrix::isColumn(unsigned long column) const { return column < m_colmap.size(); }

        bool NamedMatrix::isColumn(const std::string& column) const { return m_colmap.find(column) != m_colmap.end(); }

        void NamedMatrix::addRow(const std::string& row, float value) {
            m_rowmap.insert({row, m_rowmap.size()});
            m_matrix.emplace_back(std::vector<float>(m_colmap.size(), value));
        }

        void NamedMatrix::addColumn(const std::string& column, float value) {
            m_colmap.insert({column, m_colmap.size()});
            for(auto& row : m_matrix) {
                row.emplace_back(value);
            }
        }

        void NamedMatrix::toCSV(const std::string& filename, char sep) const {
            std::ofstream file(filename);
            if(file.is_open()) {
                file << "x";
                for(const auto& kv : m_colmap) {
                    file << sep << " \"" << kv.first << "\"";
                }
                for(const auto& kv : m_rowmap) {
                    file << "\n\"" << kv.first << "\"";
                    for(const auto& ckv : m_colmap) {
                        file << sep << " " << at(kv.first, ckv.first);
                    }
                }
                file.close();
            } else {
                throw std::runtime_error("Unable to open file with filename '" + filename + "'");
            }
        }

        NamedMatrix NamedMatrix::fromCSV(const std::string& filename, char sep) {
            std::ifstream file(filename);
            if(file.is_open()) {
                NamedMatrix nm;
                std::string line;
                if(getline_safe(file, line)) {
                    // Header
                    std::vector<std::string> names;
                    boost::split(names, line, boost::is_any_of(std::to_string(sep)));
                    names.erase(names.begin()); // remove useless column
                    // remove surrounding quotes and insert as column
                    for(auto& name : names) {
                        name = name.substr(1, name.length() - 2);
                        nm.addColumn(name);
                    }
                }
                while(getline_safe(file, line)) {
                    std::vector<std::string> values;
                    boost::split(values, line, boost::is_any_of(std::to_string(sep)));
                    std::string name = values.at(0);
                    nm.addRow(name);
                    values.erase(values.begin()); // remove name from sequence
                    for(unsigned int col = 0; col < values.size(); ++col) {
                        nm.at(nm.m_rowmap.at(name), col) = std::stof(values.at(col));
                    }
                }
                file.close();
                return nm;
            } else {
                throw std::runtime_error("Unable to open file with filename '" + filename + "'");
            }
        }

        std::istream& getline_safe(std::istream& is, std::string& t) {
            t.clear();

            // The characters in the stream are read one-by-one using a std::streambuf.
            // That is faster than reading them one-by-one using the std::istream.
            // Code that uses streambuf this way must be guarded by a sentry object.
            // The sentry object performs various tasks,
            // such as thread synchronization and updating the stream state.

            std::istream::sentry se(is, true);
            std::streambuf*      sb = is.rdbuf();

            while(true) {
                int c = sb->sbumpc();
                switch(c) {
                case '\n': return is;
                case '\r':
                    if(sb->sgetc() == '\n')
                        sb->sbumpc();
                    return is;
                case std::streambuf::traits_type::eof():
                    // Also handle the case when the last line has no line ending
                    if(t.empty())
                        is.setstate(std::ios::eofbit);
                    return is;
                default: t += (char)c;
                }
            }
        }
    }
}