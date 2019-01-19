/*
	*** Forces Module
	*** src/modules/forces/forces.h
	Copyright T. Youngs 2012-2019

	This file is part of Dissolve.

	Dissolve is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Dissolve is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DISSOLVE_FORCESMODULE_H
#define DISSOLVE_FORCESMODULE_H

#include "module/module.h"
#include "modules/import/formats.h"

// Forward Declarations
class PotentialMap;

// Forces Module
class ForcesModule : public Module
{
	/*
	 * Calculates the total forces in a system
	 */

	public:
	// Constructor
	ForcesModule();
	// Destructor
	~ForcesModule();


	/*
	 * Instances
	 */
	public:
	// Create instance of this module
	Module* createInstance() const;


	/*
	 * Definition
	 */
	public:
	// Return type of module
	const char* type() const;
	// Return brief description of module
	const char* brief() const;
	// Return the maximum number of Configurations the Module can target (or -1 for any number)
	int nTargetableConfigurations() const;


	/*
	 * Options
	 */
	protected:
	// Set up options for Module
	void setUpKeywords();
	// Parse complex keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
	int parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix);


	/*
	 * Data
	 */
	private:
	// Reference forces for test
	ForceImportFileFormat referenceForces_;


	/*
	 * Processing
	 */
	private:
	// Run main processing
	bool process(Dissolve& dissolve, ProcessPool& procPool);

	public:
	// Run set-up stage
	bool setUp(Dissolve& dissolve, ProcessPool& procPool);


	/*
	 * Force Methods
	 */
	public:
	// Calculate total intramolecular forces
	static void intramolecularForces(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Calculate interatomic forces within the specified Configuration
	static void interatomicForces(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Calculate total forces within the system
	static void totalForces(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, Array<double>& fx, Array<double>& fy, Array<double>& fz);
};

#endif

