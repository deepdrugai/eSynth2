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
 * Facilitates validating reconstruction of molecules specified in a given file.
 * The inputmolecule type expected is mol2. 
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