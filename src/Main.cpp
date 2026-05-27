/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdio>
#include <sstream>
#include <cstdlib>
#include <cctype>

//
// Open Babel
//
// #include <openbabel/obconversion.h>
// #include <openbabel/mol.h>
// #include <openbabel/generic.h>
// #include <openbabel/atom.h>
// #include <openbabel/bond.h>
// #include <openbabel/groupcontrib.h>

//
// This project molecular representation
//
#include "Atom.h"
#include "Bond.h"
#include "Molecule.h"
#include "Brick.h"
#include "Linker.h"

//
// File processing in / out.
//
#include "OBWriter.h"
#include "Options.h"
#include "Validator.h"

// CHANGE: Added On the Fly Validators
#include "OTFValidators.h"

//
// Synthesis-Based Functionality
//
#include "EdgeAnnotation.h"
#include "Builder.h"

#include "Utilities.h"
#include "IdFactory.h"
#include "Constants.h"

#include "InputFacade.h"
#include "CommandLineParser.h"

//
// Global set of linkers and bricks read from the input files.
//

void Cleanup(const std::vector<Brick *> &bricks,
             const std::vector<Linker *> &linkers);

bool shortCircuitUniqueBuild(const std::vector<Brick*>& bricks,
                             const std::vector<Linker*>& linkers);

void reportBuild(const std::map<std::string, Molecule*>& file_mol_map);

int main(int argc, const char **argv)
{
	if (argc < 2)
	{
		CommandLineParser::usage();
		return 1;
	}

	std::vector<Linker *> linkers{};
	std::vector<Brick *> bricks{};

	Options::init();
	InputFacade inf{argc, argv};

	if (!inf.parse())
		return 0;

	bricks = inf.getBricks();
	linkers = inf.getLinkers();

	if (shortCircuitUniqueBuild(bricks, linkers))
	{
		std::cerr << "Single fragment specified with 1 occurrence;" << std::endl
		          << "unique fragment-based construction will not execute."
		          << std::endl;
		return 0;
	}

	if (Options::ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD)
		reportBuild(inf.getFileMoleculeMap());

	OBWriter* writer = new OBWriter();
	writer->InitializeFile(Options::OUTPUT_SMI_FILE);

	OTFValidators validators(Options::VALIDATION_FILE);
	Builder builder(writer, std::cout, &validators);

	builder.SerialBuild(linkers, bricks);

	validators.writeToFiles();

	std::cout << "Included (" << builder.getIncluded() << ")" << std::endl;

	delete writer;

	Cleanup(bricks, linkers);
	std::cerr << "Exiting." << std::endl;

	return 0;
}

void Cleanup(const std::vector<Brick*>& bricks, const std::vector<Linker*>& linkers)
{
	for (unsigned r = 0; r < bricks.size(); r++)
	{
		delete bricks[r];
	}

    for (unsigned ell = 0; ell < linkers.size(); ell++)
	{
		delete linkers[ell];
	}
}

// When we only are interested in unique fragment construction,
// we can short circuit all other operations.
bool shortCircuitUniqueBuild(const std::vector<Brick*>& bricks,
                             const std::vector<Linker*>& linkers)
{
	// If this is not a unique build, this logic does not apply
	if (!Options::ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD) return false;

    // Do we have more than one fragment?
    if (bricks.size() + linkers.size() != 1) return false;

    // Do we have just one fragment, but many occurrences?
	unsigned short int occurrences = 0;
	occurrences += bricks.empty() ? 0 : bricks[0]->getNumOccurrencesForUniqueBuild();
	occurrences += linkers.empty() ? 0 : linkers[0]->getNumOccurrencesForUniqueBuild();

    if (occurrences == 0)
	    std::cerr << "Something went wrong with number of occurrences for unique build;" << std::endl
		          << "Number of occurrences is 0." << std::endl;

    return occurrences <= 1;
}

// When we only are interested in unique fragment construction,
// we can short circuit all other operations.
void reportBuild(const std::map<std::string, Molecule*>& file_mol_map)
{
	std::cout << "For a unique build, we will use the following fragments the specified number of times:"
	          << std::endl;
    for (std::map<std::string, Molecule*>::const_iterator it = file_mol_map.begin();
	     it != file_mol_map.end();
		 it++)
	{
        std::cout << it->first << "\t" << it->second->getNumOccurrencesForUniqueBuild() << std::endl;
	}
}