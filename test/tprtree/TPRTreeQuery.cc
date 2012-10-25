/******************************************************************************
 * Project:  libspatialindex - A C++ library for spatial indexing
 * Author:   Marios Hadjieleftheriou, mhadji@gmail.com
 ******************************************************************************
 * Copyright (c) 2002, Marios Hadjieleftheriou
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

// include library header file.

#include <spatialindex/SpatialIndex.h>

#include <limits>

using namespace SpatialIndex;
using namespace std;

#define INSERT 1
#define DELETE 0
#define QUERY 2

// example of a Visitor pattern.
// findes the index and leaf IO for answering the query and prints
// the resulting data IDs to stdout.
class MyVisitor : public IVisitor
{
public:
	size_t m_indexIO;
	size_t m_leafIO;

public:
	MyVisitor() : m_indexIO(0), m_leafIO(0) {}

	void visitNode(const INode& n)
	{
		if (n.isLeaf()) m_leafIO++;
		else m_indexIO++;
	}

	void visitData(const IData& d)
	{
		IShape* pS;
		d.getShape(&pS);
			// do something.
		delete pS;

		// data should be an array of characters representing a Region as a string.
		//byte* pData = 0;
		//size_t cLen = 0;
		//d.getData(cLen, &pData);
			// do something.
			//string s = reinterpret_cast<char*>(pData);
			//cout << s << endl;
		//delete[] pData;

		cout << d.getIdentifier() << endl;
			// the ID of this data entry is an answer to the query. I will just print it to stdout.
	}

	void visitData(std::vector<const IData*>& v) {}
};

// example of a Strategy pattern.
// traverses the tree by level.
class MyQueryStrategy : public SpatialIndex::IQueryStrategy
{
private:
	queue<id_type> ids;

public:
	void getNextEntry(const IEntry& entry, id_type& nextEntry, bool& hasNext)
	{
		IShape* ps;
		entry.getShape(&ps);
		MovingRegion* pr = dynamic_cast<MovingRegion*>(ps);

		cout << pr->m_pLow[0] << " " << pr->m_pLow[1] << endl;
		cout << pr->m_pHigh[0] << " " << pr->m_pLow[1] << endl;
		cout << pr->m_pHigh[0] << " " << pr->m_pHigh[1] << endl;
		cout << pr->m_pLow[0] << " " << pr->m_pHigh[1] << endl;
		cout << pr->m_pLow[0] << " " << pr->m_pLow[1] << endl << endl << endl;
			// print node MBRs gnuplot style!

		delete ps;

		const INode* n = dynamic_cast<const INode*>(&entry);

		// traverse only index nodes at levels 2 and higher.
		if (n != 0 && n->getLevel() > 1)
		{
			for (size_t cChild = 0; cChild < n->getChildrenCount(); cChild++)
			{
				ids.push(n->getChildIdentifier(cChild));
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
	void getNextEntry(const IEntry& entry, id_type& nextEntry, bool& hasNext)
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
		if (argc != 3)
		{
			cerr << "Usage: " << argv[0] << " input_file tree_file." << endl;
			return -1;
		}

		ifstream fin(argv[1]);
		if (! fin)
		{
			cerr << "Cannot open data file " << argv[1] << "." << endl;
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
		ISpatialIndex* tree = TPRTree::loadTPRTree(*file, 1);

		size_t count = 0;
		id_type id;
		size_t op;
		double ax, vx, ay, vy, ct, rt, unused;
		double plow[2], phigh[2];
		double pvlow[2], pvhigh[2];
		size_t indexIO = 0;
		size_t leafIO = 0;

		while (fin)
		{
			fin >> id >> op >> ct >> rt >> unused >> ax >> vx >> unused >> ay >> vy;
			if (! fin.good()) continue; // skip newlines, etc.

			if (op == INSERT)
			{
				plow[0] = ax; plow[1] = ay;
				phigh[0] = ax; phigh[1] = ay;
				pvlow[0] = vx; pvlow[1] = vy;
				pvhigh[0] = vx; pvhigh[1] = vy;
				Tools::Interval ivT(ct, std::numeric_limits<double>::max());

				MovingRegion r = MovingRegion(plow, phigh, pvlow, pvhigh, ivT, 2);

				//ostringstream os;
				//os << r;
				//string data = os.str();
					// associate some data with this region. I will use a string that represents the
					// region itself, as an example.
					// NOTE: It is not necessary to associate any data here. A null pointer can be used. In that
					// case you should store the data externally. The index will provide the data IDs of
					// the answers to any query, which can be used to access the actual data from the external
					// storage (e.g. a hash table or a database table, etc.).
					// Storing the data in the index is convinient and in case a clustered storage manager is
					// provided (one that stores any node in consecutive pages) performance will improve substantially,
					// since disk accesses will be mostly sequential. On the other hand, the index will need to
					// manipulate the data, resulting in larger overhead. If you use a main memory storage manager,
					// storing the data externally is highly recommended (clustering has no effect).
					// A clustered storage manager is NOT provided yet.
					// Also you will have to take care of converting you data to and from binary format, since only
					// array of bytes can be inserted in the index (see RTree::Node::load and RTree::Node::store for
					// an example of how to do that).

				//tree->insertData(data.size() + 1, reinterpret_cast<const byte*>(data.c_str()), r, id);

				tree->insertData(0, 0, r, id);
					// example of passing zero size and a null pointer as the associated data.
			}
			else if (op == DELETE)
			{
				plow[0] = ax; plow[1] = ay;
				phigh[0] = ax; phigh[1] = ay;
				pvlow[0] = vx; pvlow[1] = vy;
				pvhigh[0] = vx; pvhigh[1] = vy;
				Tools::Interval ivT(rt, ct);

				MovingRegion r = MovingRegion(plow, phigh, pvlow, pvhigh, ivT, 2);

				if (tree->deleteData(r, id) == false)
				{
					cerr << "******ERROR******" << endl;
					cerr << "Cannot delete id: " << id << " , count: " << count << endl;
					return -1;
				}
			}
			else if (op == QUERY)
			{
				plow[0] = ax; plow[1] = ay;
				phigh[0] = vx; phigh[1] = vy;
				pvlow[0] = 0.0; pvlow[1] = 0.0;
				pvhigh[0] = 0.0; pvhigh[1] = 0.0;

				Tools::Interval ivT(ct, rt);

				MovingRegion r = MovingRegion(plow, phigh, pvlow, pvhigh, ivT, 2);
				MyVisitor vis;

				tree->intersectsWithQuery(r, vis);
					// this will find all data that intersect with the query range.

				indexIO += vis.m_indexIO;
				leafIO += vis.m_leafIO;
					// example of the Visitor pattern usage, for calculating how many nodes
					// were visited.
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
