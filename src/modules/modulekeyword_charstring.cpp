/*
	*** Module Keyword - CharString
	*** src/modules/modulekeyword_charstring.cpp
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

#include "modules/modulekeyword_charstring.h"
#include "base/lineparser.h"
#include "templates/genericlist.h"

// Constructor
CharStringModuleKeyword::CharStringModuleKeyword(CharString value) : ModuleKeywordBase(ModuleKeywordBase::CharStringData), ModuleKeywordData<CharString>(value)
{
}

CharStringModuleKeyword::CharStringModuleKeyword(CharString value, int nOptions, const char** options) : ModuleKeywordBase(ModuleKeywordBase::CharStringData), ModuleKeywordData<CharString>(value)
{
	// Create an Array of the values first
	Array<CharString> valueArray;
	for (int n=0; n<nOptions; ++n) valueArray.add(options[n]);

	setValidationList(valueArray);
}

// Destructor
CharStringModuleKeyword::~CharStringModuleKeyword()
{
}

/*
 * Data
 */

// Duplicate the keyword's data in the supplied GenericList
void CharStringModuleKeyword::duplicateInList(GenericList& targetList, const char* prefix)
{
	GenericListHelper<CharString>::realise(targetList, keyword(), prefix, genericItemFlags()) = data_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int CharStringModuleKeyword::minArguments()
{
	return 1;
}

// Return minimum number of arguments accepted
int CharStringModuleKeyword::maxArguments()
{
	return 1;
}

// Parse arguments from supplied LineParser, starting at argument offset specified
bool CharStringModuleKeyword::parseArguments(LineParser& parser, int startArg)
{
	if (parser.hasArg(startArg))
	{
		data_ = parser.argc(startArg);
		return true;
	}
	return false;
}


/*
 * Conversion
 */

// Return value (as bool)
bool CharStringModuleKeyword::asBool()
{
	return DUQSys::atob(data_);
}

// Return value (as int)
int CharStringModuleKeyword::asInt()
{
	return atoi(data_);
}

// Return value (as double)
double CharStringModuleKeyword::asDouble()
{
	return atof(data_);
}

// Return value (as string)
const char* CharStringModuleKeyword::asString()
{
	return data_;
}