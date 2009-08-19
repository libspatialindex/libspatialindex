/******************************************************************************
 * $Id: item.cc 1374 2009-08-05 17:07:01Z hobu $
 *
 * Project:  libsidx - A C API wrapper around libspatialindex
 * Purpose:  C++ objects to implement the index items.
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

#include "sidx_impl.h"

Item::Item( uint64_t id): 
            m_data(0), 
            m_bounds(0), 
            m_length(0)
{
    m_id = id;
}

Item::~Item()
{
    if (m_data != 0)
        delete[] m_data;
    if (m_bounds != 0)
        delete m_bounds;
}

Item::Item(Item const& other) : m_id(other.m_id), 
                                m_length(other.m_length)
{
    if (m_length > 0) {
        m_data = new uint8_t[m_length];
        memcpy(m_data, other.m_data, m_length);
    } else
        m_data = 0;

    if (other.m_bounds != 0) 
        m_bounds = other.m_bounds->clone();
    else 
        m_bounds = 0;
    
}

Item& Item::operator=(Item const& rhs)
{

    if (&rhs != this )
    {
        m_id = rhs.m_id;
        m_length = rhs.m_length;

        if (m_length > 0) {
            m_data = new uint8_t[m_length];
            memcpy(m_data, rhs.m_data, m_length);
        } else
            m_data = 0;
            
        if (rhs.m_bounds != 0)
            m_bounds = rhs.m_bounds->clone();
        else 
            m_bounds = 0;
    }
    return *this;
}
    
void Item::SetData(const uint8_t* data, uint64_t length) 
{
    m_length = length;
    
    // Don't do anything if we have no length
    if (m_length < 1) return;

    if (m_data != 0) {
        delete[] m_data;
    }
    
    m_data = new uint8_t[length];
    
    if (length > 0 && m_data != 0) {
        memcpy(m_data, data, length);
    }
}

void Item::GetData(uint8_t** data, uint64_t *length) 
{
    *length = m_length;
    *data = m_data;
}

const SpatialIndex::Region* Item::GetBounds() const 
{
    if (m_bounds == 0) 
        return 0;
    else
        return m_bounds;
}

void Item::SetBounds( const SpatialIndex::Region* b)
{
    m_bounds = new SpatialIndex::Region(*b);
}
