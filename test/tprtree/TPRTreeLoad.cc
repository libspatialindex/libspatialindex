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
// see RTreeQuery for a more elaborate example.
class MyVisitor : public IVisitor
{
public:
	void visitNode(const INode& n) {}

	void visitData(const IData& d)
	{
		cout << d.getIdentifier() << endl;
			// the ID of this data entry is an answer to the query. I will just print it to stdout.
	}

	void visitData(std::vector<const IData*>& v) {}
};

int main(int argc, char** argv)
{
	try
	{
		if (argc != 4)
		{
			cerr << "Usage: " << argv[0] << " input_file tree_file capacity." << endl;
			return -1;
		}

		ifstream fin(argv[1]);
		if (! fin)
		{
			cerr << "Cannot open data file " << argv[1] << "." << endl;
			return -1;
		}

		// Create a new storage manager with the provided base name and a 4K page size.
		string baseName = argv[2];
		IStorageManager* diskfile = StorageManager::createNewDiskStorageManager(baseName, 4096);
		StorageManager::IBuffer* file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, 10, false);
			// applies a main memory random buffer on top of the persistent storage manager
			// (LRU buffer, etc can be created the same way).

		// Create a new, empty, TPRTree with dimensionality 2, minimum load 70%, horizon 20 time instants, using "file" as
		// the StorageManager and the TPRSTAR splitting policy.
		id_type indexIdentifier;
		ISpatialIndex* tree = TPRTree::createNewTPRTree(*file, 0.7, atoi(argv[3]), atoi(argv[3]), 2, SpatialIndex::TPRTree::TPRV_RSTAR, 20, indexIdentifier);

		size_t count = 0;
		id_type id;
		size_t op;
		double ax, vx, ay, vy, ct, rt, unused;
		double plow[2], phigh[2];
		double pvlow[2], pvhigh[2];

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
			}

			if ((count % 1000) == 0)
				cerr << count << endl;

			count++;
		}

		cerr << "Operations: " << count << endl;
		cerr << *tree;
		cerr << "Buffer hits: " << file->getHits() << endl;
		cerr << "Index ID: " << indexIdentifier << endl;

		bool ret = tree->isIndexValid();
		if (ret == false) cerr << "ERROR: Structure is invalid!" << endl;
		else cerr << "The stucture seems O.K." << endl;

		delete tree;
		delete file;
		delete diskfile;
			// delete the buffer first, then the storage manager
			// (otherwise the the buffer will fail trying to write the dirty entries).
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
