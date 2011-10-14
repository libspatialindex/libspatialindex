/******************************************************************************
 * $Id: boundsquery.hpp 1361 2009-08-02 17:53:31Z hobu $
 *
 * Project:  libsidx - A C API wrapper around libspatialindex
 * Purpose:  C++ object declarations to implement the bounds query.
 * Author:   Howard Butler, hobu.inc@gmail.com
 *
 ******************************************************************************
 * Copyright (c) 2009, Howard Butler
 *
 * All rights reserved.
 * 
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.

 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 * 
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************/

#pragma once

class BoundsQuery : public SpatialIndex::IQueryStrategy
{
private:
    SpatialIndex::Region* m_bounds;
    
public:

    BoundsQuery();
    ~BoundsQuery() { if (m_bounds !=0) delete m_bounds;}
    void getNextEntry(  const SpatialIndex::IEntry& entry, 
                        SpatialIndex::id_type& nextEntry, 
                        bool& hasNext);
    
    SpatialIndex::Region* GetBounds() const {return m_bounds; }
};

