/*
* REQUIRE: -v (validation file name) in command line
the file has to be .mol2 and containing only 1 molecule
*
* Constructor:
* Initialize an array with size 1000 to store the tanimoto coefficient of
each input molecule and the validation molecule
(size 1000 for keeping 3 digit of TC and used as index of the array)
* 1. Read from a validation file
* 2. Create the fingerprint for the validation molecule by creating a OBMol
*
* Methods:
* 1.
*/

#include<vector> 
#include<iostream>
#include<fstream> 
#include<cstring>  

#include<openbabel/mol.h>
#include<openbabel/obconversion.h>
#include<openbabel/fingerprint.h>

#include "OTFValidators.h"
#include "OTFValidator.h"
#include "Options.h"
#include "Constants.h" 

// Constructor: initialize validation file (.mol2) 
OTFValidator::OTFValidator(std::string& validInfo, std::string& fName) :
	//_validationFile{ validFile },
	_validationInfo{validInfo},
  _outputFileName{fName},
	_validationFP{},
	_molFP{},

	_TCAnalysis{ new unsigned int[1000] },
	_highestTC{ -1 },
	_highestTCmol{},
	_highestTCmolecules{}
{
	// Initialize all 1000 elements to 0 in memory 
	//memset(_TCAnalysis, 0, 1000);
	for (int i = 0; i <= 1000; i++)
		_TCAnalysis[i] = 0;

	// Get the validation fingerprint 
	//readValidationFile(); //read file from OTFValidators - 6/30/2022
	createValidationFP();
}

// Destructor: return the memory of the frequency analysis array 
OTFValidator::~OTFValidator()
{
	delete[] _TCAnalysis;
}

//
// Main Method: Instantiator calls only validate()
//
void OTFValidator::validate(const std::string& smi)
{
	convertSMItoFP(smi);
	int tcValue = computeTanimoto();
	analyzeTanimoto(tcValue, smi);
	//writeToFile(); 
}

// CHANGE: reading file in OTFValidators: may read multiple validation molecules 6/30/2022
// Read in the validation file in mol2 and store the information in validationInfo
//
// void OTFValidator::readValidationFile()
// {
// 	std::ifstream infile;
// 	std::string line;

// 	// Open the validation file 
// 	infile.open(_validationFile);

// 	// Read each line of mol2 file and add it to validInfo string 
// 	while (!infile.eof() && !infile.fail())
// 	{
// 		getline(infile, line);
// 		_validationInfo += line + "\n";
// 	}
	// std::cout << "Validation Information:\n" << validInfo << std::endl; 
//}

//
// Create the fingerprint for the validation moelcule
// 
void OTFValidator::createValidationFP()
{
	OpenBabel::OBConversion obConversion;
	obConversion.SetInFormat("MOL2");

	OpenBabel::OBMol validationMol;

	// Create the OpenBabel molecule
	obConversion.ReadString(&validationMol, _validationInfo);

	// Get fingerprint for the validation molecule  
	OpenBabel::OBFingerprint* fpType = OpenBabel::OBFingerprint::FindFingerprint("");
	fpType->GetFingerprint(&validationMol, _validationFP);

	// Print out the title of the validation file (2nd line of the mol2 file)
	//std::cout << "\nValidating with: " << validationMol.GetTitle() << std::endl;

	// Print out the fingerprint of the validation molecule 
	// std::cout << "\nFingerprint of the validation molecule: " << std::endl;
	// for (unsigned int i = 0; i < _validationFP.size(); i++)
	// {
	// 	std::cout << _validationFP[i] << " ";
	// }
	// cout << endl;
}

//
// Converting generated molecule from SDF to OBMol and then get the fingerprint 
//
void OTFValidator::convertSMItoFP(const std::string& smi)
{
	// Begin open babel usage
	pthread_mutex_lock(&Molecule::openbabel_lock);

	// store result into a new molecule
	OpenBabel::OBMol mol;
	OpenBabel::OBConversion obConv;

	// Read smi input 
	if (!obConv.SetInFormat("SMI"))
		std::cerr << "Open Babel is unable to read in .smi file" << std::endl;

	// Create Open Babel Molecule from smi input 
	if (!obConv.ReadString(&mol, smi))
		std::cerr << "Open Babel is unable to create an OBMol from the smi" << std::endl;

	// End open babel usage
	pthread_mutex_unlock(&Molecule::openbabel_lock);

	// Create the fingerprint for the generated molecule 
	OpenBabel::OBFingerprint* fpType = OpenBabel::OBFingerprint::FindFingerprint("");

	// Acquire the fingerprint for the generated molecule for TC comparison 
	fpType->GetFingerprint(&mol, _molFP);

	// Print out the fingerprint for generated molecule 
	// std::cout << "\nThe fingerprint for " << mol.GetTitle() << " :" << std::endl;
	// for (unsigned int i = 0; i < _molFP.size(); i++)
	// {
	// 	std::cout << _molFP[i] << " ";
	// }
	// std::cout << endl;

	// if (g_debug_output)
	// {
	// 	std::cerr << "Unable to get fingerprint of the generated molecule";
	// 	foreach_units(u_it, molFP)
	// 	{
	// 	std:cerr << *u_it << "|";
	// 	}
	// }
}

//
// Validate a single molecule with the smi and the fingerprint of the generated molecule 
//
int OTFValidator::computeTanimoto()
{
	// Compute tanimoto coefficient for generated molecule and validation molecule  
	double tanimoto = OpenBabel::OBFingerprint::Tanimoto(_validationFP, _molFP);

	// Take the first three digits of decimal in order to store in an index-based array 
	int tcValue = trunc(tanimoto * 1000);

	// Check tcValue out of bound 
	if (tcValue < 0 || tcValue > 1000)
		std::cerr << "Tanimoto Coefficient computation fail. " << std::endl;

	// Check the size of the fingerprint: should be 32
	//std::cout << "\nValidation Fingerprint size: " << _validationFP.size() << std::endl;
	//std::cout << "Generated Molecule Fingerprint size: " << _molFP.size() << std::endl;

	// Print out the tanimoto cofficient
	//std::cout << "\nTanimoto: " << tanimoto << std::endl;

	return tcValue;
}

//
// Perform TC analysis, keep track of the molecule (in smi) with highest tc value 
//
void OTFValidator::analyzeTanimoto(int tcValue, const std::string& smi)
{
	// Add to the count of frequency analysis 
	int count = _TCAnalysis[tcValue] + 1;
	_TCAnalysis[tcValue] = count;

	// Keep track of the molecule with the highest tc value 
	if (tcValue > _highestTC)
	{
		_highestTC = tcValue;
		_highestTCmol = smi;
	}

	else if (tcValue == _highestTC)
	{

		_highestTCmolecules.push_back(smi);
	}

	//std::cout << "\nMolecule with highest TC (in smi): " << _highestTCmol << std::endl << std::endl;
}

//
// Write result of the frequency count to a file 
//
void OTFValidator::writeToFile()
{
	std::cout << "Start to write frequncy analysis result into a file: " << _outputFileName << std::endl;

	std::string fileType = ".csv";
	std::string fileName = _outputFileName + fileType; // change to the validation file name later

	//std::cout << "OTFValidator: " << fileName << std::endl; 

	std::ofstream outfile(fileName);
	//outfile.open(fileName); 

	for (int i = 0; i <= 1000; i++)
	{
		outfile << i << "," << _TCAnalysis[i] << std::endl;
		//std::cout << "tc value: " << i << " count: " << _TCAnalysis[i] << std::endl;
	}

	outfile.close();
	std::cout << "Finish writing frequncy analysis result into a file" << std::endl;
}