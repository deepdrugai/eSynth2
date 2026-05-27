/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#include <iostream>
//#include <cstring>
#include <sstream>
#include <string>
//#include <vector>
#include <cstdio>


#include "Bond.h"


/******************************************************************************************/

Bond::Bond(const Bond& that, unsigned offset)
{
    theBond.originAtomID = offset + that.theBond.originAtomID;
    theBond.targetAtomID = offset + that.theBond.targetAtomID;
    theBond.order = that.theBond.order;
}



/******************************************************************************************/

Bond::Bond(unsigned origin, unsigned target, unsigned ord)
{
    theBond.originAtomID = origin;
    theBond.targetAtomID = target;
    theBond.order = ord;
}

/*****************************************************************************************/

std::string Bond::toString() const
{
    std::ostringstream oss;

    oss << " from atom " << theBond.originAtomID
        << " to atom " << theBond.targetAtomID;

    return oss.str();
}


/*****************************************************************************************/

std::ostream& operator<< (std::ostream& os, Bond& bond)
{
    os << bond.toString();

    return os;
}

/*****************************************************************************************/
