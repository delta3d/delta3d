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

#include <dtDAL/enginepropertytypes.h>
#include <dtActors/basewateractor.h>


namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////
   // ACTOR CODE
   //////////////////////////////////////////////////////////////////////////

   BaseWaterActor::BaseWaterActor(BaseWaterActorProxy& proxy)
      : dtGame::GameActor(proxy)
      , mWaterHeight(0.0f)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   BaseWaterActor::~BaseWaterActor()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BaseWaterActor::SetWaterHeight(float meters)
   {
      mWaterHeight = meters;
   }

   ////////////////////////////////////////////////////////////////////////////////
   float BaseWaterActor::GetWaterHeight() const
   {
      return mWaterHeight;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool BaseWaterActor::GetHeightAndNormalAtPoint(const osg::Vec3& detectionPoint,
                                                  float& outHeight, osg::Vec3& outNormal) const
   {
      outHeight = mWaterHeight;
      outNormal.set(0.0f, 0.0f, 1.0f);
      return true;
   }


   //////////////////////////////////////////////////////////////////////////
   // PROXY CODE
   //////////////////////////////////////////////////////////////////////////

   const dtUtil::RefString BaseWaterActorProxy::CLASS_NAME("dtActors::BaseWaterActor");
   const dtUtil::RefString BaseWaterActorProxy::PROPERTY_WATER_HEIGHT("Water Height");

   ////////////////////////////////////////////////////////////////////////////////
   BaseWaterActorProxy::BaseWaterActorProxy()
   : dtGame::GameActorProxy()
   {
      SetClassName(BaseWaterActorProxy::CLASS_NAME.Get());
   }

   ////////////////////////////////////////////////////////////////////////////////
   BaseWaterActorProxy::~BaseWaterActorProxy()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BaseWaterActorProxy::CreateActor()
   {
      SetActor(*new BaseWaterActor(*this));
   }

   ////////////////////////////////////////////////////////////////////////////////
   void BaseWaterActorProxy::BuildPropertyMap()
   {
      dtGame::GameActorProxy::BuildPropertyMap();

      // Get the actor's interface.
      BaseWaterActor* actor = NULL;
      GetActor(actor);

      const std::string GROUP_WATER("Water");

      // FLOAT PROPERTIES
      AddProperty(new dtDAL::FloatActorProperty(
         BaseWaterActorProxy::PROPERTY_WATER_HEIGHT,
         BaseWaterActorProxy::PROPERTY_WATER_HEIGHT,
         dtDAL::MakeFunctor(*actor, &BaseWaterActor::SetWaterHeight),
         dtDAL::MakeFunctorRet(*actor, &BaseWaterActor::GetWaterHeight),
         "Sets the offset for the water height (often, this is 0.0, but it depends on the terrain).",
         GROUP_WATER));
   }
}
////////////////////////////////////////////////////////////////////////////////
