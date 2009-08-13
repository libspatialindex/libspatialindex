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

#pragma once

namespace SpatialIndex
{
	namespace MVRTree
	{
		class MVRTree;
		class Leaf;
		class Index;
		class Node;

		typedef Tools::PoolPointer<Node> NodePtr;

		class Node : public SpatialIndex::INode
		{
		public:
			virtual ~Node();

			//
			// Tools::IObject interface
			//
			virtual IObject* clone();

			//
			// Tools::ISerializable interface
			//
			virtual uint32_t getByteArraySize();
			virtual void loadFromByteArray(const byte* data);
			virtual void storeToByteArray(byte** data, uint32_t& len);

			//
			// SpatialIndex::IEntry interface
			//
			virtual id_type getIdentifier() const;
			virtual void getShape(IShape** out) const;

			//
			// SpatialIndex::INode interface
			//
			virtual uint32_t getChildrenCount() const;
			virtual id_type getChildIdentifier(uint32_t index)  const;
			virtual void getChildShape(uint32_t index, IShape** out)  const;
                        virtual void getChildData(uint32_t index, uint32_t& length, byte** data) const;
			virtual uint32_t getLevel() const;
			virtual bool isIndex() const;
			virtual bool isLeaf() const;

		private:
			Node();
			Node(MVRTree* pTree, id_type id, uint32_t level, uint32_t capacity);

			virtual Node& operator=(const Node&);

			virtual void insertEntry(uint32_t dataLength, byte* pData, TimeRegion& mbr, id_type id);
			virtual bool deleteEntry(uint32_t index);

			virtual bool insertData(
				uint32_t dataLength, byte* pData, TimeRegion& mbr, id_type id, std::stack<id_type>& pathBuffer,
				TimeRegion& mbr2, id_type id2, bool bInsertMbr2 = false, bool forceAdjust = false);
			virtual void insertData(TimeRegion& mbr1, id_type id1, TimeRegion& mbr2, id_type id2, Node* oldVersion, std::stack<id_type>& pathBuffer);
			virtual bool deleteData(id_type id, double delTime, std::stack<id_type>& pathBuffer, bool adjustMBR = false);

			virtual void rtreeSplit(
				uint32_t dataLength, byte* pData, TimeRegion& mbr, id_type id, std::vector<uint32_t>& group1, std::vector<uint32_t>& group2,
				TimeRegion& mbr2, id_type id2, bool bInsertMbr2 = false);
			virtual void rstarSplit(
				uint32_t dataLength, byte* pData, TimeRegion& mbr, id_type id, std::vector<uint32_t>& group1, std::vector<uint32_t>& group2,
				TimeRegion& mbr2, id_type id2, bool bInsertMbr2 = false);

			virtual void pickSeeds(uint32_t& index1, uint32_t& index2, uint32_t total);

			virtual NodePtr chooseSubtree(const TimeRegion& mbr, uint32_t level, std::stack<id_type>& pathBuffer) = 0;
			virtual NodePtr findLeaf(const TimeRegion& mbr, id_type id, std::stack<id_type>& pathBuffer) = 0;
			virtual NodePtr findNode(const TimeRegion& mbr, id_type id, std::stack<id_type>& pathBuffer);

			virtual void split(
				uint32_t dataLength, byte* pData, TimeRegion& mbr, id_type id, NodePtr& left, NodePtr& right,
				TimeRegion& mbr2, id_type id2, bool bInsertMbr2 = false) = 0;

			MVRTree* m_pTree;
				// Parent of all nodes.

			uint32_t m_level;
				// The level of the node in the tree.
				// Leaves are always at level 0.

			id_type m_identifier;
				// The unique ID of this node.

			uint32_t m_children;
				// The number of children pointed by this node.

			uint32_t m_capacity;
				// Specifies the node capacity.

			TimeRegion m_nodeMBR;
				// The minimum bounding region enclosing all data contained in the node.

			byte** m_pData;
				// The data stored in the node.

			TimeRegionPtr* m_ptrMBR;
				// The corresponding data MBRs.

			id_type* m_pIdentifier;
				// The corresponding data identifiers.

			uint32_t* m_pDataLength;

			uint32_t m_totalDataLength;

			class RstarSplitEntry
			{
			public:
				RstarSplitEntry(TimeRegion* pr, uint32_t index, uint32_t dimension) :
					m_pRegion(pr), m_index(index), m_sortDim(dimension) {}

				static int compareLow(const void* pv1, const void* pv2)
				{
					RstarSplitEntry* pe1 = * (RstarSplitEntry**) pv1;
					RstarSplitEntry* pe2 = * (RstarSplitEntry**) pv2;

					if (pe1->m_pRegion->m_pLow[pe1->m_sortDim] < pe2->m_pRegion->m_pLow[pe2->m_sortDim]) return -1;
					if (pe1->m_pRegion->m_pLow[pe1->m_sortDim] > pe2->m_pRegion->m_pLow[pe2->m_sortDim]) return 1;
					return 0;
				}

				static int compareHigh(const void* pv1, const void* pv2)
				{
					RstarSplitEntry* pe1 = * (RstarSplitEntry**) pv1;
					RstarSplitEntry* pe2 = * (RstarSplitEntry**) pv2;

					if (pe1->m_pRegion->m_pHigh[pe1->m_sortDim] < pe2->m_pRegion->m_pHigh[pe2->m_sortDim]) return -1;
					if (pe1->m_pRegion->m_pHigh[pe1->m_sortDim] > pe2->m_pRegion->m_pHigh[pe2->m_sortDim]) return 1;
					return 0;
				}

				TimeRegion* m_pRegion;
				uint32_t m_index;
				uint32_t m_sortDim;
			}; // RstarSplitEntry

			class DeleteDataEntry
			{
			public:
				DeleteDataEntry(uint32_t index, double d) : m_index(index), m_increase(d) {}

				static bool compare(DeleteDataEntry e1, DeleteDataEntry e2) { return e1.m_increase < e2.m_increase; }

				uint32_t m_index;
				double m_increase;
			}; // DeleteDataEntry

			// Needed to access protected members without having to cast from Node.
			// It is more efficient than using member functions to access protected members.
			friend class MVRTree;
			friend class Leaf;
			friend class Index;
			friend class Tools::PointerPool<Node>;
		}; // Node
	}
}

