/*
 *  This file is part of esynth.
 *
 *  esynth is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  esynth is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with esynth.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <cstring>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <cctype>

#include "Brick.h"
#include "Utilities.h"
#include "BrickConnectableAtom.h"

Brick::Brick(OpenBabel::OBMol *obmol, const std::string &name) : Molecule(obmol, name)
{
    //
    // Acquire the comment data, make a copy, parse that comment.
    //
    OpenBabel::OBCommentData *comment =
        static_cast<OpenBabel::OBCommentData *>(obmol->GetData("Comment"));

    std::string commentStr = comment->GetData();

    parseAppendix(commentStr, obmol->NumAtoms());
}

void Brick::parseAppendix(std::string &suffix, int numAtoms)
{
    // Use a string stream instead of manipulatiing the string
    std::stringstream suffStream(suffix);
    std::string line = "";

    //
    // Read until we get "> <"
    //
    while (line.find("> <ATOMTYPES") == std::string::npos &&
           line.find(">  <ATOMTYPES") == std::string::npos)
    {
        getline(suffStream, line);
        if (line == "$$$$")
        {
            valid = false;
            return;
        }
    }

    //
    // Read the Atom Types into a temporary
    //
    std::string atomType;
    std::vector<std::string> atomTypes;
    for (int x = 0; x < numAtoms; x++)
    {
        suffStream >> atomType;
        atomTypes.push_back(atomType);
    }

    // Get the next line.
    getline(suffStream, line);

    //
    // Read until we get "> <"
    //
    while (line.find("> <BRANCH") == std::string::npos &&
           line.find(">  <ATOM_NUMBER") == std::string::npos &&
           line.find("> <ATOM_NUMBER") == std::string::npos)
    {
        getline(suffStream, line);
        if (line == "$$$$")
        {
            valid = false;
            return;
        }
    }

    //
    // Read Branches
    //

    if (atomTypes.empty())
    {
        valid = false;
        return;
    }

    // Parallels the atom arrays
    std::vector<std::string> *conns = new std::vector<std::string>[atomTypes.size()];

    int atomId = -1;
    while (!isspace(suffStream.peek()))
    {
        suffStream >> atomId;

        while (suffStream.peek() != '\n' && suffStream.peek() != '\r')
        {
            suffStream >> atomType;

            conns[atomId - 1].push_back(atomType);

            eatWhiteToNewLineOrChar(suffStream);
        }

        // Get the newline
        suffStream.get();
    }

    //
    // Read through the $$$$
    //
    while (line.find("$$$$") == std::string::npos)
    {
        getline(suffStream, line);
    }

    //
    // Actually create the atoms for this molecule.
    //
    for (int a = 0; a < numAtoms; a++)
    {
        if (conns[a].empty())
            this->atoms.push_back(new Atom(atomTypes[a]));
        else
        {
            this->atoms.push_back(new BrickConnectableAtom(atomTypes[a], this, conns[a]));
            conns[a].clear();
        }
    }

    delete[] conns;

   // TODO
   // If this is a 'merged' fragment, identify how many occurences
   // are required for complete, unique reconstruction
   // set : this->_numOccurencesForUnique
}
