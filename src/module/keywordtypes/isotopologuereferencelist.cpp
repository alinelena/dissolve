/*
	*** Module Keyword - Isotopologue Reference List
	*** src/modules/keywordtypes/isotopologuereferencelist.cpp
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

#include "module/keywordtypes/isotopologuereferencelist.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include "templates/genericlisthelper.h"

// Constructor
IsotopologueReferenceListModuleKeyword::IsotopologueReferenceListModuleKeyword(List<IsotopologueReference>& references) : ModuleKeywordBase(ModuleKeywordBase::IsotopologueReferenceData), ModuleKeywordData<IsotopologueReference>(IsotopologueReference()), references_(references)
{
}

// Destructor
IsotopologueReferenceListModuleKeyword::~IsotopologueReferenceListModuleKeyword()
{
}

/*
 * Data
 */

// Return whether the current data value has ever been set
bool IsotopologueReferenceListModuleKeyword::isSet()
{
	return set_;
}

/*
 * Arguments
 */

// Return minimum number of arguments accepted
int IsotopologueReferenceListModuleKeyword::minArguments()
{
	return 4;
}

// Return maximum number of arguments accepted
int IsotopologueReferenceListModuleKeyword::maxArguments()
{
	return 4;
}

// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
bool IsotopologueReferenceListModuleKeyword::read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool)
{
	// Find target Configuration (first argument)
	Configuration* cfg = NULL;
	for (cfg = List<Configuration>::masterInstance().first(); cfg != NULL; cfg = cfg->next) if (DissolveSys::sameString(parser.argc(startArg), cfg->name())) break;
	if (!cfg)
	{
		Messenger::error("Error defining Isotopologue reference - no Configuration named '%s' exists.\n", parser.argc(startArg));
		return false;
	}

	// Find specified Species (second argument) - must be present in the target Configuration
	Species* sp = NULL;
	for (sp = List<Species>::masterInstance().first(); sp != NULL; sp = sp->next) if (DissolveSys::sameString(parser.argc(startArg+1), sp->name())) break;
	if (!sp) return Messenger::error("Error defining Isotopologue reference - no Species named '%s' exists.\n", parser.argc(startArg+1));
	if (!cfg->hasUsedSpecies(sp))return Messenger::error("Error defining Isotopologue reference - Species '%s' is not present in Configuration '%s'.\n", sp->name(), cfg->name());

	// Finally, locate isotopologue definition for species
	Isotopologue* iso = sp->findIsotopologue(parser.argc(startArg+2));
	if (!iso) return Messenger::error("Error defining Isotopologue reference - no Isotopologue named '%s' exists for Species '%s'.\n", parser.argc(startArg+2), sp->name());

	// Add the data to the list
	IsotopologueReference* isoRef = references_.add();
	isoRef->set(cfg, sp, iso, parser.argd(startArg+3));
	
	set_ = true;

	return true;
}

// Write keyword data to specified LineParser
bool IsotopologueReferenceListModuleKeyword::write(LineParser& parser, const char* prefix)
{
	// Loop over list of IsotopologueReferences
	ListIterator<IsotopologueReference> refIterator(references_);
	while (IsotopologueReference* ref = refIterator.iterate())
	{
		if (!parser.writeLineF("%s%s  '%s'  '%s'  '%s'  %f\n", prefix, keyword(), ref->configuration()->name(), ref->species()->name(), ref->isotopologue()->name(), ref->weight())) return false;
	}

	return true;
}

/*
 * Validation
 */

// Validate supplied value
bool IsotopologueReferenceListModuleKeyword::isValid(IsotopologueReference value)
{
	return true;
}
