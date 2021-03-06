/*
	*** Analyser
	*** src/analyse/analyser.cpp
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

#include "analyse/analyser.h"
#include "classes/configuration.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Constructor
Analyser::Analyser() : rootSequence_("EndAnalyser")
{
	rootSequence_.setParent(this);
}

// Destructor
Analyser::~Analyser()
{
}

/*
 * Data
 */

// Clear all data
void Analyser::clear()
{
	contextStack_.clear();
	rootSequence_.clear();
}

// Add (own) specified node to root sequence
void Analyser::addRootSequenceNode(AnalysisNode* node)
{
	rootSequence_.addNode(node);

	node->setParent(this);
}

// Return the context stack
const NodeContextStack& Analyser::contextStack() const
{
	return contextStack_;
}

/*
 * Execute
 */

// Run analysis for specified Configuration, storing / retrieving generated data from supplied list
bool Analyser::execute(ProcessPool& procPool, Configuration* cfg, const char* prefix, GenericList& targetList)
{
	// Check that the Configuration has changed before we do any more analysis on it
	RefListItem<Configuration,int>* ri = configurationPoints_.contains(cfg);
	if (ri)
	{
		// A Configuration we've processed before - check the index
		if (cfg->coordinateIndex() == ri->data)
		{
			Messenger::warn("Refusing to analyse Configuration '%s' since it has not changed.\n", cfg->name());
			return true;
		}
		else ri->data = cfg->coordinateIndex();
	}
	else configurationPoints_.add(cfg, cfg->coordinateIndex());

	// Prepare the nodes
	if (!rootSequence_.prepare(cfg, prefix, targetList)) return Messenger::error("Failed to prepare analysis sequence for execution.\n");

	// Execute the root sequence
	if (!rootSequence_.execute(procPool, cfg, prefix, targetList)) return Messenger::error("Failed to execute analysis sequence.\n");

	// Finalise any nodes that need it
	if (!rootSequence_.finalise(procPool, cfg, prefix, targetList)) return Messenger::error("Failed to finalise analysis sequence after execution.\n");

	return true;
}

/*
 * Read / Write
 */

// Read structure from specified LineParser
bool Analyser::read(LineParser& parser, const CoreData& coreData)
{
	contextStack_.clear();

	return rootSequence_.read(parser, coreData, contextStack_);
}

// Write structure to specified LineParser
bool Analyser::write(LineParser& parser, const char* prefix)
{
	return rootSequence_.write(parser, prefix);
}
