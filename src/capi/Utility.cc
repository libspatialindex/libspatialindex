/******************************************************************************
 * $Id: util.cc 1361 2009-08-02 17:53:31Z hobu $
 *
 * Project:	 libsidx - A C API wrapper around libspatialindex
 * Purpose:	 C++ objects to implement utilities.
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

Tools::PropertySet* GetDefaults()
{
	Tools::PropertySet* ps = new Tools::PropertySet;
	
	Tools::Variant var;
	
	// Rtree defaults
	
	var.m_varType = Tools::VT_DOUBLE;
	var.m_val.dblVal = 0.7;
	ps->setProperty("FillFactor", var);
	
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = 100;
	ps->setProperty("IndexCapacity", var);
	
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = 100;
	ps->setProperty("LeafCapacity", var);
	
	var.m_varType = Tools::VT_LONG;
	var.m_val.lVal = SpatialIndex::RTree::RV_RSTAR;
	ps->setProperty("TreeVariant", var);

	// var.m_varType = Tools::VT_LONGLONG;
	// var.m_val.llVal = 0;
	// ps->setProperty("IndexIdentifier", var);
	
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = 32;
	ps->setProperty("NearMinimumOverlapFactor", var);
	
	var.m_varType = Tools::VT_DOUBLE;
	var.m_val.dblVal = 0.4;
	ps->setProperty("SplitDistributionFactor", var);

	var.m_varType = Tools::VT_DOUBLE;
	var.m_val.dblVal = 0.3;
	ps->setProperty("ReinsertFactor", var);

	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = 2;
	ps->setProperty("Dimension", var);
		
	var.m_varType = Tools::VT_BOOL;
	var.m_val.bVal = true;
	ps->setProperty("EnsureTightMBRs", var);
	
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = 100;
	ps->setProperty("IndexPoolCapacity", var);
	
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = 100;
	ps->setProperty("LeafPoolCapacity", var);

	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = 1000;
	ps->setProperty("RegionPoolCapacity", var);

	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = 500;
	ps->setProperty("PointPoolCapacity", var);

	// horizon for TPRTree
	var.m_varType = Tools::VT_DOUBLE;
	var.m_val.dblVal = 20.0;
	ps->setProperty("Horizon", var);
	
	// Buffering defaults
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = 10;
	ps->setProperty("Capacity", var);
	
	var.m_varType = Tools::VT_BOOL;
	var.m_val.bVal = false;
	ps->setProperty("WriteThrough", var);
	
	// Disk Storage Manager defaults
	var.m_varType = Tools::VT_BOOL;
	var.m_val.bVal = true;
	ps->setProperty("Overwrite", var);
	
	var.m_varType = Tools::VT_PCHAR;
	var.m_val.pcVal = const_cast<char*>("");
	ps->setProperty("FileName", var);
	
	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = 4096;
	ps->setProperty("PageSize", var);
	
	// Our custom properties related to whether 
	// or not we are using a disk or memory storage manager

	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = RT_Disk;
	ps->setProperty("IndexStorageType", var);

	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = RT_RTree;
	ps->setProperty("IndexType", var);

	var.m_varType = Tools::VT_PCHAR;
	var.m_val.pcVal = const_cast<char*>("dat");
	ps->setProperty("FileNameDat", var);

	var.m_varType = Tools::VT_PCHAR;
	var.m_val.pcVal = const_cast<char*>("idx");
	ps->setProperty("FileNameIdx", var);

    // Custom storage manager properties
    var.m_varType = Tools::VT_ULONG;
	var.m_val.pcVal = 0;
	ps->setProperty("CustomStorageCallbacksSize", var);

    var.m_varType = Tools::VT_PVOID;
	var.m_val.pcVal = 0;
	ps->setProperty("CustomStorageCallbacks", var);

    return ps;
}
