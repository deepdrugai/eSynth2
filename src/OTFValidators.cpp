#include<vector>

#include "OTFValidators.h"
#include "OTFValidator.h"
#include "Options.h"
#include "Constants.h"

// Create a vector of validators, one for each molecule in validFile
OTFValidators::OTFValidators(const std::string& validFile) :
	_validationFile{validFile}
{
  readValidationFile();
}

OTFValidators::~OTFValidators()
{
	// delete memory for each pointer of OTFValidator 
	for (std::vector<OTFValidator*>::iterator it = _validators.begin(); it != _validators.end(); it++)
		delete *it;
}

void OTFValidators::validate(const std::string& smi)
{
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
		cerr << "Expected @<TRIPOS>MOLECULE at the begining of input file: |" << line << "|" << std::endl;
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
