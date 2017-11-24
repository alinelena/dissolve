/*
	*** Signal
	*** src/gui/uchroma/classes/signal.cpp
	Copyright T. Youngs 2013-2017

	This file is part of uChroma.

	uChroma is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	uChroma is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with uChroma.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gui/uchroma/classes/signal.h"
#include "gui/uchroma/uchromabase.h"

/*
 * Link to UChromaBase
 */

// Static members
UChromaBase* UChromaSignal::uChromaBase_ = NULL;

// Set link to UChromaBase
void UChromaSignal::setUChromaBase(UChromaBase* uChromaBase)
{
	uChromaBase_ = uChromaBase;
}

/*
 * Signalling
 */

// Send Collection-related signal
UChromaSignal::SignalAction UChromaSignal::send(UChromaSignal::CollectionSignal signal, Collection* collection)
{
	if (!uChromaBase_) return UChromaSignal::IgnoreSignal;

	// Send to ViewPanes in current ViewLayout....
	int nAccepted = 0;
	bool unique = false;
	for (ViewPane* pane = uChromaBase_->viewLayout().panes(); pane != NULL; pane = pane->next)
	{
		UChromaSignal::SignalAction result = pane->processCollectionSignal(signal, collection);
		if (result == UChromaSignal::IgnoreSignal) continue;
		++nAccepted;

		// If the object wants sole acceptance of this signal, break now
		if (result == UChromaSignal::UniqueAcceptSignal)
		{
			unique = true;
			break;
		}
	}
	if (nAccepted == 0) return UChromaSignal::IgnoreSignal;
	else return (unique ? UChromaSignal::UniqueAcceptSignal : UChromaSignal::AcceptSignal);
}