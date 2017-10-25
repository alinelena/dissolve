/*
	*** Checks Module - Definition
	*** src/modules/checks/definition.cpp
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

#include "modules/checks/checks.h"

// Return name of module
const char* ChecksModule::name()
{
	return "Checks";
}

// Return brief description of module
const char* ChecksModule::brief()
{
	return "Conduct simple checks on Configurations";
}

// Return instance type for module
Module::InstanceType ChecksModule::instanceType()
{
	return Module::MultipleInstance;
}

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int ChecksModule::nTargetableConfigurations()
{
	return -1;
}

// Whether the Module has a pre-processing stage
bool ChecksModule::hasPreProcessing()
{
	return false;
}

// Whether the Module has a processing stage
bool ChecksModule::hasProcessing()
{
	return true;
}

// Whether the Module has a post-processing stage
bool ChecksModule::hasPostProcessing()
{
	return false;
}

// Modules upon which this Module depends to have run first
const char* ChecksModule::dependentModules()
{
	return "";
}

// Setup supplied dependent module (only if it has been auto-added)
bool ChecksModule::setupDependentModule(Module* depMod)
{
	return true;
}