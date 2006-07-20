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
 * @author Bradley Anderegg
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
    * A generic implementation of the standard A* algorithm
    * 
    * @brief This class can be used as a generic graph searching tool,
    *        to do this you will need several template parameters an 
    *        example can be found in AStarWaypointUtils.h, but I will break
    *        down the basics below.
    *
    *        NodeType:  This class is used as the nodes in the graph we are
    *                   searching, it should be derived from AStarNode and most
    *                   importantly implement begin() and end() which will support
    *                   our graph searching mechanism.
    *
    *        CostFunc:  This class is used to calculate the cost between two nodes
    *                   it should be derived from AStarCostFunc and support the operator()
    *                   which takes two DataTypes (being the DataType encapsulated by our Node.
    *
    *        Container: This can be any standard container templated to take type DataType that
    *                   supports push_front.  A todo is to change it to take push_back.
    *
    *        Timer: This class is used for statistics info and must support Update() and GetDT()
    *               GetDT() should give the elapsed time between two Updates().  Be aware the the 
    *               granularity of time is only relevant to the user and should match the AStarConfig's
    *               MaxTime.
    *
    * @usage To find a path between two points you can call Reset() with the two points
    *        and then FindPath().  Alternatively, you can set a config type which contains
    *        the path points and holds statistical info as well as pathing constraints.  If you
    *        do not pass it a config, a default one will be used.  To edit the default config just 
    *        call GetConfig() which passes a config by reference.  If you want to set new path points
    *        don't call Reset() on the config, call Reset() on AStar, cause this will clear the internal
    *        bookeeping members on AStar.  The main means for editing the config is to set constraints,
    *        the const GetConfig() can be used for getting the statistics from the last path creation.
    *        The constraints can be used for finding partial paths.  When a constraint is set on a config
    *        the next call to FindPath() will use the constraints and return PARTIAL_PATH if the path could
    *        not be completed, returning the best path so far.  The next call to FindPath() will find the 
    *        next segment from the last point in the partial path to the goal and continue to work within 
    *        the constraints.  When PATH_FOUND is returned from FindPath() a path to the goal has been reached
    *        else NO_PATH will indicate there is no possible path between the two points.  To obtain the path
    *        or partial path, call GetPath() or copy it off of the config from GetConfig.
    *
    *
    * @reference Thanks to Amit and his great webpage on AStar Algorithms
    *            http://theory.stanford.edu/~amitp/GameProgramming/
    *
    * @author Bradley Anderegg
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
         /**
         * Default constructor creates a default config file
         * with no constraints, and no path to goto
         */
         AStar();

         /**
         * Takes a config as an arg and copies it, use this constructor
         * to setup constraints or configured with two path points to path between
         */
         AStar(const config_type& pConfig);
         virtual ~AStar();

         /**
         *  Resets the internal memory and copies the config class
         *  for use on the next call to FindPath()
         *
         * @param takes an AStarConfig class 
         */
         void Reset(const config_type& pConfig);

         /**
         *  Resets the internal memory and keeps the same config params
         *
         * @param the point to find a path from
         * @param the point to find a path to
         */
         void Reset(data_type pFrom, data_type pTo);

         /**
         *  Runs the AStar algorithm, using the either the current config or the
         *  the default config settings if none have been set
         *  can check for return of false if no path is found         
         *
         * @return the result of this call to find path, NO_PATH, PATH_FOUND, or PARTIAL_PATH,
         */
         AStarResult FindPath();

         /**
         * Call this after calling FindPath() to get
         * @return the result of the last call to FindPath()
         */
         container_type& GetPath(){return mConfig.mResult;}

         /**
         * Use this to get the config data to change constraints or get the
         * statistics from the last run of GetPath(), also holds a container
         * of the last found path
         */
         const config_type& GetConfig() const{return mConfig;}         
         config_type& GetConfig(){return mConfig;}         
         

      protected:
         AStar(const AStar&); //not implemented by design
         AStar& operator=(const AStar&); //not implemented by design
         void FreeMem();
      
         /**
         * Internal helper functions, pulled out of main loop to be optimized
         * at a later date
         */
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
