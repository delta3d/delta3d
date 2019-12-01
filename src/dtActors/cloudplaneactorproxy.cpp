/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
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
* William E. Johnson II
*/

#include <dtCore/cloudplane.h>
#include "dtActors/cloudplaneactorproxy.h"
#include "dtCore/enginepropertytypes.h"

namespace dtActors 
{
   void CloudPlaneActorProxy::CreateDrawable()
   {
       SetDrawable(*new dtCore::CloudPlane(6, 0.5f, 6, 1.0f, 0.3f, 0.96f, 256, 1800.0f));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void CloudPlaneActorProxy::BuildPropertyMap()
   {
      const std::string& GROUPNAME = "Cloud Plane";
      dtActors::EnvEffectActorProxy::BuildPropertyMap();

      dtCore::CloudPlane* cp = static_cast<dtCore::CloudPlane*>(GetDrawable());

      AddProperty(new dtCore::Vec2fActorProperty("Wind Vector", "Wind Vector",
         dtCore::Vec2ActorProperty::SetFuncType(this, &dtActors::CloudPlaneActorProxy::SetWindVector),
         dtCore::Vec2ActorProperty::GetFuncType(cp, &dtCore::CloudPlane::GetWind),
         "Set wind vector (speed and direction)", GROUPNAME));

   }

   void CloudPlaneActorProxy::SetWindVector(osg::Vec2 wv)
   {
      dtCore::CloudPlane* cp = static_cast<dtCore::CloudPlane*>(GetDrawable());

      cp->SetWind(wv);
   }  
}

