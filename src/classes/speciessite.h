/*
	*** Species Site
	*** src/classes/speciessite.h
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

#ifndef DISSOLVE_SPECIESSITE_H
#define DISSOLVE_SPECIESSITE_H

#include "base/charstring.h"
#include "templates/array.h"
#include "templates/list.h"
#include "templates/listitem.h"
#include "templates/vector3.h"
#include "templates/reflist.h"

// Forward Declarations
class Species;
class SpeciesAtom;

/*
 * Species Site Definition
 */
class SpeciesSite : public ListItem<SpeciesSite>
{
	public:
	// Constructor
	SpeciesSite();
	// Destructor
	~SpeciesSite();


	/*
	 * Definition
	 */
	private:
	// Name of site
	CharString name_;
	// Parent Species
	Species* parent_;
	// List of SpeciesAtoms whose average position is the origin of the site
	RefList<SpeciesAtom,bool> originAtoms_;
	// Whether the origin should be calculated with mass-weighted positions
	bool originMassWeighted_;
	// SpeciesAtom(s) that indicate the x axis with the origin
	RefList<SpeciesAtom,bool> xAxisAtoms_;
	// SpeciesAtom(s) that indicate the y axis with the origin, after orthogonalisation
	RefList<SpeciesAtom,bool> yAxisAtoms_;

	public:
	// Set name of site
	void setName(const char* newName);
	// Return anme of site
	const char* name() const;
	// Set Species parent
	void setParent(Species* sp);
	// Return species parent
	Species* parent();
	// Add origin atom
	bool addOriginAtom(SpeciesAtom* originAtom);
	// Add origin atom from index
	bool addOriginAtom(int atomIndex);
	// Return list of origin atoms
	const RefList<SpeciesAtom,bool>& originAtoms();
	// Return integer array of indices from which the origin should be formed
	Array<int> originAtomIndices() const;
	// Set whether the origin should be calculated with mass-weighted positions
	void setOriginMassWeighted(bool b);
	// Return whether the origin should be calculated with mass-weighted positions
	bool originMassWeighted() const;
	// Add x-axis atom
	bool addXAxisAtom(SpeciesAtom* xAxisAtom);
	// Add x-axis atom from index
	bool addXAxisAtom(int atomIndex);
	// Return list of x-axis atoms
	const RefList<SpeciesAtom,bool>& xAxisAtoms();
	// Return integer array of indices from which x-axis should be formed
	Array<int> xAxisAtomIndices() const;
	// Add y-axis atom
	bool addYAxisAtom(SpeciesAtom* yAxisAtom);
	// Add y-axis atom from indey
	bool addYAxisAtom(int atomIndex);
	// Return list of y-axis atoms
	const RefList<SpeciesAtom,bool>& yAxisAtoms();
	// Return integer array of indices from which y-axis should be formed
	Array<int> yAxisAtomIndices() const;
	// Return whether the site has defined axes sites
	bool hasAxes() const;
};

#endif
