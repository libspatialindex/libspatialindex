/******************************************************************************
 * $Id: datastream.hpp 1385 2009-08-13 15:45:16Z hobu $
 *
 * Project:  libsidx - A C API wrapper around libspatialindex
 * Purpose:  C++ object declarations to implement the datastream.
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

class DataStream : public SpatialIndex::IDataStream
{
public:
    DataStream(int (*readNext)(SpatialIndex::id_type* id, double **pMin, double **pMax, uint32_t *nDimension, const uint8_t **pData, uint32_t *nDataLength));
    ~DataStream();

    SpatialIndex::IData* getNext();
    bool hasNext() throw (Tools::NotSupportedException);

    uint32_t size() throw (Tools::NotSupportedException);
    void rewind() throw (Tools::NotSupportedException);

protected:
    SpatialIndex::RTree::Data* m_pNext;
    SpatialIndex::id_type m_id;

private:
    int (*iterfunct)(SpatialIndex::id_type *id, double **pMin, double **pMax, uint32_t *nDimension, const uint8_t **pData, uint32_t *nDataLength);
    
    bool readData();
    bool m_bDoneReading;

};

