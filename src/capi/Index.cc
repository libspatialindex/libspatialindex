/******************************************************************************
 * Project:  libsidx - A C API wrapper around libspatialindex
 * Purpose:	 C++ objects to implement the index.
 * Author:   Howard Butler, hobu.inc@gmail.com
 ******************************************************************************
 * Copyright (c) 2009, Howard Butler
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

#include <spatialindex/capi/sidx_impl.h>

SpatialIndex::ISpatialIndex* Index::CreateIndex()
{
	using namespace SpatialIndex;

	ISpatialIndex* index = 0;

	Tools::Variant var;

	// Cache the result set limit
    var = m_properties.getProperty("ResultSetLimit");
    if (var.m_varType != Tools::VT_EMPTY)
    {
        if (var.m_varType != Tools::VT_LONGLONG)
            throw std::runtime_error("Index::ResultSetLimit: "
                                     "Property ResultSetLimit must be Tools::VT_LONGLONG");
        m_resultSetLimit = var.m_val.llVal;
    }
    else
        m_resultSetLimit = 0;

    // Cache the result set offset
    var = m_properties.getProperty("ResultSetOffset");
    if (var.m_varType != Tools::VT_EMPTY)
    {
        if (var.m_varType != Tools::VT_LONGLONG)
            throw std::runtime_error("Index::ResultSetOffset: "
                                     "Property ResultSetOffset must be Tools::VT_LONGLONG");
        m_resultSetOffset = var.m_val.llVal;
    }
    else
        m_resultSetOffset = 0;

	if (GetIndexType() == RT_RTree) {

		try {
			index = RTree::returnRTree(	 *m_buffer, m_properties);
		} catch (Tools::Exception& e) {
			std::ostringstream os;
			os << "Spatial Index Error: " << e.what();
			throw std::runtime_error(os.str());
		}
	}

	else if (GetIndexType() == RT_MVRTree) {

		try {
			index = MVRTree::returnMVRTree(	 *m_buffer, m_properties);
		} catch (Tools::Exception& e) {
			std::ostringstream os;
			os << "Spatial Index Error: " << e.what();
			throw std::runtime_error(os.str());
		}
	}

	else if (GetIndexType() == RT_TPRTree) {

		try {
			index = TPRTree::returnTPRTree(	 *m_buffer,m_properties);
		} catch (Tools::Exception& e) {
			std::ostringstream os;
			os << "Spatial Index Error: " << e.what();
			throw std::runtime_error(os.str());
		}
	}

	return index;
}


Index::Index(const Tools::PropertySet& poProperties): m_properties(poProperties)
{
	Setup();

	Initialize();
}


Index::~Index()
{
	delete m_rtree;
	delete m_buffer;
	delete m_storage;
}

Index::Index(	const Tools::PropertySet& poProperties,
				int (*readNext)(SpatialIndex::id_type *id,
								double **pMin,
								double **pMax,
								uint32_t *nDimension,
								const uint8_t **pData,
								size_t *nDataLength))
: Index(poProperties, std::unique_ptr<DataStream>(new DataStream(readNext)))
{}

Index::Index(const Tools::PropertySet& poProperties,
             std::unique_ptr<SpatialIndex::IDataStream> ds)
: m_properties(poProperties)
{
	using namespace SpatialIndex;

	Setup();

	m_storage = CreateStorage();
	m_buffer = CreateIndexBuffer(*m_storage);

	SpatialIndex::id_type m_IdxIdentifier;

	// For memory storage ensure we do not write any files to disk during
	// the bulk-loading phase
	if (GetIndexStorage() == RT_Memory)
	{
		Tools::Variant sz;
		sz.m_varType = Tools::VT_ULONG;

		sz.m_val.ulVal = 1073741824;
		m_properties.setProperty("ExternalSortBufferPageSize", sz);

		sz.m_val.ulVal = 2;
		m_properties.setProperty("ExternalSortBufferTotalPages", sz);
	}

	Tools::Variant var = m_properties.getProperty("IndexIdentifier");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_LONGLONG)
			throw std::runtime_error("Index::Index (streaming): "
									 "Property IndexIdentifier must be Tools::VT_LONGLONG");

		m_IdxIdentifier = var.m_val.llVal;
	}

	m_rtree = RTree::createAndBulkLoadNewRTree(	  SpatialIndex::RTree::BLM_STR,
												  *ds,
												  *m_buffer,
												  m_properties,
												  m_IdxIdentifier);
}


SpatialIndex::StorageManager::IBuffer* Index::CreateIndexBuffer(SpatialIndex::IStorageManager& storage)
{
	using namespace SpatialIndex::StorageManager;
	IBuffer* buffer = 0;
	try {
		if ( m_storage == 0 )
			throw std::runtime_error("Storage was invalid to create index buffer");
		buffer = returnRandomEvictionsBuffer(storage, m_properties);
	} catch (Tools::Exception& e) {
		std::ostringstream os;
		os << "Spatial Index Error: " << e.what();
		throw std::runtime_error(os.str());
	}
	return buffer;
}


SpatialIndex::IStorageManager* Index::CreateStorage()
{
	using namespace SpatialIndex::StorageManager;

	SpatialIndex::IStorageManager* storage = 0;
	std::string filename("");

	Tools::Variant var;
	var = m_properties.getProperty("FileName");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_PCHAR)
			throw std::runtime_error("Index::CreateStorage: "
									 "Property FileName must be Tools::VT_PCHAR");

		filename = std::string(var.m_val.pcVal);
	}

	if (GetIndexStorage() == RT_Disk) {
		if (filename.empty()) {
				std::ostringstream os;
				os << "Spatial Index Error: filename was empty."
					  "	 Set IndexStorageType to RT_Memory";
				throw std::runtime_error(os.str());
		}
			try {
				storage = returnDiskStorageManager(m_properties);
				return storage;
			} catch (Tools::Exception& e) {
				std::ostringstream os;
				os << "Spatial Index Error: " << e.what();
				throw std::runtime_error(os.str());
			}

	} else if (GetIndexStorage() == RT_Memory) {

		try {
			storage = returnMemoryStorageManager(m_properties);
			return storage;
		} catch (Tools::Exception& e) {
			std::ostringstream os;
			os << "Spatial Index Error: " << e.what();
			throw std::runtime_error(os.str());
		}

	} else if (GetIndexStorage() == RT_Custom) {

		try {
            storage = returnCustomStorageManager(m_properties);
			return storage;
		} catch (Tools::Exception& e) {
			std::ostringstream os;
			os << "Spatial Index Error: " << e.what();
			throw std::runtime_error(os.str());
		}

	}
	return storage;
}




void Index::Initialize()
{
	m_storage = CreateStorage();
	m_buffer = CreateIndexBuffer(*m_storage);
	m_rtree = CreateIndex();
}

void Index::Setup()

{
	m_buffer = 0;
	m_storage = 0;
	m_rtree = 0;
	m_resultSetLimit = 0;
	m_resultSetOffset = 0;
}

RTIndexType Index::GetIndexType()
{
	Tools::Variant var;
	var = m_properties.getProperty("IndexType");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw std::runtime_error("Index::GetIndexType: "
									 "Property IndexType must be Tools::VT_ULONG");

		return static_cast<RTIndexType>(var.m_val.ulVal);
	}

	// if we didn't get anything, we're returning an error condition
	return RT_InvalidIndexType;

}
void Index::SetIndexType(RTIndexType v)
{
	Tools::Variant var;
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = v;
	m_properties.setProperty("IndexType", var);
}

RTStorageType Index::GetIndexStorage()
{

	Tools::Variant var;
	var = m_properties.getProperty("IndexStorageType");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw std::runtime_error("Index::GetIndexStorage: "
									 "Property IndexStorageType must be Tools::VT_ULONG");

		return static_cast<RTStorageType>(var.m_val.ulVal);
	}

	// if we didn't get anything, we're returning an error condition
	return RT_InvalidStorageType;
}

void Index::SetIndexStorage(RTStorageType v)
{
	Tools::Variant var;
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = v;
	m_properties.setProperty("IndexStorageType", var);
}

RTIndexVariant Index::GetIndexVariant()
{

	Tools::Variant var;
	var = m_properties.getProperty("TreeVariant");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw std::runtime_error("Index::GetIndexVariant: "
									 "Property TreeVariant must be Tools::VT_ULONG");

		return static_cast<RTIndexVariant>(var.m_val.ulVal);
	}

	// if we didn't get anything, we're returning an error condition
	return RT_InvalidIndexVariant;
}

void Index::SetIndexVariant(RTIndexVariant v)
{
	using namespace SpatialIndex;
	Tools::Variant var;

	if (GetIndexType() == RT_RTree) {
		var.m_val.ulVal = static_cast<RTree::RTreeVariant>(v);
		m_properties.setProperty("TreeVariant", var);
	} else if (GetIndexType() == RT_MVRTree) {
		var.m_val.ulVal = static_cast<MVRTree::MVRTreeVariant>(v);
		m_properties.setProperty("TreeVariant", var);
	} else if (GetIndexType() == RT_TPRTree) {
		var.m_val.ulVal = static_cast<TPRTree::TPRTreeVariant>(v);
		m_properties.setProperty("TreeVariant", var);
	}
}

int64_t Index::GetResultSetOffset()
{
    return m_resultSetOffset;
}

void Index::SetResultSetOffset(int64_t v)
{
    Tools::Variant var;
    var.m_varType = Tools::VT_LONGLONG;
    var.m_val.llVal = v;
    m_properties.setProperty("ResultSetOffset", var);
    m_resultSetOffset = v;
}


int64_t Index::GetResultSetLimit()
{
    return m_resultSetLimit;
}

void Index::SetResultSetLimit(int64_t v)
{
    Tools::Variant var;
    var.m_varType = Tools::VT_LONGLONG;
    var.m_val.llVal = v;
    m_properties.setProperty("ResultSetLimit", var);
    m_resultSetLimit = v;
}

void Index::flush()
{
	m_rtree->flush();
	m_storage->flush();
}
