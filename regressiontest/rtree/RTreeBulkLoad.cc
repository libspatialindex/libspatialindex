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

// NOTE: Please read README.txt before browsing this code.

// include library header file.
#include <SpatialIndex.h>

using namespace SpatialIndex;
using namespace std;

#define INSERT 1
#define DELETE 0
#define QUERY 2

class MyDataStream : public IDataStream
{
public:
	MyDataStream(string inputFile) : m_pNext(0)
	{
		m_fin.open(inputFile.c_str());

		if (! m_fin)
			throw Tools::IllegalArgumentException("Input file not found.");

		id_type id;
		uint32_t op;
		double low[2], high[2];

		m_fin >> op >> id >> low[0] >> low[1] >> high[0] >> high[1];

		if (m_fin.good())
		{
			if (op != INSERT)
				throw Tools::IllegalArgumentException(
					"The data input should contain insertions only."
				);

			Region r = Region(low, high, 2);
			m_pNext = new RTree::Data(0, 0, r, id);
		}
	}

	virtual ~MyDataStream()
	{
		if (m_pNext != 0) delete m_pNext;
	}

	virtual IData* getNext()
	{
		if (m_pNext == 0) return 0;

		RTree::Data* ret = m_pNext;
		m_pNext = 0;

		id_type id;
		uint32_t op;
		double low[2], high[2];

		m_fin >> op >> id >> low[0] >> low[1] >> high[0] >> high[1];

		if (m_fin.good())
		{
			if (op != INSERT)
				throw Tools::IllegalArgumentException(
					"The data input should contain insertions only."
				);

			Region r = Region(low, high, 2);
			m_pNext = new RTree::Data(0, 0, r, id);
		}

		return ret;
	}

	virtual bool hasNext() throw (Tools::NotSupportedException)
	{
		return (m_pNext != 0);
	}

	virtual size_t size() throw (Tools::NotSupportedException)
	{
		throw Tools::NotSupportedException("Operation not supported.");
	}

	virtual void rewind() throw (Tools::NotSupportedException)
	{
		id_type id;
		uint32_t op;
		double low[2], high[2];
		
		if (m_pNext != 0)
		{
			delete m_pNext;
			m_pNext = 0;
		}

		m_fin.seekg(0, std::ios::beg);
		m_fin >> op >> id >> low[0] >> low[1] >> high[0] >> high[1];

		if (m_fin.good())
		{
			if (op != INSERT)
				throw Tools::IllegalArgumentException(
					"The data input should contain insertions only."
				);

			Region r = Region(low, high, 2);
			m_pNext = new RTree::Data(0, 0, r, id);
		}
	}

	std::ifstream m_fin;
	RTree::Data* m_pNext;
};

int main(int argc, char** argv)
{
	//try
	{
		if (argc != 5)
		{
			cerr << "Usage: " << argv[0] << " input_file tree_file capacity utilization." << endl;
			return -1;
		}

		string baseName = argv[2];
		double utilization = atof(argv[4]);

		// Create a new storage manager with the provided base name and a 4K page size.
		IStorageManager* diskfile = StorageManager::createNewDiskStorageManager(baseName, 4096);
		StorageManager::IBuffer* file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, 10, false);
			// applies a main memory random buffer on top of the persistent storage manager
			// (LRU buffer, etc can be created the same way).

		MyDataStream stream(argv[1]);

		// Create and bulk load a new RTree with dimensionality 2, using "file" as
		// the StorageManager and the RSTAR splitting policy.
		id_type indexIdentifier;
		ISpatialIndex* tree = RTree::createAndBulkLoadNewRTree(
			RTree::BLM_STR, stream, *file, utilization, atoi(argv[3]), atoi(argv[3]), 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier);

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
/*
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
*/
	return 0;
}
