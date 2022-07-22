/*
* This file is part of eSynth validation 
* Creates a vector of OTFValidator objects to validate a file containing multiple validation molecules 
*/
#ifndef _OTFVALIDATORS_GUARD
#define _OTFVALIDATORS_GUARD 1 

#include<vector>
#include "OTFValidator.h"

class OTFValidators
{
	public: 
		OTFValidators(const std::string& validFile);
		virtual ~OTFValidators(); 

		virtual void validate(const std::string& smi);
		virtual void readValidationFile();
    virtual void writeToFiles();

	private: 
		const std::string _validationFile; 
		std::vector<OTFValidator*> _validators;
};

#endif 