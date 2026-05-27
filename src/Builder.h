/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _BUILDER_GUARD
#define _BUILDER_GUARD 1


#include <vector>
#include <map>
#include <queue>
#include <iostream>
#include <memory>

#include "Molecule.h"
#include "Brick.h"
#include "Linker.h"
#include "EdgeAnnotation.h"
#include "IdFactory.h"
#include "OBWriter.h"
#include "bloom_filter.hpp"
#include "OTFValidators.h"


class Builder
{
  private:
    // Create necessary synthesis containers and init the linkers and rigids
    void InitializeBaseMolecules(const std::vector<Brick*>& rigids,
                                 const std::vector<Linker*>& linkers,
                                 std::vector<Molecule*>& baseMolecules);

    void InitializeSynthesis(std::vector<Linker*>& linkers, std::vector<Brick*>& rigids);

    // To generate unique molecular ids
    IdFactory moleculeIDFactory;

    // debug stream
    std::ostream& ds;

    void HandleNewMolecules(std::queue<Molecule*>& worklist,
                            bloom_filter* const levelFilter,
                            std::vector<EdgeAggregator*>* newEdges);

    void SynthesizeWithMolecule(const Molecule* const currentMol, int level);

    bool skipWithUniqueBuild(const Molecule* const mol, int frag_index, int allowed_frags) const;

    void InitOverallFilter();
    void InitLevelFilters();

    // Indicator that a level has completed processing.
    bool* completed_level;

    // The actual producer-consumer queue for each level.
    std::queue<Molecule*>* level_queues;

    // A bloom filter for each level beyond.
    std::vector<bloom_filter*> filters;

    // A bloom filter for each level beyond.
    bloom_filter* overall_filter;

    // set of linkers and rigids (1-molecules)
    std::vector<Molecule*> baseMolecules;

    // Molecules per level (count) for debug
    int* moleculeLevelCount;
    unsigned long long overallMoleculeCount;

    // For output of molecules on the fly.
    OBWriter* const writer;

    // The maximum number of molecules allowable in a queue.
    static const unsigned MAX_QUEUE_SIZES[22];

    // The expected number of molecules in a level, at maximum.
    static const unsigned long long LEVEL_SIZES[22]; 

    // On the fly validation of molecules synthesized;
    // Exits if the validation molecule was generated.
    void Validate(const std::string& syn_smi) const;

    // The molecule to validate
    std::string validation_smi;

    OTFValidators* on_the_fly_validators; 

  public:
    Builder(OBWriter* obWriter = 0, std::ostream& out = std::cout, OTFValidators* validators = nullptr);

    ~Builder()
    {
        delete[] level_queues;
        delete[] moleculeLevelCount;

        // Delete the Bloom filters.
        delete overall_filter;
        for (std::vector<bloom_filter*>::iterator it = filters.begin(); it != filters.end(); it++)
        {
            if (*it != 0) delete *it;
        }
        filters.clear();
    }

    // Main build function for all linkers and bricks;
    void SerialBuild(std::vector<Linker*>& linkers, std::vector<Brick*>& bricks);

    // Recursive assistant for serial processing.
    void SerialBuildHelper(unsigned level, unsigned& processedMols);

    unsigned getIncluded() const { return overallMoleculeCount; }
};

#endif
