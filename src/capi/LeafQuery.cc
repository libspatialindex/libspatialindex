/******************************************************************************
 * $Id: boundsquery.cc 1361 2009-08-02 17:53:31Z hobu $
 *
 * Project:	 libsidx - A C API wrapper around libspatialindex
 * Purpose:	 C++ objects to implement a query of the index's leaves.
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

LeafQuery::LeafQuery() 
{

}

LeafQueryResult get_results(const SpatialIndex::INode* n)
{
	LeafQueryResult result (n->getIdentifier());

	SpatialIndex::IShape* ps;
	n->getShape(&ps);
	SpatialIndex::Region* pr = dynamic_cast<SpatialIndex::Region*>(ps);
	std::vector<SpatialIndex::id_type> ids;
	for (size_t cChild = 0; cChild < n->getChildrenCount(); cChild++)
	{
		ids.push_back(n->getChildIdentifier(cChild));
	}

	result.SetIDs(ids);
	result.SetBounds(pr);
    delete ps;
	
	return result;
}
void LeafQuery::getNextEntry(	const SpatialIndex::IEntry& entry, 
								SpatialIndex::id_type& nextEntry, 
								bool& hasNext) 
{

	const SpatialIndex::INode* n = dynamic_cast<const SpatialIndex::INode*>(&entry);

	// traverse only index nodes at levels 2 and higher.
	if (n != 0 && n->getLevel() > 0)
	{
		for (uint32_t cChild = 0; cChild < n->getChildrenCount(); cChild++)
		{
			m_ids.push(n->getChildIdentifier(cChild));
		}
	}

	if (n->isLeaf()) {
		m_results.push_back(get_results(n));
	}
			
	if (! m_ids.empty())
	{
		nextEntry = m_ids.front(); m_ids.pop();
		hasNext = true;
	}
	else
	{
		hasNext = false;
	}	 
}


std::vector<SpatialIndex::id_type> const& LeafQueryResult::GetIDs() const
{
    return ids;
}

void LeafQueryResult::SetIDs(std::vector<SpatialIndex::id_type>& v) 
{
    ids.resize(v.size());
    std::copy(v.begin(), v.end(), ids.begin());
}
const SpatialIndex::Region*  LeafQueryResult::GetBounds() const
{
    return bounds;
}

void LeafQueryResult::SetBounds(const SpatialIndex::Region*  b) 
{
    bounds = new SpatialIndex::Region(*b);
}

LeafQueryResult::LeafQueryResult(LeafQueryResult const& other)
{
    ids.resize(other.ids.size());
    std::copy(other.ids.begin(), other.ids.end(), ids.begin());
    m_id = other.m_id;
    
    bounds = other.bounds->clone();
}

LeafQueryResult& LeafQueryResult::operator=(LeafQueryResult const& rhs)
{
    if (&rhs != this)
    {
        ids.resize(rhs.ids.size());
        std::copy(rhs.ids.begin(), rhs.ids.end(), ids.begin());
        m_id = rhs.m_id;
        bounds = rhs.bounds->clone();
    }
    return *this;
}

