/*
	*** Calibration Module - Definition
	*** src/modules/calibration/definition.cpp
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

#include "modules/calibration/calibration.h"

// Return type of module
const char* CalibrationModule::type() const
{
	return "Calibration";
}

// Return category for module
const char* CalibrationModule::category() const
{
	return "Optimisation";
}

// Return brief description of module
const char* CalibrationModule::brief() const
{
	return "Calibrate various parameters";
}

// Return the maximum number of Configurations the Module can target (or -1 for any number)
int CalibrationModule::nTargetableConfigurations() const
{
	return 0;
}
