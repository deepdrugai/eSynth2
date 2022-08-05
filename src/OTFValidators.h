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