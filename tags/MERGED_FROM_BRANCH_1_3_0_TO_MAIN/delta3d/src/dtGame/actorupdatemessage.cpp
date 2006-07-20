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
 * @author William E. Johnson II
 */
#include "dtGame/actorupdatemessage.h"
#include "dtGame/exceptionenum.h"
#include "dtGame/messageparameter.h"
#include <dtDAL/datatype.h>

namespace dtGame
{
   ActorUpdateMessage::ActorUpdateMessage() : Message() 
   {
      AddParameter(new StringMessageParameter("Name"));
      AddParameter(new StringMessageParameter("Actor Type Name"));
      AddParameter(new StringMessageParameter("Actor Type Category"));
   }

   ActorUpdateMessage::~ActorUpdateMessage() {}

   void ActorUpdateMessage::ToString(std::string& toFill) const
   {
      Message::ToString(toFill);

      for(std::map<std::string,dtCore::RefPtr<MessageParameter> >::const_iterator i = mPropertyList.begin(); 
         i != mPropertyList.end(); ++i)
      {
         toFill.append(i->second->ToString());
         toFill.append(1, '\n');
      }
   }

   void ActorUpdateMessage::FromString(const std::string &source)
   {
      Message::FromString(source);

      std::istringstream iss(source);
      for(std::map<std::string,dtCore::RefPtr<MessageParameter> >::iterator i = mPropertyList.begin(); 
         i != mPropertyList.end(); ++i)
      {
         std::string line;
         std::getline(iss, line);
         i->second->FromString(line);
      }      
   }

   void ActorUpdateMessage::ToDataStream(DataStream& stream) const
   {
      Message::ToDataStream(stream);

      // Write out the size of the list so we know how many times to loop in FromDataStream
      stream << (unsigned int)mPropertyList.size();

      for(std::map<std::string,dtCore::RefPtr<MessageParameter> >::const_iterator i = mPropertyList.begin(); 
         i != mPropertyList.end(); ++i)
      {
         stream << i->second->GetDataType().GetTypeId();
         stream << i->second->GetName();
         i->second->ToDataStream(stream);
      }
   }

   void ActorUpdateMessage::FromDataStream(DataStream& stream)
   {
      Message::FromDataStream(stream);
     
      // Read in the size of the stream
      unsigned int size;
      stream >> size;

      for(unsigned short int i = 0; i < size; i++)
      {
         unsigned char id;
         stream >> id;
         dtDAL::DataType *type = NULL;

         for(unsigned int j = 0; j < dtDAL::DataType::Enumerate().size(); j++)
         {
            if(static_cast<dtDAL::DataType*>(dtDAL::DataType::Enumerate()[j])->GetTypeId() == id)
            {
               type = static_cast<dtDAL::DataType*>(dtDAL::DataType::Enumerate()[j]);
               break;
            }
         }
         if(type == NULL) //|| type == &dtDAL::DataType::UNKNOWN)
            EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, "The datatype was not found in the stream\n");

         std::string name;
         stream >> name;
         
         dtCore::RefPtr<MessageParameter> param = GetUpdateParameter(name);
         if (param == NULL)
            param = AddUpdateParameter(name, *type);

         param->FromDataStream(stream);
      }
   }

   MessageParameter* ActorUpdateMessage::AddUpdateParameter(const std::string &name, 
                                                            const dtDAL::DataType &type) throw(dtUtil::Exception)
   {
      std::map<std::string, dtCore::RefPtr<MessageParameter> >::iterator itor = mPropertyList.find(name);

      if(itor != mPropertyList.end())
      {
         EXCEPT(dtGame::ExceptionEnum::INVALID_PARAMETER, "Could not add new parameter: " + name + ". A "
            "parameter with that name already exists.");
      }
      else
      {
         dtCore::RefPtr<MessageParameter> param = MessageParameter::CreateFromType(type, name);
         if (param.valid())
         {
            mPropertyList.insert(std::make_pair(name, param));
            return param.get();
         }
      }
      return NULL;
   }

   MessageParameter* ActorUpdateMessage::GetUpdateParameter(const std::string &name) throw() 
   {
      std::map<std::string, dtCore::RefPtr<MessageParameter> >::iterator itor = mPropertyList.find(name);
      return itor == mPropertyList.end() ? NULL : itor->second.get();
   }

   const MessageParameter* ActorUpdateMessage::GetUpdateParameter(const std::string &name) const throw() 
   {
      std::map<std::string, dtCore::RefPtr<MessageParameter> >::const_iterator itor = mPropertyList.find(name);
      return itor == mPropertyList.end() ? NULL : itor->second.get();
   }

   void ActorUpdateMessage::GetUpdateParameters(std::vector<MessageParameter*> &toFill) throw()
   {
      toFill.clear();
      for(std::map<std::string, dtCore::RefPtr<MessageParameter> >::iterator itor = mPropertyList.begin();
          itor != mPropertyList.end(); ++itor)
          toFill.push_back(itor->second.get());
   }

   void ActorUpdateMessage::GetUpdateParameters(std::vector<const MessageParameter*> &toFill) const throw()
   {
      toFill.clear();
      for(std::map<std::string, dtCore::RefPtr<MessageParameter> >::const_iterator itor = mPropertyList.begin();
          itor != mPropertyList.end(); ++itor)
          toFill.push_back(itor->second.get());
   }
   
   void ActorUpdateMessage::CopyDataTo(Message& msg) const throw(dtUtil::Exception)
   {
      ActorUpdateMessage* aum = dynamic_cast<ActorUpdateMessage*>(&msg);
      
      if (aum == NULL)
         EXCEPT(ExceptionEnum::INVALID_PARAMETER, "The msg parameter must be of type ActorUpdateMessage to allow a copy.");

      Message::CopyDataTo(msg);
     
      
      //wipe out any existing parameters.  It's easier to just recreate them.
      aum->mPropertyList.clear(); 
      
      //copy parameters
      for (std::map<std::string, dtCore::RefPtr<MessageParameter> >::const_iterator i = mPropertyList.begin();
            i != mPropertyList.end(); ++i)
      {
         MessageParameter* newParameter = aum->AddUpdateParameter(i->first, i->second->GetDataType());
         if (newParameter == NULL)
            //This case should not happen, the method above should throw an exception if it doesn't work, but
            //this is a case of paranoid programming.
            EXCEPT(ExceptionEnum::GENERAL_GAMEMANAGER_EXCEPTION, "Unable to create parameter of type " + i->second->GetDataType().GetName());
         
         newParameter->CopyFrom(*i->second);
      }
   }
   
}
