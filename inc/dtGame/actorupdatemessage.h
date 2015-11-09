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
 * William E. Johnson II, David Guthrie, Curtiss Murphy
 */

#ifndef DELTA_ACTORUPDATEMESSAGE
#define DELTA_ACTORUPDATEMESSAGE

#include <dtGame/message.h>
#include <dtGame/messageparameter.h>
#include <vector>

// Forward declarations
namespace dtCore
{
   class DataType;
   class ActorType;
}

namespace dtGame
{
   /**
    * Class the represents messages that are related to actors.  Additional 
    * dtCore::NamedParameter can be added to this Message by using the AddUpdateParameter()
    * method.
    */
   class DT_GAME_EXPORT ActorUpdateMessage : public Message
   {
      public:

         static const dtUtil::RefString NAME_PARAMETER;
         static const dtUtil::RefString ACTOR_TYPE_NAME_PARAMETER;
         static const dtUtil::RefString ACTOR_TYPE_CATEGORY_PARAMETER;
         static const dtUtil::RefString PROTOTYPE_NAME_PARAMETER;
         static const dtUtil::RefString PROTOTYPE_ID_PARAMETER;
         static const dtUtil::RefString PARENT_ID_PARAMETER;
         static const dtUtil::RefString IS_PARTIAL_UPDATE_PARAMETER;
         static const dtUtil::RefString UPDATE_GROUP_PARAMETER;

         /// Constructor
         ActorUpdateMessage();
         
         /**
          * @return The name of the actor this is updating.  This value is also used to change the name.
          */
         const std::string& GetName() const;

         /**
          * This is used to change the value of the name of the actor being changed.
          * @param newName The name of the actor to set.
          */
         void SetName(const std::string& newName);

         /**
          * @return the actor type name of the actor.  This can be used along with the category to create the actor.
          */
         const std::string& GetActorTypeName() const;
         
         /**
          * This is used to change the value of the actor type name of the actor being changed.
          * @param newTypeName The name of the actor type to set.
          */
         void SetActorTypeName(const std::string& newTypeName);

         /**
          * @return the actor type category of the actor.  This can be used along with the actor type name to create the actor.
          */
         const std::string& GetActorTypeCategory() const;

         /**
          * This is used to change the value of the actor type category of the actor being changed.
          * @param newTypeCategory The category of the actor type to set.
          */
         void SetActorTypeCategory(const std::string& newTypeCategory);

         /**
          * Adds an dtCore::NamedParameter to an ActorUpdateMessage
          * @param name The name of the parameter to add
          * @param type The type of parameter it is, corresponding with dtCore::DataType
          * @return A pointer to the dtCore::NamedParameter that was added to the internal GroupMessageParameter
          * @see dtCore::DataType
          * @throws dtUtil::Exception if the name specified is already used.
          * @note The returned dtCore::NamedParameter will not show up when calling 
          * ActorUpdateMessage::GetParameter() since the parameter was actually added
          * to an internal GroupMessageParameter, not the Message itself.
          */
         dtCore::NamedParameter* AddUpdateParameter(const std::string& name, dtCore::DataType& type);
         
         /**
          * Adds a dtCore::NamedParameter to an ActorUpdateMessage's internal GroupMessageParameter.
          * @param paramToAdd The parameter to add to the GroupMessageParameter.
          * @throws dtUtil::Exception if a parameter with same name already exists.
          * @note The returned dtCore::NamedParameter will not show up when calling 
          * ActorUpdateMessage::GetParameter() since the parameter was actually added
          * to an internal GroupMessageParameter, not the Message itself.
          */
         void AddUpdateParameter(dtCore::NamedParameter& paramToAdd)
         {
            mUpdateParameters->AddParameter(paramToAdd);
         }

         /**
          * Retrieves the dtCore::NamedParameter for this actor update message for the given name.
          * @param name The name of the parameter to retrieve
          * @return A pointer to the dtCore::NamedParameter or NULL if no such parameter exists
          * @note The "update parameters" are stored in an internal GroupMessageParameter and
          * will not be returned when calling ActorUpdateMessage::GetParameter().
          */
         dtCore::NamedParameter* GetUpdateParameter(const std::string& name);

         /**
          * Retrieves the dtCore::NamedParameter for this actor update message for the given name.
          * @param name The name of the parameters to retrieve
          * @return A const pointer to the dtCore::NamedParameter or NULL if no such parameter exists
          * @note The "update parameters" are stored in an internal GroupMessageParameter and
          * will not be returned when calling ActorUpdateMessage::GetParameter().
          */
         const dtCore::NamedParameter* GetUpdateParameter(const std::string &name) const;

         /** 
          * Retrieves the MessageParameters that have been previously added to this
          * ActorUpdateMessage internal GroupMessageParameter.
          * @param toFill The vector to fill with the MessageParameters
          * @note The "update parameters" are stored in an internal GroupMessageParameter and
          * will not be returned when calling ActorUpdateMessage::GetParameter().
          */
         void GetUpdateParameters(std::vector<MessageParameter*> &toFill);

         /** 
          * Retrieves the MessageParameters that have been previously added to this
          * ActorUpdateMessage internal GroupMessageParameter.
          * @param toFill The vector to fill with the const MessageParameters
          * @note The "update parameters" are stored in an internal GroupMessageParameter and
          * will not be returned when calling ActorUpdateMessage::GetParameter().
          */
         void GetUpdateParameters(std::vector<const MessageParameter*> &toFill) const;

         /**
          * Include dtCore/namedgroupparameter.inl to use this function
          */
         template <typename T>
         inline void AddValue(const dtUtil::RefString& name, const T& value)
         {
            mUpdateParameters->AddValue(name, value);
         }

         /**
          * Include dtCore/namedgroupparameter.inl to use this function
          */
         template <typename T>
         inline void SetValue(const dtUtil::RefString& name, const T& value)
         {
            mUpdateParameters->SetValue(name, value);
         }

         /**
          * Include dtCore/namedgroupparameter.inl to use this function
          */
         template <typename T>
         inline const T& GetValue(const dtUtil::RefString& name, const T& defaultVal) const
         {
            return mUpdateParameters->GetValue(name, defaultVal);
         }


         /**
          * Gets the actor type that this message is about
          * @return The actor type or NULL if the current name and category do not exist
          */
         const dtCore::ActorType* GetActorType() const;

         /**
          * Sets the actor type on this message.
          * @param newActorType the actor type on this actor
          */
         void SetActorType(const dtCore::ActorType& newActorType);

         /**
          * Gets the name of the prototype this actor was created from
          * @return The name of the prototype this actor is created from, if any. 
          */
         const std::string& GetPrototypeName() const;

         /**
         * This value comes from the actor, but is set automatically by the GM when an actor is created
         * from a prototype.
         * @param newName The name of the prototype this actor is created from, if any.
         */
         void SetPrototypeName(const std::string& newPrototypeName);

         /**
          * This value is used for updating/creating remote actors that need to be recreated from prototype
          * @return The name of the prototype this actor is created from, if any. 
          */
         const dtCore::UniqueId& GetPrototypeID() const;

         /**
         * This value is used for updating/creating remote actors that need to be recreated from prototype
         * This value comes from the actor, but is set automatically by the GM when an actor is created
         * from a prototype. If the prototype is non-null, then when the actor is created by the message
         * processor, then it will attempt to look up the prototype first. Extremely useful for networking.
         * @param newName The name of the prototype this actor is created from, if any.
         */
         void SetPrototypeID(const dtCore::UniqueId& newPrototypeID);

         /**
          * If the parent id is a valid value.  A null id is a valid value, but if the value was never set
          * it needs a means to say that the parent id should be ignored.
          */
         bool IsParentIDSet() const;

         /**
          * Sets the parent id to a value that make IsParentIDSet return false.
          */
         void SetParentIDToUnset();

         /**
          * This value is used for updating/creating remote actors that need a parent actor id.
          */
         const dtCore::UniqueId& GetParentID() const;

         /**
         * This value is used for updating/creating remote actors that need a parent actor id.
         * This value comes from the actor, but is set automatically by the GM when an actor is created
         * from a prototype. If the prototype is non-null, then when the actor is created by the message
         * processor, then it will attempt to look up the prototype first. Extremely useful for networking.
         * @param newName The name of the prototype this actor is created from, if any.
         */
         void SetParentID(const dtCore::UniqueId& newPrototypeID);

         /**
          * This templated function exposes the for each parameter on the update parameters data member.
          */
         template <class UnaryFunction>
         void ForEachUpdateParameter(UnaryFunction function) const
         {
            mUpdateParameters->ForEachParameter(function);
         }

         /**
          * Indicates whether this actor update message is a partial update or not. A partial
          * actor update implies there is not enough data to create the actor if it does
          * not already exist. 
          * @return Whether it's a partial update or not.
          */
         bool IsPartialUpdate() const;

         /**
          * Sets whether this actor update message is a partial update or not. A partial
          * actor update implies there is not enough data to create the actor if it does
          * not already exist. 
          * @param newValue Set the IsPartialUdpate true or false.
          */
         void SetPartialUpdate(bool newValue);

      protected:

         /// Destructor
         virtual ~ActorUpdateMessage();

      private:

         GroupMessageParameter* mUpdateParameters;
   };
}

#endif // DELTA_ACTORUPDATEMESSAGE
