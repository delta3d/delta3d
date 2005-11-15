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
#include <dtDAL/datatype.h>

namespace dtGame
{
   ActorUpdateMessage::ActorUpdateMessage() : Message() {}

   ActorUpdateMessage::~ActorUpdateMessage() {}

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

   void ActorUpdateMessage::GetUpdateParameters(std::vector<MessageParameter*> &toFill) throw()
   {
      toFill.clear();
      for(std::map<std::string, dtCore::RefPtr<MessageParameter> >::iterator itor = mPropertyList.begin();
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
