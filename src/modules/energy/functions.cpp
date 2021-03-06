/*
	*** Energy Module - Functions
	*** src/modules/energy/functions.cpp
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

#include "modules/energy/energy.h"
#include "classes/configuration.h"
#include "classes/energykernel.h"
#include "templates/genericlisthelper.h"

// Return total intramolecular energy
double EnergyModule::intraMolecularEnergy(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap)
{
	double bondEnergy, angleEnergy, torsionEnergy;

	return intraMolecularEnergy(procPool, cfg, potentialMap, bondEnergy, angleEnergy, torsionEnergy);
}

// Return total intramolecular energy, storing components in provided variables
double EnergyModule::intraMolecularEnergy(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, double& bondEnergy, double& angleEnergy, double& torsionEnergy)
{
	/*
	 * Calculate the total intramolecular energy of the system, arising from Bond, Angle, and Torsion
	 * terms in all Molecules.
	 * 
	 * This is a parallel routine, with processes operating as a standard world group.
	 */

	// Create an EnergyKernel
	EnergyKernel kernel(procPool, cfg, potentialMap);

	bondEnergy = 0;
	angleEnergy = 0;
	torsionEnergy = 0;

	ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

	// Set start/stride for parallel loop
	int start = procPool.interleavedLoopStart(strategy);
	int stride = procPool.interleavedLoopStride(strategy);

	// Loop over defined Bonds
	Bond** bonds = cfg->bonds().array();
	for (int m=start; m<cfg->nBonds(); m += stride) bondEnergy += kernel.energy(bonds[m]);

	// Loop over defined Angles
	Angle** angles = cfg->angles().array();
	for (int m=start; m<cfg->nAngles(); m += stride) angleEnergy += kernel.energy(angles[m]);

	// Loop over defined Torsions
	Torsion** torsions = cfg->torsions().array();
	for (int m=start; m<cfg->nTorsions(); m += stride) torsionEnergy += kernel.energy(torsions[m]);

	double totalIntra = bondEnergy + angleEnergy + torsionEnergy;

	Messenger::printVerbose("Intramolecular Energy (Local) is %15.9e kJ/mol (%15.9e bond + %15.9e angle + %15.9e torsion)\n", totalIntra, bondEnergy, angleEnergy, torsionEnergy);

	// Sum energy and print
	double values[3];
	values[0] = bondEnergy;
	values[1] = angleEnergy;
	values[2] = torsionEnergy;
	procPool.allSum(values, 3, strategy);
	bondEnergy = values[0];
	angleEnergy = values[1];
	torsionEnergy = values[2];

	totalIntra = bondEnergy + angleEnergy + torsionEnergy;

	Messenger::printVerbose("Intramolecular Energy (World) is %15.9e kJ/mol (%15.9e bond + %15.9e angle + %15.9e torsion)\n", totalIntra, bondEnergy, angleEnergy, torsionEnergy);

	return totalIntra;
}

// Return total interatomic energy
double EnergyModule::interAtomicEnergy(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap)
{
	/*
	 * Calculates the total interatomic energy of the system, i.e. the energy contributions from PairPotential
	 * interactions between individual Atoms, accounting for intramolecular terms
	 * 
	 * This is a parallel routine, with processes operating as process groups.
	 */

	// Create an EnergyKernel
	EnergyKernel kernel(procPool, cfg, potentialMap);

	// Set the strategy
	ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

	// Grab the Cell array and calculate total energy
	const CellArray& cellArray = cfg->cells();
	double totalEnergy = kernel.energy(cellArray, false, strategy, false);

	// Print process-local energy
	Messenger::printVerbose("Interatomic Energy (Local) is %15.9e\n", totalEnergy);

	// Sum energy over all processes in the pool and print
	procPool.allSum(&totalEnergy, 1, strategy);
	Messenger::printVerbose("Interatomic Energy (World) is %15.9e\n", totalEnergy);

	return totalEnergy;
}

// Return total energy (interatomic and intramolecular)
double EnergyModule::totalEnergy(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap)
{
	return (interAtomicEnergy(procPool, cfg, potentialMap) + intraMolecularEnergy(procPool, cfg, potentialMap));
}

// Return total energy (interatomic and intramolecular), storing components in provided variables
double EnergyModule::totalEnergy(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap, double& interEnergy, double& bondEnergy, double& angleEnergy, double& torsionEnergy)
{
	interEnergy = interAtomicEnergy(procPool, cfg, potentialMap);
	intraMolecularEnergy(procPool, cfg, potentialMap, bondEnergy, angleEnergy, torsionEnergy);

	return interEnergy + bondEnergy + angleEnergy + torsionEnergy;
}

// Return total intermolecular energy
double EnergyModule::interMolecularEnergy(ProcessPool& procPool, Configuration* cfg, const PotentialMap& potentialMap)
{
	/*
	 * Calculates the total intermolecular energy of the system, i.e. the energy contributions from PairPotential
	 * interactions between individual Atoms of different Molecules, thus neglecting intramolecular terms
	 * 
	 * This is a parallel routine, with processes operating as process groups.
	 */

	// Create an EnergyKernel
	EnergyKernel kernel(procPool, cfg, potentialMap);

	// Set the strategy
	ProcessPool::DivisionStrategy strategy = ProcessPool::PoolStrategy;

	// Grab the Cell array and calculate total energy
	const CellArray& cellArray = cfg->cells();
	double totalEnergy = kernel.energy(cellArray, true, strategy, false);

	// Print process-local energy
	Messenger::printVerbose("Intermolecular Energy (Local) is %15.9e\n", totalEnergy);

	// Sum energy over all processes in the pool and print
	procPool.allSum(&totalEnergy, 1, strategy);
	Messenger::printVerbose("Intermolecular Energy (World) is %15.9e\n", totalEnergy);

	return totalEnergy;
}

// Check energy stability of specified Configurations, returning the number that failed, or -1 if stability could not be assessed
int EnergyModule::checkStability(const RefList<Configuration,bool>& configurations)
{
	int nFailed = 0;

	RefListIterator<Configuration,bool> configIterator(configurations);
	while (Configuration* cfg = configIterator.iterate())
	{
		/*
		 * First check is for the Configuration being targeted by any EnergyModule.
		 * Then we probe the EnergyStable data.
		 */
		if (!GenericListHelper<bool>::value(cfg->moduleData(), "_IsEnergyModuleTarget", NULL, false))
		{
			Messenger::error("Configuration '%s' is not targeted by any EnergyModule, so stability cannot be assessed. Check your setup!\n", cfg->name());
			return -1;
		}
		else if (cfg->moduleData().contains("EnergyStable"))
		{
			bool stable = GenericListHelper<bool>::value(cfg->moduleData(), "EnergyStable");
			if (!stable)
			{
				Messenger::print("Energy for Configuration '%s' is not yet stable.\n", cfg->name());
				++nFailed;
			}
		}
		else
		{
			Messenger::warn("No energy stability information is present in Configuration '%s' (yet) - check your setup.\n", cfg->name());
			++nFailed;
		}
	}

	return nFailed;
}
