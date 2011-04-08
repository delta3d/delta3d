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
 * William E. Johnson II and David Guthrie
 */
#include <prefix/dtgameprefix-src.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/functor.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>
#include <dtGame/messageparameter.h>
#include <dtGame/gameactor.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/gamemanager.h>
#include <dtGame/gmcomponent.h>
#include <dtGame/invokable.h>

#include <dtCore/shadergroup.h>
#include <dtCore/shader.h>
#include <dtCore/shadermanager.h>
#include <dtDAL/actortype.h>
#include <dtDAL/exceptionenum.h>


namespace dtGame
{
   // invokable names
   const std::string GameActorProxy::PROCESS_MSG_INVOKABLE("Process Message");
   const std::string GameActorProxy::TICK_LOCAL_INVOKABLE("Tick Local");
   const std::string GameActorProxy::TICK_REMOTE_INVOKABLE("Tick Remote");

   IMPLEMENT_ENUM(GameActorProxy::Ownership);
   GameActorProxy::Ownership GameActorProxy::Ownership::SERVER_PUBLISHED("Server+Published");
   GameActorProxy::Ownership GameActorProxy::Ownership::SERVER_LOCAL("Server Local");
   GameActorProxy::Ownership GameActorProxy::Ownership::CLIENT_LOCAL("Client Local");
   GameActorProxy::Ownership GameActorProxy::Ownership::CLIENT_AND_SERVER_LOCAL("Client and Server Local");
   GameActorProxy::Ownership GameActorProxy::Ownership::PROTOTYPE("PROTOTYPE");

	///////////////////////////////////////////
	// Actor Proxy code
	///////////////////////////////////////////
	GameActorProxy::GameActorProxy() : mParent(NULL)
      , ownership(&GameActorProxy::Ownership::SERVER_LOCAL)
      , mIsInGM(false)
	{
      SetClassName("dtGame::GameActor");
	}
   
	GameActorProxy::~GameActorProxy()
	{
	}

	void GameActorProxy::BuildPropertyMap()
	{
		GameActor& ga = GetGameActor();

      dtDAL::PhysicalActorProxy::BuildPropertyMap();

		dtDAL::BooleanActorProperty *bap = new dtDAL::BooleanActorProperty("IsGameActor", "Is Game Actor",
			dtDAL::MakeFunctor(*this, &GameActorProxy::SetIsGameActorProxy),
			dtDAL::MakeFunctorRet(*this, &GameActorProxy::IsGameActorProxy),
			"Read only property that always returns true", "");
		bap->SetReadOnly(true);
		AddProperty(bap);

      bap = new dtDAL::BooleanActorProperty("IsRemote", "Is Remote",
         dtDAL::MakeFunctor(*this, &GameActorProxy::SetRemote),
         dtDAL::MakeFunctorRet(*this, &GameActorProxy::IsRemote),
         "Sets/Gets if a game actor is remote", "");
     bap->SetReadOnly(true);
     AddProperty(bap);

	  bap = new dtDAL::BooleanActorProperty("IsPublished", "Is Published",
			dtDAL::MakeFunctor(*this, &GameActorProxy::SetPublished),
			dtDAL::MakeFunctorRet(*this, &GameActorProxy::IsPublished),
			"Sets/Gets if a game actor is published", "");
     bap->SetReadOnly(true);
     AddProperty(bap);

     AddProperty(new dtDAL::EnumActorProperty<Ownership>("Initial Ownership", "Initial Ownership",
        dtDAL::MakeFunctor(*this, &GameActorProxy::SetInitialOwnership),
        dtDAL::MakeFunctorRet(*this, &GameActorProxy::GetInitialOwnership),
        "Sets/Gets the initial ownership of the actor proxy"));

     const std::string GROUPNAME = "ShaderParams";

     AddProperty(new dtDAL::StringActorProperty("ShaderGroup","ShaderGroup",
        dtDAL::MakeFunctor(ga, &GameActor::SetShaderGroup),
        dtDAL::MakeFunctorRet(ga, &GameActor::GetShaderGroup),
        "Sets the shader group on the game actor.",GROUPNAME));
	}

   //////////////////////////////////////////////////////////////////////////////
   GameActor& GameActorProxy::GetGameActor() 
   {
      return static_cast<GameActor&>(*GetActor());
   }

   //////////////////////////////////////////////////////////////////////////////
   const GameActor& GameActorProxy::GetGameActor() const
   {
      return static_cast<const GameActor&>(*GetActor());
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameActorProxy::NotifyActorUpdate()
   {
      NotifyFullActorUpdate();
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameActorProxy::NotifyFullActorUpdate()
   {
      if (GetGameManager() == NULL || GetGameActor().IsRemote())
         return;

      dtCore::RefPtr<dtGame::Message> updateMsg =
            GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
      dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *>(updateMsg.get());
      PopulateActorUpdate(*message);
      GetGameManager()->SendMessage(*updateMsg);
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameActorProxy::NotifyPartialActorUpdate(const std::vector<std::string> &propNames)
   {
      if (GetGameManager() == NULL || GetGameActor().IsRemote())
         return;

      dtCore::RefPtr<dtGame::Message> updateMsg =
         GetGameManager()->GetMessageFactory().CreateMessage(dtGame::MessageType::INFO_ACTOR_UPDATED);
      dtGame::ActorUpdateMessage *message = static_cast<dtGame::ActorUpdateMessage *>(updateMsg.get());
      PopulateActorUpdate(*message, propNames, true);
      GetGameManager()->SendMessage(*updateMsg);
   }

   void GameActorProxy::PopulateActorUpdate(ActorUpdateMessage& update, const std::vector<std::string> &propNames) 
   {
      PopulateActorUpdate(update, propNames, true);
   }

   void GameActorProxy::PopulateActorUpdate(ActorUpdateMessage& update) 
   {
      PopulateActorUpdate(update, std::vector<std::string>(), false);
   }

   void GameActorProxy::PopulateActorUpdate(ActorUpdateMessage& update, const std::vector<std::string> &propNames, bool limitProperties) 
   {
      StringMessageParameter* nameParam = static_cast<StringMessageParameter*>(update.GetParameter("Name"));
      if (nameParam != NULL)
         nameParam->SetValue(GetName());

      StringMessageParameter* typeParam = static_cast<StringMessageParameter*>(update.GetParameter("Actor Type Name"));
      if (typeParam != NULL)
         typeParam->SetValue(GetActorType().GetName());

      StringMessageParameter* catParam = static_cast<StringMessageParameter*>(update.GetParameter("Actor Type Category"));
      if (catParam != NULL)
         catParam->SetValue(GetActorType().GetCategory());

      update.SetSendingActorId(GetId());
      update.SetAboutActorId(GetId());

      if (limitProperties)
      {
         for (unsigned i = 0; i < propNames.size(); ++i)
         {
            dtDAL::ActorProperty* property = GetProperty(propNames[i]);
            if (property != NULL && !property->IsReadOnly())
            {
               try
               {
                  MessageParameter* mp = update.AddUpdateParameter(property->GetName(), property->GetPropertyType());
                  mp->SetFromProperty(*property);
                  //if (mp != NULL)
                  //   mp->FromString(property->GetStringValue());
               }
               catch (const dtUtil::Exception&)
               {
                  //hmm, someone should not have added a property already.
                  update.GetUpdateParameter(property->GetName());
               }
            }
         }
      }
      else
      {
         std::vector<dtDAL::ActorProperty* > toFill;
         GetPropertyList(toFill);
         for (unsigned i = 0; i < toFill.size(); ++i)
         {
            dtDAL::ActorProperty& property = *toFill[i];

            //don't send read-only properties
            if (property.IsReadOnly())
               continue;

            try
            {
               MessageParameter* mp = update.AddUpdateParameter(property.GetName(), property.GetPropertyType());
               if (mp != NULL)
                  mp->SetFromProperty(property);
               //   mp->FromString(property.GetStringValue());
            }
            catch (const dtUtil::Exception&)
            {
               //hmm, someone should not have added a property already.
               update.GetUpdateParameter(property.GetName());
            }
         }

      }
   }

   void GameActorProxy::ApplyActorUpdate(const ActorUpdateMessage& msg)
   {
      const StringMessageParameter* nameParam = static_cast<const StringMessageParameter*>(msg.GetParameter("Name"));
      if (nameParam != NULL)
      {
         if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Setting name on actor type \"%s.%s\" to value \"%s\"",
               GetActorType().GetCategory().c_str(),
               GetActorType().GetName().c_str(), nameParam->ToString().c_str()
               );
         }
         // we prevent users from setting an empty name because there are many cases where a component will 
         // generate an actor update message without knowing what the actor's name is (for instance the HLA 
         // component doesn't know) in which case, it would overwrite the real name with an empty string == bad.
         // But, we go ahead and allow this if it is the create message.  So, you can create an actor with an 
         // empty name, but can't ever override it to an empty string after that.
         if (msg.GetMessageType() == MessageType::INFO_ACTOR_CREATED || !nameParam->GetValue().empty())
            SetName(nameParam->GetValue());
      }

      std::vector<const MessageParameter*> params;
      msg.GetUpdateParameters(params);

      for(unsigned int i = 0; i < params.size(); ++i)
      {
         const dtDAL::DataType& paramType = params[i]->GetDataType();

         dtDAL::ActorProperty* property = GetProperty(params[i]->GetName());

         if (property == NULL)
         {
            LOG_INFO(("Property \"" + params[i]->GetName() + "\" was not found on the actor.").c_str());
            continue;
         }

         //can't set a read-only property.
         if (property->IsReadOnly())
         {
            if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
            {
               dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
                  "Not setting property \"%s\" on actor type \"%s.%s\" to value \"%s\" because the property is read only.",
                  params[i]->GetName().c_str(), GetActorType().GetCategory().c_str(),
                  GetActorType().GetName().c_str(), params[i]->ToString().c_str()
                  );
            }
            continue;
         }

         if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
               "Setting property \"%s\" on actor type \"%s.%s\" to value \"%s\"",
               params[i]->GetName().c_str(), GetActorType().GetCategory().c_str(),
               GetActorType().GetName().c_str(), params[i]->ToString().c_str()
               );
         }


         if (paramType == dtDAL::DataType::ACTOR)
         {
            dtDAL::ActorActorProperty *aap = static_cast<dtDAL::ActorActorProperty*>(property);
            const ActorMessageParameter* amp = static_cast<const ActorMessageParameter*>(params[i]);
            if( GetGameManager() != NULL )
            {
               dtGame::GameActorProxy* valueProxy = GetGameManager()->FindGameActorById(amp->GetValue());
               aap->SetValue(valueProxy);
            }
            else
            {
               std::stringstream ss;
               ss << GetActorType().GetName().c_str() << "." << GetClassName().c_str() 
                  << " GameActorProxy (" << GetId().ToString().c_str() 
                  << ") could not access the GameManager." << std::endl;
               LOG_ERROR( ss.str() );
            }
         }
         else
         {
            try 
            {
               params[i]->ApplyValueToProperty(*property);
            } 
            catch (const dtUtil::Exception& ex)
            {
               ex.LogException(dtUtil::Log::LOG_ERROR);
            }
         }

      }

   }


   void GameActorProxy::AddInvokable(Invokable& newInvokable)
   {
      std::map<std::string,dtCore::RefPtr<Invokable> >::iterator itor =
      mInvokables.find(newInvokable.GetName());
      if(itor != mInvokables.end())
      {
         std::ostringstream ss;
         ss << "Could not add new invokable " << newInvokable.GetName() << " because "
             << "an invokable with that name already exists.";
         LOGN_ERROR("gameactor.cpp", ss.str());
      }
      else
      {
         mInvokables.insert(std::make_pair(newInvokable.GetName(), dtCore::RefPtr<Invokable>(&newInvokable)));
      }
   }

   void GameActorProxy::BuildInvokables()
   {
      AddInvokable(*new Invokable(TICK_LOCAL_INVOKABLE,
         dtDAL::MakeFunctor(GetGameActor(), &GameActor::TickLocal)));

      AddInvokable(*new Invokable(TICK_REMOTE_INVOKABLE,
         dtDAL::MakeFunctor(GetGameActor(), &GameActor::TickRemote)));

      AddInvokable(*new Invokable(PROCESS_MSG_INVOKABLE,
         dtDAL::MakeFunctor(GetGameActor(), &GameActor::ProcessMessage)));
   }

   Invokable* GameActorProxy::GetInvokable(const std::string& name)
   {
      std::map<std::string,dtCore::RefPtr<Invokable> >::iterator itor =
         mInvokables.find(name);

      if(itor == mInvokables.end())
         return NULL;
      else
         return itor->second.get();
   }

   void GameActorProxy::GetInvokables(std::vector<Invokable*>& toFill)
   {
      toFill.clear();
      toFill.reserve(mInvokables.size());

      for (std::map<std::string,dtCore::RefPtr<Invokable> >::iterator i = mInvokables.begin();
         i != mInvokables.end(); ++i)
      {
         toFill.push_back(i->second.get());
      }
   }

   void GameActorProxy::GetInvokables(std::vector<const Invokable*>& toFill) const
   {
      toFill.clear();
      toFill.reserve(mInvokables.size());

      for (std::map<std::string,dtCore::RefPtr<Invokable> >::const_iterator i = mInvokables.begin();
         i != mInvokables.end(); ++i)
      {
         toFill.push_back(i->second.get());
      }
   }

   void GameActorProxy::GetMessageHandlers(const MessageType& type, std::vector<Invokable*>& toFill)
   {
      toFill.clear();
      toFill.reserve(mMessageHandlers.size());

      for (std::multimap<const MessageType*, dtCore::RefPtr<Invokable> >::iterator i = mMessageHandlers.find(&type);
            (i != mMessageHandlers.end()) && (*i->first == type); ++i)
      {
         toFill.push_back(i->second.get());
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameActorProxy::RegisterForMessages(const MessageType& type, const std::string& invokableName)
   {
      if (IsInGM())
      {
         GetGameManager()->RegisterForMessages(type,*this, invokableName);
      }
      else
      {
         std::ostringstream oss;
         oss << "Could not register the messagetype: " << type.GetName() << " with the invokable: " <<
                invokableName << " because the actor is not in the Game Manager yet.";
         LOGN_ERROR("gameactor.cpp", oss.str())
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameActorProxy::RegisterForMessagesAboutOtherActor(const MessageType& type, 
      const dtCore::UniqueId& targetActorId, const std::string& invokableName)
   {
      if (IsInGM())
      {
         GetGameManager()->RegisterForMessagesAboutActor(type,targetActorId, *this, invokableName);
      }
      else
      {
         std::ostringstream oss;
         oss << "Could not register the messagetype: " << type.GetName() << " with the invokable: " <<
            invokableName << " because the actor is not in the Game Manager yet.";
         LOGN_ERROR("gameactor.cpp", oss.str())
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameActorProxy::RegisterForMessagesAboutSelf(const MessageType& type, const std::string& invokableName)
   {
      Invokable* invokable = GetInvokable(invokableName);
      if (invokable != NULL)
         mMessageHandlers.insert(std::make_pair(&type, invokable));
      else
      {
         std::ostringstream ss;
         ss << "Could not register invokable " << invokableName << " as a handler because "
             << "no invokable with that name exists.";

         LOGN_ERROR("gameactor.cpp", ss.str());
      }

   }

   //////////////////////////////////////////////////////////////////////////////
   void GameActorProxy::UnregisterForMessages(const MessageType& type, const std::string& invokableName)
   {
      if ( GetGameManager() != NULL)
      {
         GetGameManager()->UnregisterForMessages(type,*this, invokableName);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameActorProxy::UnregisterForMessagesAboutOtherActor(const MessageType& type, 
      const dtCore::UniqueId& targetActorId, const std::string& invokableName)
   {
      if (GetGameManager() != NULL)
      {
         GetGameManager()->UnregisterForMessagesAboutActor(type, targetActorId, *this, invokableName);
      }
   }

   //////////////////////////////////////////////////////////////////////////////
   void GameActorProxy::UnregisterForMessagesAboutSelf(const MessageType& type, const std::string& invokableName)
   {
      for (std::multimap<const MessageType*, dtCore::RefPtr<Invokable> >::iterator i = mMessageHandlers.find(&type);
            (i != mMessageHandlers.end()) && (*i->first == type); ++i)
      {
         if (i->second->GetName() == invokableName)
         {
            mMessageHandlers.erase(i);
            break;
         }
      }
   }

   void GameActorProxy::SetRemote(bool remote)
   {
      GameActor& ga = GetGameActor();
      ga.SetRemote(remote);
   }

   void GameActorProxy::SetPublished(bool published)
   {
      GameActor& ga = GetGameActor();
      ga.SetPublished(published);
   }

   void GameActorProxy::InvokeEnteredWorld()
   {
      GameActor& ga = GetGameActor();
      ga.OnEnteredWorld();

      OnEnteredWorld();
   }


   ///////////////////////////////////////////
   // Actor code
   ///////////////////////////////////////////
   const std::string GameActor::NULL_PROXY_ERROR("The actor proxy for a game actor is NULL.  This usually happens if the actor is held in RefPtr, but not the proxy.");
   
   GameActor::GameActor(GameActorProxy& proxy) : mProxy(&proxy), mPublished(false), mRemote(false)
   {
   }

   GameActor::~GameActor()
   {
   }
      
   void GameActor::TickLocal(const Message& tickMessage)
   {
   }

   void GameActor::TickRemote(const Message& tickMessage)
   {
   }

   void GameActor::ProcessMessage(const Message& message)
   {
   }

   void GameActor::SetShaderGroup(const std::string &groupName)
   {
      mShaderGroup = groupName;
      OnShaderGroupChanged();
   }

   void GameActor::OnShaderGroupChanged()
   {
      // Unassign any old setting on this, if any - works regardless if there's a node or not
      dtCore::ShaderManager::GetInstance().UnassignShaderFromNode(*GetOSGNode());

      if (mShaderGroup == "")
         return; // Do nothing, since we have nothing to load

      //First get the shader group assigned to this actor.
      const dtCore::ShaderGroup *shaderGroup =
         dtCore::ShaderManager::GetInstance().FindShaderGroupTemplate(mShaderGroup);

      if (shaderGroup == NULL)
      {
         LOG_INFO("Could not find shader group [" + mShaderGroup + "] for actor [" + GetName());
         return;
      }

      const dtCore::Shader *defaultShader = shaderGroup->GetDefaultShader();

      try
      {
         if (defaultShader != NULL)
         {
            dtCore::ShaderManager::GetInstance().AssignShaderFromTemplate(*defaultShader, *GetOSGNode());
         }
         else
         {
            LOG_WARNING("Could not find a default shader in shader group: " + mShaderGroup);
            return;
         }
      }
      catch (const dtUtil::Exception &e)
      {
         LOG_WARNING("Caught Exception while assigning shader: " + e.ToString());
         return;
      }
   }
}