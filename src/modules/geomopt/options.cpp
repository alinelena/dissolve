/*
	*** Geometry Optimisation Module - Options
	*** src/modules/geomopt/options.cpp
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

#include "modules/geomopt/geomopt.h"
#include "module/keywordtypes.h"

// Set up keywords for Module
void GeometryOptimisationModule::setUpKeywords()
{
	// Method Control
	ModuleKeywordGroup* group = addKeywordGroup("Method Control");
	group->add(new IntegerModuleKeyword(200, 1), "NCycles", "Number of minimisation cycles to perform");
	group->add(new DoubleModuleKeyword(1.0e-5, 1.0e-10), "StepSize", "Initial step size to employ");
	group->add(new DoubleModuleKeyword(1.0e-4, 1.0e-10), "Tolerance", "Tolerance controlling convergence of algorithm)");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int GeometryOptimisationModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
