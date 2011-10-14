/******************************************************************************
 * $Id: index.cc 1385 2009-08-13 15:45:16Z hobu $
 *
 * Project:	 libsidx - A C API wrapper around libspatialindex
 * Purpose:	 C++ objects to implement the index.
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

SpatialIndex::ISpatialIndex* Index::CreateIndex() 
{
	using namespace SpatialIndex;
	
	ISpatialIndex* index = 0;
	
	Tools::Variant var;

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


Index::Index(const Tools::PropertySet& poProperties) 
{
	Setup();
	
	m_properties = poProperties;

	Initialize();
}


Index::~Index() 
{
	if (m_rtree != 0)
		delete m_rtree;
	if (m_buffer != 0)
		delete m_buffer;
	if (m_storage != 0)
		delete m_storage;

}

Index::Index(	const Tools::PropertySet& poProperties, 
				int (*readNext)(SpatialIndex::id_type *id, 
								double **pMin, 
								double **pMax, 
								uint32_t *nDimension, 
								const uint8_t **pData, 
								uint32_t *nDataLength)) 
{
	using namespace SpatialIndex;
		
	Setup();
	
	m_properties = poProperties;

	m_storage = CreateStorage();
	m_buffer = CreateIndexBuffer(*m_storage);
	
	DataStream ds(readNext);
	
	double dFillFactor = 0.7;
	uint32_t nIdxCapacity = 100;
	uint32_t nIdxLeafCap = 100;
	uint32_t nIdxDimension = 2;
	SpatialIndex::RTree::RTreeVariant eVariant = SpatialIndex::RTree::RV_RSTAR;
	SpatialIndex::id_type m_IdxIdentifier;

	// Fetch a bunch of properties.	 We can't bulk load an rtree using merely 
	// properties, we have to use the helper method(s).

	Tools::Variant var;
	var = m_properties.getProperty("FillFactor");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_DOUBLE)
			throw std::runtime_error("Index::Index (streaming):"
									 " Property FillFactor must be Tools::VT_DOUBLE");
		
		dFillFactor = var.m_val.dblVal;
	}
	
	var = m_properties.getProperty("IndexCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw std::runtime_error("Index::Index (streaming): "
									 "Property IndexCapacity must be Tools::VT_ULONG");
		
		nIdxCapacity = var.m_val.ulVal;
	}

	var = m_properties.getProperty("LeafCapacity");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw std::runtime_error("Index::Index (streaming): "
									 "Property LeafCapacity must be Tools::VT_ULONG");
		
		nIdxLeafCap = var.m_val.ulVal;
	}

	var = m_properties.getProperty("Dimension");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_ULONG)
			throw std::runtime_error("Index::Index (streaming): "
									 "Property Dimension must be Tools::VT_ULONG");
		
		nIdxDimension = var.m_val.ulVal;
	}

	var = m_properties.getProperty("TreeVariant");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_LONG)
			throw std::runtime_error("Index::Index (streaming): "
									 "Property TreeVariant must be Tools::VT_LONG");
		
		eVariant = static_cast<SpatialIndex::RTree::RTreeVariant>(var.m_val.lVal);
	}

	var = m_properties.getProperty("IndexIdentifier");
	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_LONGLONG)
			throw std::runtime_error("Index::Index (streaming): "
									 "Property IndexIdentifier must be Tools::VT_LONGLONG");
		
		m_IdxIdentifier = var.m_val.llVal;
	}
	
	m_rtree = RTree::createAndBulkLoadNewRTree(	  SpatialIndex::RTree::BLM_STR,
												  ds,
												  *m_buffer,
												  dFillFactor,
												  nIdxCapacity,
												  nIdxLeafCap,
												  nIdxDimension,
												  eVariant,
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

void Index::SetIndexVariant(RTStorageType v)
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
