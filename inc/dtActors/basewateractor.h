/* -*-c++-*-
* Simulation Core
* Copyright 2007-2008, Alion Science and Technology
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
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* @author Chris Rodgers
*/

#ifndef __BASE_WATER_ACTOR_H__
#define __BASE_WATER_ACTOR_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtDAL/plugin_export.h>
#include <dtGame/gameactor.h>


namespace dtActors
{
   class BaseWaterActorProxy;

   //////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   //////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT BaseWaterActor : public dtGame::GameActor
   {
   public:
      BaseWaterActor(BaseWaterActorProxy& proxy);

      void SetWaterHeight(float meters);
      float GetWaterHeight() const;

      /**
      * Get the world-space surface height and normal at a specified detection point.
      * @param detectionPoint Point from which to detect the world-space height of
      *        the water surface from below or above the point.
      * @param outHeight Metric height of the water surface at the specified point.
      * @param outNormal Normal of the water surface at the specified point.
      * @return TRUE if a point was detected, otherwise FALSE.
      */
      virtual bool GetHeightAndNormalAtPoint(const osg::Vec3& detectionPoint,
         float& outHeight, osg::Vec3& outNormal) const;

   protected:
      virtual ~BaseWaterActor();

   private:
      float mWaterHeight;
   };


   //////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   //////////////////////////////////////////////////////////////////////////
   class DT_PLUGIN_EXPORT BaseWaterActorProxy : public dtGame::GameActorProxy
   {
   public:
      static const dtUtil::RefString CLASS_NAME;
      static const dtUtil::RefString PROPERTY_WATER_HEIGHT;

      BaseWaterActorProxy();

      virtual void CreateActor();

      virtual void BuildPropertyMap();

   protected:
      virtual ~BaseWaterActorProxy();
   };
}

#endif
