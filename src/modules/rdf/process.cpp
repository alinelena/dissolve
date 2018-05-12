/*
	*** RDF Module - Processing
	*** src/modules/rdf/process.cpp
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

#include "modules/rdf/rdf.h"
#include "main/duq.h"
#include "classes/box.h"
#include "classes/configuration.h"
#include "classes/species.h"
#include "classes/weights.h"
#include "templates/genericlisthelper.h"

// Run pre-processing stage
bool RDFModule::preProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}

// Run main processing
bool RDFModule::process(DUQ& duq, ProcessPool& procPool)
{
	/*
	 * Calculate standard partial g(r)
	 * 
	 * This is a serial routine, with each process constructing its own copy of the data.
	 * Partial calculation routines called by this routine are parallel.
	 */

	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0)
	{
		Messenger::warn("No Configuration targets for Module.\n");
		return true;
	}

	CharString varName;

	GenericList& moduleData = configurationLocal_ ? targetConfigurations_.firstItem()->moduleData() : duq.processingModuleData();

	const bool allIntra = keywords_.asBool("AllIntra");
	const int averaging = keywords_.asInt("Averaging");
	RDFModule::AveragingScheme averagingScheme = RDFModule::averagingScheme(keywords_.asString("AveragingScheme"));
	if (averagingScheme == RDFModule::nAveragingSchemes)
	{
		Messenger::error("Partials: Invalid averaging scheme '%s' found.\n", keywords_.asString("AveragingScheme"));
		return false;
	}
	const BroadeningFunction& intraBroadening = KeywordListHelper<BroadeningFunction>::retrieve(keywords_, "IntraBroadening", BroadeningFunction());
	RDFModule::PartialsMethod method = RDFModule::partialsMethod(keywords_.asString("Method"));
	if (method == RDFModule::nPartialsMethods)
	{
		Messenger::error("Partials: Invalid calculation method '%s' found.\n", keywords_.asString("Method"));
		return false;
	}
	const bool internalTest = keywords_.asBool("InternalTest");
	const bool saveData = keywords_.asBool("Save");
	const int smoothing = keywords_.asInt("Smoothing");
	const bool testMode = keywords_.asBool("Test");
	const double testThreshold = keywords_.asDouble("TestThreshold");

	// Print argument/parameter summary
	Messenger::print("RDF: Use of all pairs in intramolecular partials is %s.\n", DUQSys::onOff(allIntra));
	if (averaging <= 1) Messenger::print("RDF: No averaging of partials will be performed.\n");
	else Messenger::print("RDF: Partials will be averaged over %i sets (scheme = %s).\n", averaging, RDFModule::averagingScheme(averagingScheme));
	Messenger::print("RDF: Calculation method is '%s'.\n", RDFModule::partialsMethod(method));
	Messenger::print("RDF: Degree of smoothing to apply to calculated partial g(r) is %i (%s).\n", smoothing, DUQSys::onOff(smoothing > 0));
	Messenger::print("RDF: Save data is %s.\n", DUQSys::onOff(saveData));

	/*
	 * Regardless of whether we are a main processing task (summing some combination of Configuration's partials) or multiple independent Configurations,
	 * we must loop over the specified targetConfigurations_ and calculate the partials for each.
	 */
	RefListIterator<Configuration,bool> configIterator(targetConfigurations_);
	while (Configuration* cfg = configIterator.iterate())
	{
		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Calculate unweighted partials for this Configuration (under generic Module name 'Partials', rather than the uniqueName_)
		bool alreadyUpToDate;
		calculateGR(procPool, cfg, method, allIntra, alreadyUpToDate);
		PartialSet& originalgr = GenericListHelper<PartialSet>::retrieve(cfg->moduleData(), "OriginalGR");

		// Set names of resources (XYData) within the PartialSet
		originalgr.setObjectNames(CharString("%s//%s//%s", cfg->niceName(), "RDF", "OriginalGR"));

		// Perform averaging of unweighted partials if requested, and if we're not already up-to-date
		if ((averaging > 1) && (!alreadyUpToDate)) performGRAveraging(cfg->moduleData(), "OriginalGR", "", averaging, averagingScheme);

		// Save data if requested
		if (saveData && configurationLocal_ && (!MPIRunMaster(procPool, originalgr.save()))) return false;

		// Perform internal test of unweighted g(r)?
		if (internalTest)
		{
			// Copy the already-calculated g(r), then calculate a new set using the Test method
			PartialSet referencePartials = originalgr;
			calculateGR(procPool, cfg, RDFModule::TestMethod, allIntra, alreadyUpToDate);
			if (!testReferencePartials(referencePartials, originalgr, 1.0e-6)) return false;
		}

		// Test original g(r)?
		if (testMode && configurationLocal_)
		{
			Messenger::print("\nTesting calculated g(r) without smoothing / broadening against supplied datasets (if any)...\n");
			if (!testReferencePartials(moduleData, uniqueName(), originalgr, "TestReferenceGR-original", testThreshold)) return false;
		}
	}

	return true;
}

// Run post-processing stage
bool RDFModule::postProcess(DUQ& duq, ProcessPool& procPool)
{
	return false;
}