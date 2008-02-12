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

#ifndef __DELTA_AICHARACTER_H__
#define __DELTA_AICHARACTER_H__

#include <dtCore/refptr.h>
#include <osg/Referenced>
#include <dtCore/scene.h>
#include <dtCore/camera.h>
#include <dtAI/waypoint.h>
#include <dtAI/astarwaypointutils.h>
#include <string>

#include <dtAnim/characterwrapper.h>

namespace dtAI
{
   /**
    * A Wrapper around a dtChar::Character that allows it to go between Waypoints
    */
   class AICharacter: public osg::Referenced
   {
      public:  
         AICharacter(dtCore::Scene* pScene, const Waypoint* pWaypoint, const std::string& pFilename, unsigned pSpeed);
      protected:
         /*virtual*/ ~AICharacter();
   
      public:
         void Update(float dt);
         void SetPosition(const Waypoint* pWaypoint);         

         osg::Vec3 GetPosition() const;
         osg::Vec3 GetForward() const;

         bool FindPathAndGoToWaypoint(const Waypoint* pWaypoint);
         const Waypoint* GetCurrentWaypoint(){return mCurrentWaypoint;}

         const WaypointAStar::config_type& GetConfig() const;

         dtAnim::CharacterWrapper* GetCharacter() const {return mCharacter.get();}

      private:
         void ApplyStringPulling();
         void GoToWaypoint(float dt, const Waypoint* pWaypoint);
         bool AmAtWaypoint(const Waypoint* pWaypoint);
         void SetRotation(float dt, const Waypoint* pWaypoint);

         unsigned mSpeed;         
         dtCore::RefPtr<dtAnim::CharacterWrapper> mCharacter;

         //pathfinding stuff
         const Waypoint* mCurrentWaypoint;
         WaypointAStar::container_type mWaypointPath;
         WaypointAStar mAStar;
         dtCore::RefPtr<dtCore::Scene> mScene;

   };
}//namespace dtAI

#endif // __DELTA_AICHARACTER_H__
