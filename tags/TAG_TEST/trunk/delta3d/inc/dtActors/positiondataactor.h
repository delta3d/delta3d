/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2010, MOVES Institute & BMH Associates, Inc.
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
*/
#ifndef POSITION_DATA_ACTOR
#define POSITION_DATA_ACTOR
 
#include <dtDAL/plugin_export.h>
#include <dtDAL/transformableactorproxy.h>
#include <dtUtil/getsetmacros.h>


namespace dtActors
{
   class DT_PLUGIN_EXPORT PositionDataActorProxy : public dtDAL::TransformableActorProxy
   {
      public:
         PositionDataActorProxy();
         virtual void CreateActor();
         virtual dtDAL::ActorProxyIcon* GetBillBoardIcon();
         virtual void BuildPropertyMap();
         virtual const dtDAL::BaseActorObject::RenderMode& GetRenderMode();

         void ApplyDataTo(dtDAL::BaseActorObject* actorProxy) const;
         void ApplyDataTo(dtCore::DeltaDrawable* drawable) const;
         void ApplyDataTo(dtCore::Transformable* transformable) const;


      DT_DECLARE_ACCESSOR_INLINE(bool, ChangeTranslation)
      DT_DECLARE_ACCESSOR_INLINE(bool, ChangeRotation)
   };
}
#endif
