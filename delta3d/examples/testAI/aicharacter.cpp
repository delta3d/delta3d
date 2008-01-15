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
 * Bradley Anderegg 06/29/2006
 */

#include "aicharacter.h"
#include <dtAI/astarconfig.h>
#include <dtAI/waypoint.h>
#include <dtUtil/matrixutil.h>

#include <dtCore/isector.h>

#include <osg/MatrixTransform>

#include <algorithm>
#include <cassert>
#include <cmath>

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
   AICharacter::AICharacter(dtCore::Scene* pScene, const Waypoint* pWaypoint, const std::string& pFilename, unsigned pSpeed)
      : mSpeed(pSpeed)
      , mCharacter(new dtAnim::CharacterWrapper(pFilename))
      , mCurrentWaypoint(pWaypoint)
      , mWaypointPath()
      , mAStar()
      , mScene(pScene)
   {      
      mCharacter->SetGroundClamp(pScene, 0.0f);     

      SetPosition(pWaypoint);

      pScene->AddDrawable(mCharacter.get());
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
      mCharacter->RotateToPoint(pWaypoint->GetPosition(), dt * 3.0f);            

      //osg::Vec3 pVector = pWaypoint->GetPosition() - GetPosition();

      //pVector [2] = 0.0f;

      //float dir = std::abs(osg::RadiansToDegrees(atan2(pVector[0], -pVector[1])) - mCharacter->GetRotation());

      //while(dir >= 360.0f) dir -= 360.0f;

      //std::cout << dir << std::endl;
      //if(dir < 5.0f)
      //{
      mCharacter->SetSpeed(-float(mSpeed));

      if(!mCharacter->IsAnimationPlaying("Walk"))
      {
         mCharacter->ClearAllAnimations(0.5f);
         mCharacter->PlayAnimation("Walk");
      }
      //}
      //else
      //{
      //   mCharacter->SetVelocity(0.0f);
      //}
   }

   osg::Vec3 AICharacter::GetPosition() const
   {
     osg::Matrix mat = mCharacter->GetMatrixNode()->getMatrix();  
     osg::Vec3 pos(mat(3, 0), mat(3, 1), mat(3, 2));
     return pos;
   }

   osg::Vec3 AICharacter::GetForward() const
   {
      osg::Matrix mat = mCharacter->GetMatrixNode()->getMatrix();  
      osg::Vec3 forward(mat(1, 0), mat(1, 1), mat(1, 2));
      return forward;
   }

   bool AICharacter::AmAtWaypoint(const Waypoint* pWaypoint)
   {
      //a simple distance comparison to determine if we are within
      //range of a waypoint to be considered "at it"
      osg::Vec3 pos = GetPosition();
      osg::Vec3 wayPos = pWaypoint->GetPosition();

      float distToX = std::abs(pos[0] - wayPos[0]);
      float distToY = std::abs(pos[1] - wayPos[1]);
	  if(mWaypointPath.size() == 1)
	  {
		  return (distToX < 0.1f && distToY < 0.1f);
	  }
	  else
	  {
		  return (distToX < 1.0f && distToY < 1.0f);
	  }
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
         if(!mWaypointPath.empty())
         {
            ApplyStringPulling();
            GoToWaypoint(dt, mWaypointPath.front());
         }
         //else stop walking
         else
         {
            mCharacter->SetSpeed(0);
            if(!mCharacter->IsAnimationPlaying("Idle"))
            {
               mCharacter->ClearAllAnimations(0.5f);
               mCharacter->PlayAnimation("Idle");
            }
         }
      }

      mCharacter->Update(dt);
   }

   void AICharacter::ApplyStringPulling()
   {
      if(mWaypointPath.size() < 2) return;

      dtCore::RefPtr<dtCore::Isector> pIsector = new dtCore::Isector(mScene.get());
      do 
      {         
         pIsector->Reset();
         //osg::Vec3 vec = mWaypointPath.front()->GetPosition() - GetPosition();
         //vec.normalize();
         pIsector->SetStartPosition(GetPosition());// + vec);

         const Waypoint* pNextWaypoint = *(++(mWaypointPath.begin()));
         pIsector->SetEndPosition(pNextWaypoint->GetPosition());// - vec);

         //if there is a path between the two points
         if(!pIsector->Update())
         {
            mWaypointPath.front()->SetRenderFlag(Waypoint::RENDER_BLUE);
            mWaypointPath.pop_front();               
         }
         else break;         
      }
      while(mWaypointPath.size() > 2);
   }


}//namespace dtAI
