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

#include "Constants.h"

#include <map>
#include <string>

const std::string Constants::ESYNTH_VERSION = "eSynth v2.0, 8/1/2022";
const std::string Constants::ESYNTH_EXECUTABLE = "eSynth";

//
// Command-line arguments
//
// Singleton command-line arguments
const std::string Constants::CMD_ARG_SMI_ONLY = "smi-only";
const std::string Constants::CMD_ARG_FA_FILES = "skip-fa";
const std::string Constants::CMD_ARG_USAGE = "usage";
const std::string Constants::CMD_ARG_VERSION = "version";
const std::string Constants::CMD_ARG_SERIAL = "serial";
const std::string Constants::CMD_ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD = "unique-build";   // CTA: 6/2024
const std::vector<std::string> Constants::CMD_ARGS_SINGLETON = {Constants::CMD_ARG_SMI_ONLY,
                                                                Constants::CMD_ARG_FA_FILES,
                                                                Constants::CMD_ARG_USAGE,
                                                                Constants::CMD_ARG_VERSION,
                                                                Constants::CMD_ARG_SERIAL,
                                                                Constants::CMD_ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD};

// Paired (arg, arg-value) command-line arguments
const std::string Constants::CMD_ARG_OUTPUT_FILE = "o";
const std::string Constants::CMD_ARG_OUTPUT_DIR = "odir";
const std::string Constants::CMD_ARG_VALIDATION_FILE = "v";
const std::string Constants::CMD_ARG_TANIMOTO_COEFF = "tc";
const std::string Constants::CMD_ARG_LEVEL_BOUND = "hl";
const std::string Constants::CMD_ARG_PROB_LEVEL = "prob-level";
const std::vector<std::string> Constants::CMD_ARGS_PAIRED = {Constants::CMD_ARG_OUTPUT_FILE,
                                                             Constants::CMD_ARG_OUTPUT_DIR,
                                                             Constants::CMD_ARG_VALIDATION_FILE,
                                                             Constants::CMD_ARG_TANIMOTO_COEFF,
                                                             Constants::CMD_ARG_LEVEL_BOUND,
                                                             Constants::CMD_ARG_PROB_LEVEL};

//
// Help descriptions for each possible argument
//
const std::map<std::string, std::string> Constants::USAGE_MAP = {
    std::make_pair(Constants::CMD_ARG_SMI_ONLY, "\tOutput molecules in SMILES format."),
    std::make_pair(Constants::CMD_ARG_FA_FILES, "\tFree Atom Files."),
    std::make_pair(Constants::CMD_ARG_USAGE, "\tThis usage information"),
    std::make_pair(Constants::CMD_ARG_VERSION, "\tVersion information"),
    std::make_pair(Constants::CMD_ARG_SERIAL, "\tSerial execution compared to -threaded"),
    std::make_pair(Constants::CMD_ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD, "\tConstructed molecules will not contain repeated fragments; all input fragments are considered unique"),

    std::make_pair(Constants::CMD_ARG_OUTPUT_FILE, "<file>\tOutput file suffix"),
    std::make_pair(Constants::CMD_ARG_OUTPUT_DIR, "<path>\tPath to the desired output directory."),
    std::make_pair(Constants::CMD_ARG_VALIDATION_FILE, "<file>\tValidation will be performed with the given file"),
    std::make_pair(Constants::CMD_ARG_TANIMOTO_COEFF, "<float>\tTanimoto coefficient used to compare molecules for equivalence"),
    std::make_pair(Constants::CMD_ARG_LEVEL_BOUND, "<int>\tMaxmimum number of fragments allowed in a molecule"),
    std::make_pair(Constants::CMD_ARG_PROB_LEVEL, "\tNumber of fragments in which to start probabilistically omitting molecules.")};

//
// Input file arguments
//
const std::string Constants::INPUT_SDF_FILE_EXTENSION = "sdf";
const std::string Constants::VALIDATION_FILE_EXTENSION = "mol2";

const std::string Constants::BRICK_PREFIX = "b";
const std::string Constants::RIGID_PREFIX = "r";
const std::string Constants::LINKER_PREFIX = "l";
const std::string Constants::FREE_ATOM_PREFIX = "fa";
const std::vector<std::string>
    Constants::UNAMBIGUOUS_ACCEPTABLE_INPUT_FILE_PREFIXES = {Constants::BRICK_PREFIX,
                                                             Constants::RIGID_PREFIX,
                                                             Constants::LINKER_PREFIX,
                                                             Constants::FREE_ATOM_PREFIX};
const std::string Constants::UNIQUE_FRAGMENT_PREFIX = "u";
const std::string Constants::ALL_FRAGMENT_PREFIX = "all";
const std::vector<std::string>
    Constants::AMBIGUOUS_ACCEPTABLE_INPUT_FILE_PREFIXES = {Constants::UNIQUE_FRAGMENT_PREFIX,
                                                           Constants::ALL_FRAGMENT_PREFIX};

const std::string Constants::ACCEPTABLE_INPUT_LINKER_FILE_SUBSTR = "linker";
const std::string Constants::ACCEPTABLE_INPUT_BRICK_FILE_SUBSTR = "brick";
const std::string Constants::ACCEPTABLE_INPUT_FREE_ATOM_FILE_SUBSTR = "free";

const std::vector<std::string> Constants::ACCEPTABLE_INPUT_FILE_SUBSTRS = {Constants::ACCEPTABLE_INPUT_LINKER_FILE_SUBSTR,
                                                                           Constants::ACCEPTABLE_INPUT_BRICK_FILE_SUBSTR,
                                                                           Constants::ACCEPTABLE_INPUT_BRICK_FILE_SUBSTR};

std::string Constants::DEFAULT_OUTPUT_FILE = "molecules.sdf";
std::string Constants::DEFAULT_OUTPUT_SMI_FILE = "molecules.smi";
std::string Constants::DEFAULT_OUTPUT_DIR = "output";

double Constants::DEFAULT_TANIMOTO = 0.95;

// CTA: 6 / 2024
bool Constants::DEFAULT_ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD = false;

unsigned Constants::MAX_SYNTH_LEVEL_BOUND = 20;
unsigned Constants::DEFAULT_PROBABILITY_PRUNE_LEVEL_START = 5;

double Constants::MOLWT_UPPERBOUND = 570;
double Constants::HBD_UPPERBOUND = 5;
double Constants::HBA1_UPPERBOUND = 10;
double Constants::LOGP_UPPERBOUND = 7.2;
