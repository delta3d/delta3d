/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2009, Alion Science and Technology.
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
 * Jeffrey Houde.
 */

#include <dtDAL/containeractorproperty.h>
#include <iostream>
#include <dtDAL/datatype.h>

const char OPEN_CHAR = 1;
const char CLOSE_CHAR = 2;

namespace dtDAL
{
   ////////////////////////////////////////////////////////////////////////////////
   ContainerActorProperty::ContainerActorProperty(const std::string& name,
      const std::string& label,
      const std::string& desc,
      const std::string& groupName,
      const std::string& editorType,
      bool readOnly)
   : ActorProperty(DataType::CONTAINER, name, label, desc, groupName, readOnly)
   , mCurrentIndex(0)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   ContainerActorProperty::~ContainerActorProperty()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ContainerActorProperty::FromString(const std::string& value)
   {
      if (mProperties.size() <= 0)
      {
         return false;
      }

      std::string data = value;
      std::string token;

      for (int index = 0; index < (int)mProperties.size(); index++)
      {
         token = TakeToken(data);
         mProperties[index]->FromString(token);
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string ContainerActorProperty::TakeToken(std::string& data)
   {
      std::string returnData;

      // If the first character in the data string is not the opening character,
      //  we will just assume the entire data string is the token.
      if (data.c_str()[0] != OPEN_CHAR)
      {
         returnData = data;
         data = "";
      }

      int depth = 0;
      int dataIndex = 0;
      while (data.length() > 0)
      {
         bool appendChar = true;

         // Opening characters increase the depth counter.
         if (data[dataIndex] == OPEN_CHAR)
         {
            depth++;

            if (depth == 1)
            {
               appendChar = false;
            }
         }
         // Closing characters decrease the depth counter.
         else if (data[dataIndex] == CLOSE_CHAR)
         {
            depth--;

            if (depth == 0)
            {
               appendChar = false;
            }
         }

         // All other characters are added to the return buffer.
         if (appendChar)
         {
            returnData.append(data.c_str(), 1);
         }

         // Remove the left most character from the data string.
         data = &data[1];

         // We are done once our depth returns to 0.
         if (depth <= 0)
         {
            break;
         }
      }

      return returnData;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string ContainerActorProperty::ToString() const
   {
      if (mProperties.size() <= 0)
      {
         return "";
      }

      // Iterate through each property in the container and append the strings.
      std::string data;
      for (int index = 0; index < (int)mProperties.size(); index++)
      {
         data += OPEN_CHAR;
         data += mProperties[index]->ToString();
         data += CLOSE_CHAR;
      }

      return data;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ContainerActorProperty::CopyFrom(const ActorProperty& otherProp)
   {
      if (GetDataType() != otherProp.GetDataType())
      {
         LOG_ERROR("Property types are incompatible. Cannot make copy.");
         return;
      }

      const ContainerActorProperty* src = dynamic_cast<const ContainerActorProperty*>(&otherProp);
      if (src)
      {
         FromString(src->ToString());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ContainerActorProperty::AddProperty(ActorProperty* property)
   {
      if (property == NULL)
      {
         return;
      }

      mProperties.push_back(property);
   }

   ////////////////////////////////////////////////////////////////////////////////
   ActorProperty* ContainerActorProperty::GetProperty(int index)
   {
      if (index >= (int)mProperties.size())
      {
         return NULL;
      }

      return mProperties[index].get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   const ActorProperty* ContainerActorProperty::GetProperty(int index) const
   {
      if (index >= (int)mProperties.size())
      {
         return NULL;
      }

      return mProperties[index].get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   ActorProperty* ContainerActorProperty::GetCurrentProperty()
   {
      if (mCurrentIndex >= 0 && mCurrentIndex < (int)mProperties.size())
      {
         return mProperties[mCurrentIndex].get();
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const ActorProperty* ContainerActorProperty::GetCurrentProperty() const
   {
      if (mCurrentIndex >= 0 && mCurrentIndex < (int)mProperties.size())
      {
         return mProperties[mCurrentIndex].get();
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int ContainerActorProperty::GetCurrentPropertyIndex() const
   {
      return mCurrentIndex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ContainerActorProperty::SetCurrentPropertyIndex(int index)
   {
      mCurrentIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int ContainerActorProperty::GetPropertyCount() const
   {
      return (int)mProperties.size();
   }
}
