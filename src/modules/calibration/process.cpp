/*
	*** Calibration Module - Processing
	*** src/modules/calibration/process.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/calibration/calibration.h"
#include "main/duq.h"
#include "modules/energy/energy.h"
#include "base/sysfunc.h"
#include "templates/genericlisthelper.h"
#include "templates/praxis.h"

// Return whether the Module has a processing stage
bool CalibrationModule::hasProcessing()
{
	return true;
}

// Run main processing
bool CalibrationModule::process(DUQ& duq, ProcessPool& procPool)
{
	/* 
	 * Perform calibration of various aspects
	 */

	// Retrieve / print keyword setup
	const bool onlyWhenStable = keywords_.asBool("OnlyWhenStable");

	if (onlyWhenStable) Messenger::print("Calibration: Adjustments will only be performed if all related Configuration energies are stable.\n");


	/*
	 * IntraBroadening Calibration
	 */

	if (intraBroadeningModules_.nItems() > 0)
	{
		/*
		 * Make a list of all Configurations related to all RDF module
		 */
		RefList<Configuration,bool> configs;
		RefListIterator<Module,bool> rdfModuleIterator(intraBroadeningModules_);
		while (Module* module = rdfModuleIterator.iterate())
		{
			RefListIterator<Configuration,bool> configIterator(module->targetConfigurations());
			while (Configuration* cfg = configIterator.iterate()) configs.addUnique(cfg);
		}
		Messenger::print("%i Configuration(s) are involved over all RDF Module targets.\n", configs.nItems());


		/*
		 * Are the energies of all involved Configurations stable (if OnlyWhenStable option is on)
		 */
		if (onlyWhenStable)
		{
			int stabilityResult = EnergyModule::checkStability(configs);
			if (stabilityResult == -1) return false;
			else if (stabilityResult != 0)
			{
				Messenger::print("At least one Configuration energy is not yet stable. No adjustments will be made this iteration.\n");
				return true;
			}
		}


		/*
		 * Assemble a list of fitting parameters from the associated RDF modules and their BroadeningFunctions
		 */
		CalibrationModuleCostFunctions costFunctions(duq, procPool, intraBroadeningModules_, intraBroadeningReferences_);
		PrAxis<CalibrationModuleCostFunctions> broadeningMinimiser(costFunctions, &CalibrationModuleCostFunctions::intraBroadeningCost);
		rdfModuleIterator.restart();
		while (Module* module = rdfModuleIterator.iterate())
		{
			// Retrieve the BroadeningFunction
			BroadeningFunction& broadening = KeywordListHelper<BroadeningFunction>::retrieve(module->keywords(), "IntraBroadening", BroadeningFunction());

			// Add its parameters to our minimiser
			for (int n=0; n<broadening.nParameters(); ++n) broadeningMinimiser.addTarget(broadening.parameters()[n]);
		}

		// Optimise the parameters - the cost function will regenerate the UnweightedGR in the RDF modules, and reassemble the target NeutronSQ data
		double error = broadeningMinimiser.minimise(0.1);

		// Final update of BroadeningFunctions
		rdfModuleIterator.restart();
		while (Module* rdfModule = rdfModuleIterator.iterate())
		{
			// Retrieve the BroadeningFunction
			BroadeningFunction& broadening = KeywordListHelper<BroadeningFunction>::retrieve(rdfModule->keywords(), "IntraBroadening", BroadeningFunction());
			broadening.setUpDependentParameters();
			Messenger::print("Module '%s' IntraBroadening parameters now: %s\n", rdfModule->uniqueName(), broadening.parameterSummary().get());
		}

		Messenger::print("Total error over all specified datasets is %f%%.\n", error);
	}

	return true;
}
