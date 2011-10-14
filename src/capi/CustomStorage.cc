/******************************************************************************
 * $Id: CustomStorage.cc 1385 2009-06-17 13:45:16Z nitro $
 *
 * Project:  libsidx - A C API wrapper around libspatialindex
 * Purpose:  C++ object declarations to implement the custom storage manager.
 * Author:   Matthias (nitro)
 *
 ******************************************************************************
 * Copyright (c) 2010, Matthias (nitro)
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

//#include <spatialindex/SpatialIndexImpl.h>
#include <spatialindex/capi/sidx_impl.h>

using namespace SpatialIndex;
using namespace SpatialIndex::StorageManager;


IStorageManager* SpatialIndex::StorageManager::returnCustomStorageManager(Tools::PropertySet& ps)
{
	IStorageManager* sm = new CustomStorageManager(ps);
	return sm;
}

CustomStorageManager::CustomStorageManager(Tools::PropertySet& ps)
{
	Tools::Variant var;
	var = ps.getProperty("CustomStorageCallbacks");

	if (var.m_varType != Tools::VT_EMPTY)
	{
		if (var.m_varType != Tools::VT_PVOID)
			throw Tools::IllegalArgumentException("CustomStorageManager: Property CustomStorageCallbacks must be Tools::VT_PVOID");

        if (!var.m_val.pvVal)
			throw Tools::IllegalArgumentException("CustomStorageManager: Property CustomStorageCallbacks must not be 0.");

        // we already checked for validity in IndexProperty_SetCustomStorageCallbacks
        CustomStorageManagerCallbacks* callbackArray = static_cast<CustomStorageManagerCallbacks*>(var.m_val.pvVal);
		callbacks = *callbackArray;
	}

    int errorCode( NoError );
    if ( callbacks.createCallback ) callbacks.createCallback( callbacks.context, &errorCode );
    processErrorCode( errorCode, NewPage );
}

CustomStorageManager::~CustomStorageManager()
{
    int errorCode( NoError );
    if ( callbacks.destroyCallback ) callbacks.destroyCallback( callbacks.context, &errorCode );
    processErrorCode( errorCode, NewPage );
}

void CustomStorageManager::loadByteArray(const id_type page, uint32_t& len, byte** data)
{
    int errorCode( NoError );
    if ( callbacks.loadByteArrayCallback ) callbacks.loadByteArrayCallback( callbacks.context, page, &len, data, &errorCode );
    processErrorCode( errorCode, page );
}

void CustomStorageManager::storeByteArray(id_type& page, const uint32_t len, const byte* const data)
{
    int errorCode( NoError );
    if ( callbacks.storeByteArrayCallback ) callbacks.storeByteArrayCallback( callbacks.context, &page, len, data, &errorCode );
    processErrorCode( errorCode, page );
}

void CustomStorageManager::deleteByteArray(const id_type page)
{
    int errorCode( NoError );
    if ( callbacks.deleteByteArrayCallback ) callbacks.deleteByteArrayCallback( callbacks.context, page, &errorCode );
    processErrorCode( errorCode, page );
}

inline void CustomStorageManager::processErrorCode(int errorCode, const id_type page)
{
    switch (errorCode)
    {
    case NoError:
    break;
    
    case InvalidPageError:
        throw InvalidPageException( page );
    break;
    
    case IllegalStateError:
        throw Tools::IllegalStateException( "CustomStorageManager: Error in user implementation." );
    break;
    
    default:
        throw Tools::IllegalStateException( "CustomStorageManager: Unknown error." );
    }
}
