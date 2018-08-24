/*
	*** Analysis Node - Collect1D
	*** src/analyse/nodes/collect1d.h
	Copyright T. Youngs 2012-2018

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

#ifndef DISSOLVE_ANALYSISCOLLECT_H
#define DISSOLVE_ANALYSISCOLLECT_H

#include "analyse/nodes/node.h"
#include "base/charstring.h"
#include "templates/array.h"
#include "templates/array2d.h"
#include "templates/reflist.h"
#include "templates/vector3.h"

// Forward Declarations
class AnalysisCalculateNode;
class Histogram1D;
class LineParser;
class NodeContextStack;

// Analysis Node - Collect
class AnalysisCollect1DNode : public AnalysisNode
{
	public:
	// Constructor
	AnalysisCollect1DNode();
	// Destructor
	~AnalysisCollect1DNode();


	/*
	 * Node Keywords
	 */
	public:
	// Node Keywords
	enum Collect1DNodeKeyword { EndCollect1DKeyword, LabelXKeyword, LabelYKeyword, QuantityXKeyword, RangeXKeyword, nCollect1DNodeKeywords };
	// Convert string to control keyword
	static Collect1DNodeKeyword collect1DNodeKeyword(const char* s);
	// Convert control keyword to string
	static const char* collect1DNodeKeyword(Collect1DNodeKeyword nk);


	/*
	 * Data
	 */
	private:
	// Observable to bin
	AnalysisCalculateNode* observable_;
	// Histogram in which to bin data
	Histogram1D* histogram_;
	// Axis labels
	CharString axisLabels_[3];
	// Range minimum
	double minimum_;
	// Range maximum
	double maximum_;
	// Bin width
	double binWidth_;


	/*
	 * Execute
	 */
	public:
	// Prepare any necessary data, ready for execution
	bool prepare(Configuration* cfg, const char* dataPrefix, GenericList& targetList);
	// Execute node, targetting the supplied Configuration
	AnalysisNode::NodeExecutionResult execute(ProcessPool& procPool, Configuration* cfg, const char* dataPrefix, GenericList& targetList);
	// Finalise any necessary data after execution
	bool finalise(Configuration* cfg, const char* dataPrefix, GenericList& targetList);


	/*
	 * Read / Write
	 */
	public:
	// Read structure from specified LineParser
	bool read(LineParser& parser, NodeContextStack& contextStack);
	// Write structure to specified LineParser
	bool write(LineParser& parser, const char* prefix);
};

#endif