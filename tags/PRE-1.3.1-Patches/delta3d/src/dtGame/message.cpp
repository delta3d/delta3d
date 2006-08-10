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
* @author Matthew W. Campbell
*/
#include <dtUtil/log.h>
#include "dtGame/exceptionenum.h"
#include "dtGame/message.h"
#include "dtGame/datastream.h"
#include "dtGame/messageparameter.h"
#include "dtGame/machineinfo.h"
#include "dtGame/messagetype.h"
   
namespace dtGame 
{
   Message::Message() : mMessageType(&MessageType::UNKNOWN), mDestination(NULL), mSendingActorId(""), mAboutActorId("")
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Message::ToString(std::string &toFill) const
   {
      for (std::map<std::string,dtCore::RefPtr<MessageParameter> >::const_iterator i = mParameterList.begin(); 
         i != mParameterList.end(); ++i)
      {
         toFill.append(i->second->ToString());
         toFill.append(1, '\n');
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Message::FromString(const std::string &source)
   {
      std::istringstream iss(source);
      for (std::map<std::string,dtCore::RefPtr<MessageParameter> >::iterator i = mParameterList.begin(); 
         i != mParameterList.end(); ++i)
      {
         std::string line;
         std::getline(iss, line);
         i->second->FromString(line);
      }      
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Message::ToDataStream(DataStream &stream) const
   {
      for (std::map<std::string,dtCore::RefPtr<MessageParameter> >::const_iterator i = mParameterList.begin(); 
         i != mParameterList.end(); ++i)
      {
         i->second->ToDataStream(stream);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Message::FromDataStream(DataStream &stream)
   {
      for (std::map<std::string,dtCore::RefPtr<MessageParameter> >::iterator i = mParameterList.begin(); 
         i != mParameterList.end(); ++i)
      {
         i->second->FromDataStream(stream);
      }      
   }
  
   ///////////////////////////////////////////////////////////////////////////////
   void Message::AddParameter(MessageParameter *param)
   {
      if (param == NULL)
         EXCEPT(ExceptionEnum::INVALID_PARAMETER,"NULL parameters are not legal.");
      
      std::map<std::string,dtCore::RefPtr<MessageParameter> >::iterator itor =
         mParameterList.find(param->GetName());         
      if (itor != mParameterList.end())
      {
         LOG_ERROR("Could not add new parameter: " + param->GetName() + ". A "
            "parameter with that name already exists.");
      }
      else
      {
         mParameterList.insert(std::make_pair(param->GetName(),param));
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   MessageParameter* Message::GetParameter(const std::string &name)
   {
      std::map<std::string,dtCore::RefPtr<MessageParameter> >::iterator itor =
         mParameterList.find(name);
         
      if (itor != mParameterList.end())
         return itor->second.get();
      else 
         return NULL;
   }
   
   const MessageParameter *Message::GetParameter(const std::string &name) const
   {
      std::map<std::string,dtCore::RefPtr<MessageParameter> >::const_iterator itor =
         mParameterList.find(name);
         
      if (itor != mParameterList.end())
         return itor->second.get();
      else 
         return NULL;
   }
         
   void Message::CopyDataTo(Message& msg) const throw(dtUtil::Exception)
   {
      //copy header stuff
      msg.mMessageType = mMessageType;
      msg.mSendingActorId = mSendingActorId;
      msg.mAboutActorId = mAboutActorId;
      msg.mDestination = mDestination;
      msg.mSource = mSource;
      
      //copy parameters
      for (std::map<std::string, dtCore::RefPtr<MessageParameter> >::const_iterator i = mParameterList.begin();     
            i != mParameterList.end(); ++i)
      {
         std::map<std::string,dtCore::RefPtr<MessageParameter> >::iterator copyTo = msg.mParameterList.find(i->first);
         if (copyTo != msg.mParameterList.end())
         {
            copyTo->second->CopyFrom(*i->second);
         }
      }
   }

}
