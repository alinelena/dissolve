/*
	*** Module Keyword - CharString
	*** src/modules/modulekeyword_charstring.h
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

#ifndef DUQ_MODULEKEYWORDCHARSTRING_H
#define DUQ_MODULEKEYWORDCHARSTRING_H

#include "module/keyworddata.h"
#include "module/keywordbase.h"

// Forward Declarations
/* none */

// Keyword with CharString Data
class CharStringModuleKeyword : public ModuleKeywordBase, public ModuleKeywordData<CharString>
{
	public:
	// Constructors
	CharStringModuleKeyword(CharString value);
	CharStringModuleKeyword(CharString value, int nOptions, const char** options);
	// Destructor
	~CharStringModuleKeyword();


	/*
	 * Data
	 */
	public:
	// Duplicate the keyword's data in the supplied GenericList
	void duplicateInList(GenericList& targetList, const char* prefix);


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	int minArguments();
	// Return maximum number of arguments accepted
	int maxArguments();
	// Parse arguments from supplied LineParser, starting at argument offset specified
	bool parseArguments(LineParser& parser, int startArg);


	/*
	 * Conversion
	 */
	public:
	// Return value (as bool)
	bool asBool();
	// Return value (as int)
	int asInt();
	// Return value (as double)
	double asDouble();
	// Return value (as string)
	const char* asString();
};

#endif
