/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology
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
 * @author Christopher DuBuc
 */

#include <dtLMS/lmsmessage.h>
#include <dtLMS/lmsmessagetype.h>
#include <dtUtil/stringutils.h>

namespace dtLMS
{
   //////////////////////////////////////////////////////////////////////////
   LmsMessage::LmsMessage()
      : mDelimiter(":")
      , mType(&LmsMessageType::SIMULATION)
   {

   }

   //////////////////////////////////////////////////////////////////////////
   LmsMessage::LmsMessage(const std::string &senderID,
                          const LmsMessageType &type,
                          const std::string &value)
      : mSenderID(senderID)
      , mValue(value)
      , mDelimiter(":")
      , mType(&type)
   {

   }

   //////////////////////////////////////////////////////////////////////////
   LmsMessage::LmsMessage(const std::string &senderID,
                          const LmsMessageType &type,
                          const std::string &value,
                          const std::string &objectiveID)
      : mSenderID(senderID)
      , mValue(value)
      , mObjectiveID(objectiveID)
      , mDelimiter(":")
      , mType(&type)
   {

   }

   //////////////////////////////////////////////////////////////////////////
   LmsMessage::LmsMessage(const std::string& messageString)
      : mDelimiter(":")
      , mType(&LmsMessageType::SIMULATION)
   {
      BuildFromString(messageString);
   }

   //////////////////////////////////////////////////////////////////////////
   LmsMessage::~LmsMessage(){}


   //////////////////////////////////////////////////////////////////////////
   void LmsMessage::BuildFromString(const std::string& messageString)
   {
      //tokenize messageString to get at message parts
      std::vector<std::string> parameters;
      char delim = *mDelimiter.c_str();
      dtUtil::IsDelimeter delimCheck(delim);
      dtUtil::StringTokenizer<dtUtil::IsDelimeter>::tokenize(parameters, messageString ,delimCheck);

      if (parameters.empty())
      {
         return;
      }

      //extract message parts from vector
      if (parameters.size() > 0)
      {
         mSenderID = parameters[0];
      }

      if (parameters.size() > 1)
      {
         mType = static_cast<const LmsMessageType*> (LmsMessageType::GetValueForName(parameters[1]));
      }

      if (parameters.size() > 2)
      {
         mValue = parameters[2];
      }

      if (parameters.size() > 3)
      {
         mObjectiveID = parameters[3];
      }
   }

   //////////////////////////////////////////////////////////////////////////
   std::string LmsMessage::ToString() const
   {
      std::string tempString;

      if (mSenderID != "")
      {
         tempString += mSenderID;
      }

      if (mType != 0)
      {
         tempString += mDelimiter + mType->GetName();
      }

      if (mValue != "")
      {
         tempString += mDelimiter + mValue;
      }

      if (mObjectiveID != "")
      {
         tempString += mDelimiter + mObjectiveID;
      }

      return tempString;
   }
}
