/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * Bradley Anderegg 06/28/2006
 */

#ifndef __DELTA_ASTARNODE_H__
#define __DELTA_ASTARNODE_H__

#include <dtUtil/mathdefines.h>

namespace dtAI
{
   /**
    * AStarNode is a data container for a single node in an AStar Path
    *
    *
    * @brief This class is the base node type for AStar, it holds the cost of the path
    *        the actual data type, and a pointer to its parent for bookkeeping.
    *
    * @usage Derive this class and implement the pure virtual begin() and end() functions
    *        which should return iterators of type _IterType and support operator++ to iterate
    *        through the data types that this data type can get to.
    *
    * @see AStar.h
    */
   template<class _NodeType, class _DataType, class _IterType, class _CostType>
   class AStarNode
   {
      public:
         typedef _DataType data_type;
         typedef _CostType cost_type;
         typedef _IterType iterator;
         typedef _NodeType node_type;
         typedef AStarNode<_NodeType, _DataType, _IterType, _CostType> BaseType;

      public:
         AStarNode(node_type* pParent, data_type pData, cost_type pGn, cost_type pHn)
            : mData(pData)
            , mCostToNode(pGn)
            , mCostToGoal(pHn)
            , mParent(pParent)
            , mDepth(0)
         {
            if (pParent)
            {
               mDepth = pParent->GetDepth() + 1;
            }
         }
         virtual ~AStarNode(){}

         /**
          * equality is based on the class data_type's operator ==
          */
         bool operator==(const AStarNode& pNode) const
         {
            return mData == pNode.GetData();
         }

         bool operator!=(const AStarNode& pNode)
         {
            return mData != pNode.GetData();
         }

         /**
          * we use operator < to compare costs between nodes
          */
         bool operator<(const AStarNode& pType) const
         {
            cost_type myCost = mCostToNode + mCostToGoal;
            cost_type otherCost = pType.GetCostToNode() + pType.GetCostToGoal();
            // check if they are equivalent first
            if (dtUtil::Equivalent(myCost, otherCost)) { return false; }
            else { return myCost < otherCost; }
         }

         node_type* GetParent() { return mParent; }
         const node_type* GetParent() const { return mParent; }

         /**
          * This is the cost to get to the node from the start
          * in common AStar terms it is called cost g(n)
          */
         cost_type GetCostToNode() const { return mCostToNode; }

         /**
          * This is the cost to get from the node to the end
          * in common AStar terms it is called cost h(n)
          */
         cost_type GetCostToGoal() const { return mCostToGoal; }

         data_type& GetData() { return mData; }
         const data_type& GetData() const { return mData; }

         /**
          * returns an iterator to the first acceptable point from this node
          */
         virtual iterator begin() const = 0;

         /**
          * returns an iterator one past the last acceptable point from this node
          */
         virtual iterator end() const = 0;

         unsigned int GetDepth() const { return mDepth; }

      protected:
         AStarNode(const AStarNode& pNode); // not implemented by design
         AStarNode& operator=(const AStarNode& pNode); // not implemented by design

         data_type mData;

         cost_type mCostToNode;
         cost_type mCostToGoal;

         node_type* mParent;

         unsigned int mDepth;
   };

} // namespace dtAI

#endif // __DELTA_ASTARNODE_H__
