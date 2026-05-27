/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

#include <vector>
#include <iostream>
#include <string>
#include <map>
#include <sstream>

#include <openbabel/mol.h>
#include <openbabel/bond.h>

#include "Linker.h"
#include "LinkerConnectableAtom.h"

Linker::Linker(OpenBabel::OBMol *obmol, const std::string &name) : Molecule(obmol, name)
{
    //
    // Acquire the comment data, make a copy, parse that comment.
    //
    OpenBabel::OBCommentData *comment = static_cast<OpenBabel::OBCommentData *>(obmol->GetData("Comment"));

    std::string commentStr = comment->GetData();

    parseAppendix(commentStr, obmol->NumAtoms());
}

//
// Parse suffix to add max connection for each atom.
//
void Linker::parseAppendix(std::string &suffix, int numAtoms)
{
    // std::cerr << "Linker::parseAppendix: " << suffix << "|" << std::endl;

    // Use a string stream instead of manipulatiing the string
    std::stringstream suffStream(suffix);
    std::map<int, std::string> connectMap;

    //
    // Read until we get "> <"
    //
    std::string line = "";

    while (line.find("> <MAX-") == std::string::npos && line.find(">  <MAX-") == std::string::npos)
    {
        getline(suffStream, line);
        if (line == "$$$$")
        {
            valid = false;
            return;
        }
    }

    //
    // Now, read the MAX Connections for each atom.
    //
    int maxConnections = -1;
    std::string atomType;
    int count = 0;

    for (int a = 0; a < numAtoms; a++)
    {
        suffStream >> maxConnections;
        suffStream >> atomType;
        count += maxConnections;
        if (maxConnections < 0 || atomType.empty())
        {
            valid = false;
            return;
        }

        // A linker can link to any atom.
        this->atoms.push_back(new LinkerConnectableAtom(maxConnections, atomType, this));
    }

    if (count <= 0)
    {
        valid = false;
    }

    // If this is a 'merged' fragment, identify how many occurences
    // are required for complete, unique reconstruction
    if (Options::ONLY_USE_UNIQUE_FRAGMENTS_TO_BUILD)
    {
        _numOccurrencesForUnique = parseSimilarFragments(suffStream) + 1; // 1 for this fragment
    }
}
