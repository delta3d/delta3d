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

#include "dtActors/clouddomeactorproxy.h"
#include "dtCore/enginepropertytypes.h"
#include <dtCore/deltadrawable.h>
#include <dtCore/functor.h>

using namespace dtCore;
using namespace dtCore;

namespace dtActors
{
   void CloudDomeActorProxy::CreateDrawable()
   {
      SetDrawable(*new CloudDome(6, 1, 6.0f, 1.0f, 0.3f, 0.96f, 256.0f, 1800));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void CloudDomeActorProxy::BuildPropertyMap()
   {
      dtActors::EnvEffectActorProxy::BuildPropertyMap();

      CloudDome* cd = static_cast<dtCore::CloudDome*>(GetDrawable());
      const std::string GROUPNAME = "dtCore_CloudPlane";

      AddProperty(new FloatActorProperty("Scale", "Scale",
         FloatActorProperty::SetFuncType(cd, &CloudDome::SetScale),
         FloatActorProperty::GetFuncType(cd, &CloudDome::GetScale),
         "Sets the scaling factor of a cloud dome.", GROUPNAME));

      AddProperty(new FloatActorProperty("Exponent", "Exponent",
         FloatActorProperty::SetFuncType(cd, &CloudDome::SetExponent),
         FloatActorProperty::GetFuncType(cd, &CloudDome::GetExponent),
         "Sets the exponent of a cloud dome.", GROUPNAME));

      AddProperty(new FloatActorProperty("Cutoff", "Cutoff",
         FloatActorProperty::SetFuncType(cd, &CloudDome::SetCutoff),
         FloatActorProperty::GetFuncType(cd, &CloudDome::GetCutoff),
         "Sets the cutoff factor of a cloud dome.", GROUPNAME));

      AddProperty(new FloatActorProperty("SpeedX", "SpeedX",
         FloatActorProperty::SetFuncType(cd, &CloudDome::SetSpeedX),
         FloatActorProperty::GetFuncType(cd, &CloudDome::GetSpeedX),
         "Sets the speed x of a cloud dome.", GROUPNAME));

      AddProperty(new FloatActorProperty("SpeedY", "SpeedY",
         FloatActorProperty::SetFuncType(cd, &CloudDome::SetSpeedY),
         FloatActorProperty::GetFuncType(cd, &CloudDome::GetSpeedY),
         "Sets the speed y of a cloud dome.", GROUPNAME));

      AddProperty(new FloatActorProperty("Bias", "Bias",
         FloatActorProperty::SetFuncType(cd, &CloudDome::SetBias),
         FloatActorProperty::GetFuncType(cd, &CloudDome::GetBias),
         "Sets the bias of a cloud dome.", GROUPNAME));

      AddProperty(new BooleanActorProperty("EnableShaders", "EnableShaders",
         BooleanActorProperty::SetFuncType(cd, &CloudDome::SetShaderEnable),
         BooleanActorProperty::GetFuncType(cd, &CloudDome::GetEnable),
         "Enables shaders on a cloud dome.", GROUPNAME));

      AddProperty(new Vec3ActorProperty("Cloud Color", "Cloud Color",
         Vec3ActorProperty::SetFuncType(this, &CloudDomeActorProxy::SetCloudColor),
         Vec3ActorProperty::GetFuncType(this, &CloudDomeActorProxy::GetCloudColor),
         "Sets the cloud color of a cloud dome.", GROUPNAME));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void CloudDomeActorProxy::SetCloudColor(const osg::Vec3 &color)
   {
      CloudDome *cd = static_cast<CloudDome*> (GetDrawable());

      cd->SetCloudColor(color);
   }

   ///////////////////////////////////////////////////////////////////////////////
   osg::Vec3 CloudDomeActorProxy::GetCloudColor()
   {
      CloudDome *cd = static_cast<CloudDome*> (GetDrawable());

      return cd->GetCloudColor();
   }
}
