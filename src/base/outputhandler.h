/*
	*** Output Handler
	*** src/base/outputhandler.h
	Copyright T. Youngs 2012-2018

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_OUTPUTHANDLER_H
#define DUQ_OUTPUTHANDLER_H

// Forward Declarations
/* none */

// Messenger output handler
class OutputHandler
{
	public:
	// Constructor
	OutputHandler();
	// Destructor
	virtual ~OutputHandler();

	public:
	// Output text to device
	virtual void outputText(const char* text) = 0;
};

#endif