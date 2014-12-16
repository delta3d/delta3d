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

#include <prefix/dtcoreprefix.h>
#include <dtCore/containeractorproperty.h>
#include <dtCore/datatype.h>
#include <iostream>
#include <dtUtil/stringutils.h>

const char OPEN_CHAR = '{';
const char CLOSE_CHAR = '}';

namespace dtCore
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
   void ContainerActorProperty::InitDefault(ObjectType& type)
   {
      BaseClass::InitDefault(type);

      int count = (int)mProperties.size();
      for (int index = 0; index < count; ++index)
      {
         ActorProperty* prop = mProperties[index];
         if (prop)
         {
            prop->InitDefault(type);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ContainerActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      if (mProperties.size() <= 0)
      {
         return false;
      }

      std::string data = value;
      std::string token;

      for (int index = 0; index < int(mProperties.size()); index++)
      {
         dtUtil::TakeToken(data, token, OPEN_CHAR, CLOSE_CHAR);
         mProperties[index]->FromString(token);
      }

      return true;
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
   void ContainerActorProperty::ClearProperties()
   {
      mProperties.clear();
   }

   ////////////////////////////////////////////////////////////////////////////////
   ActorProperty* ContainerActorProperty::GetProperty(int index)
   {
      if (index < 0 || index >= (int)mProperties.size())
      {
         return NULL;
      }

      return mProperties[index].get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   const ActorProperty* ContainerActorProperty::GetProperty(int index) const
   {
      if (index < 0 || index >= (int)mProperties.size())
      {
         return NULL;
      }

      return mProperties[index].get();
   }

   ////////////////////////////////////////////////////////////////////////////////
   ActorProperty* ContainerActorProperty::GetProperty(const std::string& name)
   {
      int count = (int)mProperties.size();
      for (int index = 0; index < count; ++index)
      {
         ActorProperty* prop = mProperties[index];
         if (prop && prop->GetName() == name)
         {
            return prop;
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const ActorProperty* ContainerActorProperty::GetProperty(const std::string& name) const
   {
      int count = (int)mProperties.size();
      for (int index = 0; index < count; ++index)
      {
         const ActorProperty* prop = mProperties[index];
         if (prop && prop->GetName() == name)
         {
            return prop;
         }
      }

      return NULL;
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
