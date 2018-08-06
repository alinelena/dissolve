/*
	*** Module Keyword - Test Data Store
	*** src/modules/keywordtypes/testdatastore.cpp
	Copyright T. Youngs 2012-2018

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

#include "module/keywordtypes/xydatastore.h"
#include "base/lineparser.h"

// Constructor
XYDataStoreModuleKeyword::XYDataStoreModuleKeyword(XYDataStore& dataStore) : ModuleKeywordBase(ModuleKeywordBase::XYDataStoreData), ModuleKeywordData<XYData>(XYData()), dataStore_(dataStore)
{
}

// Destructor
XYDataStoreModuleKeyword::~XYDataStoreModuleKeyword()
{
}

/*
 * Data
 */

// Duplicate the keyword's data in the supplied GenericList
void XYDataStoreModuleKeyword::duplicateInList(GenericList& targetList, const char* prefix)
{
// 	GenericListHelper<IsotopologueReference>::realise(targetList, keyword(), prefix, genericItemFlags()) = data_;
}

// Return whether the current data value has ever been set
bool XYDataStoreModuleKeyword::isSet()
{
	return set_;
}

// Return data store
XYDataStore& XYDataStoreModuleKeyword::dataStore()
{
	return dataStore_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int XYDataStoreModuleKeyword::minArguments()
{
	// Must have filename and name of data
	return 2;
}

// Return maximum number of arguments accepted
int XYDataStoreModuleKeyword::maxArguments()
{
	// Filename, name of data, x column, y column
	return 4;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool XYDataStoreModuleKeyword::parseArguments(LineParser& parser, int startArg, ProcessPool& procPool)
{
	Messenger::print("Reading test data '%s' from file '%s'...\n", parser.argc(startArg), parser.argc(startArg+1));

	// N+2 and N+3 arguments are x and y columns respectively (defaulting to 0,1 if not given)
	int xcol = parser.hasArg(startArg+2) ? parser.argi(startArg+2)-1 : 0;
	int ycol = parser.hasArg(startArg+3) ? parser.argi(startArg+3)-1 : 1;

	if (!dataStore_.add(procPool, parser.argc(startArg), parser.argc(startArg+1), xcol, ycol))
	{
		Messenger::error("Failed to add test data.\n");
		return false;
	}

	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool XYDataStoreModuleKeyword::write(LineParser& parser, const char* prefix)
{
	// Loop over list of XYData
	ListIterator<XYData> dataIterator(dataStore_.data());
	while (XYData* data = dataIterator.iterate())
	{
		if (!parser.writeLineF("%s%s  '%s'  '%s'  '%s'  %f\n", prefix, keyword(), data->sourceFilename(), data->name(), data->xColumn(), data->yColumn())) return false;
	}

	return true;
}