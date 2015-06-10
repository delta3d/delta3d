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
 * Matthew W. Campbell and David Guthrie
 */

#ifndef DELTA_MESSAGE
#define DELTA_MESSAGE

#include <climits>

#include <dtCore/serializeable.h>
#include <dtGame/export.h>
#include <dtGame/machineinfo.h>
#include <dtGame/messageparameter.h>

#include <dtUtil/assocvector.h>

namespace dtUtil
{
   class DataStream;
}

namespace dtGame 
{
   class MessageType;
   
   class DT_GAME_EXPORT Message : public osg::Referenced, public dtCore::Serializeable
   {
      public:
         Message();

         /**
          * This should write all of the subclass specific data to the string .
          * The base class data will be read by the caller before it calls this method.
          * @param toFill the string to fill.
          */
         virtual void ToString(std::string& toFill) const;

         /**
          * This should read all of the subclass specific data from the string.
          * By default, it reads all of the message parameters.
          * The base class data will be set by the caller when it creates the object.
          * @return true if it was able to assign the value based on the string or false if not.
          * @param source the string to pull the data from.
          */
         virtual bool FromString(const std::string& source);

         /**
          * This should write all of the subclass specific data to the stream.
          * The base class data will be read by the caller before it calls this method.
          * @param stream the stream to fill.
          */
         virtual void ToDataStream(dtUtil::DataStream& stream) const;

         /**
          * This should read all of the subclass specific data from the stream.
          * By default, it reads all of the message parameters.
          * The base class data will be set by the caller when it creates the object.
          * @return true if it was able to assign the value based on the stream or false if not.
          * @param stream the stream to pull the data from.
          */
         virtual bool FromDataStream(dtUtil::DataStream& stream);

         /**
          * Non-const version of getter to return a message parameter by name.
          * @return the parameter specified or NULL of non exists.
          * @param name The name of the message parameter to return.
          */
         MessageParameter* GetParameter(const std::string& name);

         /**
          * Const version of getter to return a message parameter by name.
          * @return the parameter specified or NULL of non exists.
          * @param name The name of the message parameter to return.
          */
         const MessageParameter* GetParameter(const std::string& name) const;

         /**
          * Gets a const list of the parameters currently registered for this
          * message
          */
         void GetParameterList(std::vector<const MessageParameter *>& paramList) const;

         /**
          * Gets a list of the parameters currently registered for this
          * message
          */
         void GetParameterList(std::vector<MessageParameter *>& paramList);

         /**
          * This getter, not the class of the object, determines what type the message is.
          * @return the message type enumeration for this message.
          */
         const MessageType& GetMessageType() const { return *mMessageType; }

         /**
          * Assigns the unique id of the actor sending the message. This should not be set if the message is sent
          * by the GM or a component.  It's intended for actor to actor messages.
          * @param newId the new id of the sending actor.
          */
         void SetSendingActorId(const dtCore::UniqueId& newId) { mSendingActorId = newId; }

         /**
          * Assigns the unique id of the actor the message is about or to. This should not be set if the message is not about
          * an specific actor.
          * @param newId the new id of the actor this message is about or to.
          */
         void SetAboutActorId(const dtCore::UniqueId& newId)   { mAboutActorId   = newId; }

         /**
          * @return the actor that send the message.  This may be an empty unique id, which means there was no sending actor.
          */
         const dtCore::UniqueId& GetSendingActorId() const { return mSendingActorId; }

         /**
          * @note Actors can register invokables on themselves for message types. If they do this, any message of that type
          * with the about actor set to the id of the actor is send to it.
          * @return the actor this message is about.  This may be an empty unique id, which means there is no about actor.
          */
         const dtCore::UniqueId& GetAboutActorId()   const { return mAboutActorId;   }

         /**
          * @note The source is set by the message factory.
          * @return the machine infor for this message.  This returns a reference because it may NOT be null.
          */
         const MachineInfo& GetSource() const      { return *mSource;      }

         /**
          * @note This is NOT set by the message factory.
          * @return the destination machine information.  This returns a pointer because it defaults to null.
          */
         const MachineInfo* GetDestination() const { return mDestination.get(); }

         /**
          * Reassigns the Source.  
          * @param the machine info to assign as the source.  It is a reference so that it may not be NULL.
          */
         void SetSource(const MachineInfo& mi) { mSource = &mi; }

         /**
          * Reassigns the destination.  
          * @param the machine info to assign as the destination.  It is a pointer so that it may be NULL.
          */
         void SetDestination(const MachineInfo* mi) { mDestination = mi; }
         
         /**
          * Copys the data contents of this message to the passed in message.  
          * The param must be of the same type for this to work properly.
          * @param msg the message to copy the contents to.
          * @see MessageFactory#CloneMessage
          * @throw dtUtil::Exception with enum Exception::INVALID_PARAMETER if the message passed cannot by copied to.
          */
         virtual void CopyDataTo(Message& msg) const;
      
         /**
          * Assigns the message that caused this message.  This is used for replies, errors, and rejection messages.  
          * @param causingMessage the message that caused this message to be send.
          */
         void SetCausingMessage(const Message* causingMessage)
         {
            mCausingMessage = causingMessage;
         }

         /**
          * This is used for replies, errors, and rejection messages.  
          * @return the message that caused this message to be send.
          */
         const Message* GetCausingMessage() const
         {
            return mCausingMessage.get();
         }

         bool operator==(const Message& toCompare) const;
         bool operator!=(const Message& toCompare) const { return !(*this == toCompare); }
         

      protected:
         /**
          * Adds a parameter to this message. This method is protected because only a message class
          * should add parameters to itself.
          * @param param the new parameter to add.
          */
         void AddParameter(MessageParameter* param);
         virtual ~Message() { }
         
      private:

         friend class MessageFactory;
         /**
          * Function that sets the message type on a message
          * @param The message type
          * @see class dtGame::MessageFactory
          */
         void SetMessageType(const MessageType& msgType) { mMessageType = &msgType; }

         Message(const Message&) { }
         Message& operator=(const Message&) { return *this; }
         
         const MessageType* mMessageType;
         dtCore::RefPtr<const MachineInfo> mSource;
         dtCore::RefPtr<const MachineInfo> mDestination;
         dtCore::UniqueId mSendingActorId, mAboutActorId;
         
         typedef dtUtil::AssocVector<std::string,dtCore::RefPtr<MessageParameter> > ParameterListType;
         ParameterListType mParameterList;
         
         dtCore::RefPtr<const Message> mCausingMessage;
   };
}

#endif
