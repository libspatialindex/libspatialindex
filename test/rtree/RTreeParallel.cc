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

#include <cstring>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>

// include library header file.
#include <spatialindex/SpatialIndex.h>

using namespace SpatialIndex;
using namespace std;

#define INSERT 1
#define DELETE 0
#define QUERY 2

void StartTheClock(uint64_t& s)
{
#ifdef _MSC_VER
        FILETIME ft;
    LARGE_INTEGER li;

        GetSystemTimeAsFileTime(&ft);
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        s = (uint64_t) (li.QuadPart / 10000);
#else
        struct timeval tv;
        gettimeofday(&tv, 0);
        s = (1000 * tv.tv_sec) + (tv.tv_usec / 1000);
#endif
}

uint64_t StopTheClock(uint64_t s)
{
#ifdef _MSC_VER
    FILETIME ft;
    LARGE_INTEGER li;
    uint64_t t;

        GetSystemTimeAsFileTime(&ft);
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        t = (uint64_t) (li.QuadPart / 10000);
        return t - s;
#else
        struct timeval tv;
        gettimeofday(&tv, 0);
        return (1000 * tv.tv_sec) + (tv.tv_usec / 1000) - s;
#endif
}

// example of a Visitor pattern.
// findes the index and leaf IO for answering the query and prints
// the resulting data IDs to stdout.
class MyVisitor : public IVisitor
{
public:
	void visitNode(const INode& n)
	{
	}

	void visitData(const IData& d)
	{
		IShape* pS;
		d.getShape(&pS);
			// do something.
		delete pS;

		// data should be an array of characters representing a Region as a string.
		uint8_t* pData = 0;
		uint32_t cLen = 0;
		d.getData(cLen, &pData);
		// do something.
		//string s = reinterpret_cast<char*>(pData);
		//cout << s << "\n";
		delete[] pData;

		cout << d.getIdentifier() << "\n";
			// the ID of this data entry is an answer to the query. I will just print it to stdout.
	}

	void visitData(std::vector<const IData*>& v)
	{
		cout << v[0]->getIdentifier() << " " << v[1]->getIdentifier() << "\n";
	}
};

int main(int argc, char** argv)
{
	try
	{
		if (argc != 4)
		{
			cerr << "Usage: " << argv[0] << " query_file tree_file query_type [intersection | 10NN | selfjoin]." << "\n";
			return -1;
		}

		uint32_t queryType = 0;

		if (strcmp(argv[3], "intersection") == 0) queryType = 0;
		else if (strcmp(argv[3], "10NN") == 0) queryType = 1;
		else if (strcmp(argv[3], "selfjoin") == 0) queryType = 2;
		else
		{
			cerr << "Unknown query type." << "\n";
			return -1;
		}

		ifstream fin(argv[1]);
		if (! fin)
		{
			cerr << "Cannot open query file " << argv[1] << "." << "\n";
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
		ISpatialIndex* tree = RTree::loadRTree(*file, 1);

		size_t count = 0;
		id_type id;
		uint32_t op;
		double x1, x2, y1, y2;
		double plow[2], phigh[2];
		boost::asio::io_service IOService;
		boost::thread_group threads;

		uint64_t u64Time;
		StartTheClock(u64Time);

		{
			boost::asio::io_service::work work(IOService);

			for (size_t i = 0; i < 1; ++i)
				threads.create_thread(boost::bind(&boost::asio::io_service::run, &IOService));

			while (fin)
			{
				fin >> op >> id >> x1 >> y1 >> x2 >> y2;
				if (! fin.good()) continue; // skip newlines, etc.
	
				if (op == QUERY)
				{
					plow[0] = x1; plow[1] = y1;
					phigh[0] = x2; phigh[1] = y2;
	
					MyVisitor vis;
	
					if (queryType == 0)
					{
						Region r = Region(plow, phigh, 2);
						IOService.post(boost::bind(&ISpatialIndex::intersectsWithQuery, tree, r, vis));
							// this will find all data that intersect with the query range.
					}
					else if (queryType == 1)
					{
						Point p = Point(plow, 2);
						IOService.post(boost::bind(&ISpatialIndex::nearestNeighborQuery, tree, 10, p, vis));
							// this will find the 10 nearest neighbors.
					}
					else
					{
						Region r = Region(plow, phigh, 2);
						IOService.post(boost::bind(&ISpatialIndex::selfJoinQuery, tree, r, vis));
					}
				}
				else
				{
					cerr << "This is not a query operation." << "\n";
				}
			}
		}

		IOService.run();
		IOService.stop();
		threads.join_all();

		u64Time = StopTheClock(u64Time);

		cerr << "Operations: " << count << "\n";
		cerr << *tree;
		cerr << "Buffer hits: " << file->getHits() << "\n";
		cerr << "Total time: " << u64Time << "ms \n";

		delete tree;
		delete file;
		delete diskfile;
			// delete the buffer first, then the storage manager
			// (otherwise the the buffer will fail to write the dirty entries).
	}
	catch (Tools::Exception& e)
	{
		cerr << "******ERROR******" << "\n";
		std::string s = e.what();
		cerr << s << "\n";
		return -1;
	}
	catch (...)
	{
		cerr << "******ERROR******" << "\n";
		cerr << "other exception" << "\n";
		return -1;
	}

	return 0;
}
