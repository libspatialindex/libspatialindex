/******************************************************************************
 * $Id: idvisitor.cc 1361 2009-08-02 17:53:31Z hobu $
 *
 * Project:	 libsidx - A C API wrapper around libspatialindex
 * Purpose:	 C++ objects to implement the id visitor.
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

IdVisitor::IdVisitor(): nResults(0)
{
}

IdVisitor::~IdVisitor()
{

}

void IdVisitor::visitNode(const SpatialIndex::INode& n)
{

}

void IdVisitor::visitData(const SpatialIndex::IData& d)
{
	nResults += 1;
	
	m_vector.push_back(d.getIdentifier());
}

void IdVisitor::visitData(std::vector<const SpatialIndex::IData*>& v)
{
}
