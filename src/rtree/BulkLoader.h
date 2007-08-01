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

#ifndef __spatialindex_rtree_bulk_loader_h
#define __spatialindex_rtree_bulk_loader_h

namespace SpatialIndex
{
	namespace RTree
	{
		class BulkLoadSource : public Tools::IObjectStream
		{
		public:
			BulkLoadSource(
				Tools::SmartPointer<IObjectStream> spSstream,
				size_t howMany);
			BulkLoadSource(IObjectStream* pStream, size_t howMany);
			BulkLoadSource(IObjectStream* pStream);
			virtual ~BulkLoadSource();

			virtual Tools::IObject* getNext();
			virtual bool hasNext() throw ();
			virtual size_t size() throw (Tools::NotSupportedException);
			virtual void rewind() throw (Tools::NotSupportedException);

			Tools::SmartPointer<IObjectStream> m_spDataSource;
			size_t m_cHowMany;
		}; // BulkLoadSource

		class BulkLoadComparator : public Tools::IObjectComparator
		{
		public:
			BulkLoadComparator(size_t d);
			virtual ~BulkLoadComparator();

			virtual int compare(Tools::IObject* o1, Tools::IObject* o2);

			size_t m_compareDimension;
		};

		class BulkLoader
		{
		public:
			void bulkLoadUsingSTR(
				RTree* pTree,
				IDataStream& stream,
				size_t bindex,
				size_t bleaf,
				size_t bufferSize);

		protected:
			class TmpFile : public IDataStream
			{
			public:
				TmpFile();
				virtual ~TmpFile();

				void storeRecord(Region& r, id_type id);
				void loadRecord(Region& r, id_type& id);

				virtual IData* getNext();
				virtual bool hasNext() throw ();
				virtual size_t size()
					throw (Tools::NotSupportedException);
				virtual void rewind();

				Tools::TemporaryFile m_tmpFile;
				IData* m_pNext;
			};

			void createLevel(
				RTree* pTree,
				Tools::IObjectStream& es,
				size_t dimension,
				size_t k,
				size_t b,
				size_t level,
				size_t bufferSize,
				TmpFile& tmpFile,
				size_t& numberOfNodes,
				size_t& totalData);

			Node* createNode(
				RTree* pTree,
				std::vector<Tools::SmartPointer<IData> >& e,
				size_t level);

			friend class BulkLoadSource;
		};
	}
}

#endif /* __spatialindex_rtree_bulk_loader_h */

