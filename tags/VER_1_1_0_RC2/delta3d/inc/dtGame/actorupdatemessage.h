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

#ifndef DELTA_ACTORUPDATEMESSAGE
#define DELTA_ACTORUPDATEMESSAGE

#include <vector>
#include <map>
#include <dtUtil/exception.h>
#include "dtGame/message.h"
#include "dtGame/messageparameter.h"


// Forward declarations
namespace dtDAL
{
   class DataType;
}

namespace dtGame
{
   /**
    * Class the represents messages that are related to actors
    */
   class DT_GAME_EXPORT ActorUpdateMessage : public Message
   {
      public:

         /// Constructor
         ActorUpdateMessage();

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
          * Retrieves the update parameters for this actor update message
          * @param toFill The vector to fill with the parameters
          */
         void GetUpdateParameters(std::vector<MessageParameter*> &toFill) throw();

         /**
          * Adds support to copy the dynamic update parameter list to the other message.
          * @see Message#CopyDataTo
          * @throw dtUtil::Exception with enum Exception::INVALID_PARAMETER if the message is not an ActorUpdateMessage.
          */
         virtual void CopyDataTo(Message& msg) const throw(dtUtil::Exception);

      protected:

         /// Destructor
         virtual ~ActorUpdateMessage();

      private:

         std::map<std::string, dtCore::RefPtr<MessageParameter> > mPropertyList;
   };
}

#endif // DELTA_ACTORUPDATEMESSAGE
