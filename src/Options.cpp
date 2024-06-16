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

#include <string>

#include "Options.h"
#include "Constants.h"

//
// v2.0
//
// constructor-like funtion for default initializations
void Options::init()
{
    Options::SMI_ONLY = false;
    Options::FA_FILES = false;
    Options::SERIAL = true;
    Options::THREADED = false;

    Options::OUTPUT_FILE = Constants::DEFAULT_OUTPUT_FILE;
    Options::OUTPUT_SMI_FILE = Constants::DEFAULT_OUTPUT_SMI_FILE;
    Options::OUTPUT_DIRECTORY = Constants::DEFAULT_OUTPUT_DIR;

    Options::OTF_VALIDATE = false;
    Options::VALIDATION_FILE = "";

    Options::TANIMOTO = Constants::DEFAULT_TANIMOTO;
    Options::PROBABILITY_PRUNE_LEVEL_START = Constants::DEFAULT_PROBABILITY_PRUNE_LEVEL_START;
    Options::USER_DEFINED_LEVEL_BOUND = Constants::MAX_SYNTH_LEVEL_BOUND;

    // CTA: 6 / 2024
    Options::ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD = Constants::DEFAULT_ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD;

    //
    // v1.0
    //
    Options::shmPath = "/run/shm";
    Options::writerPath = "./";
    Options::THREADED = false;

    Options::OPENBABEL = true;

    Options::USE_LIPINSKI = false;
    Options::OBGEN_THREAD_POOL_SIZE = 15;
    Options::SA_THRESHOLD = 5.0;
}

bool Options::SMI_ONLY = false;
bool Options::FA_FILES = false;
bool Options::SERIAL = true;

std::string Options::OUTPUT_FILE = Constants::DEFAULT_OUTPUT_FILE;
std::string Options::OUTPUT_SMI_FILE = Constants::DEFAULT_OUTPUT_SMI_FILE;
std::string Options::OUTPUT_DIRECTORY = Constants::DEFAULT_OUTPUT_DIR;

bool Options::OTF_VALIDATE = false;
std::string Options::VALIDATION_FILE = "";

double Options::TANIMOTO = Constants::DEFAULT_TANIMOTO;
unsigned Options::PROBABILITY_PRUNE_LEVEL_START = Constants::DEFAULT_PROBABILITY_PRUNE_LEVEL_START;
unsigned Options::USER_DEFINED_LEVEL_BOUND = Constants::MAX_SYNTH_LEVEL_BOUND;
bool Options::ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD = Constants::DEFAULT_ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD; // CTA: 6 / 2024

//
// v1.0
//
std::string Options::shmPath = "/run/shm";
std::string Options::writerPath = "./";
bool Options::THREADED = false;

bool Options::OPENBABEL = true;

bool Options::USE_LIPINSKI = false;
unsigned Options::OBGEN_THREAD_POOL_SIZE = 15;
// unsigned Options::SMI_LEVEL_BOUND = 3;
double Options::SA_THRESHOLD = 5.0;