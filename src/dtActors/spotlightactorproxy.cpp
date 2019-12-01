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
#include <dtActors/spotlightactorproxy.h>

#include <dtCore/actorproxyicon.h>
#include <dtCore/datatype.h>
#include <dtCore/floatactorproperty.h>
#include <dtCore/functor.h>

using namespace dtCore;
using namespace dtCore;

namespace dtActors
{
   void SpotlightActorProxy::CreateDrawable()
   {
      SetDrawable(*new dtCore::SpotLight(0));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SpotlightActorProxy::BuildPropertyMap()
   {
      const std::string &GROUPNAME = "Light";
      PositionalLightActorProxy::BuildPropertyMap();

      SpotLight *sl = static_cast<SpotLight*>(GetDrawable());

      // This property manipulates the cutoff oa spotlight. It uses
      // a float type to represent the cutoff factor.
      // Default is 22.5f
      AddProperty(new dtCore::FloatActorProperty("Cutoff", "Cutoff",
         dtCore::FloatActorProperty::SetFuncType(sl, &dtCore::SpotLight::SetSpotCutoff),
         dtCore::FloatActorProperty::GetFuncType(sl, &dtCore::SpotLight::GetSpotCutoff),
         "Sets the spotlight cutoff factor.", GROUPNAME));

      // This property manipulates the exponent of a spotlight. It uses
      // a float type to represent the exponent factor. The higher the
      // SpotExponent, the more concentrated the light will be in the
      // center of the cone.
      // Default is 1.0f
      AddProperty(new dtCore::FloatActorProperty("Spot Exponent", "Spot Exponent",
         dtCore::FloatActorProperty::SetFuncType(sl, &dtCore::SpotLight::SetSpotExponent),
         dtCore::FloatActorProperty::GetFuncType(sl, &dtCore::SpotLight::GetSpotExponent),
         "Sets the concentration of the light in the center of its cone.", GROUPNAME));
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon* SpotlightActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon = new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_LIGHT);
      }

      return mBillBoardIcon.get();
   }
}
