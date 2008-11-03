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
    * A Wrapper around a dtAnim::CharacterWrapper that allows it to go between Waypoints
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
