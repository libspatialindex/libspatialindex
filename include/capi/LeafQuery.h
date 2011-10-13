/******************************************************************************
 * $Id: boundsquery.hpp 1361 2009-08-02 17:53:31Z hobu $
 *
 * Project:	 libsidx - A C API wrapper around libspatialindex
 * Purpose:	 C++ object declarations to implement a query of the index's leaves.
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

#pragma once

class LeafQueryResult;

class LeafQuery : public SpatialIndex::IQueryStrategy
{
private:
	std::queue<SpatialIndex::id_type> m_ids;
	std::vector<LeafQueryResult> m_results;
public:

	LeafQuery();
	~LeafQuery() { }
	void getNextEntry(	const SpatialIndex::IEntry& entry, 
						SpatialIndex::id_type& nextEntry, 
						bool& hasNext);
	std::vector<LeafQueryResult> const& GetResults() const {return m_results;}
};

class LeafQueryResult 
{
private:
    std::vector<SpatialIndex::id_type> ids;
    SpatialIndex::Region* bounds;
    uint32_t m_id;
    LeafQueryResult();
public:
    LeafQueryResult(uint32_t id) : bounds(0), m_id(id){}
    ~LeafQueryResult() {if (bounds!=0) delete bounds;}

    /// Copy constructor.
    LeafQueryResult(LeafQueryResult const& other);

    /// Assignment operator.
    LeafQueryResult& operator=(LeafQueryResult const& rhs);
        
    std::vector<SpatialIndex::id_type> const& GetIDs() const;
    void SetIDs(std::vector<SpatialIndex::id_type>& v);
    const SpatialIndex::Region* GetBounds() const;
    void SetBounds(const SpatialIndex::Region*  b);
    uint32_t getIdentifier() const {return m_id;}
    void setIdentifier(uint32_t v) {m_id = v;}
};
