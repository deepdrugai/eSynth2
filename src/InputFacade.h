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

#ifndef _INPUT_FACADE_GUARD_H
#define _INPUT_FACADE_GUARD_H

#include "CommandLineParser.h"
#include "InfileExistenceAnalyzer.h"
#include "InfileNameAnalyzer.h"
#include "FragmentFileParser.h"

class Brick;
class Linker;

class InputFacade
{
public:
    InputFacade(int argc, const char **argv) : _argc{argc}, _argv{argv}
    {
    }

    bool parse()
    {
        //
        // Command line arguments are broken up into:
        //     * user-defined arguments
        //     * input fragment files
        //
        CommandLineParser parser{_argc, _argv};

        // If parsing results in usage or versioning, do not perform any further analyses
        if (!parser.parse())
            return false;

        //
        // Identify the files that exist
        //
        InfileExistenceAnalyzer eAnalyzer{parser.getFiles()};

        auto dne = eAnalyzer.getInvalidFiles(); // non-existent files
        if (!dne.empty())
        {
            std::cerr << "The following paths / files cannot be found:" << std::endl;
            for (auto const &invalid : dne)
            {
                std::cerr << '\t' << invalid << std::endl;
            }
        }

        //
        // Identify the files that meet naming standards.
        //
        InfileNameAnalyzer nameAnalyzer{eAnalyzer.getValidFiles()};

        generalFileEmitter("\nThe following files do not conform to eSynth input naming requirements (and will be ignored):",
                           nameAnalyzer.getInvalidFiles());
        if (Options::FA_FILES)
        {
            if (!nameAnalyzer.getFreeAtomFiles().empty())
            {
                generalFileEmitter("\nThe following files will be skipped because the -skip-fa flag is on: (and will be ignored):",
                                   nameAnalyzer.getFreeAtomFiles());
            }
        }

        FragmentFileParser fragmentParser{nameAnalyzer.getValidFiles()};

        generalFileEmitter("\nThe following files do not conform to eSynth Missing Linkers | Bricks validation: ",
                           fragmentParser.getInvalidFileNames());

        _filteredFiles = fragmentParser.getValidFiles();

        _bricks = fragmentParser.getBricks();
        _linkers = fragmentParser.getLinkers();
        if (_bricks.empty() && _linkers.empty())
        {
            std::cerr << "No valid input files were found." << std::endl;
            return false;
        }

        fragmentFileEmitter("\nThe following fragment files conform to eSynth requirements and will be parsed:");
        return true;
    }

    std::vector<Brick *> _bricks;
    std::vector<Linker *> _linkers;

    std::vector<Brick *> getBricks() const { return _bricks; }
    std::vector<Linker *> getLinkers() const { return _linkers; }

    std::map<std::string, Constants::FRAGMENT_TYPE> getFiles() const { return _filteredFiles; }

protected:
    int _argc;
    const char **_argv;

    std::map<std::string, Constants::FRAGMENT_TYPE> _filteredFiles;

    void generalFileEmitter(const std::string &msg,
                            const std::vector<std::string> &vec) const
    {
        if (vec.empty())
            return;

        std::cerr << msg << std::endl;
        for (auto const &value : vec)
        {
            std::cerr << '\t' << value << std::endl;
        }
    }

    void fragmentFileEmitter(const std::string &msg) const
    {
        if (_filteredFiles.empty())
            return;

        std::cout << msg << std::endl;

        unsigned maxW = Utilities::maxKeyWidth(_filteredFiles);
        for (auto const &f : _filteredFiles)
        {
            std::string fType;
            switch (f.second)
            {
            case Constants::FRAGMENT_TYPE::BRICK:
                fType = Constants::ACCEPTABLE_INPUT_BRICK_FILE_SUBSTR;
                break;
            case Constants::FRAGMENT_TYPE::LINKER:
                fType = Constants::ACCEPTABLE_INPUT_LINKER_FILE_SUBSTR;
                break;
            case Constants::FRAGMENT_TYPE::FREE_ATOM:
                fType = Constants::ACCEPTABLE_INPUT_FREE_ATOM_FILE_SUBSTR + "(treated as linker)";
                break;
            default:
                fType = "ERROR";
            }

            std::cerr << '\t' << std::left << std::setw(maxW + 4) << f.first << '\t' << fType << std::endl;
        }
    }
};

#endif