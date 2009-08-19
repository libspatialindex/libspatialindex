/******************************************************************************
 * $Id: objvisitor.cc 1385 2009-08-13 15:45:16Z hobu $
 *
 * Project:  libsidx - A C API wrapper around libspatialindex
 * Purpose:  C++ objects to implement the wrapper.
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

ObjVisitor::ObjVisitor(): nResults(0)
{
}

ObjVisitor::~ObjVisitor()
{
    std::vector<Item*>::iterator it;
    for (it = m_vector.begin(); it != m_vector.end(); it++) {
        delete *it;
    }

}

void ObjVisitor::visitNode(const SpatialIndex::INode& n)
{
    if (n.isLeaf()) m_leafIO++;
    else m_indexIO++;
}

void ObjVisitor::visitData(const SpatialIndex::IData& d)
{
    SpatialIndex::IShape* pS;
    d.getShape(&pS);
    SpatialIndex::Region *r = new SpatialIndex::Region();
    pS->getMBR(*r);
    // std::cout <<"found shape: " << *r << " dimension: " <<pS->getDimension() << std::endl;


    // data should be an array of characters representing a Region as a string.
    uint8_t* data = 0;
    uint32_t length = 0;
    d.getData(length, &data);

    Item* item = new Item(d.getIdentifier());
    item->SetData(data, length);
    item->SetBounds(r);


    delete pS;
    delete r;
    delete[] data;
    
    nResults += 1;
    
    m_vector.push_back(item);
}

void ObjVisitor::visitData(std::vector<const SpatialIndex::IData*>& v)
{
    // std::cout << v[0]->getIdentifier() << " " << v[1]->getIdentifier() << std::endl;
}

