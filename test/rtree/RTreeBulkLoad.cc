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

using namespace SpatialIndex;

#define INSERT 1
#define DELETE 0
#define QUERY 2

class MyDataStream : public IDataStream
{
public:
	MyDataStream(std::string inputFile) : m_pNext(nullptr)
	{
		m_fin.open(inputFile.c_str());

		if (! m_fin)
			throw Tools::IllegalArgumentException("Input file not found.");

		readNextEntry();
	}

	~MyDataStream() override
	{
		if (m_pNext != nullptr) delete m_pNext;
	}

	IData* getNext() override
	{
		if (m_pNext == nullptr) return nullptr;

		RTree::Data* ret = m_pNext;
		m_pNext = nullptr;
		readNextEntry();
		return ret;
	}

	bool hasNext() override
	{
		return (m_pNext != nullptr);
	}

	uint32_t size() override
	{
		throw Tools::NotSupportedException("Operation not supported.");
	}

	void rewind() override
	{
		if (m_pNext != nullptr)
		{
			delete m_pNext;
			m_pNext = nullptr;
		}

		m_fin.seekg(0, std::ios::beg);
		readNextEntry();
	}

	void readNextEntry()
	{
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

			Region r(low, high, 2);
			m_pNext = new RTree::Data(sizeof(double), reinterpret_cast<uint8_t*>(low), r, id);
				// Associate a bogus data array with every entry for testing purposes.
				// Once the data array is given to RTRee:Data a local copy will be created.
				// Hence, the input data array can be deleted after this operation if not
				// needed anymore.
		}
	}

	std::ifstream m_fin;
	RTree::Data* m_pNext;
};

int main(int argc, char** argv)
{
	try
	{
		if (argc != 5)
		{
			std::cerr << "Usage: " << argv[0] << " input_file tree_file capacity utilization." << std::endl;
			return -1;
		}

		std::string baseName = argv[2];
		double utilization = atof(argv[4]);

		IStorageManager* diskfile = StorageManager::createNewDiskStorageManager(baseName, 4096);
			// Create a new storage manager with the provided base name and a 4K page size.

		StorageManager::IBuffer* file = StorageManager::createNewRandomEvictionsBuffer(*diskfile, 10, false);
			// applies a main memory random buffer on top of the persistent storage manager
			// (LRU buffer, etc can be created the same way).

		MyDataStream stream(argv[1]);

		// Create and bulk load a new RTree with dimensionality 2, using "file" as
		// the StorageManager and the RSTAR splitting policy.
		id_type indexIdentifier;
		ISpatialIndex* tree = RTree::createAndBulkLoadNewRTree(
			RTree::BLM_STR, stream, *file, utilization, atoi(argv[3]), atoi(argv[3]), 2, SpatialIndex::RTree::RV_RSTAR, indexIdentifier);

		std::cerr << *tree;
		std::cerr << "Buffer hits: " << file->getHits() << std::endl;
		std::cerr << "Index ID: " << indexIdentifier << std::endl;

		bool ret = tree->isIndexValid();
		if (ret == false) std::cerr << "ERROR: Structure is invalid!" << std::endl;
		else std::cerr << "The stucture seems O.K." << std::endl;

		delete tree;
		delete file;
		delete diskfile;
			// delete the buffer first, then the storage manager
			// (otherwise the the buffer will fail trying to write the dirty entries).
	}
	catch (Tools::Exception& e)
	{
		std::cerr << "******ERROR******" << std::endl;
		std::string s = e.what();
		std::cerr << s << std::endl;
		return -1;
	}

	return 0;
}
