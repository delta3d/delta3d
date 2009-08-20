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
 * Bradley Anderegg 
 */

#ifndef DELTA_WAYPOINTGRAPH
#define DELTA_WAYPOINTGRAPH

#include <dtAI/export.h>
#include <dtAI/waypointinterface.h>

#include <osg/Referenced>
#include <osg/Vec3>
#include <osg/Vec2>

#include <vector>

namespace dtAI 
{
   class NavMesh;
   class AIPluginInterface;
   class WaypointCollection;

   //using pimpl pattern, this is forward declared and
   //implemented in the .cpp
   struct WaypointGraphImpl;

   class DT_AI_EXPORT WaypointGraph: public osg::Referenced
   {
   public: //datatypes


      class WaypointGraphBuilderInterface
      {
         public:
            WaypointGraphBuilderInterface(){};
            virtual ~WaypointGraphBuilderInterface(){}

            //creating the graph
            virtual unsigned GetMaxChildrenPerNode() const = 0;
            virtual WaypointCollection* SelectBestCollection(const WaypointInterface& wp, WaypointGraph& wg) const = 0;
            
            //potentially we may need to break up a single leaf node into two pieces
            //we simply allow the user to overload the default heuristic
            typedef std::pair<WaypointCollection*, WaypointCollection*> WaypointCollectionPair;

            virtual WaypointCollectionPair Split(WaypointCollection* wc) const = 0;
      };

      typedef std::vector<dtAI::WaypointInterface*> WaypointArray;
      typedef std::vector<const dtAI::WaypointInterface*> ConstWaypointArray;

   public:
      WaypointGraph();

   protected:
      virtual ~WaypointGraph();

   public:

      /**
       *	Creating a graph with a NavMesh will add all waypoints into the
       * as a flat tree and then group waypoints into separate collections
       * recursively until a tree is built.
       *
       * @todo- allow user to specify a way to build their own graph
       */
      void CreateGraph(NavMesh& waypointData, WaypointGraphBuilderInterface* wb);//, const WaypointGraphBuilderInterface& builder);      

      /**
       *	Loading a WaypointGraph from file, currently unimplemented.
       */
      //bool LoadWaypointFile(const std::string& filename);
      
      /**
      *	Saving a WaypointGraph to file, currently unimplemented.
      */
      //bool SaveWaypointFile(const std::string& filename);

      /**
      * Inserting waypoints into the tree structure.
      * To move a waypoint simply re-add it, this can be expensive
      * if you have a large tree do not do this every frame.
      */
      void InsertWaypoint(WaypointInterface* waypoint);

      /**
       *	This removes the waypoint from the internal tree structure,
       * then collapses all empty parents.  The memory for the waypoint
       * is not deleted, it is assumed to be managed elsewhere. If you
       * added the waypoint by new-ing one, simply delete the ptr if this
       * returns true.
       *
       * @return Whether the waypoint was found and a remove was successful
       *
       * @todo- should this remove a collection from the tree, or should that
       *         be an error, and require the use of a different function like RemoveCollection()?
       */
      bool RemoveWaypoint(WaypointInterface* waypoint);

      /**
       *	@return true if the waypoint is found in the WaypointGraph,
       *          a waypoint can be inserted on InsertWaypoint() or AddEdge().
       */
      bool Contains(WaypointID id) const;

      /**
       *	@return a ptr to the waypoint stored internal to the graph for a given id.
       */
      const WaypointInterface* FindWaypoint(WaypointID id) const;

      /**
       *	This gets the immediate parent collection of the waypoint,
       * if the WaypointID resolves to be a WaypointCollection, it will
       * return that.  This (non-const) version is forced to const cast 
       * to keep the actual waypoints const in the tree while allowing
       * the interal nodes to be edited.
       *
       * @return Either the WaypointCollection matching the id or the base waypoint's
       *          immediate parent, if Contains(id) returns false this will return NULL.
       */
      WaypointCollection* FindCollection(WaypointID id);


      /**
      *	This gets the immediate parent collection of the waypoint,
      * if the WaypointID resolves to be a WaypointCollection, it will
      * return that.  
      *
      * @return Either the WaypointCollection matching the id or the base waypoint's
      *          immediate parent, if Contains(id) returns false this will return NULL.
      */
      const WaypointCollection* FindCollection(WaypointID id) const;

      /**
       *	This finds the waypoint and traverss up the tree to find the top most node
       * containing this as a child.
       */
      WaypointCollection* GetRootParent(WaypointID id);
      const WaypointCollection* GetRootParent(WaypointID id) const;

      //adding and removing path segments
      void AddEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo);
      bool RemoveEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo);
      void RemoveAllEdgesFromWaypoint(const WaypointInterface* pFrom);
      void GetAllEdgesFromWaypoint(const WaypointInterface& pFrom, ConstWaypointArray& result) const;

      //searching and intersecting waypoints
      bool HasPath(const WaypointInterface& rhs, const WaypointInterface& lhs) const;
      //WaypointInterface* FindClosest(const osg::Vec3& point3d) const;
      //void FindRadial(const osg::Vec3& pos, float radius, WaypointArray& vectorIn) const;

      //path finding
      //int GetNumLevels() const;
      //void SetCurrentSearchLevel(int l);
      //int GetCurrentSearchLevel() const;
      //int GetWaypointLevel(WaypointInterface& id);
      //WaypointID MapWaypointToLevel(int atLevel, WaypointInterface& point, int outLevel) const;
      //bool FindPath(WaypointInterface& from, WaypointInterface& to, WaypointArray& result) const;

      //clears all memory, does a lot of deleting!
      void Clear();

      /**
      *	These are here just for debugging purposes
      */
      //\todo
      //bool ValidateTree();
      //bool ValidateMemory();

   protected:
      //override for derived cleanup
      virtual void OnClear();


   private:     

      WaypointGraphImpl* mImpl;
   };

}//namespace 

#endif // DELTA_WAYPOINTGRAPH
