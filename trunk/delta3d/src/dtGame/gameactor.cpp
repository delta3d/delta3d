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
	}
   
   void GameActorProxy::PopulateActorUpdate(ActorUpdateMessage& update, const std::vector<std::string> &propNames) throw()
   {
      PopulateActorUpdate(update, propNames, true); 
   }
   
   void GameActorProxy::PopulateActorUpdate(ActorUpdateMessage& update) throw()
   {
      PopulateActorUpdate(update, std::vector<std::string>(), false); 
   }
   
   void GameActorProxy::PopulateActorUpdate(ActorUpdateMessage& update, const std::vector<std::string> &propNames, bool limitProperties) throw()
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

      update.SetAboutActorId(GetId());
      if (limitProperties)
      {
         for (unsigned i = 0; i < propNames.size(); ++i)
         {
            dtDAL::ActorProperty* property = GetProperty(propNames[i]);
            if (property != NULL)
            {
               
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
                  mp->FromString(property.GetStringValue());
            }
            catch (const dtUtil::Exception&)
            {
               //hmm, someone should not have added a property already.
               update.GetUpdateParameter(property.GetName());
            }
         }
         
      } 
   }
  
   void GameActorProxy::ApplyActorUpdate(const ActorUpdateMessage& msg) throw()
   {
      
      const StringMessageParameter* nameParam = static_cast<const StringMessageParameter*>(msg.GetParameter("Name"));
      if (nameParam != NULL)
         SetName(nameParam->GetValue()); 

      std::vector<const MessageParameter*> params;
      msg.GetUpdateParameters(params);

      for(unsigned int i = 0; i < params.size(); ++i)
      {
         const dtDAL::DataType& paramType = params[i]->GetDataType();

         dtDAL::ActorProperty* property = GetProperty(params[i]->GetName());
   
         if (property == NULL)
         {
            LOG_ERROR(("Property " + params[i]->GetName() + " was not found on the actor.").c_str());
            continue;
         }

         if (dtUtil::Log::GetInstance().IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            dtUtil::Log::GetInstance().LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__, 
               "Setting property %s on actor type %s.%s to value %s",
               params[i]->GetName().c_str(), GetActorType().GetCategory().c_str(),
               GetActorType().GetName().c_str(), params[i]->ToString().c_str()
               );
         }
         
         //can't set a read-only property.
         if (property->IsReadOnly())
            continue;
         
         if (paramType == dtDAL::DataType::BOOLEAN)
         {
            dtDAL::BooleanActorProperty *bap = static_cast<dtDAL::BooleanActorProperty*> (property);
            bap->SetValue(static_cast<const BooleanMessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::INT)
         {
            dtDAL::IntActorProperty *iap = static_cast<dtDAL::IntActorProperty*> (property);
            iap->SetValue(static_cast<const IntMessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::LONGINT)
         {
            dtDAL::LongActorProperty *lap = static_cast<dtDAL::LongActorProperty*> (property);
            lap->SetValue(static_cast<const LongIntMessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::FLOAT)
         {
            dtDAL::FloatActorProperty *fap = static_cast<dtDAL::FloatActorProperty*> (property);
            fap->SetValue(static_cast<const FloatMessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::DOUBLE)
         {
            dtDAL::DoubleActorProperty *dap = static_cast<dtDAL::DoubleActorProperty*> (property);
            dap->SetValue(static_cast<const DoubleMessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::STRING)
         {
            dtDAL::StringActorProperty *sap = static_cast<dtDAL::StringActorProperty*> (property);
            sap->SetValue(static_cast<const StringMessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::ENUMERATION)
         {
            dtDAL::AbstractEnumActorProperty *prop = dynamic_cast<dtDAL::AbstractEnumActorProperty*>(property);
            std::string value = params[i]->ToString();
            
            if (!prop->SetValueFromString(value))
               LOG_ERROR(("Failed to set the value on property " + params[i]->GetName() + ".").c_str());
         }
         else if (paramType == dtDAL::DataType::ACTOR)
         {
            dtDAL::ActorActorProperty *aap = static_cast<dtDAL::ActorActorProperty*>(property);
            const ActorMessageParameter* amp = static_cast<const ActorMessageParameter*>(params[i]);
            dtGame::GameActorProxy* valueProxy = GetGameManager()->FindGameActorById(amp->GetValue());
            aap->SetValue(valueProxy);
         }
         else if (paramType == dtDAL::DataType::VEC2)
         {
            dtDAL::Vec2ActorProperty *vap = static_cast<dtDAL::Vec2ActorProperty*> (property);
            vap->SetValue(static_cast<const Vec2MessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::VEC2F)
         {
            dtDAL::Vec2fActorProperty *vap = static_cast<dtDAL::Vec2fActorProperty*> (property);
            vap->SetValue(static_cast<const Vec2fMessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::VEC2D)
         {
            dtDAL::Vec2dActorProperty *vap = static_cast<dtDAL::Vec2dActorProperty*> (property);
            vap->SetValue(static_cast<const Vec2dMessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::VEC3)
         {
            dtDAL::Vec3ActorProperty *vap = static_cast<dtDAL::Vec3ActorProperty*> (property);
            vap->SetValue(static_cast<const Vec3MessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::VEC3F)
         {
            dtDAL::Vec3fActorProperty *vap = static_cast<dtDAL::Vec3fActorProperty*> (property);
            vap->SetValue(static_cast<const Vec3fMessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::VEC3D)
         {
            dtDAL::Vec3dActorProperty *vap = static_cast<dtDAL::Vec3dActorProperty*> (property);
            vap->SetValue(static_cast<const Vec3dMessageParameter*>(params[i])->GetValue());
         }  
         else if (paramType == dtDAL::DataType::VEC4)
         {
            dtDAL::Vec4ActorProperty *vap = static_cast<dtDAL::Vec4ActorProperty*> (property);
            vap->SetValue(static_cast<const Vec4MessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::VEC4F)
         {
            dtDAL::Vec4fActorProperty *vap = static_cast<dtDAL::Vec4fActorProperty*> (property);
            vap->SetValue(static_cast<const Vec4fMessageParameter*>(params[i])->GetValue());
         }
         else if (paramType == dtDAL::DataType::VEC4D)
         {
            dtDAL::Vec4dActorProperty *vap = static_cast<dtDAL::Vec4dActorProperty*> (property);
            vap->SetValue(static_cast<const Vec4dMessageParameter*>(params[i])->GetValue());
         }
         else if (paramType.IsResource())
         {
            dtDAL::ResourceActorProperty *vap = static_cast<dtDAL::ResourceActorProperty*> (property);
            dtDAL::ResourceDescriptor newValue(*static_cast<const ResourceMessageParameter*>(params[i])->GetValue());
            vap->SetValue(&newValue);
         }
         else
            LOG_ERROR(("Message parameter type " + paramType.GetName() + " is not supported").c_str());
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

   void GameActorProxy::GetInvokables(std::vector<Invokable*>& toFill)
   {
      toFill.clear();
      for (std::map<std::string,dtCore::RefPtr<Invokable> >::iterator i = mInvokables.begin();
         i != mInvokables.end(); ++i)
      {
         toFill.push_back(i->second.get());
      }
   }

   void GameActorProxy::GetInvokables(std::vector<const Invokable*>& toFill) const
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
}
