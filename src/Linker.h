/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _LINKER_GUARD
#define _LINKER_GUARD 1

#include <vector>

#include <openbabel/mol.h>

#include "EdgeAggregator.h"
#include "Molecule.h"

class Linker : public Molecule
{
public:
  Linker(OpenBabel::OBMol *, const std::string &name);
  Linker() {}

  ~Linker() {}

  virtual bool IsLinker() const { return true; }
  virtual bool IsComplex() const { return false; }
  virtual bool IsRigid() const { return false; }

  bool operator==(const Linker &that) const
  {
    return this->getUniqueIndexID() == that.getUniqueIndexID();
  }

protected:
  virtual void parseAppendix(std::string &suffix, int numAtoms = -1);
};

#endif
