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

#ifndef _OPTIONS_GUARD
#define _OPTIONS_GUARD 1

#include <string>
#include <vector>
#include <iostream>

#include "Utilities.h"
#include "Constants.h"

//
// A aggregation class for options specified in the command-line
//
struct Options
{
    //
    // v1.0 Constants
    //
    static std::string writerPath;

    std::string outFile;

    static bool OPENBABEL;
    static double SA_THRESHOLD;

    //
    // v2.0 Constants
    //
    static void init(); // constructor-like funtion for default initializations
    static bool FA_FILES;

    static std::string OUTPUT_FILE;
    static std::string OUTPUT_SMI_FILE;
    static std::string OUTPUT_DIRECTORY;

    // As molecules are generated, perform (on-the-fly) validation
    // Once all molecules have been validated, operations should cease.
    static bool OTF_VALIDATE;
    static std::string VALIDATION_FILE;

    static double TANIMOTO;
    static unsigned USER_DEFINED_LEVEL_BOUND;
    static bool ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD; // CTA: 6 / 2024
    static bool MERGED_FRAGMENTS_TO_BUILD; // CTA: 6 / 2024
    static bool LOOSE_ATOMTYPE_GENERATION; // CTA: 9 / 2024

    // FA FILE for Free Atom Files
    static void setFreeAtomFileFlag() { FA_FILES = true; }

    static void setOnlyUseUniqueFragments() { ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD = true; } // CTA: 6 / 2024
    static void setMergedFragmentsToBuild() { MERGED_FRAGMENTS_TO_BUILD = true; } // CTA: 6 / 2024
    static void setLooseGeneration() { LOOSE_ATOMTYPE_GENERATION = true; } // CTA: 9 / 2024

    static void setOutputFile(const std::string &name) { OUTPUT_FILE = name; }
    static void setOutputDirectory(const std::string &name) { OUTPUT_DIRECTORY = name; }
    static void setValidationFile(const std::string &name)
    {
        OTF_VALIDATE = true;
        VALIDATION_FILE = name;
    }

    /*
     * Set the Tanimoto coefficient for molecular 'equivalence'
     */
    static void setTanimotoCoeff(double value)
    {
        if (value < 0 || value > 1)
            return;

        TANIMOTO = value;
    }

    /*
     * Set the upper bound for number of fragments to combine.
     */
    static void setLevelBound(unsigned level)
    {
        if (level > Constants::MAX_SYNTH_LEVEL_BOUND)
            return;

        USER_DEFINED_LEVEL_BOUND = level;
    }
};

#endif