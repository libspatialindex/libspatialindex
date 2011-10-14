/******************************************************************************
 * $Id: objvisitor.cc 1385 2009-08-13 15:45:16Z hobu $
 *
 * Project:	 libsidx - A C API wrapper around libspatialindex
 * Purpose:	 C++ objects to implement the wrapper.
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

ObjVisitor::ObjVisitor(): nResults(0)
{
}

ObjVisitor::~ObjVisitor()
{
	std::vector<SpatialIndex::IData*>::iterator it;
	for (it = m_vector.begin(); it != m_vector.end(); it++) {
		delete *it;
	}

}

void ObjVisitor::visitNode(const SpatialIndex::INode& n)
{
}

void ObjVisitor::visitData(const SpatialIndex::IData& d)
{

	SpatialIndex::IData* item = dynamic_cast<SpatialIndex::IData*>(const_cast<SpatialIndex::IData&>(d).clone()) ; 
	
	nResults += 1;
	
	m_vector.push_back(item);
}

void ObjVisitor::visitData(std::vector<const SpatialIndex::IData*>& v)
{
}

