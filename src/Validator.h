/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _VALIDATOR_GUARD
#define _VALIDATOR_GUARD 1


#include <vector>


#include <openbabel/mol.h>


//
// A class to perform validation on the synthesis:
//    Given (1) a hypergraph (formed from linkers and rigids)
//          (2) a set of Open Babel molecules,
//    Verify that the set of molecules are vertices in the hypergraph.
//
class Validator
{
  public:
    Validator(std::vector<OpenBabel::OBMol*>& mols) : molecules(mols) {}
    bool Validate(OpenBabel::OBMol&);
    void Validate(const std::string& fileName);
    void Validate(std::vector<OpenBabel::OBMol>&);

  private:

    std::vector<OpenBabel::OBMol*>& molecules; 
};

#endif
