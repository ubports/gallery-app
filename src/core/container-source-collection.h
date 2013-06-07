/*
 * Copyright (C) 2011 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 * Jim Nelson <jim@yorba.org>
 */

#ifndef GALLERY_CONTAINER_SOURCE_COLLECTION_H_
#define GALLERY_CONTAINER_SOURCE_COLLECTION_H_

#include "data-object.h"
#include "source-collection.h"

#include <QString>

/**
  * ContainerSourceCollection is a SourceCollection that holds ContainerSources.
  * That is, it represents all active ContainerSources of a particular finalized
  * type.  It (will have) a more complicated set of signals than other
  * DataCollection classes because of the layering issues involved;
  * ContainerSources hold DataSources, and ContainerSourceCollections hold
  * ContainerSources that hold DataSources.
  */
class ContainerSourceCollection : public SourceCollection
{
    Q_OBJECT

public:
    ContainerSourceCollection(const QString& name, DataObjectComparator comparator);
};

#endif  // GALLERY_CONTAINER_SOURCE_COLLECTION_H_
