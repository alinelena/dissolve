/*
	*** Keyword Widget - IsotopologueList
	*** src/gui/keywordwidgets/isotopologuelist_funcs.cpp
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

#include "gui/keywordwidgets/isotopologuelist.h"
#include "gui/keywordwidgets/dropdown.h"
#include "gui/delegates/combolist.hui"
#include "gui/delegates/exponentialspin.hui"
#include "gui/delegates/isotopologuecombo.hui"
#include "gui/delegates/usedspeciescombo.hui"
#include "gui/helpers/tablewidgetupdater.h"
#include "module/module.h"
#include "classes/configuration.h"
#include "classes/coredata.h"
#include "classes/species.h"
#include "templates/genericlisthelper.h"
#include "templates/variantpointer.h"

// Constructor
IsotopologueListKeywordWidget::IsotopologueListKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix) : KeywordDropDown(this), KeywordWidgetBase(coreData, moduleData, prefix)
{
	// Create and set up the UI for our widget in the drop-down's widget container
	ui_.setupUi(dropWidget());

	// Set delegates for table
	ui_.IsotopologueTable->setItemDelegateForColumn(0, new ComboListDelegate(this, new ComboNameListItems<Configuration>(coreData.constConfigurations())));
	ui_.IsotopologueTable->setItemDelegateForColumn(1, new UsedSpeciesComboDelegate(this));
	ui_.IsotopologueTable->setItemDelegateForColumn(2, new IsotopologueComboDelegate(this));
	ui_.IsotopologueTable->setItemDelegateForColumn(3, new ExponentialSpinDelegate(this));

	// Connect signals / slots
	connect(ui_.AutoButton, SIGNAL(clicked(bool)), this, SLOT(autoButton_clicked(bool)));
	connect(ui_.AddButton, SIGNAL(clicked(bool)), this, SLOT(addButton_clicked(bool)));
	connect(ui_.RemoveButton, SIGNAL(clicked(bool)), this, SLOT(removeButton_clicked(bool)));
	connect(ui_.IsotopologueTable, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(isotopologueTable_itemChanged(QTableWidgetItem*)));

	// Cast the pointer up into the parent class type
	keyword_ = dynamic_cast<IsotopologueListModuleKeyword*>(keyword);
	if (!keyword_) Messenger::error("Couldn't cast base module keyword into IsotopologueListModuleKeyword.\n");
	else
	{
		// Set current information
		updateWidgetValues(coreData_);
	}

	// Summary text on KeywordDropDown button
	setSummaryText("Edit...");
}

/*
 * Widgets
 */

void IsotopologueListKeywordWidget::autoButton_clicked(bool checked)
{
	// First, assemble a list of all Species over all Configurations targeted by the Module that are currently missing from our definition
	RefList<Species,Configuration*> missingSpecies;
	List<IsotopologueReference>& topeReferences = keyword_->data();
	RefListIterator<Configuration,bool> configurationIterator(keyword_->moduleParent()->targetConfigurations());
	while (Configuration* cfg = configurationIterator.iterate())
	{
		ListIterator<SpeciesInfo> speciesIterator(cfg->usedSpecies());
		while (SpeciesInfo* spInfo = speciesIterator.iterate())
		{
			if (spInfo->population() < 1) continue;

			// Loop over our isotopologue references to see if the Species / Configuration is already represented
			ListIterator<IsotopologueReference> topeIterator(topeReferences);
			IsotopologueReference* topeRef = NULL;
			while (topeRef = topeIterator.iterate()) if (topeRef->matches(cfg, spInfo->species())) break;
			
			if (!topeRef) missingSpecies.add(spInfo->species(), cfg);
		}
	}

	// Add missing entries...
	if (missingSpecies.nItems() > 0)
	{
		RefListIterator<Species,Configuration*> missingIterator(missingSpecies);
		while (Species* sp = missingIterator.iterate())
		{
			IsotopologueReference* topeRef = topeReferences.add();
			topeRef->set(missingIterator.currentData(), sp, sp->isotopologues().first());
		}

		// Manually flag that the keyword data has changed
		keyword_->dataHasBeenSet();

		updateWidgetValues(coreData_);

		emit(keywordValueChanged());
	}
}

void IsotopologueListKeywordWidget::addButton_clicked(bool checked)
{
	IsotopologueReference* isoRef = keyword_->data().add();

	// Try to set some sensible default values
	isoRef->setConfiguration(keyword_->moduleParent()->targetConfigurations().firstItem());
	if (isoRef->configuration())
	{
		SpeciesInfo* spInfo = isoRef->configuration()->usedSpecies().first();
		if (spInfo)
		{
			isoRef->setSpecies(spInfo->species());
			isoRef->setIsotopologue(spInfo->species()->isotopologues().first());
		}
	}

	// Manually flag that the keyword data has changed
	keyword_->dataHasBeenSet();

	updateWidgetValues(coreData_);

	emit(keywordValueChanged());
}

void IsotopologueListKeywordWidget::removeButton_clicked(bool checked)
{
	if (!ui_.IsotopologueTable->currentItem()) return;

	// Get target IsotopologueReference from the first column
	IsotopologueReference* isoRef = VariantPointer<IsotopologueReference>(ui_.IsotopologueTable->currentItem()->data(Qt::UserRole));
	if (!isoRef) return;

	keyword_->data().remove(isoRef);

	// Manually flag that the keyword data has changed
	keyword_->dataHasBeenSet();

	updateWidgetValues(coreData_);

	emit(keywordValueChanged());
}

void IsotopologueListKeywordWidget::isotopologueTable_itemChanged(QTableWidgetItem* w)
{
	if (refreshing_) return;

	// Get target IsotopologueReference from the first column
	IsotopologueReference* isoRef = w ? VariantPointer<IsotopologueReference>(ui_.IsotopologueTable->item(w->row(), 0)->data(Qt::UserRole)) : NULL;
	if (!isoRef) return;

	// Column of passed item tells us the type of data we need to change
	Configuration* cfg = isoRef->configuration();
	Species* sp = isoRef->species();
	Isotopologue* iso;
	switch (w->column())
	{
		// Configuration
		case (0):
			cfg = coreData_.findConfiguration(qPrintable(w->text()));
			if (cfg)
			{
				// Set new Configuration, and update Species and Isotopologue
				isoRef->setConfiguration(cfg);

				// Does the previously-defined Species exist in this Configuration?
				if (!cfg->hasUsedSpecies(isoRef->species()))
				{
					// Not present, so set to first in list
					isoRef->setSpecies(cfg->usedSpecies().first() ? cfg->usedSpecies().first()->species() : NULL);
					isoRef->setIsotopologue(isoRef->species() ? isoRef->species()->isotopologues().first() : NULL);
				}

				// Set the variant pointer in the Species column to reflect the updated Configuration
				QTableWidgetItem* speciesItem = ui_.IsotopologueTable->item(w->row(), 1);
				if (speciesItem) speciesItem->setData(Qt::UserRole, VariantPointer<Configuration>(cfg));

				// Manually flag that the keyword data has changed
				keyword_->dataHasBeenSet();

				emit(keywordValueChanged());
			}
			else Messenger::error("Couldn't find Configuration '%s'.\n", qPrintable(w->text()));
			break;
		// Species
		case (1):
			if (!cfg)
			{
				Messenger::error("No valid Configuration, so can't set Species.\n");
				break;
			}
			sp = coreData_.findSpecies(qPrintable(w->text()));
			if (sp)
			{
				// Make sure this Species is referenced in the Configuration
				if (cfg->hasUsedSpecies(sp))
				{
					isoRef->setSpecies(sp);
					isoRef->setIsotopologue(sp->isotopologues().first());

					// Set the variant pointer in the Isotopologue column to reflect the updated Species
					QTableWidgetItem* topeItem = ui_.IsotopologueTable->item(w->row(), 2);
					if (topeItem) topeItem->setData(Qt::UserRole, VariantPointer<Species>(sp));

					// Manually flag that the keyword data has changed
					keyword_->dataHasBeenSet();

					emit(keywordValueChanged());
				}
				else Messenger::error("Configuration '%s' doesn't contain Species '%s'.\n", cfg->name(), sp->name());
			}
			else Messenger::error("Couldn't find Species '%s'.\n", qPrintable(w->text()));
			break;
		// Isotopologue
		case (2):
			if (!cfg)
			{
				Messenger::error("No valid Configuration, so can't set Isotopologue.\n");
				break;
			}
			if (!sp)
			{
				Messenger::error("No valid Species, so can't set Isotopologue.\n");
				break;
			}
			iso = sp->findIsotopologue(qPrintable(w->text()));
			isoRef->setIsotopologue(iso);

			// Manually flag that the keyword data has changed
			keyword_->dataHasBeenSet();

			emit(keywordValueChanged());
			break;
		// Weight
		case (3):
			isoRef->setWeight(w->text().toDouble());

			// Manually flag that the keyword data has changed
			keyword_->dataHasBeenSet();

			emit(keywordValueChanged());
			break;
		default:
			Messenger::error("Don't know what to do with data from column %i of Isotopologue table.\n", w->column());
			break;
	}
}

/*
 * Update
 */

// Table row update function
void IsotopologueListKeywordWidget::updateTableRow(int row, IsotopologueReference* isoRef, bool createItems)
{
	QTableWidgetItem* item;

	// Configuration
	if (createItems)
	{
		item = new QTableWidgetItem;
		item->setData(Qt::UserRole, VariantPointer<IsotopologueReference>(isoRef));
		ui_.IsotopologueTable->setItem(row, 0, item);
	}
	else item = ui_.IsotopologueTable->item(row, 0);
	item->setText(isoRef->configuration() ? isoRef->configuration()->name() : "N/A");

	// Species
	if (createItems)
	{
		item = new QTableWidgetItem;
		ui_.IsotopologueTable->setItem(row, 1, item);
	}
	else item = ui_.IsotopologueTable->item(row, 1);
	item->setText(isoRef->species() ? isoRef->species()->name() : "N/A");
	item->setData(Qt::UserRole, VariantPointer<Configuration>(isoRef->configuration()));

	// Isotopologue
	if (createItems)
	{
		item = new QTableWidgetItem;
		ui_.IsotopologueTable->setItem(row, 2, item);
	}
	else item = ui_.IsotopologueTable->item(row, 2);
	item->setText(isoRef->isotopologue() ? isoRef->isotopologue()->name() : "N/A");
	item->setData(Qt::UserRole, VariantPointer<Species>(isoRef->species()));

	// Weight
	if (createItems)
	{
		item = new QTableWidgetItem;
		ui_.IsotopologueTable->setItem(row, 3, item);
	}
	else item = ui_.IsotopologueTable->item(row, 3);
	item->setText(QString::number(isoRef->weight()));
}

// Update value displayed in widget, using specified source if necessary
void IsotopologueListKeywordWidget::updateValue()
{
	updateWidgetValues(coreData_);
}

// Update widget values data based on keyword data
void IsotopologueListKeywordWidget::updateWidgetValues(const CoreData& coreData)
{
	refreshing_ = true;

	// Update the table
	TableWidgetUpdater<IsotopologueListKeywordWidget,IsotopologueReference> tableUpdater(ui_.IsotopologueTable, keyword_->data(), this, &IsotopologueListKeywordWidget::updateTableRow);
	ui_.IsotopologueTable->resizeColumnsToContents();

	ui_.RemoveButton->setEnabled(ui_.IsotopologueTable->currentRow());

	refreshing_ = false;
}

// Update keyword data based on widget values
void IsotopologueListKeywordWidget::updateKeywordData()
{
	// Not used
}
