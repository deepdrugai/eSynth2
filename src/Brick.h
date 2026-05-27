/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _BRICK_GUARD
#define _BRICK_GUARD 1

#include <vector>

#include <openbabel/mol.h>

#include "EdgeAggregator.h"
#include "Molecule.h"

class Brick : public Molecule
{
  public:
    Brick(OpenBabel::OBMol* obmol, const std::string& name);
    Brick() {}
    ~Brick() {}

    virtual bool IsLinker() const { return false; }
    virtual bool IsComplex() const { return false; }
    virtual bool IsBrick() const { return true; }

    bool operator==(const Brick& that) const
    {
        return this->getUniqueIndexID() == that.getUniqueIndexID();
    }

  protected:  
    virtual void parseAppendix(std::string& suffix, int numAtoms = -1);
};

#endif
