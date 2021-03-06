/*
	*** Species Grain
	*** src/classes/speciesgrain.cpp
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

#include "classes/speciesgrain.h"
#include "classes/speciesangle.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "classes/empiricalformula.h"
#include "base/processpool.h"

// Constructor
SpeciesGrain::SpeciesGrain() : ListItem<SpeciesGrain>()
{
	parent_ = NULL;
}

// Destructor
SpeciesGrain::~SpeciesGrain()
{
}

/*
 * Basic Character
 */

// Set parent Species
void SpeciesGrain::setParent(Species* parent)
{
	parent_ = parent;
}

// Return parent Species
Species* SpeciesGrain::parent() const
{
	return parent_;
}

// Set name of SpeciesGrain
void SpeciesGrain::setName(const char* s)
{
	name_ = s;
}

// Return name of SpeciesGrain
const char* SpeciesGrain::name() const
{
	return name_.get();
}

/*
 * Atoms
 */

// Search current list for Atom
bool SpeciesGrain::containsAtom(SpeciesAtom* i) const
{
	return atoms_.contains(i);
}

// Add specified Atom to list
void SpeciesGrain::addAtom(SpeciesAtom* i)
{
	atoms_.add(i);
	i->setGrain(this);
}

// Remove specified Atom from list
void SpeciesGrain::removeAtom(SpeciesAtom* i)
{
	atoms_.remove(i);
	i->setGrain(NULL);
}

// Return number of Atoms in the list
int SpeciesGrain::nAtoms() const
{
	return atoms_.nItems();
}

// Return first Atom reference in list
RefListItem<SpeciesAtom,int>* SpeciesGrain::atoms() const
{
	return atoms_.first();
}

// Return nth Atom reference in list
RefListItem<SpeciesAtom,int>* SpeciesGrain::atom(int n)
{
	return atoms_[n];
}

// Return empirical name of SpeciesGrain from constituent Atoms
const char* SpeciesGrain::nameFromAtoms()
{
	static EmpiricalFormula formula;
	formula.reset();
	for (RefListItem<SpeciesAtom,int>* ri = atoms_.first(); ri != NULL; ri = ri->next) formula.add(ri->item->element());
	return formula.formula();
}

/*
 * Connections
 */

// Clear all intra- and inter-Grain terms
void SpeciesGrain::clearConnections()
{
	internalBonds_.clear();
	internalAngles_.clear();
	bondConnections_.clear();
	angleConnections_.clear();
}

// Add intra-Grain Bond
void SpeciesGrain::addInternalBond(SpeciesBond* b)
{
	internalBonds_.add(b);
}

// Return first local intra-Grain Bond
RefListItem<SpeciesBond, int>* SpeciesGrain::internalBonds() const
{
	return internalBonds_.first();
}

// Add intra-Grain Angle
void SpeciesGrain::addInternalAngle(SpeciesAngle* a)
{
	internalAngles_.add(a);
}

// Return first local intra-Grain Angle
RefListItem<SpeciesAngle, int>* SpeciesGrain::internalAngles() const
{
	return internalAngles_.first();
}

// Add Bond connection
void SpeciesGrain::addBondConnection(SpeciesBond* b)
{
	bondConnections_.add(b);
}

// Return first Bond connection
RefListItem<SpeciesBond, int>* SpeciesGrain::bondConnections() const
{
	return bondConnections_.first();
}

// Add Angle connection
void SpeciesGrain::addAngleConnection(SpeciesAngle* a)
{
	angleConnections_.add(a);
}

// Return first Angle connection
RefListItem<SpeciesAngle, int>* SpeciesGrain::angleConnections() const
{
	return angleConnections_.first();
}

/*
 * Parallel Comms
 */

// Broadcast data from Master to all Slaves
bool SpeciesGrain::broadcast(ProcessPool& procPool, const List<SpeciesAtom>& atoms, const List<SpeciesBond>& bonds, const List<SpeciesAngle>& angles)
{
#ifdef PARALLEL
	int n, count, index;

	// Name
	if (!procPool.broadcast(name_)) return false;
	
	// Atoms
	count = atoms_.nItems();
	if (!procPool.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (procPool.isMaster()) index = atoms_.item(n)->index();
		if (!procPool.broadcast(&index, 1)) return false;
		if (procPool.isSlave()) addAtom(atoms.item(index));
	}

	// Intra-grain bonds / angles
	count = internalBonds_.nItems();
	if (!procPool.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (procPool.isMaster()) index = bonds.indexOf(internalBonds_.item(n));
		if (!procPool.broadcast(&index, 1)) return false;
		if (procPool.isSlave()) addInternalBond(bonds.item(index));
	}
	count = internalAngles_.nItems();
	if (!procPool.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (procPool.isMaster()) index = angles.indexOf(internalAngles_.item(n));
		if (!procPool.broadcast(&index, 1)) return false;
		if (procPool.isSlave()) addInternalAngle(angles.item(index));
	}

	// Inter-grain (connective) bonds / angles
	count = bondConnections_.nItems();
	if (!procPool.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (procPool.isMaster()) index = bonds.indexOf(bondConnections_.item(n));
		if (!procPool.broadcast(&index, 1)) return false;
		if (procPool.isSlave()) addBondConnection(bonds.item(index));
	}
	count = angleConnections_.nItems();
	if (!procPool.broadcast(&count, 1)) return false;
	for (n=0; n<count; ++n)
	{
		if (procPool.isMaster()) index = angles.indexOf(angleConnections_.item(n));
		if (!procPool.broadcast(&index, 1)) return false;
		if (procPool.isSlave()) addAngleConnection(angles.item(index));
	}
#endif
	return true;
}
