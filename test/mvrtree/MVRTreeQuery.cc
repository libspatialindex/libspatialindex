/******************************************************************************
 * Project:  libspatialindex - A C++ library for spatial indexing
 * Author:   Marios Hadjieleftheriou, mhadji@gmail.com
 ******************************************************************************
 * Copyright (c) 2003, Marios Hadjieleftheriou
 *
 * All rights reserved.
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
******************************************************************************/

// NOTE: Please read README.txt before browsing this code.

#include <cstring>

// include library header file.
#include <spatialindex/SpatialIndex.h>

using namespace SpatialIndex;
using namespace std;

#define DELETE 0
#define INSERT 1
#define QUERY 2

// example of a Visitor pattern.
// findes the index and leaf IO for answering the query and prints
// the resulting data IDs to stdout.
class MyVisitor : public IVisitor
{
public:
	size_t m_indexIO{0};
	size_t m_leafIO{0};

public:
    MyVisitor() = default;

	void visitNode(const INode& n) override
	{
		if (n.isLeaf()) m_leafIO++;
		else m_indexIO++;
	}

	void visitData(const IData& d) override
	{
		//IShape* pS;
		//d.getShape(&pS);
			// do something.
		//delete pS;

		// data should be an array of characters representing a Region as a string.
		//uint8_t* pData = 0;
		//size_t cLen = 0;
		//d.getData(cLen, &pData);
			// do something.
			//string s = reinterpret_cast<char*>(pData);
			//cout << s << endl;
		//delete[] pData;

		cout << d.getIdentifier() << endl;
			// the ID of this data entry is an answer to the query. I will just print it to stdout.
	}

	void visitData(std::vector<const IData*>& /*v*/) override {}
};

// example of a Strategy pattern.
// traverses the tree by level.
class MyQueryStrategy : public SpatialIndex::IQueryStrategy
{
private:
	queue<id_type> ids;

public:
	void getNextEntry(const IEntry& entry, id_type& nextEntry, bool& hasNext) override
	{
		IShape* ps;
		entry.getShape(&ps);
		Region* pr = dynamic_cast<Region*>(ps);

		cout << pr->m_pLow[0] << " " << pr->m_pLow[1] << endl;
		cout << pr->m_pHigh[0] << " " << pr->m_pLow[1] << endl;
		cout << pr->m_pHigh[0] << " " << pr->m_pHigh[1] << endl;
		cout << pr->m_pLow[0] << " " << pr->m_pHigh[1] << endl;
		cout << pr->m_pLow[0] << " " << pr->m_pLow[1] << endl << endl << endl;
			// print node MBRs gnuplot style!

		delete ps;

		const INode* n = dynamic_cast<const INode*>(&entry);

		// traverse only index nodes at levels 2 and higher.
		if (n != nullptr && n->getLevel() > 1)
		{
			for (size_t cChild = 0; cChild < n->getChildrenCount(); cChild++)
			{
				ids.push(n->getChildIdentifier((uint32_t)cChild));
			}
		}

		if (! ids.empty())
		{
			nextEntry = ids.front(); ids.pop();
			hasNext = true;
		}
		else
		{
			hasNext = false;
		}
	}
};

// example of a Strategy pattern.
// find the total indexed space managed by the index (the MBR of the root).
class MyQueryStrategy2 : public IQueryStrategy
{
public:
	Region m_indexedSpace;

public:
	void getNextEntry(const IEntry& entry, id_type& /*nextEntry*/, bool& hasNext) override
	{
		// the first time we are called, entry points to the root.

		// stop after the root.
		hasNext = false;

		IShape* ps;
		entry.getShape(&ps);
		ps->getMBR(m_indexedSpace);
		delete ps;
	}
};

int main(int argc, char** argv)
{
	try
	{
		if (argc != 4)
		{
			cerr << "Usage: " << argv[0] << " query_file tree_file query_type [intersection | 10NN]." << endl;
			return -1;
		}

		uint32_t queryType = 0;

		if (strcmp(argv[3], "intersection") == 0) queryType = 0;
		else if (strcmp(argv[3], "10NN") == 0) queryType = 1;
		else
		{
			cerr << "Unknown query type." << endl;
			return -1;
		}

		ifstream fin(argv[1]);
		if (! fin)
		{
			cerr << "Cannot open query file " << argv[1] << "." << endl;
			return -1;
		}

		string baseName = argv[2];
		IStorageManager* diskfile = StorageManager::loadDiskStorageManager(baseName);
			// this will try to locate and open an already existing storage manager.

		StorageManager::IBuffer* file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, 10, false);
			// applies a main memory random buffer on top of the persistent storage manager
			// (LRU buffer, etc can be created the same way).

		// If we need to open an existing tree stored in the storage manager, we only
		// have to specify the index identifier as follows
		ISpatialIndex* tree = MVRTree::loadMVRTree(*file, 1);

		size_t count = 0;
		size_t indexIO = 0;
		size_t leafIO = 0;
		id_type id;
		uint32_t op;
		double x1, x2, y1, y2, t;
		double plow[2], phigh[2];

		while (fin)
		{
			fin >> t >> op >> id >> x1 >> y1 >> x2 >> y2;
			if (! fin.good()) continue; // skip newlines, etc.

			if (op == QUERY)
			{
				size_t qt1, qt2;
				fin >> qt1 >> qt2;
				if (! fin.good()) continue;

				plow[0] = x1; plow[1] = y1;
				phigh[0] = x2; phigh[1] = y2;

				MyVisitor vis;

				if (queryType == 0)
				{
					TimeRegion r = TimeRegion(plow, phigh, (double)qt1, (double)qt2, 2);
					tree->intersectsWithQuery(r, vis);
						// this will find all data that intersect with the query range.
				}
				else
				{
					//Point p = Point(plow, 2);
					//tree->nearestNeighborQuery(10, p, vis);
						// this will find the 10 nearest neighbors.
				}

				indexIO += vis.m_indexIO;
				leafIO += vis.m_leafIO;
					// example of the Visitor pattern usage, for calculating how many nodes
					// were visited.
			}
			else
			{
				cerr << "This is not a query operation." << endl;
			}

			if ((count % 1000) == 0)
				cerr << count << endl;

			count++;
		}

		MyQueryStrategy2 qs;
		tree->queryStrategy(qs);

		cerr << "Indexed space: " << qs.m_indexedSpace << endl;
		cerr << "Operations: " << count << endl;
		cerr << *tree;
		cerr << "Index I/O: " << indexIO << endl;
		cerr << "Leaf I/O: " << leafIO << endl;
		cerr << "Buffer hits: " << file->getHits() << endl;

		delete tree;
		delete file;
		delete diskfile;
			// delete the buffer first, then the storage manager
			// (otherwise the the buffer will fail writting the dirty entries).
	}
	catch (Tools::Exception& e)
	{
		cerr << "******ERROR******" << endl;
		std::string s = e.what();
		cerr << s << endl;
		return -1;
	}
	catch (...)
	{
		cerr << "******ERROR******" << endl;
		cerr << "other exception" << endl;
		return -1;
	}

	return 0;
}
