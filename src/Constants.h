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

#ifndef _CONSTANTS_GUARD
#define _CONSTANTS_GUARD 1

#include <vector>
#include <string>
#include <utility>
#include <map>

class Constants
{
public:
    static const std::string ESYNTH_VERSION;
    static const std::string ESYNTH_EXECUTABLE;

    //
    // Command-line arguments
    //
    // Singleton command-line arguments
    static const std::string CMD_ARG_FA_FILES;
    static const std::string CMD_ARG_USAGE;
    static const std::string CMD_ARG_VERSION;
    static const std::string CMD_ARG_SERIAL;
    static const std::string CMD_ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD;  // CTA: 6/2024
    static const std::string CMD_MERGED_FRAGMENTS_TO_BUILD;
    static const std::string CMD_LOOSE_ATOMTYPE_GENERATION;       // CTA: 9 / 2024 
    static const std::string CMD_ALLOW_LINKER_LINKER_CONNECTIONS; // CTA: 9 / 2024 
    static const std::vector<std::string> CMD_ARGS_SINGLETON;

    // Paired (arg, arg-value) command-line arguments
    static const std::string CMD_ARG_OUTPUT_FILE;
    static const std::string CMD_ARG_OUTPUT_DIR;
    static const std::string CMD_ARG_VALIDATION_FILE;
    static const std::string CMD_ARG_TANIMOTO_COEFF;
    static const std::string CMD_ARG_LEVEL_BOUND;
    static const std::vector<std::string> CMD_ARGS_PAIRED;

    // Help descriptions for each possible argument
    static const std::map<std::string, std::string> USAGE_MAP;

    //
    // Input file arguments
    //
    static const std::string INPUT_SDF_FILE_EXTENSION;
    static const std::string VALIDATION_FILE_EXTENSION;

    static const std::string BRICK_PREFIX;
    static const std::string RIGID_PREFIX;
    static const std::string LINKER_PREFIX;
    static const std::string FREE_ATOM_PREFIX;
    static const std::string UNIQUE_FRAGMENT_PREFIX;
    static const std::string ALL_FRAGMENT_PREFIX;
    static const std::vector<std::string> UNAMBIGUOUS_ACCEPTABLE_INPUT_FILE_PREFIXES;
    static const std::vector<std::string> AMBIGUOUS_ACCEPTABLE_INPUT_FILE_PREFIXES;

    static const std::string ACCEPTABLE_INPUT_LINKER_FILE_SUBSTR;
    static const std::string ACCEPTABLE_INPUT_BRICK_FILE_SUBSTR;
    static const std::string ACCEPTABLE_INPUT_FREE_ATOM_FILE_SUBSTR;

    static const std::vector<std::string> ACCEPTABLE_INPUT_FILE_SUBSTRS;

    typedef enum
    {
        LINKER,
        BRICK,
        FREE_ATOM,
        ERROR
    } FRAGMENT_TYPE;

    //
    // User-defined options
    //
    static std::string DEFAULT_OUTPUT_FILE;
    static std::string DEFAULT_OUTPUT_SMI_FILE;
    static std::string DEFAULT_OUTPUT_DIR;

    static double DEFAULT_TANIMOTO;

    // CTA: 6 / 2024
    static bool DEFAULT_ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD;
    static bool DEFAULT_MERGED_FRAGMENTS_TO_BUILD;

    // CTA: 9 / 2024
    static bool DEFAULT_LOOSE_ATOMTYPE_GENERATION;
    static bool ALLOW_LINKER_LINKER_CONNECTIONS;

    static unsigned MAX_SYNTH_LEVEL_BOUND;
};

const int NOT_FOUND = -1;

// Debugging constants
const bool DEBUG = true;

const bool g_debug_output = false;

#endif