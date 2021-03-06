/*
	*** Keyword Widget - Double
	*** src/gui/keywordwidgets/double_funcs.cpp
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

#include "gui/keywordwidgets/double.hui"
#include "gui/helpers/mousewheeladjustmentguard.h"
#include "templates/genericlisthelper.h"

// Constructor
DoubleKeywordWidget::DoubleKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix) : ExponentialSpin(parent), KeywordWidgetBase(coreData, moduleData, prefix)
{
	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<DoubleModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword into DoubleModuleKeyword.\n");
	else
	{
		// Set minimum and maximum values
		setRange(keyword_->hasValidationMin(), keyword_->validationMin(), keyword_->hasValidationMax(), keyword_->validationMax());

		// Set current value
		setValue(keyword_->asDouble());
	}

	// Connect the valueChanged signal to our own slot
	connect(this, SIGNAL(valueChanged(double)), this, SLOT(myValueChanged(double)));

	// Set event filtering so that we do not blindly accept mouse wheel events (problematic since we will exist in a QScrollArea)
	installEventFilter(new MouseWheelWidgetAdjustmentGuard(this));
}

/*
 * Slots
 */

// Spin box value changed
void DoubleKeywordWidget::myValueChanged(double newValue)
{
	if (refreshing_) return;

	keyword_->setData(newValue);

	emit(keywordValueChanged());
}

/*
 * Update
 */

// Update value displayed in widget, using specified source if necessary
void DoubleKeywordWidget::updateValue()
{
	refreshing_ = true;

	// Check to see if the associated Keyword may have been stored/updated in the specified moduleData
	if ((keyword_->genericItemFlags()&GenericItem::InRestartFileFlag) && moduleData_.contains(keyword_->keyword(), modulePrefix_))
	{
		// Retrieve the item from the list
		setValue(GenericListHelper<double>::value(moduleData_, keyword_->keyword(), modulePrefix_));
	}
	else setValue(keyword_->asDouble());

	refreshing_ = false;
}
