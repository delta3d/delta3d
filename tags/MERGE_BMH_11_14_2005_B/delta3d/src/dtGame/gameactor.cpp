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
 * @author William E. Johnson II and David Guthrie
 */

#include <dtCore/scene.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/functor.h>
#include "dtGame/message.h"
#include "dtGame/messagetype.h"
#include "dtGame/messageparameter.h"
#include "dtGame/gameactor.h"
#include "dtGame/actorupdatemessage.h"
#include "dtGame/gamemanager.h"

namespace dtGame
{
    IMPLEMENT_ENUM(GameActorProxy::Ownership);
    GameActorProxy::Ownership GameActorProxy::Ownership::SERVER_PUBLISHED("Server+Published");
    GameActorProxy::Ownership GameActorProxy::Ownership::SERVER_LOCAL("Server Local");
    GameActorProxy::Ownership GameActorProxy::Ownership::CLIENT_LOCAL("Client Local");
    GameActorProxy::Ownership GameActorProxy::Ownership::CLIENT_AND_SERVER_LOCAL("Client and Server Local");

	///////////////////////////////////////////
	// Actor Proxy code
	///////////////////////////////////////////
	GameActorProxy::GameActorProxy() : mParent(NULL), ownership(&GameActorProxy::Ownership::SERVER_LOCAL)
	{
      SetClassName("dtGame::GameActor");
	}
	GameActorProxy::~GameActorProxy()
	{
	}
   
	void GameActorProxy::BuildPropertyMap()
	{
		//GameActor& ga = GetGameActor();
		
		dtDAL::BooleanActorProperty *bap = new dtDAL::BooleanActorProperty("IsGameActor", "Is Game Actor", 
			dtDAL::MakeFunctor(*this, &GameActorProxy::SetIsGameActorProxy), 
			dtDAL::MakeFunctorRet(*this, &GameActorProxy::IsGameActorProxy), 
			"Read only property that always returns true", "");
		
		bap->SetReadOnly(true);
		
		AddProperty(bap);
		
		AddProperty(new dtDAL::BooleanActorProperty("IsRemote", "Is Remote", 
			dtDAL::MakeFunctor(*this, &GameActorProxy::SetRemote), 
			dtDAL::MakeFunctorRet(*this, &GameActorProxy::IsRemote), 
			"Sets/Gets if a game actor is remote", ""));
		
		AddProperty(new dtDAL::BooleanActorProperty("IsPublished", "Is Published", 
			dtDAL::MakeFunctor(*this, &GameActorProxy::SetPublished), 
			dtDAL::MakeFunctorRet(*this, &GameActorProxy::IsPublished), 
			"Sets/Gets if a game actor is published", ""));

     AddProperty(new dtDAL::EnumActorProperty<Ownership>("Initial Ownership", "Initial Ownership", 
        dtDAL::MakeFunctor(*this, &GameActorProxy::SetInitialOwnership),
        dtDAL::MakeFunctorRet(*this, &GameActorProxy::GetInitialOwnership), 
        "Sets/Gets the initial ownership of the actor proxy"));
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
      if (limitProperties)
      {
      }
      else
      {
         std::vector<dtDAL::ActorProperty* > toFill;
         GetPropertyList(toFill);
         for (unsigned i = 0; i < toFill.size(); ++i)
         {
            dtDAL::ActorProperty& property = *toFill[i];
            try 
            {
               MessageParameter* mp = update.AddUpdateParameter(property.GetName(), property.GetPropertyType());
               if (mp != NULL)
                  mp->FromString(property.GetStringValue());
            }
            catch (const dtUtil::Exception& ex)
            {
               //hmm, someone should not have added a property already.
               update.GetUpdateParameter(property.GetName());
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
      AddInvokable(*new Invokable("Tick Local", 
         dtDAL::MakeFunctor(GetGameActor(), &GameActor::TickLocal)));

      AddInvokable(*new Invokable("Tick Remote", 
         dtDAL::MakeFunctor(GetGameActor(), &GameActor::TickRemote)));
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

   void GameActorProxy::GetInvokableList(std::vector<Invokable*>& toFill)
   {
      toFill.clear();
      for (std::map<std::string,dtCore::RefPtr<Invokable> >::iterator i = mInvokables.begin();
         i != mInvokables.end(); ++i)
      {
         toFill.push_back(i->second.get());
      }
   }

   void GameActorProxy::GetInvokableList(std::vector<const Invokable*>& toFill) const
   {
      toFill.clear();
      for (std::map<std::string,dtCore::RefPtr<Invokable> >::const_iterator i = mInvokables.begin();
         i != mInvokables.end(); ++i)
      {
         toFill.push_back(i->second.get());
      }
   }
   
   void GameActorProxy::GetMessageHandlers(const MessageType& type, std::vector<Invokable*>& toFill)
   {
      toFill.clear();
      for (std::multimap<const MessageType*, dtCore::RefPtr<Invokable> >::iterator i = mMessageHandlers.find(&type);
            (i != mMessageHandlers.end()) && (*i->first == type); ++i)
      {
         toFill.push_back(i->second.get());
      }
   }

   void GameActorProxy::RegisterMessageHandler(const MessageType& type, const std::string& invokableName) 
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
   
   void GameActorProxy::UnregisterMessageHandler(const MessageType& type, const std::string& invokableName)
   {
      for (std::multimap<const MessageType*, dtCore::RefPtr<Invokable> >::iterator i = mMessageHandlers.find(&type);
            (i != mMessageHandlers.end()) && (*i->first == type); ++i)
      {
         if (i->second->GetName() == invokableName)
            mMessageHandlers.erase(i);
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
   GameActor::GameActor() : isPublished(false), isRemote(false)
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
}
