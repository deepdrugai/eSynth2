/*
 * SPDX-License-Identifier: MIT
 * Copyright (c) 2026 deepdrugai
 */

/*
 * eSynth 2.0
 * Author: Ting Chen
 *         C. Alvin 8/2022
 */

/*
 * Facilitates validating reconstruction of a single molecule.
 * Input is mol2 format. 
 */
 
#ifndef _OTFVALIDATOR_GUARD
#define _OTFVALIDATOR_GUARD 1

#include<vector>

#include<openbabel/mol.h>

#include "Molecule.h" 

//
// A class to perform validation on the fly, 
//    compare each generated molecule to the validation molecule 
//    by calculating the Tanimotto Coefficient and add the TC value to a frequency list 
// TC Values are calculated by *1000 and truncate to get 3 digits of decimals 
// to assign each TC value as an index in the vector.
//
class OTFValidator
{
  public:
	OTFValidator(std::string& validationInfo, std::string& fileName);

	virtual ~OTFValidator();
	virtual void validate(const std::string& smi);
	virtual void createValidationFP();
	virtual void convertSMItoFP(const std::string& smi);
	virtual int computeTanimoto();
	virtual void analyzeTanimoto(int tcValue, const std::string& smi);
	virtual void writeToFile();


  protected:
	std::string _validationInfo;
    std::string _outputFileName; 
	std::vector<unsigned int> _validationFP;
	std::vector<unsigned int> _molFP;

	unsigned int* _TCAnalysis;
	int _highestTC;
	std::string _highestTCmol;
	std::vector<std::string> _highestTCmolecules;
};

#endif 