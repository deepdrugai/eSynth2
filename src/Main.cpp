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

#include <vector>
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

void Cleanup(const std::vector<Linker *> &linkers,
			 const std::vector<Brick *> &bricks);

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

    // When we only are interested in unique fragment construction,
	// we can short circuit all other operations.
    if (Options::ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD && bricks.size() + linkers.size() == 1)
	{
	   std::cerr << "Single fragment specified; unique fragment-based construction will not execute."
	             << std::endl;
       return 0;
	}

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

	Cleanup(linkers, bricks);
	std::cerr << "Exiting." << std::endl;

	return 0;
}

void Cleanup(const std::vector<Linker *> &linkers, const std::vector<Brick *> &bricks)
{
	for (unsigned ell = 0; ell < linkers.size(); ell++)
	{
		delete linkers[ell];
	}

	for (unsigned r = 0; r < bricks.size(); r++)
	{
		delete bricks[r];
	}
}