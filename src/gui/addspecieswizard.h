/*
	*** Add Species Wizard
	*** src/gui/addspecieswizard.h
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

#ifndef DISSOLVE_ADDSPECIESWIZARD_H
#define DISSOLVE_ADDSPECIESWIZARD_H

#include "gui/ui_addspecieswizard.h"
#include "gui/wizardwidget.hui"
#include "main/dissolve.h"

// Forward Declarations
class MasterIntra;

// Add Species Wizard
class AddSpeciesWizard : public WizardWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	public:
	// Constructor / Destructor
	AddSpeciesWizard(QWidget* parent);
	~AddSpeciesWizard();


	/*
	 * Data
	 */
	private:
	// Main instance of Dissolve that we're using as a reference
	const Dissolve* dissolveReference_;
	// Temporary core data for creating / importing new Species
	CoreData temporaryCoreData_;
	// Temporary Dissolve reference for creating / importing new Species
	Dissolve temporaryDissolve_;
	// Target Species (in temporaryCoreData_) for import
	Species* importTarget_;

	public:
	// Set Dissolve reference
	void setMainDissolveReference(const Dissolve* dissolveReference);
	// Copy imported Species over to the specified Dissolve object, returning the new pointer to it
	Species* importSpecies(Dissolve& dissolve);


	/*
	 * Controls
	 */
	private:
	// Main form declaration
	Ui::AddSpeciesWizard ui_;
	// Lock counter for the widget refreshing
	int lockedForRefresh_;
	// Pages Enum
	enum WidgetPage
	{
		StartPage,			/* Starting page, offering choices of how to proceed */
		CreateAtomicPage,		/* Create atomic Species, optionally with new AtomType */
		ImportSpeciesSelectFilePage,	/* Import Species - select file */
		ImportSpeciesSelectSpeciesPage,	/* Import Species - choose species */
		AtomTypesPage,			/* AtomTypes page - check / re-map AtomTypes */
		MasterTermsPage,		/* MasterTerms page - check / re-map MasterTerms */
		SpeciesNamePage,		/* Final page, setting name for Species */
		nPages
	};

	protected:
	// Go to specified page index in the controls widget
	bool displayControlPage(int index);
	// Return whether progression to the next page from the current page is allowed
	bool progressionAllowed(int index) const;
	// Perform any necessary actions before moving to the next page
	bool prepareForNextPage(int currentIndex);
	// Determine next page for the current page, based on current data
	int determineNextPage(int currentIndex);
	// Perform any necessary actions before moving to the previous page
	bool prepareForPreviousPage(int currentIndex);

	public:
	// Reset, ready for adding a new Species
	void reset();


	/*
	 * Start Page
	 */
	private slots:
	void on_StartCreateEmptyButton_clicked(bool checked);
	void on_StartCreateAtomicButton_clicked(bool checked);
	void on_StartAddPredefinedButton_clicked(bool checked);
	void on_StartImportCoordinatesButton_clicked(bool checked);
	void on_StartImportSpeciesButton_clicked(bool checked);


	/*
	 * Create Atomic Page
	 */
	private slots:
	void createAtomicElementChanged();


	/*
	 * Import Species Pages
	 */
	private slots:
	// Input File Page
	void on_InputFileEdit_textChanged(const QString text);
	void on_InputFileSelectButton_clicked(bool checked);
	void on_SpeciesList_currentRowChanged(int currentRow);


	/*
	 * AtomTypes Page
	 */
	private:
	// Row update function for AtomTypesList
	void updateAtomTypesListRow(int row, AtomType* atomType, bool createItem);
	// Update page with AtomTypes in our temporary Dissolve reference
	void updateAtomTypesPage();

	private slots:
	void on_AtomTypesList_itemSelectionChanged();
	void atomTypesListEdited(QWidget* lineEdit);
	void on_AtomTypesPrefixButton_clicked(bool checked);
	void on_AtomTypesSuffixButton_clicked(bool checked);


	/*
	 * MasterTerms Page
	 */
	private:
	// Parental tree widgets for master terms
	QTreeWidgetItem* masterBondItemParent_, *masterAngleItemParent_, *masterTorsionItemParent_;
	
	private:
	// Row update function for MasterTermsTree
	void updateMasterTermsTreeChild(QTreeWidgetItem* parent, int childIndex, MasterIntra* masterIntra, bool createItem);
	void updateMasterTermsPage();

	private slots:
	void on_MasterTermsTree_itemSelectionChanged();
	void masterTermsTreeEdited(QWidget* lineEdit);
	void on_MasterTermsPrefixButton_clicked(bool checked);
	void on_MasterTermsSuffixButton_clicked(bool checked);


	/*
	 * Species Name Page (final page)
	 */
	private slots:
	void on_SpeciesNameEdit_textChanged(const QString text);

	public:
	// Return name of new Species to be
	const char* speciesName() const;
};

#endif
