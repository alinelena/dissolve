/*
	*** MolShake Module - Method
	*** src/modules/molshake/method.cpp
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

#include "modules/molshake/molshake.h"
#include "main/duq.h"
#include "classes/box.h"
#include "classes/cell.h"
#include "classes/changestore.h"
#include "classes/configuration.h"
#include "classes/energykernel.h"
#include "base/processpool.h"
#include "base/timer.h"

// Perform set up tasks for module
bool MolShakeModule::setup(ProcessPool& procPool)
{
	return true;
}

// Execute pre-processing stage
bool MolShakeModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Execute Method
bool MolShakeModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Perform a Molecule shake
	 * 
	 * This is a parallel routine, with processes operating in groups.
	 */

	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0)
	{
		Messenger::warn("No Configuration targets for Module.\n");
		return true;
	}

	// Get target Configuration
	Configuration* cfg = targetConfigurations_.firstItem();

	// Retrieve control parameters from Configuration
	double cutoffDistance = GenericListHelper<double>::retrieve(cfg->moduleData(), "CutoffDistance", uniqueName(), keywords_.asDouble("CutoffDistance"));
	if (cutoffDistance < 0.0) cutoffDistance = duq.pairPotentialRange();
	const int nShakesPerMolecule = GenericListHelper<int>::retrieve(cfg->moduleData(), "ShakesPerMolecule", uniqueName(), keywords_.asInt("ShakesPerMolecule"));
	const double targetAcceptanceRate = GenericListHelper<double>::retrieve(cfg->moduleData(), "TargetAcceptanceRate", uniqueName(), keywords_.asDouble("TargetAcceptanceRate"));
	double stepSize = GenericListHelper<double>::retrieve(cfg->moduleData(), "StepSize", uniqueName(), keywords_.asDouble("StepSize"));
	const double termScale = 1.0;
	const double rRT = 1.0/(.008314472*cfg->temperature());

	// Print argument/parameter summary
	Messenger::print("MolShake: Cutoff distance is %f\n", cutoffDistance);
	Messenger::print("MolShake: Performing %i shake(s) per Molecule\n", nShakesPerMolecule);
	Messenger::print("MolShake: Translation step is %f Angstroms, target acceptance rate is %f.\n", stepSize, targetAcceptanceRate);

	// Initialise a Molecule distributor
// 	xxx write this!
	const bool willBeModified = true, allowRepeats = false;
	CellArray& cellArray = cfg->cells();
	cellArray.initialiseDistribution();

	// Create a local ChangeStore and EnergyKernel
	ChangeStore changeStore(procPool);
	EnergyKernel kernel(procPool, cfg, duq.potentialMap(), cutoffDistance);

	// Initialise the random number buffer
	procPool.initialiseRandomBuffer(ProcessPool::Group);

	// Enter calculation loop until no more Cells are available
	int cellId, shake, n, nbr;
	int nTries = 0, nAccepted = 0;
	bool accept;
	double currentEnergy, intraEnergy, newEnergy, newIntraEnergy, delta, totalDelta = 0.0;
	Cell* cell;
	Vec3<double> centre, rDelta;
	Atom** cellAtoms;

	Timer timer;
	procPool.resetAccumulatedTime();
	while (cellId = cellArray.nextAvailableCell(procPool, willBeModified, allowRepeats), cellId != Cell::AllCellsComplete)
	{
		// Check for valid cell
		if (cellId == Cell::NoCellsAvailable)
		{
			// No valid cell, but still need to enter into change distribution with other processes
			changeStore.distributeAndApply(cfg);
			cellArray.finishedWithCell(procPool, willBeModified, cellId);
			continue;
		}
		cell = cellArray.cell(cellId);
		Messenger::printVerbose("MolShake: Cell %i now the target on process %s, containing %i Atoms interacting with %i neighbour cells.\n", cellId, procPool.processInfo(), cell->nAtoms(), cell->nTotalCellNeighbours());

		/*
		 * Calculation Begins
		 */

		// Set current atom targets in ChangeStore (entire cell contents)
		changeStore.add(cell);

		// Loop over atoms in this cell
		cellAtoms = cell->atoms().objects();
		for (n = 0; n < cell->atoms().nItems(); ++n)
		{
			// Grab Atom pointer
			Atom* i = cellAtoms[n];

			// Calculate reference energy for atom, including intramolecular terms
			currentEnergy = kernel.energy(i, ProcessPool::OverGroupProcesses);
			intraEnergy = kernel.intraEnergy(i) * termScale;

			// Loop over number of shakes per atom
			for (shake=0; shake<nShakesPerMolecule; ++shake)
			{
				// Create a random translation vector
				rDelta.set(procPool.randomPlusMinusOne()*stepSize, procPool.randomPlusMinusOne()*stepSize, procPool.randomPlusMinusOne()*stepSize);

				// Translate atom and calculate new energy
				i->translateCoordinates(rDelta);
				newEnergy = kernel.energy(i, ProcessPool::OverGroupProcesses);
				newIntraEnergy = kernel.intraEnergy(i) * termScale;
				
				// Trial the transformed atom position
				delta = (newEnergy + newIntraEnergy) - (currentEnergy + intraEnergy);
// 				printf("delta = %f\n", delta);
				accept = delta < 0 ? true : (procPool.random() < exp(-delta*rRT));

				if (accept)
				{
// 					Messenger::print("Accepts move with delta %f\n", delta);
					// Accept new (current) position of target Atom
					changeStore.updateAtom(n);
					currentEnergy = newEnergy;
					intraEnergy = newIntraEnergy;
					totalDelta += delta;
					++nAccepted;
				}
				else changeStore.revert(n);
				
				++nTries;
			}

		}

		// Store modifications to Atom positions ready for broadcast later
		changeStore.storeAndReset();

		/*
		 * Calculation End
		 */

		// Distribute coordinate changes to all processes
		changeStore.distributeAndApply(cfg);
		changeStore.reset();

		// Must unlock the Cell when we are done with it!
		cellArray.finishedWithCell(procPool, willBeModified, cellId);
	}
	timer.stop();

	// Collect statistics from process group leaders
	if (!procPool.allSum(&nAccepted, 1, ProcessPool::Leaders)) return false;
	if (!procPool.allSum(&nTries, 1, ProcessPool::Leaders)) return false;
	if (!procPool.allSum(&totalDelta, 1, ProcessPool::Leaders)) return false;
	if (procPool.groupLeader())
	{
		double rate = double(nAccepted)/nTries;

		Messenger::print("MolShake: Overall acceptance rate was %4.2f% (%i of %i attempted moves) (%s work, %s comms)\n", 100.0*rate, nAccepted, nTries, timer.totalTimeString(), procPool.accumulatedTimeString());
		Messenger::print("MolShake: Total energy delta was %10.4e kJ/mol.\n", totalDelta);

		// Adjust step size - if nAccepted was zero, just decrease the current stepSize by a constant factor
		stepSize *= (nAccepted == 0) ? 0.8 : rate/targetAcceptanceRate;

		// Clamp step size
// 		if (stepSize > 0.5) stepSize = 0.5;
// 		else if (stepSize_ > maxTranslationStep_) stepSize_ = maxTranslationStep_;
// 		if (rotationStep_ < 3.0) rotationStep_ = 3.0;
	}

	// Store updated parameter values
	if (!procPool.broadcast(&stepSize, 1, 0, ProcessPool::Group)) return false;
	GenericListHelper<double>::realise(cfg->moduleData(), "StepSize", uniqueName(), GenericItem::InRestartFileFlag) = stepSize;
	Messenger::print("MolShake: Updated translation step is %f Angstroms.\n", stepSize);
	
	// Increment configuration changeCount_
	if (nAccepted > 0) cfg->incrementCoordinateIndex();

	// Update total energy
	cfg->registerEnergyChange(totalDelta);
	cfg->accumulateEnergyChange();

	return true;
}

// // Perform a world atom shake
// bool DUQ::worldMolShake(Configuration& cfg, double cutoffDistance, int nShakes, double targetAcceptanceRate, double stepSize)
// {
// 	// Control Parameters
// 	const double termScale = 1.0;	
// 	if (cutoffDistance < 0.0) cutoffDistance = pairPotentialRange_;
// 	const double rRT = 1.0/(.008314472*cfg->temperature());
// 	
// 	// Print argument/parameter summary
// 	Messenger::print("WorldMolShake: Cutoff distance is %f\n", cutoffDistance);
// 	Messenger::print("WorldMolShake: Translation step is %f Angstroms, target acceptance rate is %f.\n", stepSize, targetAcceptanceRate);
// 
// 	// Initialise the Cell distributor
// 	const bool willBeModified = true, allowRepeats = false;
// 	cfg->initialiseCellDistribution();
// 
// 	// Create a local ChangeStore and EnergyKernel
// 	ChangeStore changeStore;
// 	EnergyKernel kernel(cfg, potentialMap_, cutoffDistance);
// 
// 	// Initialise the random number buffer
// 	procPool.initialiseRandomBuffer(ProcessPool::Pool);
// 
// 	// Enter calculation loop until no more Cells are available
// 	int cellId, shake, n, i;
// 	int nTries = 0, nAccepted = 0;
// 	bool accept;
// 	double currentEnergy, newEnergy, delta, totalDelta = 0.0;
// 	Cell* cell;
// 	Vec3<double> rDelta;
// 	int nAtoms = cfg->nAtoms();
// 	Atom* atoms = cfg->atoms();
// 
// 	Timer timer;
// 	procPool.resetAccumulatedTime();
// 
// 	// Add entire cell contents to a ChangeStore
// 	for (n=0; n<nAtoms; ++n) changeStore.add(&atoms[n]);
// 
// 	// Calculate reference energy
// 	currentEnergy = interatomicEnergy(cfg) + intramolecularEnergy(cfg);
// 
// 	// Loop over nShakes
// 	for (int n=0; n<nShakes; ++n)
// 	{
// 		// Randomly displace each atom in the system
// 		for (i=0; i<nAtoms; ++i)
// 		{
// 			// Create a random translation vector
// 			rDelta.set(procPool.randomPlusMinusOne()*stepSize, procPool.randomPlusMinusOne()*stepSize, procPool.randomPlusMinusOne()*stepSize);
// 
// 			// Translate atom
// 			atoms[i].translateCoordinates(rDelta);
// 		}
// 
// 		// Update neighbour lists
// 		cfg->recreateCellAtomNeighbourLists(pairPotentialRange_);
// 
// 		// Calculate new energy
// 		newEnergy = interatomicEnergy(cfg) + intramolecularEnergy(cfg);
// 
// 		delta = newEnergy - currentEnergy;
// 		printf("delta = %f\n", delta);
// 		accept = delta < 0 ? true : (procPool.random() < exp(-delta*rRT));
// 
// 		if (accept)
// 		{
// // 			Messenger::print("Accepts move with delta %f\n", delta);
// 			// Accept new (current) positions of atoms
// 			changeStore.updateAll();
// 			currentEnergy = newEnergy;
// 			totalDelta += delta;
// 			++nAccepted;
// 		}
// 		else
// 		{
// 			changeStore.revertAll();
// 			cfg->recreateCellAtomNeighbourLists(pairPotentialRange_);
// 		}
// 	}
// 
// 	// Collect statistics from process group leaders
// 	if (!procPool.allSum(&nAccepted, 1, ProcessPool::Leaders)) return false;
// 	if (!procPool.allSum(&nTries, 1, ProcessPool::Leaders)) return false;
// 	if (!procPool.allSum(&totalDelta, 1, ProcessPool::Leaders)) return false;
// 	if (procPool.groupLeader())
// 	{
// 		double rate = double(nAccepted)/nTries;
// 
// 		Messenger::print("WorldMolShake: Overall acceptance rate was %4.2f% (%i of %i attempted moves) (%s work, %s comms)\n", 100.0*rate, nAccepted, nTries, timer.totalTimeString(), procPool.accumulatedTimeString());
// 		Messenger::print("WorldMolShake: Total energy delta was %10.4e kJ/mol.\n", totalDelta);
// 
// 		// Adjust step size
// 		stepSize *= rate/targetAcceptanceRate;
// // 		if (stepSize_ < 0.05) stepSize_ = 0.05;
// // 		else if (stepSize_ > maxTranslationStep_) stepSize_ = maxTranslationStep_;
// // 		if (rotationStep_ < 3.0) rotationStep_ = 3.0;
// 	}
// 
// 	// Store updated parameter values
// 	if (!procPool.broadcast(&stepSize, 1, 0, ProcessPool::Group)) return false;
// // 	stepSizeParam->setValue(stepSize); TODO
// 	Messenger::print("WorldMolShake: Updated translation step is %f Angstroms.\n", stepSize);
// 	
// 	// Increment configuration changeCount_
// 	if (nAccepted > 0) cfg->incrementCoordinateIndex();
// 
// 	// Update total energy
// 	cfg->registerEnergyChange(totalDelta);
// 	cfg->accumulateEnergyChange();
// }

// Execute post-processing stage
bool MolShakeModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return true;
}