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

#ifndef __DELTA_ASTAR_H__
#define __DELTA_ASTAR_H__

#include <dtAI/astarconfig.h>

#include <cassert>
#include <algorithm>
#include <list>

namespace dtAI
{
   /**
    * AStar is a generic implementation of the standard A* algorithm
    */
   template<class _NodeType, class _CostFunc, class _Container, class _Timer>
   class AStar
   {
   public:
      enum AStarResult{ NO_PATH = 0, PATH_FOUND, PARTIAL_PATH};

      typedef _NodeType node_type;
      typedef typename _NodeType::cost_type cost_type;
      typedef typename _NodeType::data_type data_type;
      typedef std::list<node_type*> AStarContainer;
      typedef typename AStarContainer::iterator AStarIterator;      
      typedef _CostFunc cost_function;
      typedef _Container container_type;
      typedef AStarConfig<data_type, cost_type, container_type> config_type;
      typedef AStar<node_type, cost_function, container_type, _Timer> MyType;

      public:
         AStar();
         AStar(config_type pConfig);
         virtual ~AStar();


         void Reset(config_type pConfig);

         void Reset(data_type pFrom, data_type pTo);

         AStarResult FindPath();

         container_type& GetPath(){return mConfig.mResult;}

         const config_type& GetConfig() const{return mConfig;}         
         config_type& GetConfig(){return mConfig;}         
         

      protected:
         AStar(const AStar&); //not implemented by design
         AStar& operator=(const AStar&); //not implemented by design
         void FreeMem();
      
         void AddNodeLink(node_type* pParent, data_type pData);
         bool Contains(const AStarContainer& pCont, data_type pNode);
         node_type* Remove(AStarContainer& pCont, data_type pNode);
         node_type* FindLowestCost(const AStarContainer& pCont);


         config_type mConfig;
         AStarContainer mOpen;
         AStarContainer mClosed;
         cost_function mCostFunc;
         _Timer mTimer;
   };

#include "astar.inl"

}//namespace dtAI

#endif // __DELTA_ASTAR_H__
