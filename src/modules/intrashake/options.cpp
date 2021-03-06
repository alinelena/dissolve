/*
	*** IntraShake Module - Options
	*** src/modules/intrashake/options.cpp
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

#include "modules/intrashake/intrashake.h"
#include "module/keywordtypes.h"

// Set up keywords for Module
void IntraShakeModule::setUpKeywords()
{
	// Calculation
	ModuleKeywordGroup* group = addKeywordGroup("Calculation");
	group->add(new DoubleModuleKeyword(-1.0), "CutoffDistance", "Interatomic cutoff distance to employ", "<distance>");
	group->add(new IntegerModuleKeyword(1), "ShakesPerTerm", "Number of shakes per term", "<n>");
	group->add(new DoubleModuleKeyword(0.33), "TargetAcceptanceRate", "Target acceptance rate for Monte Carlo moves", "<rate (0.0-1.0)>");
	group->add(new BoolModuleKeyword(false), "TermEnergyOnly", "Whether only the energy of the intramolecular term is calculated and assessed");

	// Bonds
	group = addKeywordGroup("Bonds");
	group->add(new BoolModuleKeyword(true), "AdjustBonds", "Whether Bonds in the molecule should be shaken", "<true|false>");
	group->add(new DoubleModuleKeyword(0.01), "BondStepSize", "Step size for Bond adjustments (Angstroms)", "<stepsize>", GenericItem::InRestartFileFlag);
	group->add(new DoubleModuleKeyword(0.001), "BondStepSizeMin", "Minimum step size for Bond adjustments (Angstroms)", "<stepsize>");
	group->add(new DoubleModuleKeyword(0.2), "BondStepSizeMax", "Maximum step size for Bond adjustments (Angstroms)", "<stepsize>");

	// Angles
	group = addKeywordGroup("Angles");
	group->add(new BoolModuleKeyword(true), "AdjustAngles", "Whether Angles in the molecule should be shaken", "<true|false>");
	group->add(new DoubleModuleKeyword(5.0), "AngleStepSize", "Step size for Angle adjustments (degrees)", "<stepsize>", GenericItem::InRestartFileFlag);
	group->add(new DoubleModuleKeyword(0.01), "AngleStepSizeMin", "Minimum step size for Angle adjustments (degrees)", "<stepsize>");
	group->add(new DoubleModuleKeyword(20.0), "AngleStepSizeMax", "Maximum step size for Angle adjustments (degrees)", "<stepsize>");

	// Torsions
	group = addKeywordGroup("Torsions");
	group->add(new BoolModuleKeyword(true), "AdjustTorsions", "Whether Torsions in the molecule should be shaken", "<true|false>");
	group->add(new DoubleModuleKeyword(10.0), "TorsionStepSize", "Step size for Torsion adjustments (degrees)", "<stepsize>", GenericItem::InRestartFileFlag);
	group->add(new DoubleModuleKeyword(0.5), "TorsionStepSizeMin", "Minimum step size for Torsion adjustments (degrees)", "<stepsize>");
	group->add(new DoubleModuleKeyword(45.0), "TorsionStepSizeMax", "Maximum step size for Torsion adjustments (degrees)", "<stepsize>");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int IntraShakeModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
