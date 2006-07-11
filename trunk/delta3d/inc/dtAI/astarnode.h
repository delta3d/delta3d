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
 * @author Bradley Anderegg 06/28/2006
 */

#ifndef __DELTA_ASTARNODE_H__
#define __DELTA_ASTARNODE_H__


namespace dtAI
{
   /**
    * AStarNode is a data container for a single node in an AStar Path
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
         AStarNode(node_type* pParent, data_type pData, cost_type pGn, cost_type pHn): mData(pData), mCostToNode(pGn), mCostToGoal(pHn), mParent(pParent){}      
         virtual ~AStarNode(){}
   
         AStarNode(const AStarNode& pNode)
         {
            mData = pNode.GetData();
         };
         
         AStarNode& operator=(const AStarNode& pNode)
         {
            mData = pNode.GetData(); return *this;
         };

         bool operator==(const AStarNode& pNode) const
         {
            return mData == pNode.GetData();
         }
         bool operator!=(const AStarNode& pNode)
         {
            return mData != pNode.GetData();
         }

         bool operator<(const AStarNode& pType) const
         {
            return (mCostToNode + mCostToGoal) < (pType.GetCostToNode() + pType.GetCostToGoal());
         }

         node_type* GetParent(){return mParent;}

         //g(n)
         cost_type GetCostToNode() const{return mCostToNode;}

         //h(n)
         cost_type GetCostToGoal() const{return mCostToGoal;}

         data_type GetData(){return mData;}
         const data_type GetData() const{return mData;}

         virtual iterator begin() const = 0;
         virtual iterator end() const = 0;


      protected:
         data_type mData;

         cost_type mCostToNode;
         cost_type mCostToGoal;

         node_type* mParent;     
   };

}//namespace dtAI

#endif // __DELTA_ASTARNODE_H__
