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

#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>
#include <sys/stat.h>
#include <dirent.h>

#include <openbabel/mol.h>
#include <openbabel/obconversion.h>

#include "OBWriter.h"
#include "Molecule.h"
#include "Brick.h"
#include "Linker.h"
#include "Constants.h"
#include "Utilities.h"
#include "IdFactory.h"
#include "Options.h"
#include "zpipe.h"



std::ofstream OBWriter::out;
std::string OBWriter::outFileName;
bool OBWriter::synthesis_complete = false;
bool OBWriter::performValidation = true;
OpenBabel::OBConversion OBWriter::SDF_to_SMI_conv;


// ****************************************************************************

OBWriter::OBWriter() : mCounter(0),
                       mFailCounter(0),
                       writing_complete(false),
                       writing_started(false)          
{
    molCounter = 0;
    prefix = "molecules";
    sdfSuffix = ".sdf";
    smiSuffix = ".smi";
    UPPERBOUND = 250000;
    outputDir = Options::OUTPUT_DIRECTORY;
    sdfOutfileName = outputDir + "/" + prefix + "-1-10000" + sdfSuffix;
    smiOutfileName = outputDir + "/" + prefix + "-1-250000" + smiSuffix;
}

// ****************************************************************************

OBWriter::~OBWriter() { }

// ****************************************************************************

void OBWriter::InitializeFile(const std::string& outFile)
{
    outFileName = outFile;
}

// ****************************************************************************

void OBWriter::IndicateSynthesisStarted()
{
    //
    // Construct the actual directory
    //
    std::string theDir = Options::OUTPUT_DIRECTORY;

    // std::cout << "Will output to directory: " << theDir << std::endl;

    bool overwrite = true;
    if (DoesDirectoryExist(theDir))
    {
        std::cout << "The output directory " << theDir << " exists."
                  << " This should not happen." << std::endl;
    }
    else
    {
        MakeDirectory(theDir);
    }

    if (!overwrite)
    {
        std::cerr << "Re-run ./esynth with different output directory." << std::endl;
        exit(1);
    }

    // Set specific output file information.
    outputDir = theDir;
    sdfOutfileName = outputDir + "/" + prefix + "-1-10000" + sdfSuffix;
    smiOutfileName = outputDir + "/" + prefix + "-1-250000" + smiSuffix;    
}

// ****************************************************************************

void OBWriter::IndicateSynthesisComplete()
{
    synthesis_complete = true;

    OBWriter::out.close();

    zlib_compress(smiOutfileName, smiOutfileName + ".zlib");
    remove(smiOutfileName.c_str());
}

// ****************************************************************************

void OBWriter::OutputMoleculeAppendExternalSMI(const std::string& smi)
{
    //
    // Append an SMI version of the molecule to the output file.
    //
    molCounter++;

    //
    // Update the file we are writing to.
    //
    if (molCounter % UPPERBOUND == 0)
    {
        // zip the file we just created and remove it.
        zlib_compress(smiOutfileName, smiOutfileName + ".zlib");
        remove(smiOutfileName.c_str());

        //
        // Create the new output file name.
        //
        std::ostringstream oss;

        oss << outputDir << "/" << prefix << "-" << molCounter
            << "-" << (molCounter + UPPERBOUND) << smiSuffix;

        smiOutfileName = oss.str();
    }


    std::ofstream outfile(smiOutfileName.c_str(),
                          std::ofstream::out | std::ofstream::app); // append

    outfile << smi << std::endl;

    outfile.close();
}

///////////////////////////////////////////////////////////////////

void OBWriter::ConvertToSMI(const std::string& sdf, std::string& smi)
{
    // store sdf into an open babel molecule
    OpenBabel::OBMol mol;

    // Use the static converter in OBWriter; output for Canonical SMILES 6/2024
    SDF_to_SMI_conv.SetInAndOutFormats("SDF", "SMI");

    // Convert to SDF
    SDF_to_SMI_conv.ReadString(&mol, sdf);

    // Convert to SMI
    smi = SDF_to_SMI_conv.WriteString(&mol);

    // Clean up the smi value; ensures only molecule values
    smi = smi.substr(0, smi.find('\t'));
}