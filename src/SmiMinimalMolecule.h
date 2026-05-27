/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _SMI_MINIMAL_MOLECULE_GUARD
#define _SMI_MINIMAL_MOLECULE_GUARD 1


#include <string>


#include "Options.h"
#include "SimpleFragmentGraph.h"


//
// An aggregator that contains the minimal amount of information we require per molecule. 
//
class SmiMinimalMolecule : public MinimalMolecule
{
  public:

    std::string smi;
	
    SmiMinimalMolecule(const std::string& smiStr, SimpleFragmentGraph* const g,
                       unsigned short int* const fcounter, unsigned sz) 
        : MinimalMolecule(g, fcounter, sz), smi(smiStr)
    {
    }

    ~SmiMinimalMolecule() { }

    bool equals(MinimalMolecule* const that) const
    {
        //
        // Compare molecules using the SMILES format: string comparison.
        //
        return this->smi == static_cast<SmiMinimalMolecule* const>(that)->smi;
    }

    std::string toString() const
    {
        std::ostringstream oss;

        oss << "Key |" << *this->key << "|" << std::endl;
        oss << "SMI |" << this->smi << "|" << std::endl;

        oss << "Graph: " << this->fingerprint->toString() << std::endl;

        return oss.str();
    }

    friend std::ostream& operator<< (std::ostream& os, const SmiMinimalMolecule& mol)
    {
        os << mol.toString();

        return os;
    }
};

#endif
