/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _BOND_GUARD
#define _BOND_GUARD 1


class Bond
{
  private:
    typedef struct CompressedBondT
    {
        unsigned originAtomID : 15;
        unsigned targetAtomID : 15;
        unsigned order        : 2;
    } BondT;

    BondT theBond;

  public:
    //
    // Get Functions
    //
    int getOriginAtomID() const { return this->theBond.originAtomID; }
    int getTargetAtomID() const { return this->theBond.targetAtomID; }
    unsigned int getOrder() const { return this->theBond.order; }

    Bond(const Bond&, unsigned offset = 0);
    Bond(unsigned origin, unsigned target, unsigned order);
    ~Bond() {}

    std::string toString() const;
    friend std::ostream& operator<< (std::ostream& os, Bond& bond);

    bool operator==(const Bond& that) const
    {
        if (this->theBond.originAtomID != that.theBond.originAtomID) return false;

        if (this->theBond.targetAtomID != that.theBond.targetAtomID) return false;

        return true;
    }
};

#endif
