/*
	*** Species Definition
	*** src/classes/species.h
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

#ifndef DUQ_SPECIES_H
#define DUQ_SPECIES_H

#include "templates/list.h"
#include "templates/array2d.h"
#include "classes/speciesangle.h"
#include "classes/speciesatom.h"
#include "classes/speciesbond.h"
#include "classes/speciestorsion.h"
#include "classes/speciesgrain.h"
#include "classes/isotopologue.h"
#include "base/charstring.h"

// Forward Declarations
class Box;

/*
 * Species Definition
 */
class Species : public ListItem<Species>
{
	public:
	// Constructor
	Species();
	// Destructor
	~Species();
	// Clear Data
	void clear();


	/*
	 * Basic Information
	 */
	private:
	// Name of the Species
	CharString name_;

	public:
	// Set name of the Species
	void setName(const char* name);
	// Return the name of the Species
	const char* name() const;
	// Check setup
	bool checkSetup(const List<AtomType>& atomTypes);
	// Print Species information
	void print();


	/*
	 * Atomic Information
	 */
	private:
	// List of Atoms in the Species
	List<SpeciesAtom> atoms_;
	// List of selected Atoms
	RefList<SpeciesAtom,int> selectedAtoms_;
	
	public:
	// Add a new Atom to the Species
	SpeciesAtom* addAtom(int element = 0, double rx = 0.0, double ry = 0.0, double rz = 0.0);
	// Return the number of atoms in the species
	int nAtoms() const;
	// Return the first atom in the Species
	SpeciesAtom* atoms() const;
	// Return the nth atom in the Species
	SpeciesAtom* atom(int n);
	// Clear current Atom selection
	void clearAtomSelection();
	// Add Atom to selection
	void selectAtom(SpeciesAtom* i);
	// Select Atoms along any path from the specified one
	void selectFromAtom(SpeciesAtom* i, SpeciesBond* exclude);
	// Return first selected Atom reference
	RefListItem<SpeciesAtom,int>* selectedAtoms() const;
	// Return nth selected Atom
	SpeciesAtom* selectedAtom(int n);
	// Return number of selected Atoms
	int nSelectedAtoms() const;
	// Return whether specified Atom is selected
	bool isAtomSelected(SpeciesAtom* i) const;
	// Change element of specified Atom
	void changeAtomElement(SpeciesAtom* i, int el, AtomType* at);
	// Return total atomic mass of Species
	double mass() const;


	/*
	 * Intramolecular Data
	 */
	private:
	// List of Bonds between Atoms in the Species
	List<SpeciesBond> bonds_;
	// List of Angles between Atoms in the Species
	List<SpeciesAngle> angles_;
	// List of Torsions between Atoms in the Species
	List<SpeciesTorsion> torsions_;
	// List of master Bonds (parameters only) for this Species
	List<SpeciesBond> masterBonds_;
	// List of master Angles (parameters only) for this Species
	List<SpeciesAngle> masterAngles_;
	// List of master Torsions (parameters only) for this Species
	List<SpeciesTorsion> masterTorsions_;

	public:
	// Add new Bond definition (from SpeciesAtom*)
	SpeciesBond* addBond(SpeciesAtom* i, SpeciesAtom* j);
	// Add new Bond definition
	SpeciesBond* addBond(int i, int j);
	// Return number of Bonds in list
	int nBonds() const;
	// Return list of Bonds
	SpeciesBond* bonds() const;
	// Return nth Bond
	SpeciesBond* bond(int n);
	// Return whether Bond between Atoms exists
	SpeciesBond* hasBond(SpeciesAtom* i, SpeciesAtom* j) const;
	// Add new Angle definition
	SpeciesAngle* addAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k);
	// Add new Angle definition
	SpeciesAngle* addAngle(int i, int j, int k);
	// Return number of Angles in list
	int nAngles() const;
	// Return list of Angles
	SpeciesAngle* angles() const;
	// Return nth Angle
	SpeciesAngle* angle(int n);
	// Return whether Angle between Atoms exists
	bool hasAngle(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k) const;
	// Add new Torsion definition (from SpeciesAtom*)
	SpeciesTorsion* addTorsion(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l);
	// Add new Torsion definition
	SpeciesTorsion* addTorsion(int i, int j, int k, int l);
	// Return number of Torsions in list
	int nTorsions() const;
	// Return list of Torsions
	SpeciesTorsion* torsions() const;
	// Return nth Torsion
	SpeciesTorsion* torsion(int n);
	// Return whether Torsion between Atoms exists
	bool hasTorsion(SpeciesAtom* i, SpeciesAtom* j, SpeciesAtom* k, SpeciesAtom* l) const;
	// Add new master Bond definition
	SpeciesBond* addMasterBond(const char* name);
	// Return number of master Bonds in list
	int nMasterBonds() const;
	// Return list of master Bonds
	SpeciesBond* masterBonds() const;
	// Return nth master Bond
	SpeciesBond* masterBond(int n);
	// Return whether named master Bond exists
	SpeciesBond* hasMasterBond(const char* name) const;
	// Add new master Angle definition
	SpeciesAngle* addMasterAngle(const char* name);
	// Return number of master Angles in list
	int nMasterAngles() const;
	// Return list of master Angles
	SpeciesAngle* masterAngles() const;
	// Return nth master Angle
	SpeciesAngle* masterAngle(int n);
	// Return whether named master Angle exists
	SpeciesAngle* hasMasterAngle(const char* name) const;
	// Add new master Torsion definition
	SpeciesTorsion* addMasterTorsion(const char* name);
	// Return number of master Torsions in list
	int nMasterTorsions() const;
	// Return list of master Torsions
	SpeciesTorsion* masterTorsions() const;
	// Return nth master Torsion
	SpeciesTorsion* masterTorsion(int n);
	// Return whether named master Torsion exists
	SpeciesTorsion* hasMasterTorsion(const char* name) const;


	/*
	 * Grains
	 */
	private:
	// List of Grains, dividing the Atoms of this Species into individual groups
	List<SpeciesGrain> grains_;
	
	public:
	// Update SpeciesGrains after change
	void updateGrains();
	// Add default grain definition (i.e. one which contains all atoms) for this Species 
	void addDefaultGrain();
	// Automatically determine Grains based on chemical connectivity
	void autoAddGrains();
	// Add new grain for this Species
	SpeciesGrain* addGrain();
	// Remove grain with ID specified
	void removeGrain(SpeciesGrain* sg);
	// Return number of grain present for this Species
	int nGrains() const;
	// Return first grain in list
	SpeciesGrain* grains() const;
	// Return nth grain in list
	SpeciesGrain* grain(int n);
	// Add Atom to grain
	void addAtomToGrain(SpeciesAtom* i, SpeciesGrain* gd);
	// Generate unique grain name with base name provided
	const char* uniqueGrainName(const char* baseName, SpeciesGrain* exclude = NULL) const;
	// Order atoms within grains
	void orderAtomsWithinGrains();


	/*
	 * Isotopologues
	 */
	private:
	// List of isotopic variants defined for this species
	List<Isotopologue> isotopologues_;

	public:
	// Update current Isotopologues
	void updateIsotopologues(const List<AtomType>& atomTypes);
	// Add a new Isotopologue to this Species
	Isotopologue* addIsotopologue(const char* baseName = "UnnamedIsotopologue");
	// Remove specified Isotopologue from this Species
	void removeIsotopologue(Isotopologue* iso);
	// Return number of defined Isotopologues
	int nIsotopologues() const;
	// Return first Isotopologue defined
	Isotopologue* isotopologues() const;
	// Return nth Isotopologue defined
	Isotopologue* isotopologue(int n);
	// Return whether the specified Isotopologue exists
	bool hasIsotopologue(Isotopologue* iso) const;
	// Generate unique Isotopologue name with base name provided
	const char* uniqueIsotopologueName(const char* baseName, Isotopologue* exclude = NULL) const;
	// Search for Isotopologue by name
	Isotopologue* findIsotopologue(const char* name) const;
	// Return index of specified Isotopologue
	int indexOfIsotopologue(Isotopologue* iso) const;


	/*
	 * Transforms
	 */
	public:
	// Calculate and return centre of geometry
	Vec3<double> centreOfGeometry(const Box* box) const;
	// Set centre of geometry
	void setCentre(const Box* box, const Vec3<double> newCentre);
	// Centre coordinates at origin
	void centreAtOrigin();


	/*
	 * File Input / Output
	 */
	public:
	// Load Species information from XYZ file
	bool loadFromXYZ(const char* filename);
	// Load Species from file
	bool load(const char* filename);


	/*
	 * Parallel Comms
	 */
	public:
	// Broadcast data from Master to all Slaves
	bool broadcast(ProcessPool& procPool, const List<AtomType>& atomTypes);
};

#endif
