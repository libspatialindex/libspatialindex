// Spatial Index Library
//
// Copyright (C) 2002 Navel Ltd.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Email:
//    mhadji@gmail.com

#include <spatialindex/SpatialIndex.h>

#include "Statistics.h"

using namespace SpatialIndex::MVRTree;

Statistics::Statistics()
{
	reset();
}

Statistics::Statistics(const Statistics& s)
{
	m_u64Reads  = s.m_u64Reads;
	m_u64Writes = s.m_u64Writes;
	m_u64Splits = s.m_u64Splits;
	m_u64Hits   = s.m_u64Hits;
	m_u64Misses = s.m_u64Misses;
	m_u32Nodes  = s.m_u32Nodes;
	m_u32DeadIndexNodes = s.m_u32DeadIndexNodes;
	m_u32DeadLeafNodes = s.m_u32DeadLeafNodes;
	m_u64Adjustments = s.m_u64Adjustments;
	m_u64QueryResults = s.m_u64QueryResults;
	m_u64Data = s.m_u64Data;
	m_u64TotalData = s.m_u64TotalData;
	m_treeHeight = s.m_treeHeight;
	m_nodesInLevel = s.m_nodesInLevel;
}

Statistics::~Statistics()
{
}

Statistics& Statistics::operator=(const Statistics& s)
{
	if (this != &s)
	{
		m_u64Reads  = s.m_u64Reads;
		m_u64Writes = s.m_u64Writes;
		m_u64Splits = s.m_u64Splits;
		m_u64Hits   = s.m_u64Hits;
		m_u64Misses = s.m_u64Misses;
		m_u32Nodes  = s.m_u32Nodes;
		m_u32DeadIndexNodes = s.m_u32DeadIndexNodes;
		m_u32DeadLeafNodes = s.m_u32DeadLeafNodes;
		m_u64Adjustments = s.m_u64Adjustments;
		m_u64QueryResults = s.m_u64QueryResults;
		m_u64Data = s.m_u64Data;
		m_u64TotalData = s.m_u64TotalData;
		m_treeHeight = s.m_treeHeight;
		m_nodesInLevel = s.m_nodesInLevel;
	}

	return *this;
}

uint64_t Statistics::getReads() const
{
	return m_u64Reads;
}

uint64_t Statistics::getWrites() const
{
	return m_u64Writes;
}

uint32_t Statistics::getNumberOfNodes() const
{
	return m_u32Nodes;
}

uint64_t Statistics::getNumberOfData() const
{
	return m_u64Data;
}

uint64_t Statistics::getSplits() const
{
	return m_u64Splits;
}

uint64_t Statistics::getHits() const
{
	return m_u64Hits;
}

uint64_t Statistics::getMisses() const
{
	return m_u64Misses;
}

uint64_t Statistics::getAdjustments() const
{
	return m_u64Adjustments;
}

uint64_t Statistics::getQueryResults() const
{
	return m_u64QueryResults;
}

uint32_t Statistics::getTreeHeight() const
{
	uint32_t ret = 0;

	for (size_t cIndex = 0; cIndex < m_treeHeight.size(); ++cIndex)
	{
		ret = std::max(ret, m_treeHeight[cIndex]);
	}

	return ret;
}

uint32_t Statistics::getNumberOfNodesInLevel(uint32_t l) const
{
	try
	{
		return m_nodesInLevel.at(l);
	}
	catch (...)
	{
		throw Tools::IndexOutOfBoundsException(l);
	}
}

void Statistics::reset()
{
	m_u64Reads = 0;
	m_u64Writes = 0;
	m_u64Splits = 0;
	m_u64Hits = 0;
	m_u64Misses = 0;
	m_u32Nodes = 0;
	m_u32DeadIndexNodes = 0;
	m_u32DeadLeafNodes = 0;
	m_u64Adjustments = 0;
	m_u64QueryResults = 0;
	m_u64Data = 0;
	m_u64TotalData = 0;
	m_treeHeight.clear();
	m_nodesInLevel.clear();
}

std::ostream& SpatialIndex::MVRTree::operator<<(std::ostream& os, const Statistics& s)
{
	os	<< "Reads: " << s.m_u64Reads << std::endl
		<< "Writes: " << s.m_u64Writes << std::endl
		<< "Hits: " << s.m_u64Hits << std::endl
		<< "Misses: " << s.m_u64Misses << std::endl
		<< "Number of live data: " << s.m_u64Data << std::endl
		<< "Total number of data: " << s.m_u64TotalData << std::endl
		<< "Number of nodes: " << s.m_u32Nodes << std::endl
		<< "Numer of dead index nodes: " << s.m_u32DeadIndexNodes << std::endl
		<< "Numer of dead leaf nodes: " << s.m_u32DeadLeafNodes << std::endl;

	for (size_t cTree = 0; cTree < s.m_treeHeight.size(); ++cTree)
	{
		os << "Tree " << cTree << ", Height " << s.m_treeHeight[cTree] << std::endl;
	}

	for (size_t cLevel = 0; cLevel < s.m_nodesInLevel.size(); ++cLevel)
	{
		os << "Level " << cLevel << " pages: " << s.m_nodesInLevel[cLevel] << std::endl;
	}

	os	<< "Splits: " << s.m_u64Splits << std::endl
		<< "Adjustments: " << s.m_u64Adjustments << std::endl
		<< "Query results: " << s.m_u64QueryResults << std::endl;

	return os;
}
