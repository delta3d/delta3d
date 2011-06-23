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

#include <prefix/dtdalprefix.h>
#include <dtDAL/propertycontainer.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/defaultpropertymanager.h>
#include <dtUtil/exception.h>
#include <dtUtil/log.h>

#include <sstream>

namespace dtDAL
{

   PropertyContainer::PropertyContainer()
   {
   }

   PropertyContainer::~PropertyContainer()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::InitDefaults()
   {
      std::string keyName = GetDefaultPropertyKey();

      // If the key already exists, we don't need to add it again.
      if (DefaultPropertyManager::GetInstance().KeyExists(keyName))
      {
         return;
      }

      std::vector<ActorProperty*> propList;
      GetPropertyList(propList);

      int propCount = (int)propList.size();
      for (int propIndex = 0; propIndex < propCount; ++propIndex)
      {
         ActorProperty* prop = propList[propIndex];
         if (prop)
         {
            prop->InitDefault(keyName);
         }
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string PropertyContainer::GetDefaultPropertyKey() const
   {
      return "";
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::AddProperty(ActorProperty* newProp)
   {
      InsertProperty(newProp, -1);
   }

   //////////////////////////////////////////////////////////////////////////
   void PropertyContainer::InsertProperty(ActorProperty* newProp, int index)
   {
      if(newProp == NULL)
      {
         throw dtDAL::InvalidParameterException(
            "AddProperty cannot add a NULL property", __FILE__, __LINE__);
      }

      PropertyMapType::iterator itor =
         mPropertyMap.find(newProp->GetName());
      if(itor != mPropertyMap.end())
      {
         std::ostringstream ss;
         ss << "Could not add new property " << newProp->GetName() << " because "
            << "a property with that name already exists.";
         LOG_ERROR(ss.str());
      }
      else
      {
         mPropertyMap.insert(std::make_pair(dtUtil::RefString(newProp->GetName()),newProp));

         if (index >= 0 && index < (int)mProperties.size())
         {
            mProperties.insert(mProperties.begin() + index, newProp);
         }
         else
         {
            mProperties.push_back(newProp);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::RemoveProperty(const std::string& nameToRemove)
   {
      PropertyMapType::iterator itor =
         mPropertyMap.find(nameToRemove);
      if (itor != mPropertyMap.end())
      {
         mPropertyMap.erase(itor);
         for (size_t i = 0; i < mProperties.size(); ++i)
         {
            if (mProperties[i]->GetName() == nameToRemove)
            {
               mProperties.erase(mProperties.begin() + i);
               break;
            }
         }
      }
      else
      {
         std::ostringstream msg;
         msg << "Could not find property " << nameToRemove << " to remove. Reason was: " << "was not found in mPropertyMap";
         LOG_DEBUG(msg.str());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   ActorProperty* PropertyContainer::GetProperty(const std::string& name)
   {
      PropertyMapType::iterator itor =
         mPropertyMap.find(name);

      if (itor == mPropertyMap.end())
      {
         return NULL;
      }
      else
      {
         return itor->second.get();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   const ActorProperty* PropertyContainer::GetProperty(const std::string& name) const
   {
      PropertyMapType::const_iterator itor =
         mPropertyMap.find(name);

      if (itor == mPropertyMap.end())
      {
         return NULL;
      }
      else
      {
         return itor->second.get();
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<ActorProperty> PropertyContainer::GetDeprecatedProperty(const std::string& name)
   {
      dtCore::RefPtr<ActorProperty> prop = NULL;
      return prop;
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::GetPropertyList(std::vector<const ActorProperty*>& propList) const
   {
      propList.clear();
      propList.reserve(mProperties.size());

      for (size_t i = 0; i < mProperties.size(); ++i)
      {
         propList.push_back(mProperties[i].get());
      }
   }


   ///////////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::GetPropertyList(std::vector<ActorProperty*>& propList)
   {
      propList.clear();
      propList.reserve(mProperties.size());

      for (size_t i = 0; i < mProperties.size(); ++i)
      {
         propList.push_back(mProperties[i].get());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyContainer::DoesDefaultExist(const dtDAL::ActorProperty& prop) const
   {
      if (prop.IsReadOnly())
      {
         return false;
      }

      const NamedParameter* param =
         DefaultPropertyManager::GetInstance().GetDefaultValue(
         GetDefaultPropertyKey(), prop.GetName());
      if (param)
      {
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyContainer::IsPropertyDefault(const dtDAL::ActorProperty& prop) const
   {
      if (prop.IsReadOnly())
      {
         return true;
      }

      const NamedParameter* param =
         DefaultPropertyManager::GetInstance().GetDefaultValue(
         GetDefaultPropertyKey(), prop.GetName());
      if (param && *param == prop)
      {
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::ResetProperty(dtDAL::ActorProperty& prop)
   {
      if (prop.IsReadOnly())
      {
         return;
      }

      const NamedParameter* param =
         DefaultPropertyManager::GetInstance().GetDefaultValue(
         GetDefaultPropertyKey(), prop.GetName());
      if (param)
      {
         param->ApplyValueToProperty(prop);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyContainer::ShouldPropertySave(const dtDAL::ActorProperty& prop) const
   {
      return !IsPropertyDefault(prop);
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::CopyPropertiesFrom(const PropertyContainer& copyFrom)
   {
      //Now copy all of the properties from this proxy to the clone.
      for (size_t i = 0; i < mProperties.size(); ++i)
      {
         const ActorProperty* prop = copyFrom.GetProperty(mProperties[i]->GetName());
         if (prop != NULL && !prop->IsReadOnly())
         {
            mProperties[i]->CopyFrom(*prop);
         }
      }
   }
}
