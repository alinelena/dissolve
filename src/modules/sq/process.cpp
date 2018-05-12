/*
	*** SQ Module - Processing
	*** src/modules/sq/process.cpp
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

#include "modules/sq/sq.h"
#include "main/duq.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "modules/rdf/rdf.h"
#include "templates/genericlisthelper.h"

// Run pre-processing stage
bool SQModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Run main processing
bool SQModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Calculate S(Q) from Configuration's g(r).
	 * 
	 * This is a serial routine, with each process constructing its own copy of the data.
	 */

	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0)
	{
		Messenger::warn("No Configuration targets for Module.\n");
		return true;
	}

	CharString varName;

	GenericList& moduleData = configurationLocal_ ? targetConfigurations_.firstItem()->moduleData() : duq.processingModuleData();

	const BroadeningFunction& qBroadening = KeywordListHelper<BroadeningFunction>::retrieve(keywords_, "QBroadening", BroadeningFunction());
	const double qDelta = keywords_.asDouble("QDelta");
	const double qMin = keywords_.asDouble("QMin");
	double qMax = keywords_.asDouble("QMax");
	if (qMax < 0.0) qMax = 30.0;
	const bool saveData = keywords_.asBool("Save");
	const bool testMode = keywords_.asBool("Test");
	const double testThreshold = keywords_.asDouble("TestThreshold");
	const WindowFunction& windowFunction = KeywordListHelper<WindowFunction>::retrieve(keywords_, "WindowFunction", WindowFunction());

	// Print argument/parameter summary
	Messenger::print("SQ: Calculating S(Q)/F(Q) over %f < Q < %f Angstroms**-1 using step size of %f Angstroms**-1.\n", qMin, qMax, qDelta);
	if (windowFunction.function() == WindowFunction::NoWindow) Messenger::print("SQ: No window function will be applied in Fourier transforms of g(r) to S(Q).");
	else Messenger::print("SQ: Window function to be applied in Fourier transforms is %s (%s).", WindowFunction::functionType(windowFunction.function()), windowFunction.parameterSummary().get());
	if (qBroadening.function() == BroadeningFunction::NoFunction) Messenger::print("SQ: No broadening will be applied to calculated S(Q).");
	else Messenger::print("SQ: Broadening to be applied in calculated S(Q) is %s (%s).", BroadeningFunction::functionType(qBroadening.function()), qBroadening.parameterSummary().get());
	Messenger::print("SQ: Save data is %s.\n", DUQSys::onOff(saveData));

	/*
	 * Loop over target Configurations and Fourier transform their UnweightedGR into the UnweightedSQ.
	 */
	RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
	while (Configuration* cfg = configIterator.iterate())
	{
		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Get unweighted g(r) for this Configuration - we don't supply a specific Module prefix, since the unweighted g(r) may come from one of many RDF-type modules
		if (!cfg->moduleData().contains("UnweightedGR")) return Messenger::error("Couldn't locate UnweightedGR for Configuration '%s'.\n", cfg->name());
		PartialSet& unweightedgr = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "UnweightedGR");

		// Does a PartialSet already exist for this Configuration?
		bool wasCreated;
		PartialSet& unweightedsq = GenericListHelper<PartialSet>::realise(cfg->moduleData(), "UnweightedSQ", "SQ", GenericItem::InRestartFileFlag, &wasCreated);
		if (wasCreated) unweightedsq.setUpPartials(unweightedgr.atomTypes(), cfg->niceName(), "unweighted", "sq", "Q, 1/Angstroms");

		// Is the PartialSet already up-to-date?
		if (DUQSys::sameString(unweightedsq.fingerprint(), CharString("%i", cfg->coordinateIndex())))
		{
			Messenger::print("SQ: Unweighted partial S(Q) are up-to-date for Configuration '%s'.\n", cfg->name());
			continue;
		}

		// Transform g(r) into S(Q)
		if (!calculateUnweightedSQ(procPool, cfg, unweightedgr, unweightedsq, qMin, qDelta, qMax, cfg->atomicDensity(), windowFunction, qBroadening)) return false;

		// Set names of resources (XYData) within the PartialSet
		unweightedsq.setObjectNames(CharString("%s//%s//%s", cfg->niceName(), "SQ", "UnweightedSQ"));

		// Save data if requested
		if (saveData && configurationLocal_ && (!MPIRunMaster(procPool, unweightedsq.save()))) return false;

		// If we are associated to a local Configuration, copy the partial data over to the processing module list
		if (configurationLocal_)
		{
			PartialSet newSet = GenericListHelper<PartialSet>::realise(duq.processingModuleData(), "UnweightedSQ", uniqueName_, GenericItem::InRestartFileFlag);
			newSet = unweightedsq;
			newSet.setObjectNames(CharString("%s//UnweightedSQ", uniqueName_.get()));
		}

		// Test unweighted S(Q)?
		if (testMode && configurationLocal_)
		{
			Messenger::print("\nTesting calculated unweighted S(Q) data against supplied datasets (if any)...\n");
			if (!RDFModule::testReferencePartials(moduleData, uniqueName(), unweightedsq, "TestReferenceSQ-unweighted", testThreshold)) return false;
		}
	}

	// If we are a main processing task, construct the weighted sum of Configuration partials and store in the processing module data list
	if (!configurationLocal_)
	{
		// Create/retrieve PartialSet for summed partial S(Q)
		PartialSet& summedUnweightedSQ = GenericListHelper<PartialSet>::realise(duq.processingModuleData(), "UnweightedSQ", uniqueName_, GenericItem::InRestartFileFlag);

		// Sum the partials from the associated Configurations
		if (!sumUnweightedSQ(procPool, this, duq.processingModuleData(), summedUnweightedSQ)) return false;

		// Test unweighted S(Q)?
		if (testMode)
		{
			Messenger::print("\nTesting calculated unweighted S(Q) data against supplied datasets (if any)...\n");
			if (!RDFModule::testReferencePartials(moduleData, uniqueName(), summedUnweightedSQ, "TestReferenceSQ-unweighted", testThreshold)) return false;
		}
	}

	return true;
}

// Run post-processing stage
bool SQModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}