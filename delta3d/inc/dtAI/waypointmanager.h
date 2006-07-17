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
 * @author Bradley Anderegg 06/07/2006
 */

#ifndef __DELTA_WaypointManager_H__
#define __DELTA_WaypointManager_H__

#include <osg/ref_ptr>
#include <osg/Drawable>
#include <osg/Geode>
#include <dtCore/deltadrawable.h>

#include <dtAI/waypoint.h>
#include <dtAI/waypointpair.h>
#include <dtAI/waypointactor.h>
#include <dtAI/navmesh.h>
#include <dtAI/export.h>

#include <vector>
#include <map>


#define WAYPOINT_HELPER_FILE_ID 42193


namespace dtAI
{

   /**
    * A static class which manages all waypoints
    */
   class DT_AI_EXPORT WaypointManager: public dtCore::DeltaDrawable
   {
      public:     
         typedef std::map<unsigned, Waypoint*> WaypointMap;
         typedef WaypointMap::iterator WaypointIterator;

      public:
         WaypointManager();
      protected:        
         ~WaypointManager();
         WaypointManager(const WaypointManager&);                //not implemented by design
         WaypointManager& operator=(const WaypointManager&);    //not implemented by design

      public:

         /**
         * Explicitly instantiates our static member
         */
         static void CreateInstance(); 

         /**
         * Returns an instance to our static member
         */
         static WaypointManager* GetInstance();           
 
         /**
         * returns an internal const reference to the waypoint mapping
         * of indexes to Waypoints
         */
         const WaypointMap& GetWaypoints() const;    

         /**
         * Given a scene we create a nav mesh
         * the scene is used for doing isector tests
         * to test if pairs of waypoints are traversable
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
         void SetDrawWaypoints(bool pDraw);
         void SetWaypointColor(const osg::Vec4& pColor);
         void SetWaypointSize(float pSize);

         void SetDrawNavMesh(bool pDraw);
         void SetNavMeshColor(const osg::Vec4& pColor);
         void SetNavMeshSize(float pSize);         

         /**
         * Formats and writes all waypoints out to an ostream
         */
         std::ostream& GetWaypoints(std::ostream& pStream);

         /**
         * Returns the WaypointDrawable 
         */
         const osg::Node* GetOSGNode() const;
         osg::Node* GetOSGNode();

         /**
         * These are utility functions used by STAGE and dtDAL::Project         
         */
         void AddWaypoint(WaypointActor* pWaypoint);
         void RemoveWaypoint(const WaypointActor* pWaypoint);    
         void MoveWaypoint(unsigned pIndex, const osg::Vec3& pPos);

         void OnMapLoad(const std::string& pWaypointFilename);
         void OnMapSave(const std::string& pWaypointFilename);
         void OnMapClose();

         //before operating on our waypoint map obtain a lock
         bool ObtainLock();
         void ReleaseLock();

         /**
         * writes a new waypoint file, returns true if successful
         */
         bool WriteFile(const std::string& pFileToWrite) const;
         
         /**
         * attempts to read a waypoint file, returns true if successful
         */
         bool ReadFile(const std::string& pFileToRead);         

         /**
         * Frees memory and clears waypoint mapping
         */
         void Clear();

      private:

         //setting a bool for notification that we read in new waypoints
         //we cannot call LoadActors() unless this flag is true
         //and we cannot add waypoints unless this is false
         //note: the effect of keying off of this is that we cannot add waypoints
         //after loading unless we call LoadActors()
         //in short we can only add waypoints in STAGE which is a short coming
         //but can be fixed later
         bool mLoadActors;
         WaypointMap mWaypoints;

         ///our static singleton
         static osg::ref_ptr<WaypointManager> mSingleton;



         //////////////////////////////////////////////////////////////////////////
         //////////////////////////////////////////////////////////////////////////
         friend class WaypointManagerDrawable;

         ///a helper class to do our rendering for us
         class WaypointManagerDrawable: public osg::Drawable
         {
         public:

            META_Object(dtAI, WaypointManagerDrawable);
            WaypointManagerDrawable(const WaypointManagerDrawable& bd, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
            {
               
            }
            WaypointManagerDrawable(){}
            WaypointManagerDrawable(WaypointManager* pHelper)
            {
               mHelper = pHelper;
               setUseDisplayList(false);
            }

            /*virtual*/ void drawImplementation(osg::State& state) const;

         private:
            WaypointManager*                       mHelper;
         };
         //////////////////////////////////////////////////////////////////////////
         //////////////////////////////////////////////////////////////////////////


         //a flag to toggle drawing of the waypoints
         bool mDrawWaypoints;

         //a flag to toggle drawing of the navmesh
         bool mDrawNavMesh;

         //this flag is set when we are reading so we dont try to draw
         //at the same time
         bool mReadingFile;        

         ///a lock obtained during loading of actors         
         bool mLoadActorsLock;

         //waypoint drawing params
         float mWaypointSize;
         osg::Vec4 mWaypointColor;

         //navmesh drawing params
         float mNavMeshWidth;
         osg::Vec4 mNavMeshColor;

         //our drawable
         osg::ref_ptr<osg::Geode> mDrawable;

         NavMesh mNavMesh;
   
   };   

}//namespace 


#endif // __DELTA_WaypointManager_H__
