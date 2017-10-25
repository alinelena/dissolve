/*
	*** Module Keyword - Double
	*** src/modules/modulekeyword_double.h
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

#ifndef DUQ_MODULEKEYWORDDOUBLE_H
#define DUQ_MODULEKEYWORDDOUBLE_H

#include "modules/modulekeyworddata.h"
#include "modules/modulekeywordbase.h"

// Forward Declarations
/* none */

// Keyword with Double Data
class DoubleModuleKeyword : public ModuleKeywordBase, public ModuleKeywordData<double>
{
	public:
	// Constructors
	DoubleModuleKeyword(double value);
	DoubleModuleKeyword(double value, double minValue);
	DoubleModuleKeyword(double value, double minValue, double maxValue);
	// Destructor
	~DoubleModuleKeyword();


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
	 * Validation
	 */
	public:
	// Set maximum value (no minimum limit)
	ModuleKeywordBase* max(double value);


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
