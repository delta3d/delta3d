/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, MOVES Institute
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
 * Erik Johnson
 */

#include <prefix/dtgameprefix.h>
#include <dtGame/shaderactorcomponent.h>
#include <dtGame/gameactor.h>
#include <dtCore/shadermanager.h>
#include <dtDAL/resourceactorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/project.h>
#include <dtDAL/actortype.h>
#include <dtUtil/log.h>

const dtGame::ActorComponent::ACType dtGame::ShaderActorComponent::SHADER_ACTOR_COMPONENT_TYPE("ShaderActorComponent");

////////////////////////////////////////////////////////////////////////////////
dtGame::ShaderActorComponent::ShaderActorComponent()
: ActorComponent(SHADER_ACTOR_COMPONENT_TYPE),
mCurrentShaderResource(dtDAL::ResourceDescriptor::NULL_RESOURCE)
{

}

////////////////////////////////////////////////////////////////////////////////
dtGame::ShaderActorComponent::~ShaderActorComponent()
{
   SetCurrentShader(dtDAL::ResourceDescriptor::NULL_RESOURCE);
}

////////////////////////////////////////////////////////////////////////////////
void dtGame::ShaderActorComponent::OnAddedToActor(dtGame::GameActor& actor)
{
   //called when parent GameActor gets added to the GameManager
}

////////////////////////////////////////////////////////////////////////////////
void dtGame::ShaderActorComponent::OnRemovedFromActor(dtGame::GameActor& actor)
{
   //called when parent GameActor gets removed from the GameManager
}

////////////////////////////////////////////////////////////////////////////////
void dtGame::ShaderActorComponent::BuildPropertyMap()
{
   static const dtUtil::RefString GROUPNAME("ShaderParams");

   AddProperty(new dtDAL::ResourceActorProperty(dtDAL::DataType::SHADER,
      "CurrentShader", "Current Shader", 
      dtDAL::ResourceActorProperty::SetDescFuncType(this, &dtGame::ShaderActorComponent::SetCurrentShader),
      dtDAL::ResourceActorProperty::GetDescFuncType(this, &dtGame::ShaderActorComponent::GetCurrentShader),
      "The currently applied pixel shader", GROUPNAME));
}

////////////////////////////////////////////////////////////////////////////////
void dtGame::ShaderActorComponent::SetCurrentShader(const dtDAL::ResourceDescriptor& shaderResource)
{
   mCurrentShaderResource = shaderResource;

   dtGame::GameActor* actor;
   GetOwner(actor);
   if (actor == NULL)
   {
      return;  //no parent GameActor yet!
   }

   if (!mCurrentShaderResource.IsEmpty())
   {
      if (!actor->GetShaderGroup().empty())
      {
         LOG_WARNING(std::string("Setting a current shader resource on an actor that already has a shader group assigned.  "
                  "The shader group is being cleared. Actor: ") + actor->GetName() + " " + actor->GetGameActorProxy().GetActorType().GetFullName());
      }

      actor->SetShaderGroup(std::string());
   }

   // This will only be not empty if the current shader resource passed in is empty
   if (actor->GetShaderGroup().empty())
   {
      // Unassign any old setting on this, if any - works regardless if there's a node or not
      dtCore::ShaderManager::GetInstance().UnassignShaderFromNode(*actor->GetOSGNode());
   }

   if (mCurrentShaderResource.IsEmpty())
   {
      return; // Do nothing, since we have nothing to load
   }

   try
   {
      const std::string shaderFile = dtDAL::Project::GetInstance().GetResourcePath(mCurrentShaderResource);
      dtCore::ShaderManager::GetInstance().LoadAndAssignShader(*actor, shaderFile);
   }
   catch (const dtUtil::Exception& e)
   {
      LOG_ERROR("Could not assign the shader '" + 
                  shaderResource.GetResourceName() +
                  "' to GameActor. " +
                  e.ToString());
   }    
}

////////////////////////////////////////////////////////////////////////////////
const dtDAL::ResourceDescriptor dtGame::ShaderActorComponent::GetCurrentShader() const
{
   return mCurrentShaderResource;
}
