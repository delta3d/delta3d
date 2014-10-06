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

namespace dtActors
{
   //////////////////////////////////////////////////////////////////////////
   // CONSTANTS
   //////////////////////////////////////////////////////////////////////////
   const dtUtil::RefString ParticleSystemActor::PROPERTY_ENABLED("Enabled");
   const dtUtil::RefString ParticleSystemActor::PROPERTY_PARENT_RELATIVE("Parent Relative");
   const dtUtil::RefString ParticleSystemActor::PROPERTY_PARTICLE_FILE("Particle(s) File");


   //////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   //////////////////////////////////////////////////////////////////////////
   ParticleSystemActor::ParticleSystemActor()
   {
      SetClassName("dtCore::ParticleSystem");
   }

   //////////////////////////////////////////////////////////////////////////
   ParticleSystemActor::~ParticleSystemActor()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   void ParticleSystemActor::CreateDrawable()
   {
      SetDrawable(*new dtCore::ParticleSystem);
   }

   //////////////////////////////////////////////////////////////////////////
   void ParticleSystemActor::BuildPropertyMap()
   {
      using namespace dtUtil;

      RefString GROUP("Particle System");
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
         RefString("Toggles the visibility of a particle system to the camera."),
         GROUP));

      // This property toggles whether or not a Particle System is parent
      // relative. Sets the parent-relative state of this particle system.
      // If parent-relative mode is enabled, the entire particle system
      // will be positioned relative to the parent.
      // Default is false
      AddProperty(new BooleanActorProperty("Parent Relative", "Parent Relative",
         BooleanActorProperty::SetFuncType(ps, &ParticleSystem::SetParentRelative),
         BooleanActorProperty::GetFuncType(ps, &ParticleSystem::IsParentRelative),
         RefString("Sets if a partical system is relative to its parent, if any. "),
         GROUP));

      // This property enables the loading of a particle resource file.
      AddProperty(new ResourceActorProperty(DataType::PARTICLE_SYSTEM, "Particle(s) File",
         "particle file", ResourceActorProperty::SetFuncType(this, &ParticleSystemActor::LoadFile),
         RefString("Sets the resource file of this particle system"),
         GROUP));
   }

   //////////////////////////////////////////////////////////////////////////
   const dtCore::BaseActorObject::RenderMode& ParticleSystemActor::GetRenderMode()
   {
      return GetDrawable() != NULL
         ? dtCore::BaseActorObject::RenderMode::DRAW_ACTOR
         : dtCore::BaseActorObject::RenderMode::DRAW_BILLBOARD_ICON;
   }

   //////////////////////////////////////////////////////////////////////////
   void ParticleSystemActor::LoadFile(const std::string &fileName)
   {
      dtCore::ParticleSystem* ps = NULL;
      GetDrawable(ps);

      if (ps == NULL)
      {
         throw dtCore::InvalidActorException(
         "Actor should be type dtCore::ParticleSystem", __FILE__, __LINE__);
      }

      if (ps->LoadFile(fileName) == NULL)
      {
         LOG_ERROR("Could not load the file" + fileName);
      }
   }

   //////////////////////////////////////////////////////////////////////////
   dtCore::ActorProxyIcon* ParticleSystemActor::GetBillBoardIcon()
   {
      if (!mBillBoardIcon.valid())
      {
         mBillBoardIcon =
            new dtCore::ActorProxyIcon(dtCore::ActorProxyIcon::IMAGE_BILLBOARD_PARTICLESYSTEM);
      }

      return mBillBoardIcon.get();
   }
}
