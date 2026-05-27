/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _LIKE_MOLECULES_CONTAINER_GUARD
#define _LIKE_MOLECULES_CONTAINER_GUARD 1

#include <cstring>

#include "MinimalMolecule.h"
 
class LikeMoleculesContainer
{
  private:
    std::vector<MinimalMolecule* > table;

  public:
    LikeMoleculesContainer() : table() { }
	
    //
    // Delete all entries
    //
    virtual ~LikeMoleculesContainer()
    {
        for (unsigned i = 0; i < table.size(); i++)
        {
            // std::cerr << "Deleting MinimalMol " << i << std::endl;
            delete table[i];
        }
        table.clear();
    }
	
    MinimalMolecule* contains(MinimalMolecule* const that) const
    {
        for (std::vector<MinimalMolecule*>::const_iterator it = table.begin();
	     it != table.end();
	     it++)
        {
	    if ((*it)->equals(that)) return *it;
        }

        return 0;
    }
	
    // We assume a call to contains has already been made (as to limit the
    // number of graph isomorphisms)
    void add(MinimalMolecule* const that)
    {
        table.push_back(that);
    }
 
    // Are all the molecules in this container defined by the given key value?
    bool definesKey(char* const thatKey)
    {
        return strcmp((*(table.begin()))->key, thatKey) == 0;
    }
};

#endif
