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

/*
 * eSynth 2.0
 * Author: Ting Chen
 *         C. Alvin 8/2022
 */

/*
 * Facilitates validating reconstruction of molecules specified in a given file.
 * The inputmolecule type expected is mol2. 
 */

#include<vector>

#include "OTFValidators.h"
#include "OTFValidator.h"
#include "Options.h"
#include "Constants.h"

// Create a vector of validators, one for each molecule in validFile
OTFValidators::OTFValidators(const std::string& validFile) :
	_validationFile{validFile}
{
	// When the user does not seek validation, go no further
    if (!Options::OTF_VALIDATE) return;

    readValidationFile();
}

OTFValidators::~OTFValidators()
{
	// delete each OTFValidator 
	for (auto it = _validators.begin(); it != _validators.end(); it++)
	{
		delete *it;
	}
}

void OTFValidators::validate(const std::string& smi)
{
    // When the user does not seek validation, go no further
    if (!Options::OTF_VALIDATE) return;

	for (OTFValidator* vali : _validators)
	{
		vali->validate(smi); 
	}
}

void OTFValidators::writeToFiles()
{
	for (OTFValidator* vali : _validators)
	{
		vali->writeToFile(); 
	}
}

void OTFValidators::readValidationFile()
{
	std::ifstream infile;
	std::string line;

	// Open the validation file 
	infile.open(_validationFile);

    getline(infile, line);
    //std::cout << line << std::endl; 
    //cout << line.size() << std::endl; 

	if (!(line.substr(0, 17) == "@<TRIPOS>MOLECULE"))
    {
		cerr << "Expected @<TRIPOS>MOLECULE at the begining of input .mol2 file: |" << line << "|" << std::endl;
		throw "not mol2 standard";
	}

	// Read each line of mol2 file and add it to validInfo string 
	while (!infile.eof() && !infile.fail())
	{
		std::string validationInfo = line + "\n";		

		// 2nd line of mol2 file is the unique identifier used as the output file name 
		getline(infile, line);
		std::string fileName = _validationFile + "-" + line; 

        //check output file name
        //std::cout << "OTFValidators: " << fileName << std::endl; 
		
		while (line.substr(0, 17) != "@<TRIPOS>MOLECULE" && !infile.eof() && !infile.fail())
		{
			validationInfo += line + "\n";
			getline(infile, line);
		}
		
		// create validator object
		_validators.push_back(new OTFValidator(validationInfo, fileName));
        //std::cout << "the vector contains: " << _validators.size() << std::endl; 
	}

	// std::cout << "Validation Information:\n" << validInfo << std::endl; 
}
