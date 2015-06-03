/* -*-c++-*-
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
 * William E. Johnson II, Curtiss Murphy
 */
#include <prefix/dtgameprefix.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/messageparameter.h>
#include <dtCore/datatype.h>
#include <dtCore/actorfactory.h>

namespace dtGame
{
   const dtUtil::RefString ActorUpdateMessage::NAME_PARAMETER("Name");
   const dtUtil::RefString ActorUpdateMessage::ACTOR_TYPE_NAME_PARAMETER("Actor Type Name");
   const dtUtil::RefString ActorUpdateMessage::ACTOR_TYPE_CATEGORY_PARAMETER("Actor Type Category");
   const dtUtil::RefString ActorUpdateMessage::UPDATE_GROUP_PARAMETER("Update Group Parameter");
   const dtUtil::RefString ActorUpdateMessage::PROTOTYPE_NAME_PARAMETER("Prototype Parameter");
   const dtUtil::RefString ActorUpdateMessage::PROTOTYPE_ID_PARAMETER("Prototype ID Parameter");
   const dtUtil::RefString ActorUpdateMessage::PARENT_ID_PARAMETER("Parent ID Parameter");
   const dtUtil::RefString ActorUpdateMessage::IS_PARTIAL_UPDATE_PARAMETER("Is Partial Update");

   static const dtCore::UniqueId INVALID_PARENT_ID("I");

   /////////////////////////////////////////////////////////////////
   ActorUpdateMessage::ActorUpdateMessage() : Message()
   {
      AddParameter(new StringMessageParameter(NAME_PARAMETER));
      AddParameter(new StringMessageParameter(ACTOR_TYPE_NAME_PARAMETER));
      AddParameter(new StringMessageParameter(ACTOR_TYPE_CATEGORY_PARAMETER));
      AddParameter(new StringMessageParameter(PROTOTYPE_NAME_PARAMETER));
      AddParameter(new ActorMessageParameter(PROTOTYPE_ID_PARAMETER));
      AddParameter(new ActorMessageParameter(PARENT_ID_PARAMETER, INVALID_PARENT_ID));

      // Default the partial update param to false and assume all actor updates
      // are full unless set explicitly (see GameActorProxy->NotifyPartialActorUpdate())
      BooleanMessageParameter* partialParam = new BooleanMessageParameter(IS_PARTIAL_UPDATE_PARAMETER);
      partialParam->SetValue(false);
      AddParameter(partialParam);

      mUpdateParameters = new GroupMessageParameter(UPDATE_GROUP_PARAMETER);
      AddParameter(mUpdateParameters);
   }

   /////////////////////////////////////////////////////////////////
   ActorUpdateMessage::~ActorUpdateMessage()
   {

   }

   /////////////////////////////////////////////////////////////////
   const std::string& ActorUpdateMessage::GetName() const
   {
      return static_cast<const StringMessageParameter*>(GetParameter(NAME_PARAMETER))->GetValue();
   }

   /////////////////////////////////////////////////////////////////
   void ActorUpdateMessage::SetName(const std::string& newName)
   {
      static_cast<StringMessageParameter*>(GetParameter(NAME_PARAMETER))->SetValue(newName);
   }

   /////////////////////////////////////////////////////////////////
   const std::string& ActorUpdateMessage::GetActorTypeName() const
   {
      return static_cast<const StringMessageParameter*>(GetParameter(ACTOR_TYPE_NAME_PARAMETER))->GetValue();
   }

   /////////////////////////////////////////////////////////////////
   void ActorUpdateMessage::SetActorTypeName(const std::string& newTypeName)
   {
      static_cast<StringMessageParameter*>(GetParameter(ACTOR_TYPE_NAME_PARAMETER))->SetValue(newTypeName);
   }

   /////////////////////////////////////////////////////////////////
   const std::string& ActorUpdateMessage::GetActorTypeCategory() const
   {
      return static_cast<const StringMessageParameter*>(GetParameter(ACTOR_TYPE_CATEGORY_PARAMETER))->GetValue();
   }

   /////////////////////////////////////////////////////////////////
   void ActorUpdateMessage::SetActorTypeCategory(const std::string& newTypeCategory)
   {
      static_cast<StringMessageParameter*>(GetParameter(ACTOR_TYPE_CATEGORY_PARAMETER))->SetValue(newTypeCategory);
   }

   /////////////////////////////////////////////////////////////////
   dtCore::NamedParameter* ActorUpdateMessage::AddUpdateParameter(const std::string& name,
                                                            dtCore::DataType& type)
   {
      return mUpdateParameters->AddParameter(name, type);
   }

   /////////////////////////////////////////////////////////////////
   dtCore::NamedParameter* ActorUpdateMessage::GetUpdateParameter( const std::string& name )
   {
      return mUpdateParameters->GetParameter(name);
   }

   /////////////////////////////////////////////////////////////////
   const dtCore::NamedParameter* ActorUpdateMessage::GetUpdateParameter(const std::string& name) const
   {
      return mUpdateParameters->GetParameter(name);
   }

   /////////////////////////////////////////////////////////////////
   void ActorUpdateMessage::GetUpdateParameters(std::vector<MessageParameter*>& toFill)
   {
      mUpdateParameters->GetParameters(toFill);
   }

   /////////////////////////////////////////////////////////////////
   void ActorUpdateMessage::GetUpdateParameters(std::vector<const MessageParameter*>& toFill) const
   {
      mUpdateParameters->GetParameters(toFill);
   }

   /////////////////////////////////////////////////////////////////
   const dtCore::ActorType* ActorUpdateMessage::GetActorType() const
   {
      return dtCore::ActorFactory::GetInstance().FindActorType(GetActorTypeCategory(), GetActorTypeName());
   }

   /////////////////////////////////////////////////////////////////
   void ActorUpdateMessage::SetActorType(const dtCore::ActorType& newActorType)
   {
      SetActorTypeCategory(newActorType.GetCategory());
      SetActorTypeName(newActorType.GetName());
   }

   /////////////////////////////////////////////////////////////////
   const std::string& ActorUpdateMessage::GetPrototypeName() const
   {
      return static_cast<const StringMessageParameter*>(GetParameter(PROTOTYPE_NAME_PARAMETER))->GetValue();
   }

   /////////////////////////////////////////////////////////////////
   void ActorUpdateMessage::SetPrototypeName(const std::string& newPrototypeName)
   {
      static_cast<StringMessageParameter*>(GetParameter(PROTOTYPE_NAME_PARAMETER))->SetValue(newPrototypeName);
   }

   /////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& ActorUpdateMessage::GetPrototypeID() const
   {
      return static_cast<const ActorMessageParameter*>(GetParameter(PROTOTYPE_ID_PARAMETER))->GetValue();
   }

   /////////////////////////////////////////////////////////////////
   void ActorUpdateMessage::SetPrototypeID(const dtCore::UniqueId& newPrototypeID)
   {
      static_cast<ActorMessageParameter*>(GetParameter(PROTOTYPE_ID_PARAMETER))->SetValue(newPrototypeID);
   }

   /////////////////////////////////////////////////////////////////
   bool ActorUpdateMessage::IsParentIDSet() const
   {
      return GetParentID() != INVALID_PARENT_ID;
   }

   /////////////////////////////////////////////////////////////////
   void ActorUpdateMessage::SetParentIDToUnset()
   {
      SetParentID(INVALID_PARENT_ID);
   }

   /////////////////////////////////////////////////////////////////
   const dtCore::UniqueId& ActorUpdateMessage::GetParentID() const
   {
      return static_cast<const ActorMessageParameter*>(GetParameter(PARENT_ID_PARAMETER))->GetValue();
   }

   /////////////////////////////////////////////////////////////////
   void ActorUpdateMessage::SetParentID(const dtCore::UniqueId& newPrototypeID)
   {
      static_cast<ActorMessageParameter*>(GetParameter(PARENT_ID_PARAMETER))->SetValue(newPrototypeID);
   }

   /////////////////////////////////////////////////////////////////
   bool ActorUpdateMessage::IsPartialUpdate() const
   {
      return static_cast<const BooleanMessageParameter*>(GetParameter(IS_PARTIAL_UPDATE_PARAMETER))->GetValue();
   }

   /////////////////////////////////////////////////////////////////
   void ActorUpdateMessage::SetPartialUpdate(bool newValue)
   {
      static_cast<BooleanMessageParameter*>(GetParameter(IS_PARTIAL_UPDATE_PARAMETER))->SetValue(newValue);
   }

}
