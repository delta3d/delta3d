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
 * Matthew W. Campbell
 */

#include <prefix/dtgameprefix.h>
#include <dtUtil/log.h>
#include <dtUtil/datastream.h>
#include <dtGame/exceptionenum.h>
#include <dtGame/message.h>
#include <dtGame/messagetype.h>

using dtUtil::DataStream;
   
namespace dtGame 
{
   Message::Message()
      : osg::Referenced(true)
      , mMessageType(&MessageType::UNKNOWN)
      , mDestination(NULL)
      , mSendingActorId("")
      , mAboutActorId("")
   {
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   bool Message::operator==(const dtGame::Message& toCompare) const
   {
      if ((&toCompare) == this)
      {
         return true;
      }
      
      if (*mMessageType != *toCompare.mMessageType)
      {
         return false;
      }
      
      if (mDestination != toCompare.mDestination)
      {
         return false;
      }
      
      if (mSource != toCompare.mSource)
      {
         return false;
      }
      
      if (mDestination != toCompare.mDestination)
      {
         return false;
      }
      
      if (mSendingActorId != toCompare.mSendingActorId)
      {
         return false;
      }
      
      if (mAboutActorId != toCompare.mAboutActorId)
      {
         return false;
      }
      
      if (mParameterList.size() != toCompare.mParameterList.size())
      {
         return false;
      }
      
      ParameterListType::const_iterator i, j;
      i = mParameterList.begin();
      j = toCompare.mParameterList.begin();
      
      // must compare the value of each parameter.
      while (i != mParameterList.end() && j != toCompare.mParameterList.end())
      {
         if (*i->second != *j->second)
         {
            return false;
         }
         ++i;
         ++j;
      }
         
      return (mCausingMessage == toCompare.mCausingMessage
               || (mCausingMessage.valid() && toCompare.mCausingMessage.valid() && 
                     *mCausingMessage == *toCompare.mCausingMessage));
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void Message::ToString(std::string& toFill) const
   {
      for (ParameterListType::const_iterator i = mParameterList.begin();
         i != mParameterList.end();
         ++i)
      {
         toFill.append(i->second->ToString());
         toFill.append(1, '\n');
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool Message::FromString(const std::string& source)
   {
      bool okay = true;

      std::istringstream iss(source);
      for (ParameterListType::iterator i = mParameterList.begin();
         i != mParameterList.end();
         ++i)
      {
         std::string line;
         std::getline(iss, line);
         okay = okay && i->second->FromString(line);
      }

      return okay;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void Message::ToDataStream(DataStream& stream) const
   {
      for (ParameterListType::const_iterator i = mParameterList.begin();
         i != mParameterList.end();
         ++i)
      {
         i->second->ToDataStream(stream);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool Message::FromDataStream(DataStream& stream)
   {
      bool okay = true;

      for (ParameterListType::iterator i = mParameterList.begin();
         i != mParameterList.end();
         ++i)
      {
         okay = okay && i->second->FromDataStream(stream);
      }

      return okay;
   }
  
   ///////////////////////////////////////////////////////////////////////////////
   void Message::AddParameter(dtCore::NamedParameter* param)
   {
      if (param == NULL)
      {
         throw dtGame::InvalidParameterException(
         "NULL parameters are not legal.", __FILE__, __LINE__);
      }
      
      ParameterListType::iterator itor =
         mParameterList.find(param->GetName());         
      if (itor != mParameterList.end())
      {
         LOG_ERROR("Could not add new parameter: " + param->GetName() + ". A "
            "parameter with that name already exists.");
      }
      else
      {
         mParameterList.insert(std::make_pair(param->GetName(), param));
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   MessageParameter* Message::GetParameter(const std::string& name)
   {
      ParameterListType::iterator itor =
         mParameterList.find(name);
         
      if (itor != mParameterList.end())
      {
         return itor->second.get();
      }
      else
      {
         return NULL;
      }
   }
   
   const MessageParameter* Message::GetParameter(const std::string& name) const
   {
      ParameterListType::const_iterator itor =
         mParameterList.find(name);
         
      if (itor != mParameterList.end())
      {
         return itor->second.get();
      }
      else
      {
         return NULL;
      }
   }
         
   void Message::GetParameterList(std::vector<const MessageParameter *>& paramList) const
   {
      paramList.clear();
      paramList.reserve(mParameterList.size());

      ParameterListType::const_iterator itor = mParameterList.begin();
      for (; itor != mParameterList.end(); ++itor)
      {
         paramList.push_back((*itor).second.get());
      }
   }
       
   void Message::GetParameterList(std::vector<MessageParameter *>& paramList)
   {
      paramList.clear();
      paramList.reserve(mParameterList.size());

      ParameterListType::iterator itor = mParameterList.begin();
      for (; itor != mParameterList.end(); ++itor)
      {
         paramList.push_back((*itor).second.get());
      }
   }

   void Message::CopyDataTo(Message& msg) const
   {
      //copy header stuff
      msg.mMessageType    = mMessageType;
      msg.mSendingActorId = mSendingActorId;
      msg.mAboutActorId   = mAboutActorId;
      msg.mDestination    = mDestination;
      msg.mSource         = mSource;
      
      //copy parameters
      for (ParameterListType::const_iterator i = mParameterList.begin();
            i != mParameterList.end();
            ++i)
      {
         ParameterListType::iterator copyTo = msg.mParameterList.find(i->first);
         if (copyTo != msg.mParameterList.end())
         {
            copyTo->second->CopyFrom(*i->second);
         }
      }
   }

}
