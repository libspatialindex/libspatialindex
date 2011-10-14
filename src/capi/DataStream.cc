/******************************************************************************
 * $Id: datastream.cc 1385 2009-08-13 15:45:16Z hobu $
 *
 * Project:	 libsidx - A C API wrapper around libspatialindex
 * Purpose:	 C++ objects to implement the datastream.
 * Author:	 Howard Butler, hobu.inc@gmail.com
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
 * 51 Franklin Street, Fifth Floor, Boston, MA	02110-1301	USA
 ****************************************************************************/

#include <spatialindex/capi/sidx_impl.h>


DataStream::DataStream(int (*readNext)(SpatialIndex::id_type * id, 
					   double **pMin, 
					   double **pMax, 
					   uint32_t *nDimension, 
					   const uint8_t** pData, 
					   uint32_t *nDataLength) ) 
  : m_pNext(0),
    m_bDoneReading(false)
{
	iterfunct = readNext;
	
	// Read the first one.
	readData();
}

DataStream::~DataStream()
{
	if (m_pNext != 0) delete m_pNext;
}

bool DataStream::readData()
{
	SpatialIndex::id_type id;
	double *pMin=0;
	double *pMax=0;
	uint32_t nDimension=0;
	uint8_t *p_data=0;
	uint32_t nDataLength=0;
	
	if (m_bDoneReading == true) {
		return false;
	}
	
	int ret = iterfunct(&id, &pMin, &pMax, &nDimension, const_cast<const uint8_t**>(&p_data), &nDataLength);

	// The callback should return anything other than 0 
	// when it is done.
	if (ret != 0) 
	{
		m_bDoneReading = true;
		return false;
	}
	
	SpatialIndex::Region r = SpatialIndex::Region(pMin, pMax, nDimension);
	
	// Data gets copied here anyway. We should fix this part of SpatialIndex::RTree::Data's constructor
	m_pNext = new SpatialIndex::RTree::Data(nDataLength, p_data, r, id);

	return true;
}


SpatialIndex::IData* DataStream::getNext()
{
	if (m_pNext == 0) return 0;

	SpatialIndex::RTree::Data* ret = m_pNext;
	m_pNext = 0;
	readData();
	return ret;
}

bool DataStream::hasNext() throw (Tools::NotSupportedException)
{
	return (m_pNext != 0);
}

uint32_t DataStream::size() throw (Tools::NotSupportedException)
{
	throw Tools::NotSupportedException("Operation not supported.");
}

void DataStream::rewind() throw (Tools::NotSupportedException)
{
	throw Tools::NotSupportedException("Operation not supported.");

	if (m_pNext != 0)
	{
	 delete m_pNext;
	 m_pNext = 0;
	}
}
