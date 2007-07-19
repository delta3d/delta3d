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
 * William E. Johnson II
 */
#include <prefix/dtgameprefix-src.h>
#include <dtGame/actorupdatemessage.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/messageparameter.h>
#include <dtDAL/datatype.h>
#include <dtDAL/librarymanager.h>

namespace dtGame
{
   const std::string ActorUpdateMessage::NAME_PARAMETER("Name");
   const std::string ActorUpdateMessage::ACTOR_TYPE_NAME_PARAMETER("Actor Type Name");
   const std::string ActorUpdateMessage::ACTOR_TYPE_CATEGORY_PARAMETER("Actor Type Category");
   const std::string ActorUpdateMessage::UPDATE_GROUP_PARAMETER("Update Group Parameter");

   ActorUpdateMessage::ActorUpdateMessage() : Message() 
   {
      AddParameter(new StringMessageParameter(NAME_PARAMETER));
      AddParameter(new StringMessageParameter(ACTOR_TYPE_NAME_PARAMETER));
      AddParameter(new StringMessageParameter(ACTOR_TYPE_CATEGORY_PARAMETER));
      mUpdateParameters = new GroupMessageParameter(UPDATE_GROUP_PARAMETER);
      AddParameter(mUpdateParameters);
   }

   ActorUpdateMessage::~ActorUpdateMessage() 
   {

   }

   const std::string& ActorUpdateMessage::GetName() const
   {
      return static_cast<const StringMessageParameter*>(GetParameter(NAME_PARAMETER))->GetValue();
   }

   void ActorUpdateMessage::SetName(const std::string& newName)
   {
      static_cast<StringMessageParameter*>(GetParameter(NAME_PARAMETER))->SetValue(newName);
   }

   const std::string& ActorUpdateMessage::GetActorTypeName() const
   {
      return static_cast<const StringMessageParameter*>(GetParameter(ACTOR_TYPE_NAME_PARAMETER))->GetValue();
   }
   
   void ActorUpdateMessage::SetActorTypeName(const std::string& newTypeName)
   {
      static_cast<StringMessageParameter*>(GetParameter(ACTOR_TYPE_NAME_PARAMETER))->SetValue(newTypeName);
   }

   const std::string& ActorUpdateMessage::GetActorTypeCategory() const
   {
      return static_cast<const StringMessageParameter*>(GetParameter(ACTOR_TYPE_CATEGORY_PARAMETER))->GetValue();
   }

   void ActorUpdateMessage::SetActorTypeCategory(const std::string& newTypeCategory)
   {
      static_cast<StringMessageParameter*>(GetParameter(ACTOR_TYPE_CATEGORY_PARAMETER))->SetValue(newTypeCategory);
   }

   dtDAL::NamedParameter* ActorUpdateMessage::AddUpdateParameter(const std::string &name, 
                                                            const dtDAL::DataType &type)
   {
      return mUpdateParameters->AddParameter(name, type);
   }

   dtDAL::NamedParameter* ActorUpdateMessage::GetUpdateParameter( const std::string &name )
   {
      return mUpdateParameters->GetParameter(name);
   }

   const dtDAL::NamedParameter* ActorUpdateMessage::GetUpdateParameter(const std::string &name) const
   {
      return mUpdateParameters->GetParameter(name);
   }

   void ActorUpdateMessage::GetUpdateParameters(std::vector<MessageParameter*> &toFill) 
   {
      mUpdateParameters->GetParameters(toFill);
   }

   void ActorUpdateMessage::GetUpdateParameters(std::vector<const MessageParameter*> &toFill) const 
   {
      mUpdateParameters->GetParameters(toFill);
   }

   dtDAL::ActorType* ActorUpdateMessage::GetActorType() const
   {
      return dtDAL::LibraryManager::GetInstance().FindActorType(GetActorTypeCategory(), GetActorTypeName());
   }

   void ActorUpdateMessage::SetActorType(const dtDAL::ActorType& newActorType)
   {
      SetActorTypeCategory(newActorType.GetCategory());
      SetActorTypeName(newActorType.GetName());
   }

}
