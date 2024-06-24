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

#ifndef _OB_WRITER_GUARD
#define _OB_WRITER_GUARD 1


#include <vector>
#include <string>
#include <iostream>
#include <queue>

#include <openbabel/mol.h>

#include "Molecule.h"
#include "IdFactory.h"


//
// A class to dump all of the molecules to a file.
//
class OBWriter
{
  public:
     OBWriter();
    ~OBWriter();

    void OutputMoleculeAppendExternalSMI(const std::string& smi);

    void IndicateSynthesisStarted();
    void IndicateSynthesisComplete();
    void InitiateOutputThreadPool();

    static void InitializeFile(const std::string& outFile);

    static void TurnValidationOff() { performValidation = false; }

    static void ConvertToSMI(const std::string& sdf, std::string& smi);

  private:
    unsigned int mCounter; 
    unsigned int mFailCounter; 
    bool writing_complete;
    bool writing_started;

    static bool synthesis_complete;
    static bool performValidation;
    static std::ofstream out;
    static std::string outFileName;
    static OpenBabel::OBConversion SDF_to_SMI_conv;

    unsigned molCounter;
    std::string prefix;
    std::string sdfSuffix;
    std::string smiSuffix;
    unsigned UPPERBOUND;
    std::string infix;
    std::string outputDir;
    std::string sdfOutfileName;
    std::string smiOutfileName;
};

#endif
