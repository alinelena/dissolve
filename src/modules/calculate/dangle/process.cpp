/*
	*** Calculate Distance-Angle Module - Processing
	*** src/modules/calculate/dangle/process.cpp
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

#include "modules/calculate/dangle/dangle.h"
#include "main/dissolve.h"
#include "analyse/nodes/calculate.h"
#include "analyse/nodes/collect1d.h"
#include "analyse/nodes/process1d.h"
#include "analyse/nodes/select.h"
#include "base/sysfunc.h"

// Run set-up stage
bool CalculateDAngleModule::setUp(Dissolve& dissolve, ProcessPool& procPool)
{
	// Reconstruct the Analyser
	analyser_.clear();

	// Get relevant Module options
	const double angleBin = keywords_.asDouble("AngleBin");
	const double angleMax = keywords_.asDouble("AngleMax");
	const double angleMin = keywords_.asDouble("AngleMin");
	const double distanceBin = keywords_.asDouble("DistanceBin");
	const double distanceMax = keywords_.asDouble("DistanceMax");
	const double distanceMin = keywords_.asDouble("DistanceMin");
	const bool excludeSameMolecule = keywords_.asBool("ExcludeSameMolecule");

	CharString dataName = rdfName();
	SpeciesSite* originSite = KeywordListHelper<SpeciesSite*>::retrieve(keywords_, "OriginSite", NULL);
	if (!originSite) return Messenger::error("Origin site is not defined.\n");
	SpeciesSite* otherSite = KeywordListHelper<SpeciesSite*>::retrieve(keywords_, "OtherSite", NULL);
	if (!otherSite) return Messenger::error("Other (surrounding) site is not defined.\n");


	/*
	 * Assemble the code below (@var indicates local variable 'var')
	 *
	 * Select  'A'
	 *   Site  @siteA
	 *   ForEach
	 *     Select  'B'
	 *       Site  @siteB
	 *       SameMoleculeAsSite  'A'
	 *       ForEach
	 *         Select  'C'
	 *           Site  @siteC
	 *           ExcludeSameMolecule  (if @excludeSameMolecule then 'A')
	 *           ForEach
	 *             Calculate  'rAB'
	 *               Distance  'A'  'B'
	 *             EndCalculate
	 *             Calculate  'aABC'
	 *               Angle  'A'  'B'  'C'
	 *             EndCalculate
	 *             Collect2D  @dataName
	 *               QuantityX  'rAB'
	 *               QuantityY  'aABC'
	 *               RangeX  @distanceMin  @distanceMax  @distanceBin
	 *               RangeY  @angleMin  @angleMax  @angleBin
	 *             EndCollect2D
	 *           EndForEach  'C'
	 *         EndSelect  'C'
	 *       EndForEach  'B'
	 *     EndSelect  'B'
	 *   EndForEach  'A'
	 * EndSelect  'A'
	 * Process2D  @dataName
	 *   NormaliseToOne  On
	 *   LabelValue  'g(r)'
	 *   LabelX  'r, Angstroms'
	 *   LabelY  'theta, Degrees'
	 *   LabelValue  'Probability'
	 * EndProcess2D
	 */

	// Select: Site 'A' (@originSite)
	AnalysisSelectNode* originSelect = new AnalysisSelectNode(originSite);
	originSelect->setName("A");
	analyser_.addRootSequenceNode(originSelect);

	// -- Select: Site 'B' (@otherSite)
	AnalysisSelectNode* otherSelect = new AnalysisSelectNode(otherSite);
	otherSelect->setName("B");
	otherSelect->addSameSiteExclusion(originSelect);
	if (excludeSameMolecule) otherSelect->addSameMoleculeExclusion(originSelect);
	originSelect->addToForEachBranch(otherSelect);

	// -- -- Calculate: 'rAB'
	AnalysisCalculateNode* calcDistance = new AnalysisCalculateNode(AnalysisCalculateNode::DistanceObservable, originSelect, otherSelect);
	otherSelect->addToForEachBranch(calcDistance);

	// -- -- Collect1D: @dataName
// 	AnalysisCollect1DNode* collect1D = new AnalysisCollect1DNode(calcDistance, rMin, rMax, binWidth);
// 	collect1D->setName(dataName);
// 	otherSelect->addToForEachBranch(collect1D);
// 
// 	// Process1D: @dataName
// 	AnalysisProcess1DNode* process1D = new AnalysisProcess1DNode(collect1D);
// 	process1D->setName(dataName);
// 	process1D->addSitePopulationNormaliser(originSelect);
// 	process1D->addNumberDensityNormaliser(otherSelect);
// 	process1D->setNormaliseBySphericalShellVolume(true);
// 	process1D->setSaveData(true);
// 	process1D->setValueLabel("g(r)");
// 	process1D->setXAxisLabel("r, \\symbol{Angstrom}");
// 	analyser_.addRootSequenceNode(process1D);

	return true;
}

// Run main processing
bool CalculateDAngleModule::process(Dissolve& dissolve, ProcessPool& procPool)
{
	// Check for zero Configuration targets
	if (targetConfigurations_.nItems() == 0)
	{
		Messenger::warn("No Configuration targets for Module.\n");
		return true;
	}

	// Loop over target Configurations
	for (RefListItem<Configuration,bool>* ri = targetConfigurations_.first(); ri != NULL; ri = ri->next)
	{
		// Grab Configuration pointer
		Configuration* cfg = ri->item;

		// Set up process pool - must do this to ensure we are using all available processes
		procPool.assignProcessesToGroups(cfg->processPool());

		// Execute the analysis
		if (!analyser_.execute(procPool, cfg, uniqueName(), dissolve.processingModuleData())) return Messenger::error("CalculateDAngle experience problems with its analysis.\n");
	}

	return true;
}

