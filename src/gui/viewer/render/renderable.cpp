/*
	*** Renderable Data
	*** src/gui/view/render/renderable.cpp
	Copyright T. Youngs 2013-2019

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

#include "gui/viewer/render/renderable.h"
#include "gui/viewer/render/renderablegroupmanager.h"
#include "gui/viewer/render/view.h"
#include "math/interpolator.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"
#include <limits>

// Renderable Type
const char* RenderableTypeKeywords[] = { "Data1D", "Species" };

// Convert text string to RenderableType
Renderable::RenderableType Renderable::renderableType(const char* s)
{
	for (int n=0; n<nRenderableTypes; ++n) if (DissolveSys::sameString(s, RenderableTypeKeywords[n])) return (Renderable::RenderableType) n;
	return nRenderableTypes;
}

// Convert RenderableType to text string
const char* Renderable::renderableType(RenderableType rt)
{
	return RenderableTypeKeywords[rt];
}

// Constructor
Renderable::Renderable(Renderable::RenderableType type, const char* objectTag)
{
	// Identity
	type_ = type;
	name_ = "New Renderable";

	// Data tag
	objectTag_ = objectTag;

	// Group
	group_ = NULL;

	// Transform
	transformDataVersion_ = -1;
	transformMin_.zero();
	transformMax_.set(10.0, 10.0, 10.0);
	transformMinPositive_.set(0.1, 0.1, 0.1);
	transformMaxPositive_.set(10.0, 10.0, 10.0);
	transforms_[0].setEnabled(false);
	transforms_[1].setEnabled(false);
	transforms_[2].setEnabled(false);
	transforms_[0].setEquation("x");
	transforms_[1].setEquation("y");
	transforms_[2].setEquation("z");

	// Rendering Versions
	lastDataVersion_ = -1;
	lastAxesVersion_ = -1;
	lastStyleVersion_ = -1;

	// Display
	visible_ = true;
	displayStyle_ = -1;
	displaySurfaceShininess_ = 128.0;
	styleVersion_ = 0;
}

// Destructor
Renderable::~Renderable()
{
}

/*
 * Identity
 */

// Set name of Renderable
void Renderable::setName(const char* name)
{
	name_ = name;
}

// Return name of Renderable
const char* Renderable::name()
{
	return name_.get();
}

// Return type of Renderable
Renderable::RenderableType Renderable::type() const
{
	return type_;
}

// Set legend text to display
void Renderable::setLegendText(const char* legendText)
{
	legendText_ = legendText;
}

// Return legend text to display
const char* Renderable::legendText() const
{
	return legendText_.get();
}

/*
 * Data
 */

// Return identifying tag for source data object
const char* Renderable::objectTag() const
{
	return objectTag_.get();
}

/*
 * Transforms
 */

// Return transformed data minima, calculating if necessary
Vec3<double> Renderable::transformMin()
{
	// Make sure transformed data is up to date
	transformData();

	return transformMin_;
}

// Return transformed data maxima, calculating if necessary
Vec3<double> Renderable::transformMax()
{
	// Make sure transformed data is up to date
	transformData();

	return transformMax_;
}

// Return transformed positive data minima, calculating if necessary
Vec3<double> Renderable::transformMinPositive()
{
	// Make sure transformed data is up to date
	transformData();

	return transformMinPositive_;
}

// Return transformed positive data maxima, calculating if necessary
Vec3<double> Renderable::transformMaxPositive()
{
	// Make sure transformed data is up to date
	transformData();

	return transformMaxPositive_;
}

// Set transform equation for data
void Renderable::setTransformEquation(int axis, const char* transformEquation)
{
	transforms_[axis].setEquation(transformEquation);

	// Make sure transformed data is up to date
	if (transforms_[axis].enabled())
	{
		transformDataVersion_ = -1;
		transformData();
	}
}

// Return transform equation for data
const char* Renderable::transformEquation(int axis) const
{
	return transforms_[axis].text();
}

// Return whether specified transform equation is valid
bool Renderable::transformEquationValid(int axis) const
{
	return transforms_[axis].valid();
}

// Set whether specified transform is enabled
void Renderable::setTransformEnabled(int axis, bool enabled)
{
	transforms_[axis].setEnabled(enabled);

	// Make sure transformed data is up to date
	transformDataVersion_ = -1;
	transformData();
}

// Return whether specified transform is enabled
bool Renderable::transformEnabled(int axis) const
{
	return transforms_[axis].enabled();
}

/*
 * Group
 */

// Set group that this Renderable is associated to
void Renderable::setGroup(RenderableGroup* group)
{
	group_ = group;
}

// Return group that this Renderable is associated to
RenderableGroup* Renderable::group() const
{
	return group_;
}

/*
 * Style
 */

// Set whether data is visible
void Renderable::setVisible(bool visible)
{
	visible_ = visible;
}

// Return whether data is visible
bool Renderable::isVisible() const
{
	return visible_;
}

// Set display style index
void Renderable::setDisplayStyle(int id)
{
	displayStyle_ = id;

	++styleVersion_;
}

// Return display style index
int Renderable::displayStyle() const
{
	return displayStyle_;
}

// Set basic colour
void Renderable::setColour(int r, int g, int b, int a)
{
	colour_.setSingleColour(QColor(r, g, b, a));
}

// Set basic colour
void Renderable::setColour(ColourDefinition::StockColour stockColour)
{
	colour_.setSingleColour(ColourDefinition::stockColour(stockColour));
}

// Return local colour definition for display
ColourDefinition& Renderable::colour()
{
	return colour_;
}

// Return local colour definition for display (const)
const ColourDefinition& Renderable::constColour() const
{
	return colour_;
}

// Return line style
LineStyle& Renderable::lineStyle()
{
	return lineStyle_;
}

// Return style version
int Renderable::styleVersion() const
{
	return styleVersion_;
}

/*
 * Rendering Primitives
 */

// Create new Primitive, whose instances will be managed by the Renderable
Primitive* Renderable::createPrimitive(GLenum type, bool colourData)
{
	return primitives_.add(type, colourData);
}

// Remove specified Primitive
void Renderable::removePrimitive(Primitive* primitive)
{
	primitives_.remove(primitive);
}

// Update primitives and send to display
void Renderable::updateAndSendPrimitives(const View& view, const RenderableGroupManager& groupManager, bool forceUpdate, bool pushAndPop, const QOpenGLContext* context, double pixelScaling)
{
	// If this Renderable is not visible, return now
	if (!visible_) return;

	// Grab axes for the View
	const Axes& axes = view.constAxes();

	// Grab copy of the relevant colour definition for this Renderable
	const ColourDefinition& colourDefinition = groupManager.colourDefinition(this);

	// Check whether the primitive for this Renderable needs updating
	bool upToDate = true;
	if (forceUpdate) upToDate = false;
	else if (lastAxesVersion_ != axes.version()) upToDate = false;
	else if (!DissolveSys::sameString(lastColourDefinitionFingerprint_, CharString("%p@%i", group_, colourDefinition.version()), true)) upToDate = false;
	else if (lastDataVersion_ != dataVersion()) upToDate = false;
	else if (lastStyleVersion_ != styleVersion()) upToDate = false;

	// If the primitive is out of date, recreate it's data.
	if (!upToDate)
	{
		// Recreate Primitives for the underlying data
		recreatePrimitives(view, colourDefinition);

		// Pop old Primitive instance (if they exist)
		if (primitives_.nInstances() != 0) primitives_.popInstance(context);
	}

	// If there are no current instances, or we are forcing a push/pop of an instance, push an instance here
	if ((primitives_.nInstances() == 0) || pushAndPop) primitives_.pushInstance(context);

	// Send to GL
	sendToGL(pixelScaling);

	// Pop current instances if required
	if (pushAndPop) primitives_.popInstance(context);

	// Store version points for the up-to-date primitive
	lastAxesVersion_ = axes.version();
	lastColourDefinitionFingerprint_.sprintf("%p@%i", group_, colourDefinition.version());
	lastDataVersion_ = dataVersion();
	lastStyleVersion_ = styleVersion();
}
