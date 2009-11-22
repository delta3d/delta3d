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
   class AIDebugDrawable;
   class WaypointGraphBuilder;

   // using pimpl pattern, this is forward declared and
   // implemented in the .cpp
   struct WaypointGraphImpl;

   class DT_AI_EXPORT WaypointGraph: public osg::Referenced
   {
   public: // datatypes
      typedef std::vector<dtAI::WaypointInterface*> WaypointArray;
      typedef std::vector<const dtAI::WaypointInterface*> ConstWaypointArray;
      typedef std::vector<dtCore::RefPtr<dtAI::WaypointCollection> > WaypointCollectionArray;
      typedef std::vector<const dtAI::WaypointCollection*> ConstWaypointCollectionArray;

      class DT_AI_EXPORT SearchLevel: public osg::Referenced
      {
      public:
         SearchLevel();

      protected:
         virtual ~SearchLevel();

      public:
         unsigned mLevelNum;
         ConstWaypointArray mNodes;
         dtCore::RefPtr<NavMesh> mNavMesh;
      };

      typedef std::vector< dtCore::RefPtr<SearchLevel> > SearchLevelArray;

   public:
      WaypointGraph();

   protected:
      virtual ~WaypointGraph();

   public:
      /**
       *  You must run Create() after you make changes to the underlying structure.
       *     This function will recreate the whole tree.
       */
      void CreateSearchGraph(WaypointGraphBuilder* builder, unsigned maxLevels);

      /**
       *  Creates an individual level of the tree, SearchLevel level-1 must exist.
       *     level 0 is by default the concrete waypoints.
       */
      bool CreateSearchLevel(WaypointGraphBuilder* builder, unsigned level);

      /**
      *  Use this function to generate the higher level edges from the lower level edges.	
      *  This is only needed if you created your own WaypointCollections and Assigned them manually.
      */
      void CreateAbstractEdges();

      /**
      *  Use this to only refresh a single search level.	
      *  Level must be from 1 to NumSearchLevels - 1.
      */
      void CreateAbstractEdgesAtLevel(unsigned level);

      /**
       * Inserting waypoints into the tree structure.
       * To move a waypoint simply re-add it, this can be expensive
       * so if you have a large tree do not do this every frame.
       */
      void InsertWaypoint(WaypointInterface* waypoint);

      /**
       *  Takes the WaypointID of a waypoint already added, and assigns it
       *     to a parent waypoint which may or may not be already inserted.
       * 
       *   Unless you know specifically which level to insert the WaypointCollection you
       *     must use this function to insert it.  It will auto insert at the childWp level + 1.
       *
       *  @return returns false if childWP was not found, or if parentWp has already been added
       *             at the wrong search level
       *
       *  @note Calling this function will add the childWp to the parentWp using Insert().
       */
      bool Assign(WaypointID childWp, WaypointCollection* parentWp);

      /**
       * This is for inserting a WaypointCollection at a specific search level,
       *  this is done on CreateSearchGraph(), use InsertWaypoint()
       *  for default inserting of WaypointCollections.
       */
      void InsertCollection(WaypointCollection* waypoint, unsigned level);

      /**
       * This removes the waypoint from the internal tree structure,
       * then collapses all empty parents.  The memory for the waypoint
       * is not deleted, it is assumed to be managed elsewhere. If you
       * added the waypoint by new-ing one, simply delete the ptr.
       */
      void RemoveWaypoint(WaypointID id);

      /**
       * @return true if the waypoint is found in the WaypointGraph,
       *          a waypoint can be inserted on InsertWaypoint() or AddEdge().
       */
      bool Contains(WaypointID id) const;

      /**
       * @return a ptr to the waypoint stored internal to the graph for a given id.
       */
      const WaypointInterface* FindWaypoint(WaypointID id) const;

      /**
       * This gets the immediate parent collection of the waypoint,
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
       * This gets the immediate parent collection of the waypoint,
       * if the WaypointID resolves to be a WaypointCollection, it will
       * return that.
       *
       * @return Either the WaypointCollection matching the id or the base waypoint's
       *         immediate parent, if Contains(id) returns false this will return NULL.
       */
      const WaypointCollection* FindCollection(WaypointID id) const;

      /**
       * Returns the waypoint's parent WaypointCollection, id may be a WaypointCollection.
       * @return Waypoint's parent or NULL if waypoint or parent not found
       */
      WaypointCollection* GetParent(WaypointID id);
      const WaypointCollection* GetParent(WaypointID id) const;

      /**
       * This finds the waypoint and traverss up the tree to find the top most node
       * containing this as a child.
       */
      WaypointCollection* GetRootParent(WaypointID id);
      const WaypointCollection* GetRootParent(WaypointID id) const;

      // adding and removing path segments
      void AddEdge(WaypointID idFrom, WaypointID idTo);
      bool RemoveEdge(WaypointID idFrom, WaypointID idTo);
      void RemoveAllEdgesFromWaypoint(WaypointID idFrom);
      void GetAllEdgesFromWaypoint(WaypointID idFrom, ConstWaypointArray& result) const;

      /**
       * This does a hierarchical search for a common parent, returns true if one exists.
       */
      bool HasPath(WaypointID idFrom, WaypointID idTo) const;

      /**
       * Searches for a common parent, which represents the top of the search tree
       * to path between these points.
       */
      const WaypointCollection* FindCommonParent(WaypointID lhs, WaypointID rhs) const;
      const WaypointCollection* FindCommonParent(const WaypointCollection& lhs, const WaypointCollection& rhs) const;

      /**
       * Traverses up the tree, adding each parent to the end of the array until it reaches
       *    the specified node.
       *  @return true if parentNode was found as a parent to childNode
       */
      bool GetNodePath(const WaypointInterface* childNode, const WaypointCollection* parentNode, ConstWaypointCollectionArray& result);
      bool GetNodePath(const WaypointCollection& childNode, const WaypointCollection& parentNode, ConstWaypointCollectionArray& result);

      // path finding
      NavMesh* GetNavMeshAtSearchLevel(unsigned level);
      const NavMesh* GetNavMeshAtSearchLevel(unsigned level) const;

      unsigned GetNumSearchLevels() const;
      SearchLevel* GetSearchLevel(unsigned levelNum);
      SearchLevel* GetOrCreateSearchLevel(unsigned levelNum);
      const SearchLevel* GetSearchLevel(unsigned levelNum) const;

      // returns -1 if not found
      int GetSearchLevelNum(WaypointID id) const;
      //WaypointCollection* MapNodeToLevel(WaypointInterface* wp, unsigned levelNum);

      //void AddSearchLevel(dtCore::RefPtr<SearchLevel> newLevel);

      // clears all memory, does a lot of deleting!
      void Clear();

      /**
       * These are here just for debugging purposes
       */
      // \todo
      //bool ValidateTree();
      //bool ValidateMemory();
      //bool ValidateLevels();
      AIDebugDrawable* GetDrawableAtSearchLevel(const WaypointCollection* root, unsigned level) const;

   protected:
      // override for derived cleanup
      virtual void OnClear();

      virtual bool HasPath_Protected(const WaypointCollection* pFrom, const WaypointCollection* pTo) const;
      virtual void AddEdge_Protected(const WaypointInterface* pFrom, const WaypointInterface* pTo);
      virtual void RemoveWaypoint_Protected(const WaypointInterface* waypoint);
      virtual bool RemoveEdge_Protected(const WaypointInterface* pFrom, const WaypointInterface* pTo);
      virtual void RemoveAllEdgesFromWaypoint_Protected(const WaypointInterface* pFrom);
      virtual void GetAllEdgesFromWaypoint_Protected(const WaypointInterface& pFrom, ConstWaypointArray& result) const;

   private:
      WaypointGraphImpl* mImpl;
   };

} // namespace dtAI

#endif // DELTA_WAYPOINTGRAPH
