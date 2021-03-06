/*
	*** MPI Linked List Item Class
	*** src/templates/mpilistitem.h
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

#ifndef DISSOLVE_MPILISTITEM_H
#define DISSOLVE_MPILISTITEM_H

#include <stdlib.h>     // To get NULL

// Forward Declarations
class CoreData;
class ProcessPool;

/*
 * MPIListItem Class
 * Basic class providing linked list pointers. Any class which is required to be contained in a linked list, and be 
 * broadcastable with the general broadcast routines must subclass MPIListItem.
 */
template <class T> class MPIListItem
{
	public:
	// Constructor
	MPIListItem<T>()
	{
		prev = NULL;
		next = NULL;
	}
	// Destructor (virtual)
	virtual ~MPIListItem<T>()
	{
	}
	// List pointers
	T* prev, *next;

	public:
	// Broadcast data from Master to all Slaves
	virtual bool broadcast(ProcessPool& procPool, const int root, const CoreData& coreData) = 0;
};

#endif
