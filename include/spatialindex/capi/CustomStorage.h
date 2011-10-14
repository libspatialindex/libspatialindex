/******************************************************************************
 * $Id: CustomStorage.h 1385 2009-06-17 13:45:16Z nitro $
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
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************/
 
#pragma once

namespace SpatialIndex
{
	namespace StorageManager
	{
        struct CustomStorageManagerCallbacks
        {
            CustomStorageManagerCallbacks() : context(0), createCallback(0), destroyCallback(0), loadByteArrayCallback(0), storeByteArrayCallback(0), deleteByteArrayCallback(0)
            {}

            void* context;
            void (*createCallback)( const void* context, int* errorCode ); 
            void (*destroyCallback)( const void* context, int* errorCode ); 
            void (*loadByteArrayCallback)( const void* context, const id_type page, uint32_t* len, byte** data, int* errorCode ); 
            void (*storeByteArrayCallback)( const void* context, id_type* page, const uint32_t len, const byte* const data, int* errorCode ); 
            void (*deleteByteArrayCallback)( const void* context, const id_type page, int* errorCode ); 
        };

        class CustomStorageManager : public SpatialIndex::IStorageManager
        {
        public:
            // I'd like this to be an enum, but casting between enums and ints is not nice
            static const int NoError = 0;
            static const int InvalidPageError = 1;
            static const int IllegalStateError = 2;

	        CustomStorageManager(Tools::PropertySet&);

	        virtual ~CustomStorageManager();

	        virtual void loadByteArray(const id_type page, uint32_t& len, byte** data);
	        virtual void storeByteArray(id_type& page, const uint32_t len, const byte* const data);
	        virtual void deleteByteArray(const id_type page);

        private:
            CustomStorageManagerCallbacks   callbacks;

            inline void processErrorCode(int errorCode, const id_type page);
        }; // CustomStorageManager
    
        // factory function
        IStorageManager* returnCustomStorageManager(Tools::PropertySet& in);
    }
}

