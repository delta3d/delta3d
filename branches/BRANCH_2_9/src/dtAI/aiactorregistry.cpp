/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2004-2009 Alion Science and Technology
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
* Bradley Anderegg
*/

#include <dtAI/aiactorregistry.h>
#include <dtAI/aiinterfaceactor.h>

#include <dtAI/baseaicomponent.h>

namespace dtAI
{

   extern "C" DT_AI_EXPORT dtCore::ActorPluginRegistry* CreatePluginRegistry()
   {
      return new AIActorRegistry;
   }

   ///////////////////////////////////////////////////////////////////////////////
   extern "C" DT_AI_EXPORT void DestroyPluginRegistry(dtCore::ActorPluginRegistry *registry)
   {
      if (registry != NULL)
         delete registry;
   }


   dtCore::RefPtr<dtCore::ActorType> AIActorRegistry::AI_INTERFACE_ACTOR_TYPE(
      new dtCore::ActorType("AI Interface Actor", "dtai.AIInterface","Interface to the AI Library"));

   const dtCore::RefPtr<dtCore::SystemComponentType> BaseAIComponent::TYPE(new dtCore::SystemComponentType("BaseAIComponent","GMComponents",
         "Base component for dealing with system-wide AI.", dtGame::GMComponent::BaseGMComponentType));
   const std::string BaseAIComponent::DEFAULT_NAME(TYPE->GetName());


   AIActorRegistry::AIActorRegistry()
      : dtCore::ActorPluginRegistry("AIActors")
   {

   }

   void AIActorRegistry::RegisterActorTypes()
   {
      mActorFactory->RegisterType<dtAI::AIInterfaceActor>(AI_INTERFACE_ACTOR_TYPE.get());
   }

} //namespace dtAI

