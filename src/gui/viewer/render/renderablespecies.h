/*
	*** Renderable - Species
	*** src/gui/viewer/render/renderablespecies.h
	Copyright T. Youngs 2019

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

#ifndef DISSOLVE_RENDERABLESPECIES_H
#define DISSOLVE_RENDERABLESPECIES_H

#include "gui/viewer/render/renderable.h"
#include "classes/species.h"

// Forward Declarations
class Axes;

// Renderable for Species
class RenderableSpecies : public Renderable
{
	public:
	// Constructor / Destructor
	RenderableSpecies(const Species* source, const char* objectTag);
	~RenderableSpecies();


	/*
	 * Data
	 */
	private:
	// Source data
	const Species* source_;

	private:
	// Return whether a valid data source is available (attempting to set it if not)
	bool validateDataSource();
	// Return version of data
	const int version() const;


	/*
	 * Transform / Limits
	 */
	public:
	// Calculate min/max y value over specified x range (if possible in the underlying data)
	bool yRangeOverX(double xMin, double xMax, double& yMin, double& yMax);


	/*
	 * Rendering Primitives
	 */
	private:
	// Create line strip primitive
	void constructLineXY(const Array<double>& displayAbscissa, const Array<double>& displayValues, Primitive* primitive, const Axes& axes, const ColourDefinition& colourDefinition, double zCoordinate = 0.0);

	public:
	// Update primitives and send for display
	void updateAndSendPrimitives(View& view, RenderableGroupManager& groupManager, bool forceUpdate, bool pushAndPop, const QOpenGLContext* context);
};

#endif