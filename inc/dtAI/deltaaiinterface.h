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

#ifndef DELTA_AI_INTERFACE
#define DELTA_AI_INTERFACE

#include <dtAI/aiplugininterface.h>
#include <dtAI/export.h>
#include <dtAI/waypointgraphastar.h>
#include <dtUtil/kdtree.h>

namespace dtAI
{
   //////////////////////////////////////////////////////////////////////////////////
   // This is the default AI plugin interface implementation
   //////////////////////////////////////////////////////////////////////////////////////////
   struct KDHolder
   {
      typedef float value_type;

      KDHolder(const osg::Vec3& pos)
      {
         d[0] = pos[0];
         d[1] = pos[1];
         d[2] = pos[2];
      }

      KDHolder(const osg::Vec3& pos, WaypointID id)
      {
         d[0] = pos[0];
         d[1] = pos[1];
         d[2] = pos[2];

         mID = id;
      }

      KDHolder(value_type a, value_type b, value_type c)
      {
         d[0] = a;
         d[1] = b;
         d[2] = c;
      }

      KDHolder(const KDHolder& x)
      {
         d[0] = x.d[0];
         d[1] = x.d[1];
         d[2] = x.d[2];
         mID = x.mID;
      }

      operator osg::Vec3()
      {
         return osg::Vec3(d[0], d[1], d[2]);
      }

      inline value_type operator[](size_t const N) const { return d[N]; }

      WaypointID mID;
      value_type d[3];
   };

   inline float KDHolderIndexFunc(KDHolder t, size_t k ) { return t[k]; }

   typedef std::pointer_to_binary_function<KDHolder, size_t, float> tree_search_func;
   typedef dtUtil::KDTree<3, KDHolder, tree_search_func> WaypointKDTree;
   typedef std::pair<WaypointKDTree::const_iterator, float> find_result;

   class DT_AI_EXPORT DeltaAIInterface: public AIPluginInterface
   {
   public:

      DeltaAIInterface();

      void InsertWaypoint(WaypointInterface* waypoint);
      void InsertCollection(WaypointCollection* waypoint, unsigned level);
      WaypointGraph& GetWaypointGraph();
      const WaypointGraph& GetWaypointGraph() const;
      bool Assign(WaypointID childWp, WaypointCollection* parentWp);
      bool MoveWaypoint(WaypointInterface* wi, const osg::Vec3& newPos);
      bool RemoveWaypoint(WaypointInterface* wi);
      WaypointInterface* GetWaypointById(WaypointID id);
      const WaypointInterface* GetWaypointById(WaypointID id) const;
      WaypointInterface* GetWaypointByName(const std::string& name);
      void GetWaypointsByName(const std::string& name, WaypointArray& arrayToFill);
      bool HasEdge(WaypointID from, WaypointID to);
      void AddEdge(WaypointID pFrom, WaypointID pTo);
      bool RemoveEdge(WaypointID pFrom, WaypointID pTo);
      void RemoveAllEdges(WaypointID pFrom);
      void GetAllEdgesFromWaypoint(WaypointID pFrom, ConstWaypointArray& result) const;
      PathFindResult FindPath(WaypointID pFrom, WaypointID pTo, ConstWaypointArray& result);
      PathFindResult HierarchicalFindPath(WaypointID pFrom, WaypointID pTo, ConstWaypointArray& result);
      void ClearMemory();
      bool LoadLegacyWaypointFile(const std::string& filename);
      bool LoadWaypointFile(const std::string& filename);
      bool SaveWaypointFile(const std::string& filename);
      void SetDebugDrawable(AIDebugDrawable* debugDrawable);
      AIDebugDrawable* GetDebugDrawable();
      void GetWaypoints(WaypointArray& toFill);
      void GetWaypoints(ConstWaypointArray& toFill) const;
      size_t GetNumWaypoints() const;
      dtAI::WaypointID GetMaxWaypointID() const;
      void GetWaypointsByType(const dtCore::ObjectType& type, WaypointArray& toFill);
      WaypointInterface* GetClosestWaypoint(const osg::Vec3& pos, float maxRadius);
      WaypointInterface* GetClosestNamedWaypoint(const std::string& name, const osg::Vec3& pos, float maxRadius);
      bool GetWaypointsAtRadius(const osg::Vec3& pos, float radius, WaypointArray& arrayToFill);

   protected:

      virtual ~DeltaAIInterface();
      void Optimize();
      void UpdateDebugDrawable();

   private:
      dtCore::RefPtr<AIDebugDrawable> mDrawable;
      dtCore::RefPtr<WaypointGraph> mWaypointGraph;
      WaypointGraphAStar mAStar;

      typedef std::vector< dtCore::RefPtr<dtAI::WaypointInterface> > WaypointRefArray;
      WaypointRefArray mWaypoints;

      bool mKDTreeDirty;
      WaypointKDTree* mKDTree;

      std::string mLastFileLoaded;
   };

} // namespace dtAI

#endif // DELTA_AI_INTERFACE
