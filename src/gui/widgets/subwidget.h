/*
	*** SubWindow Widget
	*** src/gui/widgets/subwidget.h
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

#ifndef DISSOLVE_SUBWIDGET_H
#define DISSOLVE_SUBWIDGET_H

#include "base/charstring.h"
#include "templates/listitem.h"
#include <QWidget>

// Forward Declarations
class DissolveWindow;
class LineParser;
class SubWindow;

// Subwidget (root class for any widget to be displayed in a QMdiSubWindow)
class SubWidget : public QWidget, public ListItem<SubWidget>
{
	protected:
	// Pointer to DissolveWindow
	DissolveWindow* dissolveWindow_;
	// Whether widget is currently refreshing
	bool refreshing_;

	public:
	// Constructor / Destructor
	SubWidget(DissolveWindow* dissolveWindow, const char* title);
	virtual ~SubWidget();
	// Update controls within widget
	virtual void updateControls() = 0;
	// Disable sensitive controls within widget, ready for main code to run
	virtual void disableSensitiveControls() = 0;
	// Enable sensitive controls within widget, ready for main code to run
	virtual void enableSensitiveControls() = 0;


	/*
	 * SubWindow Parent
	 */
	protected:
	// SubWindow in which this widget is displayed (if any)
	SubWindow* subWindow_;

	public:
	// Set SubWindow in which this widget is displayed
	void setSubWindow(SubWindow* subWindow);
	// Return SubWindow in which this widget is displayed
	SubWindow* subWindow();


	/*
	 * Identification
	 */
	private:
	// Title of widget
	CharString title_;

	public:
	// Set title of widget
	void setTitle(const char* title);
	// Return title of widget
	const char* title();
	// Return string specifying widget type
	virtual const char* widgetType() = 0;


	/*
	 * State
	 */
	public:
	// Write widget state through specified LineParser
	virtual bool writeState(LineParser& parser) = 0;
	// Read widget state through specified LineParser
	virtual bool readState(LineParser& parser) = 0;


	/*
	 * Reimplementations
	 */
	protected:
	virtual void closeEvent(QCloseEvent* event) = 0;
};

#endif
