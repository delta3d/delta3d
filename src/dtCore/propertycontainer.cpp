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
#include <dtCore/propertycontainer.h>
#include <dtCore/exceptionenum.h>
#include <dtUtil/exception.h>
#include <dtUtil/log.h>
#include <algorithm>
#include <typeinfo>

namespace dtCore
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
      // Must const cast to be able to set the defaults.
      ObjectType& type = const_cast<ObjectType&>(GetObjectType());

      // Assume the defaults don't need to be set again.
      if (!type.DefaultsEmpty())
      {
         return;
      }

      std::vector<ActorProperty*> propList;
      GetPropertyList(propList);

      int propCount = (int)propList.size();
      for (int propIndex = 0; propIndex < propCount; ++propIndex)
      {
         ActorProperty* prop = propList[propIndex];
         dtCore::RefPtr<NamedParameter> param = NamedParameter::CreateFromType(
            prop->GetDataType(), prop->GetName());

         param->SetFromProperty(*prop);
         type.SetDefaultValue(prop->GetName(), *param);
      }
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
         throw dtCore::InvalidParameterException(
            "AddProperty cannot add a NULL property", __FILE__, __LINE__);
      }

      PropertyMapType::iterator itor =
         mPropertyMap.find(newProp->GetName());
      if(itor != mPropertyMap.end())
      {
         LOGN_ERROR("propertycontainer.cpp", "Could not add new property " + newProp->GetName() + " because a property with that name already exists.");
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
   void PropertyContainer::RemoveProperty(ActorProperty* toRemove)
   {
      if (toRemove == NULL) return;
      PropertyMapType::iterator itor =
         mPropertyMap.find(toRemove->GetName());
      if (itor != mPropertyMap.end() && itor->second == toRemove)
      {
         mPropertyMap.erase(itor);
         mProperties.erase(std::remove(mProperties.begin(), mProperties.end(), dtCore::RefPtr<ActorProperty>(toRemove)), mProperties.end());
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
         LOGN_INFO("propertycontainer.cpp", "Could not find property " + nameToRemove + " to remove. Reason was: Was not found in propertyMap");
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
   void PropertyContainer::GetPropertyList(PropertyConstVector& propList) const
   {
      propList.reserve(propList.size() + mProperties.size());

      for (size_t i = 0; i < mProperties.size(); ++i)
      {
         propList.push_back(mProperties[i].get());
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::GetPropertyList(PropertyVector& propList)
   {
      propList.reserve(propList.size() + mProperties.size());

      for (size_t i = 0; i < mProperties.size(); ++i)
      {
         propList.push_back(mProperties[i].get());
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyContainer::HasProperty(ActorProperty& prop) const
   {
      bool found = false;

      size_t limit = mProperties.size();
      for (size_t i = 0; i < limit; ++i)
      {
         if (&prop == mProperties[i].get())
         {
            found = true;
            break;
         }
      }

      return found;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyContainer::DoesDefaultExist(const dtCore::ActorProperty& prop) const
   {
      if (prop.IsReadOnly())
      {
         return false;
      }

      return GetObjectType().DefaultExists(prop.GetName());
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyContainer::IsPropertyDefault(const dtCore::ActorProperty& prop) const
   {
      if (prop.IsReadOnly())
      {
         return true;
      }

      const NamedParameter* param =
            GetObjectType().GetDefaultValue(prop.GetName());
      if (param != NULL && *param == prop)
      {
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::ResetProperty(dtCore::ActorProperty& prop)
   {
      if (prop.IsReadOnly())
      {
         return;
      }

      const NamedParameter* param =
            GetObjectType().GetDefaultValue(prop.GetName());
      if (param)
      {
         param->ApplyValueToProperty(prop);
         // This is set when property values are set to the default manually, but resetting needs to clear it.
         prop.SetAlwaysSave(false);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool PropertyContainer::ShouldPropertySave(const dtCore::ActorProperty& prop) const
   {
      if (prop.IsReadOnly() || prop.GetIgnoreWhenSaving()) return false;

      return !IsPropertyDefault(prop) || prop.GetAlwaysSave();
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   void PropertyContainer::CopyPropertiesFrom(const PropertyContainer& copyFrom, bool copyMetadata)
   {
      //Now copy all of the properties from this proxy to the clone.
      for (size_t i = 0; i < mProperties.size(); ++i)
      {
         const ActorProperty* prop = copyFrom.GetProperty(mProperties[i]->GetName());
         if (prop != nullptr)
         {
            if (!prop->IsReadOnly())
            {
               mProperties[i]->CopyFrom(*prop);
            }
            if (copyMetadata)
               mProperties[i]->CopyMetadata(*prop);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////////////
   unsigned PropertyContainer::GetNumProperties() const
   {
      return mProperties.size();
   }

}
