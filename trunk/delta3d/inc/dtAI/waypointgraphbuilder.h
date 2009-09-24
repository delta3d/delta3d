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

#ifndef DELTA_WAYPOINTGRAPH_BUILDER
#define DELTA_WAYPOINTGRAPH_BUILDER

#include <vector>
#include <osg/Vec3>

#include <dtAI/export.h>
#include <dtAI/waypointinterface.h>
#include <dtAI/navmesh.h>
#include <dtAI/waypointgraph.h>

#include <osg/Referenced>
#include <dtCore/refptr.h>

namespace dtAI
{
   class AIPluginInterface;
   class WaypointCollection;
   class BuilderSearchLevel;

   class DT_AI_EXPORT WaypointGraphBuilder : public osg::Referenced
   {
   public:
      typedef std::vector< dtCore::RefPtr<NavMesh> > NavMeshArray;
      typedef std::vector<const WaypointInterface*> ConstWaypointArray;

   public:
      // AIPluginInterface used for factory creation of waypoint collections
      WaypointGraphBuilder(AIPluginInterface& aiInterface, WaypointGraph& wpGraph);

   protected:
      virtual ~WaypointGraphBuilder();

   public:
      //virtual dtCore::RefPtr<WaypointGraph> CreateWaypointGraph(const NavMesh& nm);
      virtual bool CreateNextSearchLevel(WaypointGraph::SearchLevel* sl);

   protected:
      virtual WaypointCollection* CreateWaypointCollection(const osg::Vec3& pos, unsigned searchLevel);

      WaypointCollection* CreateClique(int numChildren, const WaypointInterface* curWay, const ConstWaypointArray& cliques, const NavMesh& nm);
      void FindCliques(const WaypointInterface* wp, const NavMesh& nm, ConstWaypointArray& result);
      void FindCanidates(const WaypointInterface* wp, const NavMesh& nm, ConstWaypointArray& result);

   private:
      virtual void CreateAll2Cliques(ConstWaypointArray& nodesToBuild, ConstWaypointArray& unassignedNodes, const NavMesh& nm, BuilderSearchLevel* wl);
      virtual void CreateAll4Cliques(ConstWaypointArray& nodesToBuild, ConstWaypointArray& unassignedNodes, const NavMesh& nm, BuilderSearchLevel* wl);

      virtual void AssignRemainingCliques(ConstWaypointArray& nodesToBuild, const NavMesh& nm, BuilderSearchLevel* wl);

      dtCore::RefPtr<WaypointGraph::SearchLevel> ConvertFromBuilderSearchLevel(BuilderSearchLevel* sl);

      virtual bool Assign(const WaypointInterface* wp, WaypointCollection* parent);

      unsigned mCurrentCreationLevel;
      dtCore::RefPtr<AIPluginInterface> mAIInterface;
      dtCore::RefPtr<WaypointGraph> mWPGraph;

      ConstWaypointArray mAssignedNodes;
      ConstWaypointArray mUnAssignedNodes;
      ConstWaypointArray mNodesUnMatched;
   };

} // namespace dtAI

#endif // DELTA_WAYPOINTGRAPH_BUILDER
