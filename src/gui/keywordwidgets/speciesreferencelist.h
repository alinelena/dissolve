/*
	*** Keyword Widget - Species Reference List
	*** src/gui/keywordwidgets/speciesreferencelist.h
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

#ifndef DISSOLVE_SPECIESREFERENCELISTKEYWORDWIDGET_H
#define DISSOLVE_SPECIESREFERENCELISTKEYWORDWIDGET_H

#include "gui/keywordwidgets/ui_speciesreferencelist.h"
#include "gui/keywordwidgets/dropdown.h"
#include "module/keywordtypes/speciesreferencelist.h"
#include "gui/keywordwidgets/base.h"
#include <QWidget>

// Forward Declarations
class Species;

class SpeciesReferenceListKeywordWidget: public KeywordDropDown, public KeywordWidgetBase
{
	// All Qt declarations must include this macro
	Q_OBJECT

	public:
	// Constructor
	SpeciesReferenceListKeywordWidget(QWidget* parent, ModuleKeywordBase* keyword, const CoreData& coreData, GenericList& moduleData, const char* prefix);
        // Main form declaration
        Ui::SpeciesReferenceListWidget ui;


	/*
	 * Keyword
	 */
	private:
	// Associated keyword
	SpeciesReferenceListModuleKeyword* keyword_;


	/*
	 * Signals / Slots
	 */
	private:
	// Selection list update function
	void updateSelectionRow(int row, Species* atomType, bool createItem);

	private slots:
	// List item changed
	void itemChanged(QListWidgetItem* item);

	signals:
	// Keyword value changed
	void keywordValueChanged();


	/*
	 * Update
	 */
	public:
	// Update value displayed in widget, using specified source if necessary
	void updateValue();
	// Update widget values data based on keyword data
	void updateWidgetValues(const CoreData& coreData);
	// Update keyword data based on widget values
	void updateKeywordData();
	// Update summary text
	void updateSummaryText();
};

#endif
