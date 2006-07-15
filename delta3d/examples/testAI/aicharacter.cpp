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

#include <dtUtil/matrixutil.h>

#include <osg/Matrix>

#include <cassert>


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
      mAStar.Reset(mCurrentWaypoint, pWaypoint);
      
      WaypointAStar::AStarResult pHasPath = mAStar.FindPath();      

      if(pHasPath != WaypointAStar::NO_PATH)
      {
         pWaypoint->SetActive(true);
         mWaypointPath = mAStar.GetPath();
         return true;
      }      

      return false;
   }

   const WaypointAStar::config_type& AICharacter::GetConfig() const
   {
      return mAStar.GetConfig();
   }

   void AICharacter::GoToWaypoint(float dt, const Waypoint* pWaypoint)
   {  
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
      osg::Vec3 pos = GetPosition();
      osg::Vec3 wayPos = pWaypoint->GetPosition();

      float distToX = fabs(pos[0] - wayPos[0]);
      float distToY = fabs(pos[1] - wayPos[1]);
      return (distToX < 1.0f && distToY < 1.0f);
   }


   void AICharacter::Update(float dt)
   {
      if(!mWaypointPath.empty())  
      {
         if(AmAtWaypoint(mWaypointPath.front()))
         {
            mCurrentWaypoint = mWaypointPath.front();
            mWaypointPath.pop_front();
            mCurrentWaypoint->SetActive(false);
         }

         if(!mWaypointPath.empty()) GoToWaypoint(dt, mWaypointPath.front());
         else mCharacter->SetVelocity(0);
      }
   }


}//namespace dtAI
