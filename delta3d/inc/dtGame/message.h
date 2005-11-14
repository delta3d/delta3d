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
* @author Matthew W. Campbell and David Guthrie
*/
#ifndef DELTA_MESSAGE
#define DELTA_MESSAGE

#include <string>
#include <map>
#include <limits.h>
#include <osg/Referenced>
#include <dtCore/refptr.h>
#include <dtCore/uniqueid.h>
#include <dtDAL/exceptionenum.h>
#include <dtUtil/exception.h>
#include "dtGame/export.h"
#include "dtGame/machineinfo.h"
#include "dtGame/messageparameter.h"

namespace dtGame 
{
   class DataStream;
   class MessageType;
   
   class DT_GAMEMANAGER_EXPORT Message : public osg::Referenced
   {
      public:
         Message();
          
         virtual void ToString(std::string& toFill) const;
         virtual void FromString(const std::string &source); 

         /**
          * This should write all of the subclass specific data to the stream.
          * The base class data will be read by the caller before it calls this method.
          * @param stream the stream to fill.
          */
         virtual void ToDataStream(DataStream& stream) const;

         /**
          * This should read all of the subclass specific data from the stream.
          * By default, it reads all of the message parameters.
          * The base class data will be set by the caller when it creates the object.
          * @param stream the stream to pull the data from.
          */
         virtual void FromDataStream(DataStream& stream);
         
         MessageParameter* GetParameter(const std::string &name);
         const MessageParameter *GetParameter(const std::string &name) const;

         const MessageType& GetMessageType() const { return *mMessageType; }

         void SetSendingActorId(const dtCore::UniqueId &newId) { mSendingActorId = newId; }
         void SetAboutActorId(const dtCore::UniqueId &newId)   { mAboutActorId   = newId; }

         const dtCore::UniqueId& GetSendingActorId() const { return mSendingActorId; }
         const dtCore::UniqueId& GetAboutActorId()   const { return mAboutActorId;   }

         const MachineInfo& GetSource() const      { return *mSource;      }
         const MachineInfo* GetDestination() const { return mDestination.get(); }

         void SetSource(const MachineInfo &mi)      { mSource      = &mi; }
         void SetDestination(const MachineInfo* mi) { mDestination = mi; }
         
         /**
          * Copys the data contents of this message to the passed in message.  
          * The param must be of the same type for this to work properly.
          * @param msg the message to copy the contents to.
          * @see MessageFactory#CloneMessage
          * @throw dtUtil::Exception with enum Exception::INVALID_PARAMETER if the message passed cannot by copied to.
          */
         virtual void CopyDataTo(Message& msg) const throw(dtUtil::Exception);
      
      protected:
         void AddParameter(MessageParameter *param);
         virtual ~Message() { }
         
      private:

         friend class MessageFactory;
         /**
          * Function that sets the message type on a message
          * @param The message type
          * @see class dtGame::MessageFactory
          */
         void SetMessageType(const MessageType &msgType) { mMessageType = &msgType; }

         Message(const Message &rhs) { }
         Message &operator=(const Message &rhs) { return *this; }
         
         const MessageType *mMessageType;
         dtCore::RefPtr<const MachineInfo> mSource;
         dtCore::RefPtr<const MachineInfo> mDestination;
         dtCore::UniqueId mSendingActorId, mAboutActorId;
         
         std::map<std::string,dtCore::RefPtr<MessageParameter> > mParameterList;
    };

}

#endif
