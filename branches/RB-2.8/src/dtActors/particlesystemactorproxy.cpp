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

#include <dtActors/particlesystemactorproxy.h>

#include <dtCore/actorproxyicon.h>
#include <dtCore/datatype.h>
#include <dtCore/booleanactorproperty.h>
#include <dtCore/functor.h>
#include <dtCore/resourceactorproperty.h>

using namespace dtCore;
using namespace dtCore;

namespace dtActors
{
   void ParticleSystemActorProxy::CreateDrawable()
   {
      SetDrawable(*new dtCore::ParticleSystem);
   }

   //////////////////////////////////////////////////////////////////////////
   void ParticleSystemActorProxy::BuildPropertyMap()
   {
      const std::string &GROUPNAME = "Particle System";
      TransformableActorProxy::BuildPropertyMap();

      ParticleSystem *ps = static_cast<ParticleSystem*>(GetDrawable());

      // This property toggles the enabling of a Particle System.
      // A value of true enables the particle system, which in turn
      // makes it visible in a scene. A vaule of false will make the
      // Particle System not visible.
      // Default is true
      AddProperty(new BooleanActorProperty("Enable", "Enabled",
         BooleanActorProperty::SetFuncType(ps, &ParticleSystem::SetEnabled),
         BooleanActorProperty::GetFuncType(ps, &ParticleSystem::IsEnabled),
         "Toggles the visibility of a particle system to the camera.", GROUPNAME));

      // This property toggles whether or not a Particle System is parent
      // relative. Sets the parent-relative state of this particle system.
      // If parent-relative mode is enabled, the entire particle system
      // will be positioned relative to the parent.
      // Default is false
      AddProperty(new BooleanActorProperty("Parent Relative", "Parent Relative",
         BooleanActorProperty::SetFuncType(ps, &ParticleSystem::SetParentRelative),
         BooleanActorProperty::GetFuncType(ps, &ParticleSystem::IsParentRelative),
         "Sets if a partical system is relative to its parent, if any. ", GROUPNAME));

      // This property enables the loading of a particle resource file.
      AddProperty(new ResourceActorProperty(*this, DataType::PARTICLE_SYSTEM, "Particle(s) File",
         "particle file", ResourceActorProperty::SetFuncType(this, &ParticleSystemActorProxy::LoadFile),
         "Sets the resource file of this particle system", GROUPNAME));
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon* ParticleSystemActorProxy::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon =
            new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_PARTICLESYSTEM);
      }

      return mBillBoardIcon.get();
   }
}
