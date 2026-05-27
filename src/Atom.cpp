/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#include <iostream>
#include <cstring>
#include <string>


#include "AtomT.h"
#include "Atom.h"
#include "ConnectableAtom.h"
#include "LinkerConnectableAtom.h"
#include "BrickConnectableAtom.h"
#include "Utilities.h"


/**********************************************************************************/

Atom::Atom(const Atom& that)
{
    this->atomType = that.atomType;
}

/**********************************************************************************/

Atom::Atom(const std::string& aType) : atomType(AtomT(aType))
{
}

/**********************************************************************************/

Atom::Atom(const AtomT& aType) : atomType(aType)
{
}

/**********************************************************************************/

Atom::~Atom()
{
}

/**********************************************************************************/

//
// Take an atom and produce the smallest representation of the atom possible:
//    (a) Convert a fulfilled molecule to a simple Atom.
//    (b) If an unfilled linker or rigid atom, keep a linker or rigid atom. 
//
Atom* Atom::ConstructAtom(const Atom& that)
{
    // To save space in future iterations, have a global pointer to simple atoms and do not allocate here
    if (that.IsSimple()) return new Atom(that);

    // If there is no more space to conenct to this atom, create a simpler version of the atom.
    if (!that.SpaceToConnect()) return new Atom(that.getAtomType());

    //
    // Determine the type of atom: linker / rigid
    //
    if (that.IsLinkerAtom())
    {
        return new LinkerConnectableAtom(static_cast<const LinkerConnectableAtom* const>(&that));
    }
    else if (that.IsBrickAtom())
    {
        return new BrickConnectableAtom(static_cast<const BrickConnectableAtom* const>(&that));
    }
    else
    {
        throw "Expected rigid or linker atom; neither found.";
    }
}

/**********************************************************************************/

bool Atom::operator==(const Atom& that) const
{
    return this->atomType == that.atomType;
}

/****************************************************************************************/

std::string Atom::toString() const
{
    return atomType.toString();
}

/****************************************************************************************/

std::ostream& operator<< (std::ostream& os, Atom& atom)
{
    os << atom.toString();

    return os;
}

/****************************************************************************************/
