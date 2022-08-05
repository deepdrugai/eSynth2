/*
 *  This file is part of esynth.
 *
 *  esynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  esynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with esynth.  If not, see <http://www.gnu.org/licenses/>.
 */

 /*
  * eSynth 2.0
  * Author: C. Alvin 8/2022
  */

#ifndef _INFILE_EXISTENCE_ANALYZER_GUARD
#define _INFILE_EXISTENCE_ANALYZER_GUARD 1

#include <string>
#include <vector>

#include "FileUtilities.h"

//
// This class bifurcates the input list into valid / invalid files based on existence.
// We assume these file names have been screened as valid for eSynth by InfileNameAnalyzer.
//
class InfileExistenceAnalyzer
{
  protected:
    std::vector<std::string> _valid;
    std::vector<std::string> _invalid;

  public:
    InfileExistenceAnalyzer(const std::vector<std::string>& names) : _valid{}, _invalid{}
    {
        analyze(names);
    }

    std::vector<std::string> getValidFiles() const { return _valid; }
    std::vector<std::string> getInvalidFiles() const { return _invalid; }

    /*
     * Verify that each of the valid files exists at the specified location
     */
    void analyze(const std::vector<std::string>& names)
    {
        for (auto const& fileName : names)
        {
            if (FileUtilities::exists(fileName)) _valid.push_back(fileName);
            else _invalid.push_back(fileName);
        }
    }
};

#endif
