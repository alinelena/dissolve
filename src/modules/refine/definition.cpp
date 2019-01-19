/*
	*** Refine Module - Definition
	*** src/modules/refine/definition.cpp
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

#include "modules/refine/refine.h"

// Return type of module
const char* RefineModule::type() const
{
	return "Refine";
}

// Return brief description of module
const char* RefineModule::brief() const
{
	return "Refine interatomic potentials based on differences between calculated and reference data";
}

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int RefineModule::nTargetableConfigurations() const
{
	return 0;
}
