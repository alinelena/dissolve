/*
	*** Import Module - Options
	*** src/modules/import/options.cpp
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

#include "modules/import/import.h"
#include "modules/import/formats.h"
#include "module/keywordtypes.h"

// Set up keywords for Module
void ImportModule::setUpKeywords()
{
	// Trajectory Import
	ModuleKeywordGroup* group = addKeywordGroup("Trajectory");
	group->add(new BoolModuleKeyword(true), "ReadTrajectory", "Read sequential frames from a trajectory file", "<True|False>");
	group->add(new FileAndFormatModuleKeyword(trajectoryFile_), "TrajectoryFile", "Source trajectory file for frames (if ReadTrajectory == True)");
}

// Parse keyword line, returning true (1) on success, false (0) for recognised but failed, and -1 for not recognised
int ImportModule::parseComplexKeyword(ModuleKeywordBase* keyword, LineParser& parser, Dissolve* dissolve, GenericList& targetList, const char* prefix)
{
	return -1;
}
