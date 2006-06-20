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

#ifndef DELTA_ACTORUPDATEMESSAGE
#define DELTA_ACTORUPDATEMESSAGE

#include <map>
#include <dtUtil/exception.h>
#include "dtGame/message.h"


// Forward declarations
namespace dtDAL
{
   class DataType;
}

namespace dtGame
{
   class MessageParameter;

   /**
    * Class the represents messages that are related to actors
    */
   class DT_GAME_EXPORT ActorUpdateMessage : public Message
   {
      public:

         /// Constructor
         ActorUpdateMessage();


         /**
          * @return The name of the actor this is updating.  This value is also used to change the name.
          */
         const std::string& GetName() const
         {
            return static_cast<const StringMessageParameter*>(GetParameter("Name"))->GetValue();
         }

         /**
          * This is used to change the value of the name of the actor being changed.
          * @param newName The name of the actor to set.
          */
         void SetName(const std::string& newName)
         {
            static_cast<StringMessageParameter*>(GetParameter("Name"))->SetValue(newName);
         }

         /**
          * @return the actor type name of the actor.  This can be used along with the category to create the actor.
          */
         const std::string& GetActorTypeName() const
         {
            return static_cast<const StringMessageParameter*>(GetParameter("Actor Type Name"))->GetValue();
         }
         
         /**
          * This is used to change the value of the actor type name of the actor being changed.
          * @param newTypeName The name of the actor type to set.
          */
         void SetActorTypeName(const std::string& newTypeName)
         {
            static_cast<StringMessageParameter*>(GetParameter("Actor Type Name"))->SetValue(newTypeName);
         }

         /**
          * @return the actor type category of the actor.  This can be used along with the actor type name to create the actor.
          */
         const std::string& GetActorTypeCategory() const
         {
            return static_cast<const StringMessageParameter*>(GetParameter("Actor Type Category"))->GetValue();
         }

         /**
          * This is used to change the value of the actor type category of the actor being changed.
          * @param newTypeCategory The category of the actor type to set.
          */
         void SetActorTypeCategory(const std::string& newTypeCategory)
         {
            static_cast<StringMessageParameter*>(GetParameter("Actor Type Category"))->SetValue(newTypeCategory);
         }

         /**
          * Adds an update parameter to an actor message
          * @param name The name of the parameter to add
          * @param type The type of parameter it is, corresponding with dtDAL::DataType
          * @return A pointer to the parameter
          * @see dtDAL::DataType
          * @throws dtUtil::Exception if the name specified is already used.
          */
         MessageParameter* AddUpdateParameter(const std::string &name, const dtDAL::DataType &type) throw(dtUtil::Exception);
         
         /**
          * Retrieves the update parameter for this actor update message for the given name.
          * @param name The name of the parameters to retrieve
          * @return A pointer to the update parameters or NULL if no such parameter exists
          */
         MessageParameter* GetUpdateParameter(const std::string &name) throw();

         /**
          * Retrieves the update parameter for this actor update message for the given name.
          * @param name The name of the parameters to retrieve
          * @return A pointer to the update parameters or NULL if no such parameter exists
          */
         const MessageParameter* GetUpdateParameter(const std::string &name) const throw();

         /** 
          * Retrieves the update parameters for this actor update message
          * @param toFill The vector to fill with the parameters
          */
         void GetUpdateParameters(std::vector<MessageParameter*> &toFill) throw();

         /** 
          * Retrieves the const update parameters for this actor update message
          * @param toFill The vector to fill with the parameters
          */
         void GetUpdateParameters(std::vector<const MessageParameter*> &toFill) const throw();

         /**
          * Adds support to copy the dynamic update parameter list to the other message.
          * @see Message#CopyDataTo
          * @throw dtUtil::Exception with enum Exception::INVALID_PARAMETER if the message is not an ActorUpdateMessage.
          */
         virtual void CopyDataTo(Message& msg) const throw(dtUtil::Exception);

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

      protected:

         /// Destructor
         virtual ~ActorUpdateMessage();

      private:

         std::map<std::string, dtCore::RefPtr<MessageParameter> > mPropertyList;
   };
}

#endif // DELTA_ACTORUPDATEMESSAGE
