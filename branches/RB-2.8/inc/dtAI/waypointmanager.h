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
 * Bradley Anderegg 06/07/2006
 */

#ifndef __DELTA_WaypointManager_H__
#define __DELTA_WaypointManager_H__

#include <osg/ref_ptr>
#include <osg/Drawable>
#include <osg/Geode>
#include <dtCore/deltadrawable.h>

#include <dtAI/waypoint.h>
#include <dtAI/waypointpair.h>
#include <dtAI/navmesh.h>
#include <dtAI/export.h>
#include <dtAI/primitives.h>

#include <vector>
#include <map>

#define WAYPOINT_HELPER_FILE_ID 42193
#define WAYPOINT_FILE_VERSION 100

namespace dtAI
{
   /**
    * A static class which manages all waypoints
    */
   class DT_AI_EXPORT WaypointManager : public dtCore::DeltaDrawable
   {
   public:
      typedef std::map<WaypointID, Waypoint*> WaypointMap;
      typedef WaypointMap::iterator WaypointIterator;

   public:
      WaypointManager();

   protected:
      ~WaypointManager();
      WaypointManager(const WaypointManager&);                //not implemented by design
      WaypointManager& operator=(const WaypointManager&);    //not implemented by design

   public:
      /**
       * Returns an instance to our static member
       */
      static WaypointManager& GetInstance();

      /**
       * Copies waypoints into a vector and returns it by value
       * definitely not efficient to say the least, but is being
       * used as a helper function for the Python bindings
       * this function may be removed once the waypoint map is exported
       */
      std::vector<Waypoint*> CopyWaypointsIntoVector() const;

      /**
       * returns an internal const reference to the waypoint mapping
       * of indexes to Waypoints
       */
      const WaypointMap& GetWaypoints() const;

      /**
       * Formats and writes all waypoints out to an ostream
       */
      std::ostream& GetWaypoints(std::ostream& pStream);

      /**
       * Returns a single waypoint by index
       */
      Waypoint* GetWaypoint(WaypointID id);

      /**
       * Returns a single waypoint by index (const version)
       */
      const Waypoint* GetWaypoint(WaypointID id) const;

      /**
       * Given a scene we create a NavMesh
       * the scene is used for doing Isector tests
       * to test if pairs of waypoints are traversable.
       * The NavMesh is created by connecting all pairs of
       * waypoints that are within the total average distance
       * between all two waypoints.
       */
      void CreateNavMesh(dtCore::Scene* pScene);

      /**
       * Returns a reference / const reference to our internal nav mesh
       * created after call to CreateNavMesh
       */
      NavMesh& GetNavMesh();
      const NavMesh& GetNavMesh() const;

      /**
       * Rendering functionality
       */
      bool GetDrawWaypoints() const { return mDrawWaypoints; }
      void SetDrawWaypoints(bool pDraw);
      void SetWaypointColor(const osg::Vec4& pColor);
      void SetWaypointSize(float pSize);

      /**
       * if draw details is set to true it will render one way paths green to red
       * note this can be a performance hit until we optimize it to be precalculated
       * also if pDraw is set to false then draw details wont do anything
       */
      void SetDrawNavMesh(bool pDraw, bool pDrawDetails);
      void SetNavMeshColor(const osg::Vec4& pColor);
      void SetNavMeshSize(float pSize);

      /**
       * Returns the WaypointDrawable
       */
      const osg::Node* GetOSGNode() const;
      osg::Node* GetOSGNode();

      /**
       * Use this function to create your own waypoints
       * @return the index to use for deleting this waypoint
       */
      WaypointID AddWaypoint(const osg::Vec3& pWaypoint);

      /**
       * @note this does not create a new waypoint, the function above does
       * @note if you are having to use this function, consider using an AIInterfaceActor
       *       and at runtime using its AIPluginInterface Ptr
       */
      void AddWaypoint(Waypoint* pWaypoint);

      /**
       * Use this function to remove waypoints you have created
       */
      void RemoveWaypoint(WaypointID id);

      /**
       * Adds a path segment to the navigation mesh
       */
      void AddPathSegment(WaypointID idFrom, WaypointID idTo);

      /**
       * Moves an existing waypoint without removing it
       */
      void MoveWaypoint(WaypointID id, const osg::Vec3& pPos);

      void OnMapLoad(const std::string& pWaypointFilename);
      void OnMapSave(const std::string& pWaypointFilename, bool pCreateNavMesh, dtCore::Scene* pScene);
      void OnMapClose();

      // before operating on our waypoint map obtain a lock
      bool ObtainLock();
      void ReleaseLock();

      /**
       * writes a new waypoint file, returns true if successful
       */
      bool WriteFile(const std::string& pFileToWrite);

      /**
       * attempts to read a waypoint file, returns true if successful
       */
      bool ReadFile(const std::string& pFileToRead);

      /**
       * Frees memory and clears waypoint mapping
       */
      void Clear();

      /**
       * returns the average distance between the waypoints
       */
      float AvgDistBetweenWaypoints() const;

      /**
       * By default if you call CreateWaypoint(osg::Vec3) then the WaypointManger
       * will create your waypoint and delete the memory on clear.  This is legacy
       * behavior and is overridden by the AIInterfaceActor
       */
      void SetDeleteOnClear(bool b);
      bool GetDeleteOnClear() const;

   private:
      /**
       * Explicitly instantiates our static member
       */
      static void CreateInstance();

      // setting a bool for notification that we read in new waypoints
      // we cannot call LoadActors() unless this flag is true
      // and we cannot add waypoints unless this is false
      // note: the effect of keying off of this is that we cannot add waypoints
      // after loading unless we call LoadActors()
      // in short we can only add waypoints in STAGE which is a short coming
      // but can be fixed later
      bool mLoadActors;
      WaypointMap mWaypoints;

      /// our static singleton
      static osg::ref_ptr<WaypointManager> mSingleton;

      //////////////////////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////
      friend class WaypointManagerDrawable;

      /// a helper class to do our rendering for us
      class WaypointManagerDrawable: public osg::Drawable
      {
      public:
         META_Object(dtAI, WaypointManagerDrawable);
         WaypointManagerDrawable(const WaypointManagerDrawable& bd, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
         {

         }
         WaypointManagerDrawable(){}
         WaypointManagerDrawable(WaypointManager* pHelper)
         {
            mHelper = pHelper;
            setUseDisplayList(false);
         }

         /*virtual*/ void drawImplementation(osg::RenderInfo& renderInfo) const;

      private:
         WaypointManager* mHelper;
      };

      //////////////////////////////////////////////////////////////////////////
      //////////////////////////////////////////////////////////////////////////

      // this flag is here specifically to support legacy behavior using
      // the AIInterfaceActor
      bool mDeleteOnClear;

      // a flag to toggle drawing of the waypoints
      bool mDrawWaypoints;

      // a flag to toggle drawing of the navmesh
      bool mDrawNavMesh;

      // a flag to toggle drawing of the navmesh details, which for now
      // renders one way links in red
      bool mDrawNavMeshDetails;

      // this flag is set when we are reading so we dont try to draw
      // at the same time
      bool mReadingFile;

      /// a lock obtained during loading of actors
      bool mLoadActorsLock;

      // waypoint drawing params
      float mWaypointSize;
      osg::Vec4 mWaypointColor;

      // navmesh drawing params
      float mNavMeshWidth;
      osg::Vec4 mNavMeshColor;

      // our drawable
      osg::ref_ptr<osg::Geode> mDrawable;

      dtCore::RefPtr<NavMesh> mNavMesh;
   };

} // namespace dtAI

#endif // __DELTA_WaypointManager_H__
