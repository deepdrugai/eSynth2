/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _ATOM_GUARD
#define _ATOM_GUARD 1


#include <string>


#include "AtomT.h"


typedef unsigned MoleculeT;
typedef unsigned BooleanT;


const unsigned BRICK   = 0;
const unsigned LINKER  = 1;
const unsigned COMPLEX = 2;


class Molecule;


class Atom
{
  protected:
    // This atom's type
    AtomT atomType;

  public:
    const AtomT& getAtomType() const { return this->atomType; }

    virtual bool CanConnectToAny() const { return false; }
    virtual bool SpaceToConnect() const { return false; }
    virtual bool CanConnectTo(const Atom& that) const { return false; }
    virtual int getConnectionID() const { return -1; }
    virtual void setConnectionID(unsigned) { throw "Should not be called."; }
    virtual int getMaxConnect() const { return 0; }
    virtual void addExternalConnection() { throw "Should not be called."; }

    static Atom* ConstructAtom(const Atom& atom);

    virtual bool IsSimple() const { return true; }
    virtual bool IsConnectable() const { return false; }

    virtual bool IsLinkerAtom() const { return false; }
    virtual bool IsBrickAtom() const { return false; }

    virtual const Molecule* getOwnerFragment() const { throw "Should not be called."; }

    Atom(const Atom&);
    Atom() {}
    Atom(const AtomT& aType);
    Atom(const std::string& aType);
    virtual ~Atom();

    virtual std::string toString() const;
    friend std::ostream& operator<< (std::ostream& os, Atom& atom);
    bool operator==(const Atom& that) const;
};

#endif
