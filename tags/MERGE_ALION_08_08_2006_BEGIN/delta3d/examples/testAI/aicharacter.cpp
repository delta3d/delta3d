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
 * @author Bradley Anderegg 06/29/2006
 */

#include "aicharacter.h"
#include <dtAI/astarconfig.h>
#include <dtAI/waypoint.h>
#include <dtUtil/matrixutil.h>

#include <osg/Matrix>

#include <algorithm>
#include <cassert>

/**
* A quick functor that will be used with for_each to render
* all waypoints in the list to green
*/
struct funcRenderGreen 
{
   template<class _WayIter>
   void operator()(const _WayIter& pWaypoint) const
   {
      (*pWaypoint).SetRenderFlag(dtAI::Waypoint::RENDER_GREEN);
   }
};



namespace dtAI
{
   AICharacter::AICharacter(dtCore::Scene* pScene, dtCore::Camera* pCamera, const Waypoint* pWaypoint, const std::string& pFilename, unsigned pSpeed):
      mSpeed(pSpeed),
      mCurrentWaypoint(pWaypoint)
   {
      mCharacter = new dtChar::Character("AICharacter");
      mCharacter->LoadFile(pFilename);
      SetPosition(pWaypoint);
      pScene->AddDrawable(mCharacter.get());
      mCharacter->AddChild(pCamera);
   }
   
   AICharacter::~AICharacter()
   {
   }

   void AICharacter::SetPosition(const Waypoint* pWaypoint)
   {
      osg::Matrix mat;
      mat(3,0) = pWaypoint->GetPosition()[0];
      mat(3,1) = pWaypoint->GetPosition()[1];
      mat(3,2) = pWaypoint->GetPosition()[2] + 3.5f;
      
      mCharacter->GetMatrixNode()->setMatrix(mat);
   }

   bool AICharacter::FindPathAndGoToWaypoint(const Waypoint* pWaypoint)
   {      
      //to use AStar, we call reset with the two points we want to path between
      mAStar.Reset(mCurrentWaypoint, pWaypoint);
      
      //a single call to find path should return PATH_FOUND if no constraints are given
      //and if a path exists
      WaypointAStar::AStarResult pHasPath = mAStar.FindPath();      

      if(pHasPath != WaypointAStar::NO_PATH)
      {
         //copy the resulting path 
         mWaypointPath = mAStar.GetPath();

         //loop through the path and turn everything to render green
         for_each(mWaypointPath.begin(), mWaypointPath.end(), funcRenderGreen());

         //set the last waypoint to render red
         pWaypoint->SetRenderFlag(Waypoint::RENDER_RED);

         return true;
      }      

      //we could not path to that point
      return false;
   }

   const WaypointAStar::config_type& AICharacter::GetConfig() const
   {
      return mAStar.GetConfig();
   }

   void AICharacter::GoToWaypoint(float dt, const Waypoint* pWaypoint)
   {  
      //simple... just rotate to the waypoint over time and set a
      //positive velocity to go there
      mCharacter->RotateCharacterToPoint(pWaypoint->GetPosition(), dt);      
      mCharacter->SetVelocity(mSpeed);
   }

   osg::Vec3 AICharacter::GetPosition() const
   {
     osg::Matrix mat = mCharacter->GetMatrixNode()->getMatrix();  
     osg::Vec3 pos(mat(3, 0), mat(3, 1), mat(3, 2));
     return pos;
   }

   bool AICharacter::AmAtWaypoint(const Waypoint* pWaypoint)
   {
      //a simple distance comparison to determine if we are within
      //range of a waypoint to be considered "at it"
      osg::Vec3 pos = GetPosition();
      osg::Vec3 wayPos = pWaypoint->GetPosition();

      float distToX = fabs(pos[0] - wayPos[0]);
      float distToY = fabs(pos[1] - wayPos[1]);
      return (distToX < 1.0f && distToY < 1.0f);
   }

   
   void AICharacter::Update(float dt)
   {
      //if we have waypoints to goto
      if(!mWaypointPath.empty())  
      {
         //if we have gotten to the current waypoint
         if(AmAtWaypoint(mWaypointPath.front()))
         {
            //set it as the current waypoint
            //meaning the last valid waypoint we visited
            mCurrentWaypoint = mWaypointPath.front();
            //remove that waypoint from the list
            mWaypointPath.pop_front();
            //set its render flag to render blue again
            mCurrentWaypoint->SetRenderFlag(Waypoint::RENDER_BLUE);
         }

         //if we have another waypoint to goto, goto it
         if(!mWaypointPath.empty()) GoToWaypoint(dt, mWaypointPath.front());
         //else stop walking
         else mCharacter->SetVelocity(0);
      }
   }


}//namespace dtAI
