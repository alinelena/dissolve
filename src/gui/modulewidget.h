/*
	*** Module Widget
	*** src/gui/modulewidget.h
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

#ifndef DISSOLVE_MODULEWIDGET_H
#define DISSOLVE_MODULEWIDGET_H

#include <QWidget>

// Forward Declarations
class LineParser;

// ModuleWidget, base class for any Module-specific widget displayed in a ModuleControlWidget
class ModuleWidget : public QWidget
{
	protected:
	// Whether widget is currently refreshing
	bool refreshing_;

	public:
	// Constructor / Destructor
	ModuleWidget(QWidget* parent);
	virtual ~ModuleWidget();
	// Update controls within widget
	virtual void updateControls() = 0;
	// Disable sensitive controls within widget, ready for main code to run
	virtual void disableSensitiveControls() = 0;
	// Enable sensitive controls within widget, ready for main code to run
	virtual void enableSensitiveControls() = 0;


	/*
	 * State I/O
	 */
	public:
	// Write widget state through specified LineParser
	virtual bool writeState(LineParser& parser) = 0;
	// Read widget state through specified LineParser
	virtual bool readState(LineParser& parser) = 0;
};

#endif
