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
#include <queue>
#include <iostream>
#include <memory>
#include <time.h>
#include <map>
#include <algorithm>


#include "Molecule.h"
#include "Brick.h"
#include "Linker.h"


#include "EdgeAggregator.h"


#include "Builder.h"
#include "OBWriter.h"
#include "Utilities.h"
#include "IdFactory.h"
#include "Constants.h"
#include "OBWriter.h"
#include "Options.h"
#include "bloom_filter.hpp"
#include "OTFValidators.h"



// 0 indicates we let the queue size be limitless.
const unsigned Builder::MAX_QUEUE_SIZES[22] = { 0,   // Level 0
0,   //       1
300, //       2
10,  //       3
200, //       4
300, //       5
500, //       6
500, //       7
500, //       8
1000,//       9
1000,//       10
1000,//       11
1000,//       12
500, //       13
500, //       14
500, //       15
500, //       16
500, //       17
500, //       18
500, //       19
500, //       20
1    //       21
};

// The anticipated sizes of the level (at max). 0 indicates we are not using a Bloom filter.
const unsigned long long Builder::LEVEL_SIZES[22] = { 0,       // Level 0
0,       //       1
500,     //       2
10000,   //       3
300000,  //       4
1000000, //       5
5000000, //       6
15000000,//       7
30000000,//       8
30000000,//       9
30000000,//       10
15000000,//       11
5000000, //       12
2500000, //       13
1000000, //       14
500000,  //       15
100000,  //       16
50000,   //       17
25000,   //       18
10000,   //       19
5000,    //       20
1000     //       21
};


Builder::Builder(OBWriter* obWriter, std::ostream& out, OTFValidators* OTFvalidators): 
	ds(out),
	overallMoleculeCount(0),
	writer(obWriter),
	excluded(0),
	on_the_fly_validators(OTFvalidators)

{
	level_queues = new std::queue<Molecule*>[Options::USER_DEFINED_LEVEL_BOUND + 1];
	moleculeLevelCount = new int[Options::USER_DEFINED_LEVEL_BOUND + 1];

	for (unsigned m = 1; m <= Options::USER_DEFINED_LEVEL_BOUND; m++)
	{
		// We have create 0 molecules at this level, thus far.
		moleculeLevelCount[m] = 0;
	}

	// Create the bloom filters
	InitOverallFilter();
	InitLevelFilters();
}

//
// Initialize the Bloom filter among all levels
//
void Builder::InitOverallFilter()
{
	bloom_parameters parameters;

	// How many elements roughly do we expect to insert?
	// Count the approximated level sizes
	unsigned long long approx_count = 0;
	for (unsigned m = 0; m <= Options::USER_DEFINED_LEVEL_BOUND + 1; m++)
	{
		approx_count += LEVEL_SIZES[m];
	}
	parameters.projected_element_count = approx_count;

	// Maximum tolerable false positive probability? (0,1)
	parameters.false_positive_probability = 0.01; // 1%

												  // Simple randomizer (optional)
	parameters.random_seed = 0xA5A5A5A5;

	if (!parameters)
	{
		std::cerr << "Error - Invalid set of bloom filter parameters!"
			<< std::endl;
		return;
	}

	parameters.compute_optimal_parameters();

	// Create the Bloom filter.
	overall_filter = new bloom_filter(parameters);
}

//
// Initialize the Bloom filter at each level
//
void Builder::InitLevelFilters()
{
	bloom_parameters parameters;

	// Maximum tolerable false positive probability? (0,1)
	parameters.false_positive_probability = 0.001; // 1%

												   // Simple randomizer (optional)
	parameters.random_seed = 0x5A5A5A5A;

	if (!parameters)
	{
		std::cerr << "Error - Invalid set of bloom filter parameters!" << std::endl;
	}

	//
	// Create the level filters
	//
	for (unsigned m = 0; m <= Options::USER_DEFINED_LEVEL_BOUND + 1; m++)
	{
		if (LEVEL_SIZES[m] == 0)
		{
			filters.push_back(0);
		}
		else
		{
			// How many elements roughly do we expect to insert?
			parameters.projected_element_count = LEVEL_SIZES[m];

			parameters.compute_optimal_parameters();

			filters.push_back(new bloom_filter(parameters));
		}
	}
}

//
// We first construct the base case of 2-Molecules.
// Then, we inductively start constructing 3-Molecules, 4-Molecules, etc.
//
void Builder::SerialBuild(std::vector<Linker*>& linkers,
	std::vector<Brick*>& bricks)
{
	//
	// Synthesizes level 2 molecules using SMI comparison.
	//
	InitializeSynthesis(linkers, bricks);

	// Indicate size of 1-M lists
	moleculeLevelCount[1] = baseMolecules.size();

	std::cout << std::endl << "Stating Synthesis" << std::endl;

	//
	// Using the level 2 molecules as a base case, process indicating non-completion.
	//
	unsigned molsProcessed = 0;
	while (!level_queues[2].empty())
	{
		SerialBuildHelper(2, molsProcessed);
	}

	//
	// Kill all levels
	//
	for (unsigned m = 2; m <= Options::USER_DEFINED_LEVEL_BOUND; m++)
	{
		// The Bloom Filter is no longer needed at this level.
		delete filters[m];
		filters[m] = 0;
	}

	std::cout << "Level\t" << "# Molecules" << std::endl;
	for (unsigned m = 1; m <= Options::USER_DEFINED_LEVEL_BOUND; m++)
	{
		std::cout << m << "\t" << moleculeLevelCount[m] << std::endl;
	}

	// Tell the output engine we have completed synthesis.
	this->writer->IndicateSynthesisComplete();
}

//
// Given the current level, generate molecules in (level + 1) up to the
// capacity specified.
// When the capacity is exceeded, call this function recursively to process
// (level + 1)
// ...inductive completion. 
//
void Builder::SerialBuildHelper(unsigned level,
	unsigned& processedMols)
{
	std::cerr << "Processing level " << level << std::endl;

	//
	// We max out at a specific level
	//
	if (level >= Options::USER_DEFINED_LEVEL_BOUND)
	{
		// Kill the contents of the queue
		while (!level_queues[level].empty())
		{
			Molecule* currentMol = level_queues[level].front();
			level_queues[level].pop();
			delete currentMol;
		}

		// Leave; no need to process.
		return;
	}

	//
	// Completely process all molecules in this level into level + 1
	//
	while (!level_queues[level].empty())
	{
		//
		// Adhere to capacities specified for each level
		//
		while (MAX_QUEUE_SIZES[level + 1] == 0 ||
			level_queues[level + 1].size() < MAX_QUEUE_SIZES[level + 1])
		{
			//
			// Take a molecule from this level queue.
			//
			Molecule* currentMol = level_queues[level].front();
			level_queues[level].pop();

			moleculeLevelCount[level]++;

			if (++processedMols % 1000 == 0 || level <= 6)
			{
				std::cerr << "Processing molecule " << moleculeLevelCount[level]
					<< " at level " << level
					<< " queue contains (" << level_queues[level].size()
					<< "); Overall Processed Count: "
					<< processedMols << std::endl;
			}

			// Dump the processed histogram of molecules
			if (processedMols % 1000000 == 0)
			{
				std::cerr << "Level\t" << "# Molecules" << std::endl;
				for (unsigned m = 2; m <= Options::USER_DEFINED_LEVEL_BOUND; m++)
				{
					std::cerr << m << "\t" << moleculeLevelCount[m] << std::endl;
				}
			}

			SynthesizeWithMolecule(currentMol, level);

			// Delete the current molecule; it has been processed completely.
			// Eliminate this code if we wish to kill an entire level, not molecule by molecule
			delete currentMol;

			// If nothing left to process at this level, quit and go to next levels above.
			if (level_queues[level].empty()) break;
		}

		//
		// Recursively process (level + 1)
		//
		SerialBuilderHelper(level + 1, processedMols);
	}
}


//
// Creates the 2-molecules and initializes the fragments.
//
void Builder::InitializeSynthesis(std::vector<Linker*>& linkers,
	std::vector<Brick*>& bricks)
{
	this->writer->IndicateSynthesisStarted();

	InitializeBaseMolecules(bricks, linkers, baseMolecules);

	// // Add  all the base molecules to the hypergraph
	// foreach_molecules(m_it, baseMolecules)
	// {
	// 	graph->addNode((*m_it)->ConstructMinimalMolecule(), 1);
	// }

	//
	// Construct the set of 2-Molecules from the bricks and linkers.
	//
	for (unsigned m1 = 0; m1 < baseMolecules.size(); m1++)
	{
		for (unsigned m2 = m1; m2 < baseMolecules.size(); m2++)
		{
			// Limit synthesis to unique fragments CTA: 6 / 2024
	        if (Options::ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD && m1 == m2) continue;

			std::vector<EdgeAggregator*>* newEdges =
				baseMolecules[m1]->Compose(*baseMolecules[m2]);

			HandleNewMolecules(level_queues[2], filters[2], newEdges);
		}
	}

	std::cerr << "Done creating level 2" << std::endl;
}

//
// Add all new deduced clauses to the worklist if they have not been deduced before.
// If the given clause has been deduced before, update the hyperedges that were generated
// previously
//
// Forward Instantiation does not permit any cycles in the resultant graph.
//
void Builder::HandleNewMolecules(std::queue<Molecule*>& worklist,
	bloom_filter* const levelFilter,
	std::vector<EdgeAggregator*>* newEdges)
{
	// Consider adding only if there are, in fact, new molecules
	if (newEdges->empty())
	{
		delete newEdges;
		return;
	}

	//
	// Since all molecules we have deduced are of the same size (using a level-based
	// construction), the size of the molecules are the same (equal num fragments)
	//
	unsigned level = (*newEdges->begin())->consequent->size();

	//
	// Add all molecules to the hypergraph
	//
	for (std::vector<EdgeAggregator*>::const_iterator e_it = newEdges->begin();
		e_it != newEdges->end();
		e_it++)
	{
		// Did we generate this molecule previously?
		bool killMolecule = false;

		// SMI for this molecule
		std::string smi = (*e_it)->consequent->ConstructSMI();

		// Validate 
		if (Options::OTF_VALIDATE && on_the_fly_validators) on_the_fly_validators->validate(smi);

		//
		// Check the memory-less dictionary for this level
		//
		static unsigned overall_filtered = 0;
		if (levelFilter->contains(smi))
		{
			killMolecule = true;
		}
		//
		// Check the filter that applies to ALL molecules
		//
		else if (overall_filter->contains(smi))
		{
			killMolecule = true;

			if (++overall_filtered % 100 == 0)
			{
				std::cerr << "Overall filtered: " << overall_filtered << std::endl;
			}
		}

		//
		// EXCLUDE
		//
		if (killMolecule)
		{
			delete (*e_it)->consequent;
		}

		//
		// INCLUDE
		//
		else
		{
			overallMoleculeCount++;

			// Add to the level bloom filters
			levelFilter->insert(smi);

			// Add to the overall bloom filter
			overall_filter->insert(smi);

			// Validation does not require output
			// validation on the fly still outputs the smi - 6/28/2022
			//if (!VALIDATE) 
			this->writer->OutputMoleculeAppendExternalSMI(smi);

			worklist.push((*e_it)->consequent);
		}

		// We are done with this edge structure; delete it.
		delete (*e_it);
	}

	// Kill the edge list itself.
	delete newEdges;
}

//
// On the fly validation of molecules synthesized;
// Exits if the validation molecule was generated.
//
void Builder::Validate(const std::string& syn_smi) const
{
	// Convert
	if (syn_smi != validation_smi) return;

	std::cerr << "The give molecule has been synthesized: " << std::endl;
	std::cerr << "Validation: |" << validation_smi << "|" << std::endl;
	std::cerr << "Synthesized: |" << syn_smi << "|" << std::endl;

	std::cerr << "Exiting..." << std::endl;
	exit(0);
}


//
// Initialize the linkers and bricks as required; the baseMolecules list will then be
// used as a reference container throughout synthesis.
//
void Builder::InitializeBaseMolecules(const std::vector<Brick*>& bricks,
	const std::vector<Linker*>& linkers,
	std::vector<Molecule*>& baseMolecules)
{
	// Clear the list just in case.
	baseMolecules.clear();

	// Assign the linkers and bricks unique ids; these correspond EXACTLY to the indices of
	// the containers used for determing molecular (non)-isomorphism.
	foreach_bricks(r_it, bricks)
	{
		(*r_it)->setUniqueIndexID(moleculeIDFactory.getNextId());
		baseMolecules.push_back(*r_it);
	}

	foreach_linkers(l_it, linkers)
	{
		(*l_it)->setUniqueIndexID(moleculeIDFactory.getNextId());
		baseMolecules.push_back(*l_it);
	}

	std::cout << std::endl << baseMolecules.size() << " fragments read." << std::endl;

	// The set of base molecules is static in the synthesis process; therefore,
	// we set the (static) reference base set of molecules in the Molecule class
	// so the corresponding molecular fingerprint graph can be constructed and compared.
	Molecule::SetBaseMoleculeInfo(baseMolecules, bricks.size(), linkers.size());

	// Each molecule will contain a reference count of the number of each specific
	// linker / brick in the particular molecule.
	foreach_molecules(m_it, baseMolecules)
	{
		(*m_it)->initFragmentDevices();
		(*m_it)->initGraphRepresentation();
	}
}

//
// Takes a single molecule and composes it with the base molecules to create the next level
// molecule.
//
void Builder::SynthesizeWithMolecule(const Molecule* const currentMol, int level)
{
	// CTA: 6 / 2024 currentMol->printConstituentFragments();

	//
	// Compose with all of the base molecules
	//
	for (unsigned m = 0; m < baseMolecules.size(); m++)
	{
		// CTA: 6 / 2024
		// Disallow multiple copies of the same fragment when specified by the user
        if (Options::ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD && 
		    currentMol->hasFragment(baseMolecules[m]->getUniqueIndexID())) continue;

        std::vector<EdgeAggregator*>* newEdges = currentMol->Compose(*baseMolecules[m]);

		//
		// Add the molecule to the next level queue; this depends on the level
		//
		HandleNewMolecules(level_queues[level + 1], filters[level + 1], newEdges);
	}
}