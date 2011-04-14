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
 * Jeff P. Houde
 */
#include <prefix/dtdalprefix.h>

#include <dtDAL/defaultpropertymanager.h>

namespace dtDAL
{
   //Singleton global variable for the library manager.
   dtCore::RefPtr<DefaultPropertyManager> DefaultPropertyManager::mInstance(NULL);

   /////////////////////////////////////////////////////////////////////////////
   DefaultPropertyManager& DefaultPropertyManager::GetInstance()
   {
      if (!DefaultPropertyManager::mInstance.valid())
      {
         DefaultPropertyManager::mInstance = new DefaultPropertyManager();
      }
      return *(DefaultPropertyManager::mInstance.get());
   }

   /////////////////////////////////////////////////////////////////////////////
   DefaultPropertyManager::DefaultPropertyManager()
   {
   }

   /////////////////////////////////////////////////////////////////////////////
   DefaultPropertyManager::~DefaultPropertyManager()
   {
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DefaultPropertyManager::KeyExists(const std::string& keyName)
   {
      if (keyName.empty()) return false;

      KeyValMap::iterator keyIter = mDefaultValues.find(keyName);
      if (keyIter != mDefaultValues.end())
      {
         return true;
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DefaultPropertyManager::PropertyExists(const std::string& keyName, const std::string& propName)
   {
      if (keyName.empty() || propName.empty()) return false;

      KeyValMap::iterator keyIter = mDefaultValues.find(keyName);
      if (keyIter != mDefaultValues.end())
      {
         ValMap& valMap = keyIter->second;
         ValMap::iterator valIter = valMap.find(propName);
         if (valIter != valMap.end())
         {
            return true;
         }
      }

      return false;
   }

   ////////////////////////////////////////////////////////////////////////////////
   const NamedParameter* DefaultPropertyManager::GetDefaultValue(const std::string& keyName, const std::string& propName)
   {
      if (keyName.empty() || propName.empty()) return NULL;

      KeyValMap::iterator keyIter = mDefaultValues.find(keyName);
      if (keyIter != mDefaultValues.end())
      {
         ValMap& valMap = keyIter->second;
         ValMap::iterator valIter = valMap.find(propName);
         if (valIter != valMap.end())
         {
            return valIter->second;
         }
      }

      return NULL;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DefaultPropertyManager::SetDefaultValue(const std::string& keyName, const std::string& propName, dtCore::RefPtr<NamedParameter> defaultValue)
   {
      if (keyName.empty() || propName.empty()) return;

      KeyValMap::iterator keyIter = mDefaultValues.find(keyName);
      if (keyIter == mDefaultValues.end())
      {
         ValMap valMap;
         mDefaultValues[keyName] = valMap;
         keyIter = mDefaultValues.find(keyName);
      }

      if (keyIter != mDefaultValues.end())
      {
         keyIter->second[propName] = defaultValue;
      }
   }
}
