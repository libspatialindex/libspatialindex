/******************************************************************************
 * $Id: sidx_config.h 1359 2009-07-31 04:44:50Z hobu $
 *
 * Project:  libsidx - A C API wrapper around libspatialindex
 * Purpose:  C API configuration
 * Author:   Howard Butler, hobu.inc@gmail.com
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
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************/

#ifndef SIDX_CONFIG_H_INCLUDED
#define SIDX_CONFIG_H_INCLUDED



#ifdef _MSC_VER

#if _MSC_VER <= 1500
  typedef __int8 int8_t;
  typedef __int16 int16_t;
  typedef __int32 int32_t;
  typedef __int64 int64_t;
  typedef unsigned __int8 uint8_t;
  typedef unsigned __int16 uint16_t;
  typedef unsigned __int32 uint32_t;
  typedef unsigned __int64 uint64_t;
#endif

   #include <windows.h>
   #define STRDUP _strdup
   #include <spatialindex/SpatialIndex.h>
   #include <windows.h>

#else

   #include <stdint.h>
   #define SIDX_THREAD  __thread
   #include <spatialindex/SpatialIndex.h>
   #define STRDUP strdup
#endif

#include <sys/stat.h>



class Item;
class Index;

typedef enum
{
   RT_None = 0,
   RT_Debug = 1,
   RT_Warning = 2,
   RT_Failure = 3,
   RT_Fatal = 4
} RTError;

typedef enum
{
   RT_RTree = 0,
   RT_MVRTree = 1,
   RT_TPRTree = 2,
   RT_InvalidIndexType = -99
} RTIndexType;

typedef enum
{
   RT_Memory = 0,
   RT_Disk = 1,
   RT_Custom = 2,
   RT_InvalidStorageType = -99
} RTStorageType;

typedef enum
{
   RT_Linear = 0,
   RT_Quadratic = 1,
   RT_Star = 2,
   RT_InvalidIndexVariant = -99
} RTIndexVariant;


#ifdef __cplusplus
#  define IDX_C_START           extern "C" {
#  define IDX_C_END             }
#else
#  define IDX_C_START
#  define IDX_C_END
#endif

typedef Index *IndexH;
typedef SpatialIndex::IData *IndexItemH;
typedef Tools::PropertySet *IndexPropertyH;

#ifndef SIDX_C_DLL
#if defined(_MSC_VER)
#  define SIDX_C_DLL     __declspec(dllexport)
#else
#  if defined(USE_GCC_VISIBILITY_FLAG)
#    define SIDX_C_DLL     __attribute__ ((visibility("default")))
#  else
#    define SIDX_C_DLL
#  endif
#endif
#endif


#endif
