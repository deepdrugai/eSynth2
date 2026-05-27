/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#include "ConnectableAtom.h"

ConnectableAtom::ConnectableAtom()
{
}

ConnectableAtom::~ConnectableAtom()
{
}

void ConnectableAtom::addExternalConnection()
{
    theAtom.numExternalConnections++;
}

