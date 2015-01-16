/* -*-c++-*-
 * testAI - aicharacter (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2004-2006 MOVES Institute
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Bradley Anderegg 06/29/2006
 */

#include "aicharacter.h"
#include <dtAI/astarconfig.h>
#include <dtAI/waypoint.h>
#include <dtUtil/matrixutil.h>

#include <dtCore/batchisector.h>

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
      const dtAI::Waypoint* way = dynamic_cast<const dtAI::Waypoint*>(&*pWaypoint);
      if(way != NULL)
      {
         way->SetRenderFlag(dtAI::Waypoint::RENDER_GREEN);
      }
   }
};

namespace dtAI
{
   /////////////////////////////////////////////////////////////////////////////
   AICharacter::AICharacter(dtCore::Scene* pScene, const WaypointInterface* pWaypoint, const dtCore::ResourceDescriptor& pResource, unsigned pSpeed)
      : mSpeed(pSpeed)
      , mCurrentWaypoint(pWaypoint)
      , mWaypointPath()
      , mAStar()
      , mScene(pScene)
   {
      SetPosition(pWaypoint);
   }

   /////////////////////////////////////////////////////////////////////////////
   AICharacter::~AICharacter()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   void AICharacter::SetPosition(const WaypointInterface* pWaypoint)
   {
      osg::Matrix mat;
      mat(3,0) = pWaypoint->GetPosition()[0];
      mat(3,1) = pWaypoint->GetPosition()[1];
      mat(3,2) = pWaypoint->GetPosition()[2] + 3.5f;

   }

   /////////////////////////////////////////////////////////////////////////////
   bool AICharacter::FindPathAndGoToWaypoint(const WaypointInterface* pWaypoint)
   {
      // to use AStar, we call reset with the two points we want to path between
      mAStar.Reset(mCurrentWaypoint, pWaypoint);

      // a single call to find path should return PATH_FOUND if no constraints are given
      // and if a path exists
      PathFindResult pHasPath = mAStar.FindPath();

      if (pHasPath != NO_PATH)
      {
         // copy the resulting path
         mWaypointPath = mAStar.GetPath();

         // loop through the path and turn everything to render green
         for_each(mWaypointPath.begin(), mWaypointPath.end(), funcRenderGreen());

         const dtAI::Waypoint* way = dynamic_cast<const dtAI::Waypoint*>(pWaypoint);
         if(way != NULL)
         {
            // set the last waypoint to render red
            way->SetRenderFlag(dtAI::Waypoint::RENDER_RED);
         }

         return true;
      }

      // we could not path to that point
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////
   const WaypointAStar::config_type& AICharacter::GetConfig() const
   {
      return mAStar.GetConfig();
   }

   /////////////////////////////////////////////////////////////////////////////
   void AICharacter::GoToWaypoint(float dt, const WaypointInterface* pWaypoint)
   {
      // simple... just rotate to the waypoint over time and set a
      // positive velocity to go there
      //mCharacter->RotateToPoint(pWaypoint->GetPosition(), dt * 3.0f);

      //osg::Vec3 pVector = pWaypoint->GetPosition() - GetPosition();

      //pVector [2] = 0.0f;

      //float dir = std::abs(osg::RadiansToDegrees(atan2(pVector[0], -pVector[1])) - mCharacter->GetRotation());

      //while (dir >= 360.0f) dir -= 360.0f;

      //std::cout << dir << std::endl;
      //if (dir < 5.0f)
      //{
//      mCharacter->SetSpeed(-float(mSpeed));
//
//      if (!mCharacter->IsAnimationPlaying("Walk"))
//      {
//         mCharacter->ClearAllAnimations(0.5f);
//         mCharacter->PlayAnimation("Walk");
//      }
      //}
      //else
      //{
      //   mCharacter->SetVelocity(0.0f);
      //}
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3 AICharacter::GetPosition() const
   {
//     osg::Matrix mat = mCharacter->GetMatrixNode()->getMatrix();
//     osg::Vec3 pos(mat(3, 0), mat(3, 1), mat(3, 2));
//     return pos;
      return osg::Vec3();
   }

   /////////////////////////////////////////////////////////////////////////////
   osg::Vec3 AICharacter::GetForward() const
   {
//      osg::Matrix mat = mCharacter->GetMatrixNode()->getMatrix();
//      osg::Vec3 forward(mat(1, 0), mat(1, 1), mat(1, 2));
//      return forward;
      return osg::Vec3();
   }

   /////////////////////////////////////////////////////////////////////////////
   bool AICharacter::AmAtWaypoint(const WaypointInterface* pWaypoint)
   {
      // a simple distance comparison to determine if we are within
      // range of a waypoint to be considered "at it"
      osg::Vec3 pos = GetPosition();
      osg::Vec3 wayPos = pWaypoint->GetPosition();

      float distToX = std::abs(pos[0] - wayPos[0]);
      float distToY = std::abs(pos[1] - wayPos[1]);
      if (mWaypointPath.size() == 1)
      {
         return (distToX < 0.1f && distToY < 0.1f);
      }
      else
      {
         return (distToX < 1.0f && distToY < 1.0f);
      }
   }

   /////////////////////////////////////////////////////////////////////////////
   void AICharacter::Update(float dt)
   {
      // if we have waypoints to goto
      if (!mWaypointPath.empty())
      {
         // if we have gotten to the current waypoint
         if (AmAtWaypoint(mWaypointPath.front()))
         {
            // set it as the current waypoint
            // meaning the last valid waypoint we visited
            mCurrentWaypoint = mWaypointPath.front();
            // remove that waypoint from the list
            mWaypointPath.pop_front();

            const dtAI::Waypoint* way = dynamic_cast<const dtAI::Waypoint*>(mCurrentWaypoint);
            // set its render flag to render blue again
            if(way != NULL)
            {
               way->SetRenderFlag(dtAI::Waypoint::RENDER_BLUE);
            }
         }

         // if we have another waypoint to goto, goto it
         if (!mWaypointPath.empty())
         {
            ApplyStringPulling();
            GoToWaypoint(dt, mWaypointPath.front());
         }
         // else stop walking
         else
         {
//            mCharacter->SetSpeed(0);
//            if (!mCharacter->IsAnimationPlaying("Idle"))
//            {
//               mCharacter->ClearAllAnimations(0.5f);
//               mCharacter->PlayAnimation("Idle");
//            }
         }
      }

//      mCharacter->Update(dt);
   }

   /////////////////////////////////////////////////////////////////////////////
   void AICharacter::ApplyStringPulling()
   {
      if (mWaypointPath.size() < 2) return;

      dtCore::RefPtr<dtCore::BatchIsector> pIsector = new dtCore::BatchIsector(mScene.get());
      do
      {
         pIsector->Reset();
         //osg::Vec3 vec = mWaypointPath.front()->GetPosition() - GetPosition();
         //vec.normalize();

         const WaypointInterface* pNextWaypoint = *(++(mWaypointPath.begin()));
         pIsector->EnableAndGetISector(0).SetSectorAsLineSegment(GetPosition(), pNextWaypoint->GetPosition());

         // if there is a path between the two points
         if (!pIsector->Update())
         {
            const dtAI::Waypoint* way = dynamic_cast<const dtAI::Waypoint*>(mWaypointPath.front());
            if (way != NULL)
            {
               way->SetRenderFlag(Waypoint::RENDER_BLUE);
            }
            mWaypointPath.pop_front();
         }
         else
         {
            break;
         }
      }
      while (mWaypointPath.size() > 2);
   }

   /////////////////////////////////////////////////////////////////////////////
} // namespace dtAI
