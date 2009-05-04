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

#include <cstring>
#include <stdio.h>

#ifndef _MSC_VER
#include <unistd.h>
#endif

#include "../spatialindex/SpatialIndexImpl.h"
#include "RTree.h"
#include "Leaf.h"
#include "Index.h"
#include "BulkLoader.h"

using namespace SpatialIndex::RTree;

BulkLoadSource::BulkLoadSource(
	Tools::SmartPointer<IObjectStream> spStream, size_t howMany
) : m_spDataSource(spStream), m_cHowMany(howMany)
{
}

BulkLoadSource::BulkLoadSource(IObjectStream* pStream, size_t howMany)
 : m_spDataSource(pStream), m_cHowMany(howMany)
{
}

BulkLoadSource::BulkLoadSource(IObjectStream* pStream)
 : m_spDataSource(pStream),
   m_cHowMany(std::numeric_limits<size_t>::max())
{
}

BulkLoadSource::~BulkLoadSource()
{
}

Tools::IObject* BulkLoadSource::getNext()
{
	if (m_cHowMany == 0 || ! m_spDataSource->hasNext()) return 0;
	m_cHowMany--;
	return m_spDataSource->getNext();
}

bool BulkLoadSource::hasNext() throw ()
{
	return (m_cHowMany != 0 && m_spDataSource->hasNext());
}

size_t BulkLoadSource::size() throw (Tools::NotSupportedException)
{
	throw Tools::NotSupportedException("SpatialIndex::RTree::BulkLoadSource::size: this should never be called.");
}

void BulkLoadSource::rewind() throw (Tools::NotSupportedException)
{
	throw Tools::NotSupportedException("SpatialIndex::RTree::BulkLoadSource::rewind: this should never be called.");
}

BulkLoadComparator::BulkLoadComparator(size_t d) : m_compareDimension(d)
{
}

BulkLoadComparator::~BulkLoadComparator()
{
}

int BulkLoadComparator::compare(Tools::IObject* o1, Tools::IObject* o2)
{
	IData* d1 = dynamic_cast<IData*>(o1);
	IData* d2 = dynamic_cast<IData*>(o2);

	IShape* s1;	d1->getShape(&s1);
	IShape* s2;	d2->getShape(&s2);
	Region r1; s1->getMBR(r1);
	Region r2; s2->getMBR(r2);

	int ret = 0;
	if (
		r1.m_pHigh[m_compareDimension] + r1.m_pLow[m_compareDimension] <
		r2.m_pHigh[m_compareDimension] + r2.m_pLow[m_compareDimension]) ret = -1;
	else if (
		r1.m_pHigh[m_compareDimension] + r1.m_pLow[m_compareDimension] >
		r2.m_pHigh[m_compareDimension] + r2.m_pLow[m_compareDimension]) ret = 1;

	delete s1;
	delete s2;

	return ret;
}

BulkLoader::TmpFile::TmpFile() : m_pNext(0)
{
}

BulkLoader::TmpFile::~TmpFile()
{
	if (m_pNext != 0) delete m_pNext;
}

void BulkLoader::TmpFile::storeRecord(Region& r, id_type id)
{
	size_t len = sizeof(id_type) + sizeof(size_t) + 2 * r.m_dimension * sizeof(double);
	byte* data = new byte[len];
	byte* ptr = data;

	memcpy(ptr, &id, sizeof(id_type));
	ptr += sizeof(id_type);
	memcpy(ptr, &(r.m_dimension), sizeof(size_t));
	ptr += sizeof(size_t);
	memcpy(ptr, r.m_pLow, r.m_dimension * sizeof(double));
	ptr += r.m_dimension * sizeof(double);
	memcpy(ptr, r.m_pHigh, r.m_dimension * sizeof(double));

	m_tmpFile.storeNextObject(len, data);
	delete[] data;
}

void BulkLoader::TmpFile::loadRecord(Region& r, id_type& id)
{
	size_t len;
	byte* data;
	m_tmpFile.loadNextObject(&data, len);

	byte* ptr = data;
	memcpy(&id, ptr, sizeof(id_type));
	ptr += sizeof(id_type);

	size_t dim;
	memcpy(&dim, ptr, sizeof(size_t));
	ptr += sizeof(size_t);

	if (dim != r.m_dimension)
	{
		delete[] r.m_pLow;
		delete[] r.m_pHigh;
		r.m_dimension = dim;
		r.m_pLow = new double[dim];
		r.m_pHigh = new double[dim];
	}

	memcpy(r.m_pLow, ptr, dim * sizeof(double));
	ptr += dim * sizeof(double);
	memcpy(r.m_pHigh, ptr, dim * sizeof(double));

	delete[] data;
}

IData* BulkLoader::TmpFile::getNext()
{
	if (m_pNext == 0) return 0;

	IData* ret = m_pNext;

	try
	{
		Region r;
		id_type id;
		loadRecord(r, id);
		m_pNext = new Data(0, 0, r, id);
	}
	catch (Tools::EndOfStreamException& e)
	{
		m_pNext = 0;
	}
	catch (...)
	{
		m_pNext = 0;
		throw;
	}

	return ret;
}

bool BulkLoader::TmpFile::hasNext() throw ()
{
	return (m_pNext != 0);
}

size_t BulkLoader::TmpFile::size() throw (Tools::NotSupportedException)
{
	throw Tools::NotSupportedException("Not supported yet.");
}

void BulkLoader::TmpFile::rewind()
{
	Region r;
	id_type id;

	if (m_pNext != 0)
	{
		delete m_pNext;
		m_pNext = 0;
	}

	m_tmpFile.rewindForReading();

	try
	{
		loadRecord(r, id);
		m_pNext = new Data(0, 0, r, id);
	}
	catch (Tools::EndOfStreamException& e)
	{
	}
}

void BulkLoader::bulkLoadUsingSTR(
	SpatialIndex::RTree::RTree* pTree,
	IDataStream& stream,
	size_t bindex,
	size_t bleaf,
	size_t bufferSize)
{
	NodePtr n = pTree->readNode(pTree->m_rootID);
	pTree->deleteNode(n.get());

	// create the leaf level first.
	TmpFile* tmpFile = new TmpFile();
	size_t cNodes = 0;
	size_t cTotalData = 0;

#ifdef DEBUG
	std::cerr << "RTree::BulkLoader: Building level 0" << std::endl;
#endif

	createLevel(pTree, stream, pTree->m_dimension, pTree->m_dimension, bleaf, 0, bufferSize, *tmpFile, cNodes, cTotalData);

	pTree->m_stats.m_data = cTotalData;

	// create index levels afterwards.
	size_t level = 1;
	tmpFile->rewind();
	BulkLoadSource* bs = new BulkLoadSource(tmpFile);

	while (cNodes > 1)
	{
		cNodes = 0;
		TmpFile* pTF = new TmpFile();

		#ifndef NDEBUG
		std::cerr << "RTree::BulkLoader: Building level " << level << std::endl;
		#endif
		pTree->m_stats.m_nodesInLevel.push_back(0);

		createLevel(pTree, *bs, pTree->m_dimension, pTree->m_dimension, bindex, level, bufferSize, *pTF, cNodes, cTotalData);
		delete bs;

		level++;
		pTF->rewind();
		bs = new BulkLoadSource(pTF);
	}

	pTree->m_stats.m_treeHeight = level;

	delete bs;

	pTree->storeHeader();
}

void BulkLoader::createLevel(
	SpatialIndex::RTree::RTree* pTree,
	Tools::IObjectStream& stream,
	size_t dimension,
	size_t k,
	size_t b,
	size_t level,
	size_t bufferSize,
	BulkLoader::TmpFile& tmpFile,
	size_t& numberOfNodes,
	size_t& totalData)
{
	BulkLoadComparator bc(dimension - k);
	Tools::SmartPointer<Tools::IObjectStream> es(Tools::externalSort(stream, bc, bufferSize));
	size_t r = es->size();
	totalData = r;

	if (k == dimension - 1)
	{
		// store new pages in storage manager and page information in temporary file.

		std::vector<Tools::SmartPointer<IData> > entries;

		while (es->hasNext())
		{
			entries.push_back(Tools::SmartPointer<IData>(static_cast<IData*>(es->getNext())));

			if (entries.size() == b)
			{
				Node* n = createNode(pTree, entries, level);
				pTree->writeNode(n);
				if (r <= b) pTree->m_rootID = n->m_identifier;
				numberOfNodes++;
				tmpFile.storeRecord(n->m_nodeMBR, n->m_identifier);
				entries.clear();
				delete n;
			}
		}

		if (! entries.empty())
		{
			Node* n = createNode(pTree, entries, level);
			pTree->writeNode(n);
			if (r <= b) pTree->m_rootID = n->m_identifier;
			numberOfNodes++;
			tmpFile.storeRecord(n->m_nodeMBR, n->m_identifier);
			entries.clear();
			delete n;
		}
	}
	else
	{
		size_t P = static_cast<size_t>(std::ceil(static_cast<double>(r) / static_cast<double>(b)));
		size_t D = static_cast<size_t>(std::ceil(std::pow(static_cast<double>(P), static_cast<double>(k - 1) / static_cast<double>(k))));

		while (es->hasNext()) // this will happen S = ceil[P^(1 / k)] times
		{
			BulkLoadSource bs(es, D * b);
			size_t cTotalData;
			createLevel(pTree, bs, dimension, k - 1, b, level, bufferSize, tmpFile, numberOfNodes, cTotalData);
		}
	}
}

Node* BulkLoader::createNode(SpatialIndex::RTree::RTree* pTree, std::vector<Tools::SmartPointer<IData> >& e, size_t level)
{
	Node* n;

	if (level == 0) n = new Leaf(pTree, -1);
	else n = new Index(pTree, -1, level);

	for (size_t cChild = 0; cChild < e.size(); cChild++)
	{
		size_t len;
		byte* data;
		e[cChild]->getData(len, &data);
		IShape* s; e[cChild]->getShape(&s);
		RegionPtr mbr = pTree->m_regionPool.acquire();
		s->getMBR(*mbr);
		delete s;
		id_type id = e[cChild]->getIdentifier();
		n->insertEntry(len, data, *mbr, id);
	}

	return n;
}

