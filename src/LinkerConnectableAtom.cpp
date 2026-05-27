/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#include <iostream>
#include <cstring>
#include <sstream>
#include <string>
#include <vector>


#include "LinkerConnectableAtom.h"
#include "BrickConnectableAtom.h"

#include "Options.h"


LinkerConnectableAtom::LinkerConnectableAtom(const LinkerConnectableAtom* const that)
{
    this->atomType = that->atomType;
    this->ownerFragment = that->ownerFragment;

    this->theAtom.connectionID = that->theAtom.connectionID;
    this->theAtom.maxConnect = that->theAtom.maxConnect;
    this->theAtom.numExternalConnections = that->theAtom.numExternalConnections;
    this->theAtom.numAllowConns = 1;
}

/**********************************************************************************/

LinkerConnectableAtom::LinkerConnectableAtom(int maxConn, const std::string& aType,
                                             const Linker* const owner)
{
    this->atomType = AtomT(aType);
    this->ownerFragment = (Molecule*)owner;

    this->theAtom.connectionID = 0;
    this->theAtom.maxConnect = maxConn;
    this->theAtom.numExternalConnections = 0;
    this->theAtom.numAllowConns = 1;
}

/**********************************************************************************/

LinkerConnectableAtom::~LinkerConnectableAtom()
{
}

/**********************************************************************************/

bool LinkerConnectableAtom::CanConnectTo(const Atom& that) const
{
    if (that.IsSimple()) return false;

    //
    // Disallow Linker-Linker connections.
    //
    if (!Options::ALLOW_LINKER_LINKER_CONNECTIONS)
    {
        if (that.IsLinkerAtom()) return false;
    }

    //
    // Are there any allowable spots in the atoms to connect?
    //
    if(!this->SpaceToConnect()) return false;

    if(!that.SpaceToConnect()) return false;

    // If the linkers have space and linkers can connect, indicate connection possible
    if (that.IsLinkerAtom() && Options::ALLOW_LINKER_LINKER_CONNECTIONS) return true;

    //
    // Does that atom allow the connection to this?
    //
    const BrickConnectableAtom& rAtom = static_cast<const BrickConnectableAtom&>(that);
    AtomT** thatAllowableTypes = rAtom.getAllowableTypes();
    unsigned numAllowableTypes = rAtom.getNumAllowableTypes();

    for (unsigned t = 0; t < numAllowableTypes; t++)
    {
        if ((*thatAllowableTypes[t]) == this->atomType)
        {
            return true;
        }
    }

    return false;
}

/****************************************************************************************/

std::string LinkerConnectableAtom::toString() const
{
    std::ostringstream oss;

    oss << " Connections{ Max: " << theAtom.maxConnect;

    oss << " Allow: All Conn";

    oss << "\tNum ExtBonds: (" << theAtom.numExternalConnections << ")";

    oss << "  Conn Id: (" << theAtom.connectionID << ")";

    oss << " }";

    return oss.str();
}

/****************************************************************************************/

std::ostream& operator<< (std::ostream& os, ConnectableAtom& atom)
{
    os << atom.toString();

    return os;
}

/****************************************************************************************/
