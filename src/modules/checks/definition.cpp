/*
	*** Checks Module - Definition
	*** src/modules/checks/definition.cpp
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

#include "modules/checks/checks.h"

// Return type of module
const char* ChecksModule::type() const
{
	return "Checks";
}

// Return category for module
const char* ChecksModule::category() const
{
	return "Checks & Tests";
}

// Return brief description of module
const char* ChecksModule::brief() const
{
	return "Conduct simple checks on Configurations";
}

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int ChecksModule::nTargetableConfigurations() const
{
	return -1;
}
