/*
	*** Export Module - Options
	*** src/modules/export/options.cpp
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

#include "modules/export/export.h"
#include "module/keywordtypes.h"

// Set up options for Module
void ExportModule::setUpKeywords()
{
	// Export
	ModuleKeywordGroup* group = addKeywordGroup("Export");
	group->add(new FileAndFormatModuleKeyword(coordinatesFormat_), "WriteCoordinates", "Write coordinates for the Configuration targets");
	group->add(new FileAndFormatModuleKeyword(pairPotentialFormat_), "WritePairPotentials", "Write all pair potentials to supplied basename");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int ExportModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
