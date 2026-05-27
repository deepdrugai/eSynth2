/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _CONNECTABLE_ATOM_GUARD
#define _CONNECTABLE_ATOM_GUARD 1

#include <string>
#include <vector>
#include <algorithm>
#include <map>  // for std::pair


#include "AtomT.h"
#include "Atom.h"


typedef unsigned MoleculeT;
typedef unsigned BooleanT;


class Molecule;


class ConnectableAtom : public Atom
{
  protected:
    typedef struct CompressedAtomT
    {
        // Unique connection identifier for graph sub-node connecting
        int connectionID : 20;

        // The actual (index) connections to external linkers or rigids.
        int numExternalConnections: 4;

        // The maximum number of connections to this atom allowable.
        int maxConnect : 4;

        // The number of connections: to be used ONLY by RigidConnectableAtoms
        unsigned numAllowConns : 4;

    } SimpleAtomT; 

    SimpleAtomT theAtom;

    // Original linker / rigid owner 
    const Molecule* ownerFragment;

  public:
    const AtomT& getAtomType() const { return this->atomType; }
    int getMaxConnect() const { return this->theAtom.maxConnect; }

    virtual void setConnectionID(unsigned id) { theAtom.connectionID = id; }
    int getConnectionID() const { return theAtom.connectionID; }

    virtual bool CanConnectToAny() const { throw "Should not be called."; }
    virtual bool SpaceToConnect() const { return theAtom.maxConnect > theAtom.numExternalConnections; }

    virtual bool CanConnectTo(const Atom& that) const { throw "Should not be called."; }

    virtual bool IsLinkerAtom() const { throw "Should not be called."; }
    virtual bool IsBrickAtom() const { throw "Should not be called."; }

    virtual bool IsSimple() const { return false; }
    virtual bool IsConnectable() const { return true; }

    const Molecule* getOwnerFragment() const { return ownerFragment; }

    ConnectableAtom();
    virtual ~ConnectableAtom();

    void addExternalConnection(); 
};

#endif
