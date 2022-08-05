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
  * Author: C. Alvin 7/27/2022
  */

#ifndef _INFILE_NAME_ANALYZER_GUARD
#define _INFILE_NAME_ANALYZER_GUARD 1

#include <string>
#include <vector>
#include <iostream>

  // TODO: Guards around selecting C++ standard

  // #ifdef __cplusplus

  // Requires C++-14
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

// Requires C++-17
//#include <filesystem>
//namespace fs = std::filesystem;

#include "InfileExistenceAnalyzer.h"

#include "Utilities.h"
#include "Constants.h"

//
// This class bifurcates the input list into valid / invalid files based on file names
//
class InfileNameAnalyzer
{
  protected:
    std::map<std::string, Constants::FRAGMENT_TYPE> _valid;
    std::vector<std::string> _invalid;

  public:
    InfileNameAnalyzer(const std::vector<std::string>& names) : _valid{}, _invalid{}
    {
        analyze(names);
    }

    std::map<std::string, Constants::FRAGMENT_TYPE> getValidFiles() const { return _valid; }
    std::vector<std::string> getInvalidFiles() const { return _invalid; }

  protected:

    //
    // Split the files into valid and invalid
    //
    void analyze(const std::vector<std::string>& names)
    {
        for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++)
        {
            fs::path p = *it;

            Constants::FRAGMENT_TYPE fType = isValidFileName(p);
            
            if (fType != Constants::FRAGMENT_TYPE::ERROR) _valid[*it] = fType;
            else _invalid.push_back(*it);
        }
    }

public:

    /*
     *  Does the input file conform to what we allow for fragment file names?
     *
     *  Constraints
     *    (1) Extension is   sdf   (no '.')
     *    (2) File name begin with:
     *            'b' (brick)
     *            'l' (linker)
     *            'r' (rigid) -- to be consistent with eSynth v1.0
     *    (3) If the prefix is not valid the file prefix may contain
     *            brick
     *            linker
     * 
     * Assumes input of a file name (no path information)
     */
    static Constants::FRAGMENT_TYPE isValidFileName(const fs::path& p)
    {
        if (p.empty())
        {
            std::cerr << "This is odd; no input file name specified: " << p << std::endl;
            return Constants::FRAGMENT_TYPE::ERROR;
        }

        if (!hasValidExtension(p))
        {
            std::cerr << "Extension of file " << p.extension().string() << " is not "
                      << Constants::INPUT_SDF_FILE_EXTENSION
                      << " as required." << std::endl;

            return Constants::FRAGMENT_TYPE::ERROR;
        }

        // l- b- r- are unambiguous
        Constants::FRAGMENT_TYPE fType = hasUnambiguousValidPrefix(p.filename().string());
        if (fType != Constants::FRAGMENT_TYPE::ERROR) return fType;

        // u- all- are ambiguous
        if (!hasAmbiguousValidPrefix(p.filename().string())) return Constants::FRAGMENT_TYPE::ERROR;

        fType = hasValidInternalIdentifier(p.stem().string());
        if (fType != Constants::FRAGMENT_TYPE::ERROR) return fType;

        // Otherwise we have an error setting
        std::string substrs = Utilities::join(", ", Constants::ACCEPTABLE_INPUT_FILE_SUBSTRS);
        std::string prefixes = Utilities::join(", ", Constants::UNAMBIGUOUS_ACCEPTABLE_INPUT_FILE_PREFIXES);

        std::cerr << "Without valid internal file name substrings [" << substrs
                  << "] input file must have an unambiguous prefix [" << prefixes
                  << "]" << std::endl;

        return Constants::FRAGMENT_TYPE::ERROR;
    }

    //
    // (1) Valid Extenion:    sdf
    //
    static bool hasValidExtension(fs::path p)
    {
        // Check existence before comparing for valid extension
        if (!p.has_extension()) return false;

        std::string ext = p.extension().string().substr(1);

        return ext == Constants::INPUT_SDF_FILE_EXTENSION;
    }

    //
    // (2) File name begins with :
    //     'b' (brick)
    //     'l' (linker)
    //     'r' (rigid) --to be consistent with eSynth v1.0
    //
    static Constants::FRAGMENT_TYPE hasUnambiguousValidPrefix(const std::string& name)
    {
        std::string preDash = "";
        std::string postDash = "";
        Utilities::splitFirst(name, preDash, postDash, "-");

        // Is the prefix of the given file in our accepted list?
        if (!Utilities::contains(Constants::UNAMBIGUOUS_ACCEPTABLE_INPUT_FILE_PREFIXES, preDash))

        
        Utilities::tolower(preDash);
        if (preDash == Constants::BRICK_PREFIX) return Constants::FRAGMENT_TYPE::BRICK;
        if (preDash == Constants::LINKER_PREFIX) return Constants::FRAGMENT_TYPE::LINKER;
        if (preDash == Constants::FREE_ATOM_PREFIX) return Constants::FRAGMENT_TYPE::FREE_ATOM;

        return Constants::FRAGMENT_TYPE::ERROR;
    }

    //
    // (2) File name begins with :
    //     'all'
    //     'u;
    //
    static bool hasAmbiguousValidPrefix(const std::string& name)
    {
        std::string preDash = "";
        std::string postDash = "";
        Utilities::splitFirst(name, preDash, postDash, "-");

        return Utilities::contains(Constants::AMBIGUOUS_ACCEPTABLE_INPUT_FILE_PREFIXES, preDash);
    }

    //
    // (3) If the prefix is not valid the file prefix may contain (as a substring)
    //     brick
    //     linker
    //
    static Constants::FRAGMENT_TYPE hasValidInternalIdentifier(const std::string& prefix)
    {
        std::string copy = prefix;
        Utilities::tolower(copy);

        if (copy.find(Constants::ACCEPTABLE_INPUT_LINKER_FILE_SUBSTR) != std::string::npos)
            return Constants::FRAGMENT_TYPE::LINKER;

        else if (copy.find(Constants::ACCEPTABLE_INPUT_BRICK_FILE_SUBSTR) != std::string::npos)
            return Constants::FRAGMENT_TYPE::BRICK;

        else if (copy.find(Constants::ACCEPTABLE_INPUT_FREE_ATOM_FILE_SUBSTR) != std::string::npos)
            return Constants::FRAGMENT_TYPE::FREE_ATOM;

        return Constants::FRAGMENT_TYPE::ERROR;
    }
};

#endif
