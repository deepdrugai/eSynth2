/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#ifndef _BRICK_CONNECTABLE_ATOM_GUARD
#define _BRICK_CONNECTABLE_ATOM_GUARD 1


#include <string>
#include <vector>


#include "AtomT.h"
#include "ConnectableAtom.h"


class Brick;


class BrickConnectableAtom : public ConnectableAtom
{
	protected:
		// Allowable atom types for connections
		AtomT** allowableTypes;

	public:
		bool CanConnectToAny() const { return false; }
		bool CanConnectTo(const Atom& that) const;

		BrickConnectableAtom(const BrickConnectableAtom* const that);
		BrickConnectableAtom(const std::string&, const Brick* const owner, const std::vector<std::string>& types);
		~BrickConnectableAtom();

		 // CTA 2/2024 : a connection has been made with of Atom of aType; remove this possibility
		bool reduceAtomType(const AtomT& aType);

		bool IsLinkerAtom() const { return false; }
		bool IsBrickAtom() const { return true; }

		AtomT** getAllowableTypes() const { return allowableTypes; }
		unsigned getNumAllowableTypes() const { return theAtom.numAllowConns; }

		std::string toString() const;
		friend std::ostream& operator<< (std::ostream& os, BrickConnectableAtom& atom);
};

#endif
