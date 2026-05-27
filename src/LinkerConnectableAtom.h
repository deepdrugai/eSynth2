/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _LINKER_CONNECTABLE_ATOM_GUARD
#define _LINKER_CONNECTABLE_ATOM_GUARD 1


#include <string>
#include <iostream>


#include "AtomT.h"
#include "ConnectableAtom.h"


class Linker;


class LinkerConnectableAtom : public ConnectableAtom
{
  protected:

  public:
    bool CanConnectToAny() const { return true; }
    bool CanConnectTo(const Atom& that) const;

    virtual bool IsLinkerAtom() const { return true; }
    virtual bool IsBrickAtom() const { return false; }

    LinkerConnectableAtom(const LinkerConnectableAtom* const);
    LinkerConnectableAtom(int maxConn, const std::string&, const Linker* const owner);
    ~LinkerConnectableAtom();

    std::string toString() const;
    friend std::ostream& operator<< (std::ostream& os, LinkerConnectableAtom& atom);
};

#endif
