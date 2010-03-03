/******************************************************************************
 * $Id: objvisitor.hpp 1385 2009-08-13 15:45:16Z hobu $
 *
 * Project:  libsidx - A C API wrapper around libspatialindex
 * Purpose:  C++ object declarations to implement the object visitor.
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

class ObjVisitor : public SpatialIndex::IVisitor
{
private:
    std::vector<SpatialIndex::IData*> m_vector;
    uint64_t nResults;

public:

    ObjVisitor();
    ~ObjVisitor();

    uint32_t GetResultCount() const { return nResults; }
    std::vector<SpatialIndex::IData*>& GetResults()  { return m_vector; }
    
    void visitNode(const SpatialIndex::INode& n);
    void visitData(const SpatialIndex::IData& d);
    void visitData(std::vector<const SpatialIndex::IData*>& v);
};

