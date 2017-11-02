/*
	*** Keywords
	*** src/module/keywords.h
	Copyright T. Youngs 2012-2017

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

#ifndef DUQ_KEYWORDS_H
#define DUQ_KEYWORDS_H

/*
 * This file should be included if the use of Module Keywords is required.
 * It simply includes all other derived classes and specialisations.
 */

// PODs
#include "module/keywordtypes/bool.h"
#include "module/keywordtypes/double.h"
#include "module/keywordtypes/int.h"

// Complex
#include "module/keywordtypes/complex.h"

// Classes
#include "module/keywordtypes/charstring.h"
#include "module/keywordtypes/broadeningfunction.h"

#endif