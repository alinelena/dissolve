/*
	*** SpeciesIntra Definition
	*** src/classes/speciesintra.h
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_SPECIESINTRA_H
#define DUQ_SPECIESINTRA_H

#include "base/charstring.h"

#define MAXINTRAPARAMS 4

// Forward Declarations
class Species;

/*
 * SpeciesIntra Definition
 */
class SpeciesIntra
{
	public:
	// Constructor
	SpeciesIntra();
	// Destructor
	~SpeciesIntra();


	/*
	 * Basic Data
	 */
	protected:
	// Parent Species
	Species* parent_;
	// Name for interaction (if relevant)
	CharString name_;
	
	public:
	// Set parent Species
	void setParent(Species* parent);
	// Return parent Species
	Species* parent() const;
	// Set name for interaction (if relevant)
	void setName(const char* name);
	// Return name for interaction
	const char* name();


	/*
	 * Interaction Parameters
	 */
	protected:
	// Linked master from which parameters should be taken (if relevant)
	SpeciesIntra* masterParameters_;
	// Parameters for interaction
	double parameters_[MAXINTRAPARAMS];


	public:
	// Set linked master from which parameters should be taken
	void setMasterParameters(SpeciesIntra* master);
	// Return linked master from which parameters should be taken
	SpeciesIntra* masterParameters();
	// Set nth parameter
	void setParameter(int id, double value);
	// Return nth parameter
	double parameter(int id) const;
};

#endif
