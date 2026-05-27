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

	OpenBabel::OBCommentData* cData = new OpenBabel::OBCommentData();
	cData->SetAttribute("Comment");
	cData->SetData(suffix);
	mol->SetData(cData);

	//
	// Create this particular molecule type based on the name of the file.
	//
	if (mType == LINKER)
	{
		return new Linker(mol, name);
	}
	else if (mType == BRICK)
	{
		return new Brick(mol, name);
	}

	return 0;
}

void addMolecule(Constants::FRAGMENT_TYPE fType, Molecule* molecule)
{
    switch(fType)
    {
        case Constants::FRAGMENT_TYPE::BRICK:
            bricks.push_back(static_cast<Brick*>(molecule));
            break;

        case Constants::FRAGMENT_TYPE::LINKER:
            linkers.push_back(static_cast<Linker*>(molecule));
            break;

        case Constants::FRAGMENT_TYPE::FREE_ATOM:
			      linkers.push_back(static_cast<Linker*>(molecule));
            // std::cerr << "Internal unexpected FREE_ATOM type." << std::endl;
            break;

        default:
        		std::cerr << "Unrecognized input fragment type." << std::endl;
    }
}

void readMoleculeFile(const char* fileName, Constants::FRAGMENT_TYPE fType)
{
	//
	// Input parser conversion functionality for Open babel
	//
	OpenBabel::OBConversion obConversion;
	obConversion.SetInFormat("SDF");

	bricks = inf.getBricks();
	linkers = inf.getLinkers();

    // Is the input one fragment with one occurrence
    if (shortCircuitUniqueBuild(bricks, linkers))
	{
        std::cerr << "Single fragment specified with 1 occurrence;" << std::endl
		          << "unique fragment-based construction will not execute."
	              << std::endl;
		return 0;
	}

    if (Options::ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD) reportBuild(inf.getFileMoleculeMap());

	// Output object for the nodes of the hypergraph.
	OBWriter* writer = new OBWriter();
	writer->InitializeFile(Options::OUTPUT_SMI_FILE);

	// Create On_the_Fly_Validators
	OTFValidators validators(Options::VALIDATION_FILE);

	// The main object that performs synthesis.
	Builder builder(writer, cout, &validators);

    builder.SerialBuild(linkers, bricks);

	// With building complete, output the TC-based analysis of generated
	// molecule similarity
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