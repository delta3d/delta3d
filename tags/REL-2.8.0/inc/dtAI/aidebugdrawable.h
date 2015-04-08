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

#ifndef DELTA_AI_DEBUG_DRAWABLE
#define DELTA_AI_DEBUG_DRAWABLE

#include <dtAI/export.h>
#include <dtUtil/refstring.h>
#include <dtCore/deltadrawable.h>

//forward declared
//#include <dtAI/waypointrenderinfo.h>

//this includes osg::Vec3, Vec4, and all those
#include <osg/Array>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Group;
   class Geode;
   class Geometry;
}
/// @endcond

namespace dtAI
{
   class WaypointInterface;
   class WaypointRenderInfo;
   class NavMesh;
   class WaypointPair;

   class AIDebugDrawableImpl;

   class DT_AI_EXPORT AIDebugDrawable : public dtCore::DeltaDrawable
   {
   public:
      typedef osg::Vec3 Color;

   public:
      AIDebugDrawable();
      AIDebugDrawable(WaypointRenderInfo& pRenderInfo);

      /**
       * @note replaces all geometry, this can be SLOW, if the editor interface must
       * call this every frame we can make a separate function to reset ALL the geometry
       */
      void SetRenderInfo(WaypointRenderInfo* pRenderInfo);
      WaypointRenderInfo* GetRenderInfo();
      const WaypointRenderInfo* GetRenderInfoConst() const;

      osg::Node* GetOSGNode();
      const osg::Node* GetOSGNode() const;

      /// @return true if this debug drawable has any renderable data.
      bool HasWaypointData() const;

      /**
       * @note This clears out all existing waypoints and replaces them with those in wpArray
       * @param wpArray The array of all WaypointInterface objects to visualize
       * @param createText True if a text billboard should be created for each waypoint
       */
      virtual void SetWaypoints(const std::vector<dtAI::WaypointInterface*>& wpArray, bool createText = true);

      /**
       * @note This clears out all existing edges and replaces them with those in edgeArray.
       *       This is a more granular version of UpdateWaypointGraph(below).
       * @param edgeArray The array of all edges to visualize
       */
      virtual void SetEdges(const std::vector<dtAI::WaypointPair>& edgeArray);

      /**
       * @note This clears out all existing text and replaces them with those
       *       corresponding to the waypoints in wpArray.
       * @param wpArray The array of all waypoints to show text for
       */
      virtual void SetText(const std::vector<dtAI::WaypointInterface*>& wpArray);

      /**
       * Note: Adding an existing waypoint will reset just its position
       *       currently use this to move the waypoint as well.
       */
      virtual void InsertWaypoint(const WaypointInterface& wp, bool addText = true);
      virtual void RemoveWaypoint(unsigned int id);
      virtual void RemoveWaypoints(const std::vector<unsigned int>& idList);

      /**
       * Set the color of an individual WaypointInterface, overwriting the color defined
       * in the WaypointRenderInfo.
       * @param wp An existing WaypointInterface
       * @param color The color to set this WaypointInterface to
       */
      void SetWaypointColor(const WaypointInterface& wp, const osg::Vec4& color);

      /**
       * Reset all the WaypointInterface colors back to the value defined in the WaypointRenderInfo.
       */
      void ResetWaypointColorsToDefault();

      /**
       * Accessor functions to add or remove a single edge to the drawable
       */
      void AddEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo);
      void AddEdges(const std::vector<WaypointPair>& pairs);
      void RemoveEdge(const WaypointInterface* pFrom, const WaypointInterface* pTo);
      void RemoveAllEdges(const WaypointInterface* pFrom);

      void UpdateWaypointGraph(const NavMesh& nm);

      // this is an expensive operation because all the geometry must be recreated
      void OnRenderInfoChanged();

      /**
       * @return -1 if no hit was found, else a WaypointID
       */
      //int Intersect(osg::Vec3& from, osg::Vec3& to) const;

      void ClearMemory();

   protected:
      /*virtual*/ ~AIDebugDrawable();

      void Init();
      void SetOSGNode(osg::Group* grp);

      osg::Geode* GetGeodeWayPoints();
      void SetGeodeWayPoints(osg::Geode* geode);

      osg::Geode* GetGeodeNavMesh();
      void SetGeodeNavMesh(osg::Geode* geode);

      osg::Geometry* GetGeometry();
      void SetGeometry(osg::Geometry* geom);

      // @return the index of the waypoint in the vertex and id arrays, -1 if it does exist
      int FindWaypoint(unsigned id);

      // call this method to reset the data on the geometry node
      // when the vertex data has changed
      void OnGeometryChanged();

      void AddPathSegment(const WaypointInterface* pFrom, const WaypointInterface* pTo);
      void RemovePathSegment(const WaypointInterface* pFrom, const WaypointInterface* pTo);
      void RemovePathSegments(const WaypointInterface* pFrom);
      void ClearWaypointGraph();
      void ClearText();

   private:
      AIDebugDrawableImpl* mImpl;
   };

} // namespace dtAI


#endif
