
/******************************************************************************
* $Id$
*
* Project:	libsidx - A C API wrapper around libspatialindex
* Purpose:	C++ objects to implement the count visitor.
* Author:	Leonard Norrgård, leonard.norrgard@refactor.fi
*
******************************************************************************
* Copyright (c) 2010, Leonard Norrgård
*
* All rights reserved.
* 
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.

* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE.	 See the GNU Lesser General Public License for more
* details.
* 
* You should have received a copy of the GNU Lesser General Public License 
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
****************************************************************************/

#include <spatialindex/capi/sidx_impl.h>

CountVisitor::CountVisitor(): nResults(0)
{
}

CountVisitor::~CountVisitor()
{

}

void CountVisitor::visitNode(const SpatialIndex::INode& n)
{

}

void CountVisitor::visitData(const SpatialIndex::IData& d)
{
   nResults += 1;
}

void CountVisitor::visitData(std::vector<const SpatialIndex::IData*>& v)
{
}
