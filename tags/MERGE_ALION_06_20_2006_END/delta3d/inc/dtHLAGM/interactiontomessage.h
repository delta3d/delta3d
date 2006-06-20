/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * @author Olen A. Bruce
 * @author David Guthrie
 */

#ifndef DELTA_INTERACTION_TO_MESSAGE
#define DELTA_INTERACTION_TO_MESSAGE

//You must include RTI.hh before including this file.

#include <string>
#include <vector>
#include <dtGame/messagetype.h>
#include <osg/Referenced>
#include "dtHLAGM/onetoonemapping.h"
#include "dtHLAGM/parametertoparameter.h"

namespace dtHLAGM
{
   /**
    * Defines a one-to-one the mapping between and HLA interacion and a game message
    */
   class DT_HLAGM_EXPORT InteractionToMessage : public osg::Referenced
   {
      public:

         InteractionToMessage(): mType(&dtGame::MessageType::UNKNOWN), 
            mInteractionHandle(0)
         {}

         const dtGame::MessageType& GetMessageType() const
         {
            return *mType;
         }

         const std::string &GetInteractionName() const
         {
            return mInteractionName;
         }

         const RTI::InteractionClassHandle &GetInteractionClassHandle() const
         {
            return mInteractionHandle;
         }

         const std::vector<ParameterToParameterList> &GetOneToManyMappingVector() const
         {
            return mOneToMany;
         }

         std::vector<ParameterToParameterList> &GetOneToManyMappingVector()
         {
            return mOneToMany;
         }

         void SetMessageType(const dtGame::MessageType& messageType)
         {
            mType = &messageType;
         }

         void SetInteractionName(const std::string& interactionTypeName)
         {
            mInteractionName = interactionTypeName;
         }

         void SetInteractionClassHandle(RTI::InteractionClassHandle interactionHandle)
         {
            mInteractionHandle = interactionHandle;
         }

         void SetOneToManyMappingVector(std::vector<ParameterToParameterList> &thisOneToManyVector)
         {
            mOneToMany = thisOneToManyVector;
         }

         InteractionToMessage& operator=(const InteractionToMessage& setTo)
         {
            mType = setTo.mType;
            mInteractionName = setTo.mInteractionName;
            mInteractionHandle = setTo.mInteractionHandle;
            mOneToMany = setTo.mOneToMany;

            return *this;
         }

         bool operator==(const InteractionToMessage& toCompare) const
         {
            return mType == toCompare.mType &&
               mInteractionName == toCompare.mInteractionName &&
               mInteractionHandle == toCompare.mInteractionHandle &&
               mOneToMany == toCompare.mOneToMany;
         }

         bool operator!=(const InteractionToMessage& toCompare) const
         {
            return !operator==(toCompare);
         }
      private:
         ~InteractionToMessage()
         {}

         const dtGame::MessageType* mType;
         std::string mInteractionName;
         RTI::InteractionClassHandle mInteractionHandle;
         std::vector<ParameterToParameterList> mOneToMany;
   };
};

#endif // DELTA_INTERACTION_TO_MESSAGE
