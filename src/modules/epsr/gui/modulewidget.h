/*
	*** Module Widget
	*** src/modules/epsr/gui/modulewidget.h
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

#ifndef DISSOLVE_EPSRMODULEWIDGET_H
#define DISSOLVE_EPSRMODULEWIDGET_H

#include "modules/epsr/gui/ui_modulewidget.h"
#include "gui/modulewidget.h"

// Forward Declarations
class Dissolve;
class Module;
class PartialSet;
class EPSRModule;
class DataViewer;

// Module Widget
class EPSRModuleWidget : public ModuleWidget
{
	// All Qt declarations derived from QObject must include this macro
	Q_OBJECT

	private:
	// Associated Module
	EPSRModule* module_;
	// DataViewers contained within this widget
	DataViewer* FQGraph_, *FQFitGraph_, *SQGraph_, *GRGraph_, *FRGraph_, *phiRGraph_, *phiMagGraph_, *rFactorGraph_;
	// Reference to Dissolve
	Dissolve& dissolve_;
	// Main form declaration
	Ui::EPSRModuleWidget ui_;

	public:
	// Constructor / Destructor
	EPSRModuleWidget(QWidget* parent, Module* module, Dissolve& dissolve);
	~EPSRModuleWidget();
	// Update controls within widget
	void updateControls();
	// Disable sensitive controls within widget, ready for main code to run
	void disableSensitiveControls();
	// Enable sensitive controls within widget, ready for main code to run
	void enableSensitiveControls();


	/*
	 * ModuleWidget Implementations
	 */
	public:
	// Write widget state through specified LineParser
	bool writeState(LineParser& parser);
	// Read widget state through specified LineParser
	bool readState(LineParser& parser);


	/*
	 * Widgets / Functions
	 */
	private:
	// Set data targets in graphs
	void setGraphDataTargets(EPSRModule* module);


	/*
	 * Debug Tab
	 */
	private:
	// Temporary data currently shown on debug tab
	List<Data1D> debugFunctionData_;

	private:
	// Update data shown on EP functions viewer
	void updateDebugEPFunctionsGraph(int from, int to);

	private slots:
	void on_DebugFromSpin_valueChanged(int value);
	void on_DebugToSpin_valueChanged(int value);
};

#endif
