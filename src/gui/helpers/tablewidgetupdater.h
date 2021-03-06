/*
	*** TableWidgetUpdater
	*** src/gui/helpers/tablewidgetupdater.h
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

#include "templates/variantpointer.h"
#include "templates/list.h"
#include <QTableWidget>

#ifndef DISSOLVE_TABLEWIDGETUPDATER_H
#define DISSOLVE_TABLEWIDGETUPDATER_H

// TableWidgetUpdater - Constructor-only template class to update contents of a QTableWidget, preserving original items as much as possible
template <class T, class I> class TableWidgetUpdater
{
	// Typedefs for passed functions
	typedef void (T::*TableWidgetRowUpdateFunction)(int row, I* item, bool createItems);

	public:
	// Constructor
	TableWidgetUpdater(QTableWidget* table, const List<I>& data, T* functionParent, TableWidgetRowUpdateFunction updateRow)
	{
		QTableWidgetItem* tableItem;

		int rowCount = 0;

		ListIterator<I> dataIterator(data);
		while (I* dataItem = dataIterator.iterate())
		{
			// Our table may or may not be populated, and with different items to those in the list.

			// If there is an item already on this row, check it
			// If it represents the current pointer data, just update it and move on. Otherwise, delete it and check again
			while (rowCount < table->rowCount())
			{
				tableItem = table->item(rowCount, 0);
				I* rowData = (tableItem ? VariantPointer<I>(tableItem->data(Qt::UserRole)) : NULL);
				if (rowData == dataItem)
				{
					// Update the current row and quit the loop
					(functionParent->*updateRow)(rowCount, dataItem, false);

					break;
				}
				else table->removeRow(rowCount);
			}

			// If the current row index is (now) out of range, add a new row to the table
			if (rowCount == table->rowCount())
			{
				// Increase row count
				table->setRowCount(rowCount+1);

				// Create new items
				(functionParent->*updateRow)(rowCount, dataItem, true);
			}

			++rowCount;
		}

		// Set the number of table rows again here in order to catch the case where there were zero data items to iterate over
		table->setRowCount(rowCount);
	}
};

// TableWidgetRefListUpdater - Constructor-only template class to update contents of a QTableWidget from a RefList, preserving original items as much as possible
template <class T, class I, class D> class TableWidgetRefListUpdater
{
	// Typedefs for passed functions
	typedef void (T::*TableWidgetRowUpdateFunction)(int row, I* item, D data, bool createItems);

	public:
	// Constructor
	TableWidgetRefListUpdater(QTableWidget* table, const RefList<I,D>& data, T* functionParent, TableWidgetRowUpdateFunction updateRow)
	{
		QTableWidgetItem* tableItem;

		int rowCount = 0;

		RefListIterator<I,D> itemIterator(data);
		while (I* item = itemIterator.iterate())
		{
			// Our table may or may not be populated, and with different items to those in the list.

			// If there is an item already on this row, check it
			// If it represents the current pointer data, just update it and move on. Otherwise, delete it and check again
			while (rowCount < table->rowCount())
			{
				tableItem = table->item(rowCount, 0);
				I* rowData = (tableItem ? VariantPointer<I>(tableItem->data(Qt::UserRole)) : NULL);
				if (rowData == item)
				{
					// Update the current row and quit the loop
					(functionParent->*updateRow)(rowCount, item, itemIterator.currentData(), false);

					break;
				}
				else table->removeRow(rowCount);
			}

			// If the current row index is (now) out of range, add a new row to the table
			if (rowCount == table->rowCount())
			{
				// Increase row count
				table->setRowCount(rowCount+1);

				// Create new items
				(functionParent->*updateRow)(rowCount, item, itemIterator.currentData(), true);
			}

			++rowCount;
		}

		// Set the number of table rows again here in order to catch the case where there were zero data items to iterate over
		table->setRowCount(rowCount);
	}
};

#endif
