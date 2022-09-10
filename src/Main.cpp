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
#include <mcheck.h>

//
// Open Babel
//
#include <openbabel/obconversion.h>
#include <openbabel/mol.h>
#include <openbabel/generic.h>
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/groupcontrib.h>

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
#include "Instantiator.h"

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

	//
	// The input facade:
	// (1) identifies and set Options in the static Options class
	// (2) Identifies legitimate, existing input fragment files
	Options::init();
	InputFacade inf{argc, argv};

	// If usage or versioning is requested.
	if (!inf.parse())
		return 0;

	// Remove log files from a previous run.
	//
	// v1.0
	//
	// system("rm molecules.smi");
	// system("rm synth_log_initial_fragments_logfile.txt");
	// system("rm ScrubAndExportSMI_logfile.txt");
	// system("rm Validation_logfile.txt");

	bricks = inf.getBricks();
	linkers = inf.getLinkers();

	//
	// Bypass synthesis for acquiring information about the input fragments.
	//
	if (g_calculate_lipinski_descriptors_for_input_fragments_only)
	{
		std::cout << "Calculated Lipinski Descriptors for input fragments, now exiting early."
				  << " (Flag set in Constants.h)" << std::endl;
		return 0;
	}

	// Output object for the nodes of the hypergraph.
	OBWriter *writer = new OBWriter(Options::OBGEN_THREAD_POOL_SIZE);
	if (Options::SMI_ONLY)
	{
		writer->InitializeFile(Options::OUTPUT_SMI_FILE);
	}
	else
		writer->InitializeFile(Options::OUTPUT_FILE);

	// Create On_the_Fly_Validators
	OTFValidators validators(Options::VALIDATION_FILE);

	// The main object that performs synthesis.
	Instantiator instantiator(writer, cout, &validators);

	// Run synthesis
	if (Options::THREADED)
		// instantiator.ThreadedInstantiate(linkers, bricks);
		instantiator.ThreadedInstantiate(linkers, bricks);
	else if (Options::SERIAL)
		// instantiator.SerialInstantiate(linkers, bricks);
		instantiator.SerialInstantiate(linkers, bricks);

	// With instantiation complete, output the TC-based analysis of generated
	// molecule similarity
	validators.writeToFiles();

	unsigned inc = instantiator.getIncluded();
	unsigned exc = instantiator.getExcluded();

	std::cout << "Excluded (" << exc << "); Included (" << inc << ") \t Excluded: "
			  << ((double)(exc) / (exc + inc)) << "\%" << std::endl;

	// Deleting the writer will kill the thread pool.
	delete writer;

	// Cleanup(linkers, bricks);
	Cleanup(linkers, bricks);
	std::cerr << "Exiting the main thread." << std::endl;

	// muntrace();

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