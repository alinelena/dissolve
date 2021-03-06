/*
	*** Dissolve - Configurations
	*** src/main/configurations.cpp
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

#include "main/dissolve.h"
#include "classes/grain.h"
#include "classes/species.h"
#include "base/lineparser.h"
#include <string.h>

// Add new Configuration
Configuration* Dissolve::addConfiguration()
{
	Configuration* cfg = coreData_.addConfiguration();

	setUp_ = false;

	return cfg;
}

// Own specified Configuration
bool Dissolve::ownConfiguration(Configuration* cfg)
{
	// Sanity check - do we already own this Configuration?
	if (coreData_.configurations().contains(cfg)) return Messenger::error("Already own Configuration '%s', so nothing to do.\n", cfg->name());

	coreData_.configurations().own(cfg);

	setUp_ = false;

	return true;
}

// Return number of defined Configurations
int Dissolve::nConfigurations() const
{
	return coreData_.nConfigurations();
}

// Return Configuration list
List<Configuration>& Dissolve::configurations()
{
	return coreData_.configurations();
}

// Return Configuration list (const)
const List<Configuration>& Dissolve::constConfigurations() const
{
	return coreData_.configurations();
}

// Find configuration by name
Configuration* Dissolve::findConfiguration(const char* name) const
{
	return coreData_.findConfiguration(name);
}

// Find configuration by 'nice' name
Configuration* Dissolve::findConfigurationByNiceName(const char* name) const
{
	for (Configuration* cfg = constConfigurations().first(); cfg != NULL; cfg = cfg->next) if (DissolveSys::sameString(name, cfg->niceName())) return cfg;

	return NULL;
}

// Write Configuration through specified LineParser
bool Dissolve::writeConfiguration(Configuration* cfg, LineParser& parser)
{
	int molId;
	SpeciesAngle* spa;
	SpeciesBond* spb;
	SpeciesTorsion* spt;

	if (!parser.writeLineF("'%s'  %i  %i  # nMolecules  nGrains\n", cfg->name(), cfg->nMolecules(), cfg->nGrains())) return false;

	// Write unit cell (box) lengths and angles
	if (!parser.writeLineF("%12e %12e %12e  %f\n", cfg->relativeBoxLengths().x, cfg->relativeBoxLengths().y, cfg->relativeBoxLengths().z, cfg->requestedSizeFactor())) return false;
	if (!parser.writeLineF("%12e %12e %12e\n", cfg->boxAngles().x, cfg->boxAngles().y, cfg->boxAngles().z)) return false;

	// Write Molecule types - write sequential Molecules with same type as single line
	int moleculeCount = 1;
	Species* lastType = cfg->nMolecules() > 0 ? cfg->molecule(0)->species() : NULL;
	for (int n=1; n<cfg->nMolecules(); ++n)
	{
		// If the last Molecule's Species is the same as this one, increment counter and move on
		if (lastType == cfg->molecule(n)->species())
		{
			++moleculeCount;
			continue;
		}

		// Species is different between this molecule and the last - write this info, and reset the counter
		if (!parser.writeLineF("%i  '%s'\n", moleculeCount, lastType->name())) return false;
		moleculeCount = 1;
		lastType = cfg->molecule(n)->species();
	}
	// Write final 
	if (!parser.writeLineF("%i  '%s'\n", moleculeCount, lastType->name())) return false;

	// Write all Atoms - for each write type, coordinates, charge, mol ID, and grain ID
	AtomTypeList usedAtomTypes = cfg->usedAtomTypesList();
	if (!parser.writeLineF("%i  # nAtoms\n", cfg->nAtoms())) return false;
	for (int n=0; n<cfg->nAtoms(); ++n)
	{
		Atom* i = cfg->atom(n);
		AtomTypeData* atd = usedAtomTypes[i->localTypeIndex()];
		if (!parser.writeLineF("%s %e %e %e %e %i %i\n", atd ? atd->atomTypeName() : "??", i->x(), i->y(), i->z(), i->charge(), i->molecule()->arrayIndex(), i->grain() ? i->grain()->arrayIndex() : -1)) return false;
	}

	// Write all Bonds - for each write Atom IDs (in global array), molecule ID, and parameter source
	if (!parser.writeLineF("%i  # nBonds\n", cfg->nBonds())) return false;
	for (int n=0; n<cfg->nBonds(); ++n)
	{
		Bond* b = cfg->bond(n);
		molId = b->molecule() ? b->molecule()->arrayIndex() : -1;
		spb = b->speciesBond();
		if (!parser.writeLineF("%i %i %i %i %i\n", b->i()->arrayIndex(), b->j()->arrayIndex(), molId, species().indexOf(spb->parent()), spb->parent()->bondIndex(spb))) return false;
	}

	// Write all Angles - for each write Atom IDs (in global array), molecule ID, and parameter source
	if (!parser.writeLineF("%i  # nAngles\n", cfg->nAngles())) return false;
	for (int n=0; n<cfg->nAngles(); ++n)
	{
		Angle* a = cfg->angle(n);
		molId = a->molecule() ? a->molecule()->arrayIndex() : -1;
		spa = a->speciesAngle();
		if (!parser.writeLineF("%i %i %i %i %i %i\n", a->i()->arrayIndex(), a->j()->arrayIndex(), a->k()->arrayIndex(), molId, species().indexOf(spa->parent()), spa->parent()->angleIndex(spa))) return false;
	}

	// Write all Torsions - for each write Atom IDs (in global array), molecule ID, and parameter source
	if (!parser.writeLineF("%i  # nTorsions\n", cfg->nTorsions())) return false;
	for (int n=0; n<cfg->nTorsions(); ++n)
	{
		Torsion* t = cfg->torsion(n);
		molId = t->molecule() ? t->molecule()->arrayIndex() : -1;
		spt = t->speciesTorsion();
		if (!parser.writeLineF("%i %i %i %i %i %i %i\n", t->i()->arrayIndex(), t->j()->arrayIndex(), t->k()->arrayIndex(), t->l()->arrayIndex(), molId, species().indexOf(spt->parent()), spt->parent()->torsionIndex(spt))) return false;
	}

	return true;
}

// Read Configuration through specified LineParser
bool Dissolve::readConfiguration(Configuration* cfg, LineParser& parser)
{
	Molecule* mol;
	Grain* grain;
	AtomType* atomType;
	Bond* bond;
	Angle* angle;
	Torsion* torsion;

	// Clear current contents of Configuration
	cfg->empty();

	// Read configuration name, nMolecules, and nGrains, and initialise those arrays
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	cfg->setName(parser.argc(0));
	cfg->initialise(parser.argi(1), parser.argi(2)); 

	// Read unit cell (box) lengths and angles
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	cfg->setRelativeBoxLengths(parser.arg3d(0));
	cfg->setRequestedSizeFactor(parser.hasArg(3) ? parser.argd(3) : 1.0);
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	cfg->setBoxAngles(parser.arg3d(0));

	// Read Species types for Molecules
	int nMolsRead = 0;
	while (nMolsRead < cfg->nMolecules())
	{
		// Read line containing number of molecules and Species name
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
		Species* sp = findSpecies(parser.argc(1));
		if (!sp) return Messenger::error("Unrecognised Species '%s' found in Configuration '%s' in restart file.\n", parser.argc(1), cfg->name());

		// Set Species pointers for this range of Molecules
		for (int n=0; n<parser.argi(0); ++n) cfg->molecule(nMolsRead+n)->setSpecies(sp);
		nMolsRead += parser.argi(0);
	}

	// Read in Atoms
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	int nAtoms = parser.argi(0);
	for (int n=0; n<nAtoms; ++n)
	{
		// Each line contains type, coordinates, charge, mol ID, and grain ID
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

		// Find the AtomType
		atomType = findAtomType(parser.argc(0));
		if (!atomType) return Messenger::error("Configuration '%s' references AtomType '%s', but it is not known to Dissolve.\n", cfg->name(), parser.argc(0));

		mol = cfg->molecule(parser.argi(5));
		grain = parser.argi(6) == -1 ? NULL : cfg->grain(parser.argi(6));

		// Add Atom
		cfg->addAtom(mol, grain, atomType, parser.arg3d(1), parser.argd(4));
	}

	// Read Bonds
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	int nBonds = parser.argi(0);
	for (int n=0; n<nBonds; ++n)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

		// Add Bond
		mol = cfg->molecule(parser.argi(2));
		bond = cfg->addBond(mol, parser.argi(0), parser.argi(1));

		// Set parameters
		bond->setSpeciesBond(species(parser.argi(3))->bond(parser.argi(4)));
	}

	// Read Angles
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	int nAngles = parser.argi(0);
	for (int n=0; n<nAngles; ++n)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

		// Add Angle
		mol = cfg->molecule(parser.argi(3));
		angle = cfg->addAngle(mol, parser.argi(0), parser.argi(1), parser.argi(2));

		// Set parameters
		angle->setSpeciesAngle(species(parser.argi(4))->angle(parser.argi(5)));
	}

	// Read Torsions
	if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;
	int nTorsions = parser.argi(0);
	for (int n=0; n<nTorsions; ++n)
	{
		if (parser.getArgsDelim(LineParser::Defaults) != LineParser::Success) return false;

		// Add Torsion
		mol = cfg->molecule(parser.argi(4));
		torsion = cfg->addTorsion(mol, parser.argi(0), parser.argi(1), parser.argi(2), parser.argi(3));

		// Set parameters
		torsion->setSpeciesTorsion(species(parser.argi(5))->torsion(parser.argi(6)));
	}

	cfg->finaliseAfterLoad(worldPool(), pairPotentialRange_, nBoxNormalisationPoints_);

	return true;
}
