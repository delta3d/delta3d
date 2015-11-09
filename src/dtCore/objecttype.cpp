/* -*-c++-*-
 * Delta3D
 * Copyright 2007-2008, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * David Guthrie
 */

#include <prefix/dtcoreprefix.h>
#include <dtCore/objecttype.h>

#include <iostream>

namespace dtCore
{

   ///////////////////////////////////////////////////////////////////////////
   ObjectType::ObjectType(const std::string& name,
                           const std::string& category,
                           const std::string& desc,
                           const ObjectType* parentType)
   : mName(name)
   , mCategory(category)
   , mDescription(desc)
   , mParentType(parentType)
   {
      GenerateUniqueId();
   }

   ///////////////////////////////////////////////////////////////////////////
   ObjectType::~ObjectType()
   {
   }

   ////////////////////////////////////////////////////////////////////////////
   std::pair<std::string, std::string > ObjectType::ParseNameAndCategory(const std::string& objectTypeFullName)
   {
      size_t index = objectTypeFullName.find_last_of('.');

      std::string objectTypeCategory;
      std::string objectTypeName;

      if (index == objectTypeFullName.length())
      {
         objectTypeName = objectTypeFullName;
         objectTypeCategory.clear();
      }
      else
      {
         objectTypeName = objectTypeFullName.substr(index + 1);
         objectTypeCategory = objectTypeFullName.substr(0, index);
      }
      return std::make_pair(objectTypeName, objectTypeCategory);
   }

   ///////////////////////////////////////////////////////////////////////////
   void ObjectType::SetName(const std::string& name)
   {
      mName = name;
      GenerateUniqueId();
   }

   ///////////////////////////////////////////////////////////////////////////
   const std::string& ObjectType::GetName() const { return mName; }

   ///////////////////////////////////////////////////////////////////////////
   void ObjectType::SetCategory(const std::string& category)
   {
      mCategory = category;
      GenerateUniqueId();
   }

   ///////////////////////////////////////////////////////////////////////////
   const std::string& ObjectType::GetCategory() const { return mCategory; }

   ///////////////////////////////////////////////////////////////////////////
   void ObjectType::SetDescription(const std::string& desc) { mDescription = desc; }

   ///////////////////////////////////////////////////////////////////////////
   const std::string& ObjectType::GetDescription() const { return mDescription; }

   ///////////////////////////////////////////////////////////////////////////
   const std::string& ObjectType::GetUniqueId() const { return mUniqueId; }

   ///////////////////////////////////////////////////////////////////////////
   const ObjectType* ObjectType::GetParentType() const { return mParentType.get(); }

   ///////////////////////////////////////////////////////////////////////////////
   void ObjectType::GenerateUniqueId()
   {
      mUniqueId = mName + mCategory;
   }

   ///////////////////////////////////////////////////////////////////////////////
   std::string ObjectType::GetFullName() const
   {
      return GetCategory() + "." + GetName();
   }


   ///////////////////////////////////////////////////////////////////////////////
   bool ObjectType::InstanceOf(const ObjectType& rhs) const
   {
      if (rhs == *this)
         return true;

      const ObjectType* parent = GetParentType();
      while (parent != NULL)
      {
         if (*parent == rhs)
            return true;
         parent = parent->GetParentType();
      }

      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool ObjectType::InstanceOf(const std::string& category, const std::string& name) const
   {
      dtCore::RefPtr<ObjectType> typeToCheck = new ObjectType(name,category);
      return InstanceOf(*typeToCheck);
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ObjectType::InstanceOf(const std::string& fullName) const
   {
      bool isInstance = false;

      if (GetFullName() == fullName)
      {
         isInstance = true;
      }
      else
      {
         const ObjectType* parent = GetParentType();
         while (parent != NULL)
         {
            if (parent->GetFullName() == fullName)
            {
               isInstance = true;
               break;
            }
            parent = parent->GetParentType();
         }
      }

      return isInstance;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool ObjectType::DefaultExists(const dtUtil::RefString& propName) const
   {
      if (propName->empty()) return false;

      ValMap::const_iterator valIter = mDefaultValues.find(propName);
      if (valIter != mDefaultValues.end())
      {
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const NamedParameter* ObjectType::GetDefaultValue(const dtUtil::RefString& propName) const
   {
      if (propName->empty()) return NULL;

      ValMap::const_iterator valIter = mDefaultValues.find(propName);
      if (valIter != mDefaultValues.end())
      {
         return valIter->second;
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void ObjectType::SetDefaultValue(const dtUtil::RefString& propName, NamedParameter& defaultValue)
   {
      if (propName->empty()) return;

      mDefaultValues[propName] = &defaultValue;
   }

   ///////////////////////////////////////////////////////////////////////////
   bool ObjectType::operator<(const ObjectType& rhs) const
   {
      return (mUniqueId < rhs.mUniqueId);
   }

   ///////////////////////////////////////////////////////////////////////////
   bool ObjectType::operator==(const ObjectType& rhs) const
   {
      return (mUniqueId == rhs.mUniqueId);
   }

   ///////////////////////////////////////////////////////////////////////////
   bool ObjectType::operator!=(const ObjectType& rhs) const
   {
      return (mUniqueId != rhs.mUniqueId);
   }

   ///////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////
   std::ostream& operator<<(std::ostream& os, const ObjectType& objectType)
   {
      os << objectType.GetFullName();
      return os;
   }

}
