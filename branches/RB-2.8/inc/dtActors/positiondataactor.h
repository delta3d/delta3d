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
 
#include <dtCore/plugin_export.h>
#include <dtCore/transformableactorproxy.h>
#include <dtCore/transformable.h>
#include <dtUtil/getsetmacros.h>


namespace dtActors
{
   class DT_PLUGIN_EXPORT PositionDataActorProxy : public dtCore::TransformableActorProxy
   {
      public:
         PositionDataActorProxy();
         virtual void CreateDrawable();
         virtual dtCore::ActorProxyIcon* GetBillBoardIcon();
         virtual void BuildPropertyMap();
         virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode();

         void ApplyDataTo(dtCore::BaseActorObject* actorProxy) const;
         void ApplyDataTo(dtCore::DeltaDrawable* drawable) const;
         void ApplyDataTo(dtCore::Transformable* transformable) const;


      DT_DECLARE_ACCESSOR_INLINE(bool, ChangeTranslation)
      DT_DECLARE_ACCESSOR_INLINE(bool, ChangeRotation)
   };
}
#endif
