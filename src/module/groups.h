/*
	*** Module Groups
	*** src/module/groups.h
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

#ifndef DISSOLVE_MODULEGROUPS_H
#define DISSOLVE_MODULEGROUPS_H

#include "templates/list.h"
#include "templates/reflist.h"

// Forward Declarations
class Module;
class ModuleGroup;

// Set of Module Groups
class ModuleGroups
{
	public:
	// Constructor
	ModuleGroups();
	// Destructor
	~ModuleGroups();


	/*
	 * Module Groups
	 */
	private:
	// Current list of groups
	List<ModuleGroup> groups_;
	// RefList of all Modules present in all groups
	RefList<Module,bool> allModules_;

	public:
	// Add Module to specified group, creating it if necessary
	ModuleGroup* addModule(Module* module, const char* groupName);
	// Return current list of groups
	const List<ModuleGroup>& groups() const;
	// Return reflist of all Modules present over all groups
	const RefList<Module,bool>& modules() const;
	// Number of Modules present of all groups
	int nModules() const;
};

#endif

