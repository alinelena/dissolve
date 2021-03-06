/*
	*** Module Keyword - Pair Broadening Function
	*** src/modules/keywordtypes/pairbroadeningfunction.cpp
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

#include "module/keywordtypes/pairbroadeningfunction.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Constructor
PairBroadeningFunctionModuleKeyword::PairBroadeningFunctionModuleKeyword(PairBroadeningFunction value) : ModuleKeywordBase(ModuleKeywordBase::PairBroadeningFunctionData), ModuleKeywordData<PairBroadeningFunction>(value)
{
}

// Destructor
PairBroadeningFunctionModuleKeyword::~PairBroadeningFunctionModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool PairBroadeningFunctionModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int PairBroadeningFunctionModuleKeyword::minArguments()
{
	return 1;
}

// Return maximum number of arguments accepted
int PairBroadeningFunctionModuleKeyword::maxArguments()
{
	return 2;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool PairBroadeningFunctionModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	if (data_.readAsKeyword(parser, startArg, coreData)) set_ = true;
	else return false;

	return true;
}

// Write keyword data to specified LineParser
bool PairBroadeningFunctionModuleKeyword::write(LineParser& parser, const char* prefix)
{
	if (!parser.writeLineF("%s%s", prefix, keyword())) return false;

	return data_.writeAsKeyword(parser, prefix, true);
}

/*
 * Validation
 */

// Validate supplied value
bool PairBroadeningFunctionModuleKeyword::isValid(PairBroadeningFunction value)
{
	return true;
}
