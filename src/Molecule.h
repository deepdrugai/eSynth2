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

#ifndef _MOLECULE_GUARD
#define _MOLECULE_GUARD 1

#include <string>
#include <cstring> // for memset
#include <vector>
#include <memory>
#include <map>
#include <sstream>

#include <openbabel/mol.h>

#include "Bond.h"
#include "Atom.h"
#include "IdFactory.h"
#include "Constants.h"
#include "MinimalMolecule.h"
#include "SmiMinimalMolecule.h"
#include "Utilities.h"

using namespace OpenBabel;

class EdgeAggregator;
class Brick;
class Linker;
class SimpleFragmentGraph;

class Molecule
{
public:
  Molecule();
  Molecule(OpenBabel::OBMol *mol, const std::string &theSMI); //, MoleculeT t);
  virtual ~Molecule();

  void setUniqueIndexID(unsigned int id) { uniqueIndexID = id; }
  unsigned int getUniqueIndexID() const { return uniqueIndexID; }
  bool valid = true;

  //
  // Get Functions
  //
  virtual bool isValid() const { return valid; }
  virtual bool IsLinker() const { return false; }
  virtual bool IsComplex() const { return !this->IsLinker() && !this->IsBrick(); }
  virtual bool IsBrick() const { return false; }

  int getNumberOfAtoms() const { return this->atoms.size(); }
  int getNumberOfBonds() const { return this->bonds.size(); }

  SimpleFragmentGraph *getFingerprint() const;

  bool addBond(int xID, int yID, unsigned int order); //, eTypeOfBondT bt, eStatusBitT s);

  std::string toString() const;
  friend std::ostream &operator<<(std::ostream &os, const Molecule &mol);
  virtual bool operator==(const Molecule &that) const;
  std::vector<EdgeAggregator *> *Compose(const Molecule &) const;

  // Acquire a summary of the linkers and rigids in this molecule.
  void GetNumLinkersBricks(int &numLinkers, int &numUniqueLinkers,
                           int &numBricks, int &numUniqueBricks) const;

  MinimalMolecule *ConstructMinimalMolecule();
  SmiMinimalMolecule *ConstructSmiMinimalMolecule();

  std::string ConstructSMI() const;

  // The 'size' of a molecule is based on the number of total fragments.
  unsigned int size() const;

  // Initialize any containers to track fragments (linkers / rigids)
  void initFragmentDevices();

  // Initialize the fragment container
  void initFragmentInfo();

  // Initialize the graph-based representation of the fragment
  void initGraphRepresentation();

  // Collection of linkers and rigids for this synthesis.
  static std::vector<Molecule *> baseMolecules;
  static void SetBaseMoleculeInfo(const std::vector<Molecule *> baseMols,
                                  unsigned int numBricks, unsigned int numLinkers);

  static unsigned int NUM_UNIQUE_FRAGMENTS;

  void WriteToOpenBabelFormat(std::string &) const;

  bool hasFragment(int uniqueId) const { return this->fragmentCounter[uniqueId] != 0; }
  bool numFragmentsOf(int uniqueId) const { return this->fragmentCounter[uniqueId]; }
  unsigned short int getNumOccurrencesForUniqueBuild() const { return _numOccurrencesForUnique; }

  // CTA: 6/2024
  void printConstituentFragments() const
  {
    for (unsigned int m = 0; m < Molecule::NUM_UNIQUE_FRAGMENTS; m++)
        std::cout << this->fragmentCounter[m] << " ";
    std::cout << std::endl;
  }

  // CTA: 6/2024
  bool allFragmentsUsed() const
  {
    for (unsigned int m = 0; m < Molecule::NUM_UNIQUE_FRAGMENTS; m++)
        if (this->fragmentCounter[m] == 0) return false;

    return true;
  }
  

  //
  /////////////////////////////////////////////////////////////////////////
  //
protected:
  //
  // Instance Variables
  //

  // The unique identifier for this molecule
  unsigned int uniqueIndexID;

  // Local atoms and bonds
  std::vector<Atom*> atoms;
  std::vector<Bond> bonds;

  // Used for molecular comparison; the molecule represented as a graph
  SimpleFragmentGraph *fingerprint;

  // An array used to count the number of each specific linker /
  // brick in this molecule
  unsigned short int *fragmentCounter;

  // If we perform a unique construction of molecules, how many copies
  // of this fragment must we include; default is 1
  unsigned short int _numOccurrencesForUnique;

  //
  // Statics
  //

  // Each linker / rigid has connection points;
  // we create unique ids for those connections.
  static IdFactory connectionIdMaker;

  virtual void parseAppendix(std::string &comment, int numAtoms = 0)
  {
    std::cerr << "Called Wrong parseAppendix::MOLECULE" << std::endl;
  }

  // TODO
  // If this is a 'merged' fragment, identify how many occurences
  // are required for complete, unique reconstruction
  // set : this->_numOccurencesForUnique
  int parseSimilarFragments(std::stringstream& suffStream) const
  {
    //
    // Read until we get "> <"
    //
    std::string line = "";
    while (line.find("> <SIMILAR_FRAGMENTS>") == std::string::npos &&
           line.find(">  <SIMILAR_FRAGMENTS>") == std::string::npos)
    {
        getline(suffStream, line);
    }

    int numSimilarFragments = 0;
    while (!isspace(suffStream.peek()))
    {
      std::string similar;
      while (suffStream.peek() != '\n' && suffStream.peek() != '\r')
      {
        suffStream >> similar;

        // Error check that these are bricks
        if (similar.substr(similar.length() - 3) != "sdf")
        {
          std::cout << "Similar fragment " << similar << " not specified as sdf" << std::endl;
        }
        else
        {
          std::cout << "Similar fragment " << similar << " found." << std::endl;
          numSimilarFragments++;
        }

        eatWhiteToNewLineOrChar(suffStream);
      }
      
      // Get the newline
      suffStream.get();
    }

    return numSimilarFragments;
  }

  //
  /////////////////////////////////////////////////////////////////////////
  //
private:
  void localizeOBMol(OpenBabel::OBMol *obmol);

  bool exceedsMaxEstimatedThresholds();
  bool ContainsLoops() const;
  bool satisfiesMoleculeSynthesisCriteria();

  Molecule *ComposeToNewLocalMolecule(const Molecule &that,
                                      int thisAtomIndex,
                                      int thatAtomIndex) const;

  static unsigned int BRICK_INDEX_START;
  static unsigned int BRICK_INDEX_END;
  static unsigned int LINKER_INDEX_START;
  static unsigned int LINKER_INDEX_END;
  static unsigned int FRAGMENT_END_INDEX;
};

#endif
