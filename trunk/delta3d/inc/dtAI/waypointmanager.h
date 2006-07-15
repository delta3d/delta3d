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

         static void CreateInstance();         
         static void DestroyInstance();         
         static WaypointManager* GetInstance();           
 
         const WaypointMap& GetWaypoints() const;

         void AddWaypoint(WaypointActor* pWaypoint);
         void RemoveWaypoint(const WaypointActor* pWaypoint);    
         void MoveWaypoint(unsigned pIndex, const osg::Vec3& pPos);

         bool WriteFile(const std::string& pFileToWrite) const;
         bool ReadFile(const std::string& pFileToRead);             

         //Given a scene we create the possible nav meshes
         //the scene is used for doing an isector
         //to test if a pair of waypoints are traversable
         void CreateNavMesh(dtCore::Scene* pScene);
         NavMesh& GetNavMesh();
         const NavMesh& GetNavMesh() const;

         void Clear();

         std::ostream& GetWaypoints(std::ostream& pStream);

         //required by DeltaDrawable
         const osg::Node* GetOSGNode() const;
         osg::Node* GetOSGNode();

         void SetDrawWaypoints(bool pDraw);
         void SetWaypointColor(const osg::Vec4& pColor);
         void SetWaypointSize(float pSize);

         void SetDrawNavMesh(bool pDraw);
         void SetNavMeshColor(const osg::Vec4& pColor);
         void SetNavMeshSize(float pSize);         


         //utility functions used by dtDAL::Project
         void OnMapLoad(const std::string& pWaypointFilename);
         void OnMapSave(const std::string& pWaypointFilename);
         void OnMapClose();

         //before operating on our waypoint map obtain a lock
         bool ObtainLock();
         void ReleaseLock();

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

         static osg::ref_ptr<WaypointManager> mSingleton;



         //////////////////////////////////////////////////////////////////////////
         //////////////////////////////////////////////////////////////////////////
         friend class WaypointManagerDrawable;

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
