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
 * William E. Johnson II, David Guthrie, Curtiss Murphy
 */

#include <prefix/dtgameprefix.h>
#include <dtGame/gameactor.h>
#include <dtGame/basemessages.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/shaderactorcomponent.h>

#include <dtCore/shadergroup.h>
#include <dtCore/shaderprogram.h>
#include <dtCore/shadermanager.h>

#include <dtUtil/log.h>
#include <dtUtil/exception.h>

namespace dtGame
{

   ///////////////////////////////////////////
   // Actor code
   ///////////////////////////////////////////
   const std::string GameActor::NULL_PROXY_ERROR("The actor proxy for a game actor is NULL.  This usually happens if the actor is held in RefPtr, but not the proxy.");

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   GameActor::GameActor(GameActorProxy& proxy)
      : mProxy(&proxy)
      , mPublished(false)
      , mRemote(false)
      , mLogger(dtUtil::Log::GetInstance("gameactor.cpp"))
      , mPrototypeName("")
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   GameActor::GameActor(GameActorProxy& proxy, TransformableNode& node, const std::string& name)
      : dtCore::Physical(node, name)
      , mProxy(&proxy)
      , mPublished(false)
      , mRemote(false)
      , mLogger(dtUtil::Log::GetInstance("gameactor.cpp"))
      , mPrototypeName("")
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   GameActor::~GameActor()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   GameActorProxy& GameActor::GetGameActorProxy()
   {
      if (!mProxy.valid())
      {
         throw dtGame::InvalidActorStateException(
                  NULL_PROXY_ERROR,
                  __FILE__, __LINE__);
      }
      return *mProxy;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   const GameActorProxy& GameActor::GetGameActorProxy() const
   {
      if (!mProxy.valid())
      {
         throw dtGame::InvalidActorStateException(
                  NULL_PROXY_ERROR,
                  __FILE__, __LINE__);
      }
      return *mProxy;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::BuildActorComponents()
   {
      AddComponent(*new ShaderActorComponent());
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::OnTickLocal(const TickMessage& tickMessage)
   {
      //Call to support older code.
      TickLocal(tickMessage);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::OnTickRemote(const TickMessage& tickMessage)
   {
      //Call to support older code.
      TickRemote(tickMessage);
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::TickLocal(const Message& tickMessage)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::TickRemote(const Message& tickMessage)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::ProcessMessage(const Message& message)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::SetShaderGroup(const std::string& groupName)
   {
      // Setting the shader group, when it didn't change can cause a massive
      // hit on performance because it unassigns everything and will make a new 
      // instance of the shader and all its params. Could also cause anomalies with 
      // oscilating shader params.
      if (groupName != mShaderGroup)
      {
         mShaderGroup = groupName;
         OnShaderGroupChanged();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::SetRemote(bool remote)
   {
      mRemote = remote;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::SetPublished(bool published)
   {
      mPublished = published;
   }

   ///////////////////////////////////////////////////////////////////////////////////////////////////////
   void GameActor::OnShaderGroupChanged()
   {
      // Unassign any old setting on this, if any - works regardless if there's a node or not
      dtCore::ShaderManager::GetInstance().UnassignShaderFromNode(*GetOSGNode());

      if (mShaderGroup.empty())
         return; // Do nothing, since we have nothing to load

      //First get the shader group assigned to this actor.
      const dtCore::ShaderGroup *shaderGroup =
         dtCore::ShaderManager::GetInstance().FindShaderGroupPrototype(mShaderGroup);

      if (shaderGroup == NULL)
      {
         mLogger.LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__, __LINE__,
                  "Could not find shader group [" + mShaderGroup + "] for actor [" + GetName());
         return;
      }

      const dtCore::ShaderProgram *defaultShader = shaderGroup->GetDefaultShader();

      try
      {
         if (defaultShader != NULL)
         {
            dtCore::ShaderManager::GetInstance().AssignShaderFromPrototype(*defaultShader, *GetOSGNode());
         }
         else
         {
            mLogger.LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                     "Could not find a default shader in shader group: " + mShaderGroup);
            return;
         }
      }
      catch (const dtUtil::Exception& e)
      {
         mLogger.LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__, __LINE__,
                  "Caught Exception while assigning shader: " + e.ToString());
         return;
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameActor::SetPrototypeName(const std::string& prototypeName)
   {
      mPrototypeName = prototypeName;
   }

   //////////////////////////////////////////////////////////////////////////////
   const std::string& GameActor::GetPrototypeName() const
   {
      return mPrototypeName;
   }

}
