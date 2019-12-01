/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2012, MOVES Institute
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
 * Jeff P. Houde
 */

#include <prefix/dtcoreprefix.h>
#include <dtCore/containerselectoractorproperty.h>
#include <dtCore/propertycontainer.h>
#include <dtCore/datatype.h>
#include <dtUtil/stringutils.h>

const char OPEN_CHAR = '{';
const char CLOSE_CHAR = '}';

namespace dtCore
{
   ////////////////////////////////////////////////////////////////////////////////
   ContainerSelectorActorProperty::ContainerSelectorActorProperty(const dtUtil::RefString& name,
      const dtUtil::RefString& label, SetFuncType set, GetFuncType get,
      GetListFuncType getList, GetContainerFuncType getContainer,
      const dtUtil::RefString& desc, const dtUtil::RefString& groupName)
      : BaseClass(DataType::CONTAINER_SELECTOR, name, label, set, get, desc, groupName)
      , mCurrentPropertyIndex(-1)
      , mGetListFunc(getList)
      , mGetContainerFunc(getContainer)
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ContainerSelectorActorProperty::SetCurrentPropertyIndex(int index)
   {
      mCurrentPropertyIndex = index;
   }

   ////////////////////////////////////////////////////////////////////////////////
   int ContainerSelectorActorProperty::GetCurrentPropertyIndex() const
   {
      return mCurrentPropertyIndex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::vector<std::string> ContainerSelectorActorProperty::GetList() const
   {
      return mGetListFunc();
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::PropertyContainer* ContainerSelectorActorProperty::GetContainer() const
   {
      return mGetContainerFunc();
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ContainerSelectorActorProperty::FromString(const std::string& value)
   {
      if (IsReadOnly())
      {
         LOG_WARNING("FromString has been called on a property that is read only.");
         return false;
      }

      std::string data = value;
      std::string token;

      dtUtil::TakeToken(data, token, OPEN_CHAR, CLOSE_CHAR);
      SetValue(token);

      dtCore::PropertyContainer* propCon = GetContainer();
      if (propCon)
      {
         std::vector<dtCore::ActorProperty*> propList;
         propCon->GetPropertyList(propList);

         int count = (int)propList.size();
         for (int index = 0; index < count; ++index)
         {
            dtCore::ActorProperty* prop = propList[index];
            if (prop)
            {
               dtUtil::TakeToken(data, token, OPEN_CHAR, CLOSE_CHAR);
               prop->FromString(token);
            }
         }
      }

      return true;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const std::string ContainerSelectorActorProperty::ToString() const
   {
      std::string data;

      data += OPEN_CHAR;
      data += GetValue();
      data += CLOSE_CHAR;

      dtCore::PropertyContainer* propCon = GetContainer();
      if (propCon)
      {
         std::vector<const dtCore::ActorProperty*> propList;
         propCon->GetPropertyList(propList);

         int count = (int)propList.size();
         for (int index = 0; index < count; ++index)
         {
            const dtCore::ActorProperty* prop = propList[index];
            if (prop)
            {
               data += OPEN_CHAR;
               data += prop->ToString();
               data += CLOSE_CHAR;
            }
         }
      }

      return data;
   }

}

