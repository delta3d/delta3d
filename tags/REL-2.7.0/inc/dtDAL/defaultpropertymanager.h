/* -*-c++-*-
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

#ifndef DELTA_DEFAULT_PROPERTY_MANAGER
#define DELTA_DEFAULT_PROPERTY_MANAGER

#include <osg/Referenced>
#include <dtCore/refptr.h>
#include <dtUtil/refstring.h>
#include <dtDAL/namedparameter.h>
#include <dtDAL/export.h>
#include <map>

namespace dtDAL
{
   /**
    * This class stores the default values for properties.
    */
   class DT_DAL_EXPORT DefaultPropertyManager: public osg::Referenced
   {
      public:
         /**
          * Gets the singleton instance of the DefaultPropertyManager.
          */
         static DefaultPropertyManager& GetInstance();

         /**
          * Returns whether a given key exists.
          *
          * @param[in]  keyName  The key.
          */
         bool KeyExists(const std::string& keyName);

         /**
          * Returns whether a property exists.
          *
          * @param[in]  keyName   The key.
          * @param[in]  propName  The property.
          */
         bool PropertyExists(const std::string& keyName, const std::string& propName);

         /**
          * Retrieves the default value of a property.
          *
          * @param[in]  keyName   The key.
          * @param[in]  propName  The property.
          *
          * @return     The default value (or NULL if none exists).
          */
         const NamedParameter* GetDefaultValue(const std::string& keyName, const std::string& propName);

         /**
          * Sets the default value of a given property.
          *
          * @param[in]  keyName       The key.
          * @param[in]  propName      The property.
          * @param[in]  defaultValue  The default value of the property.
          */
         void SetDefaultValue(const std::string& keyName, const std::string& propName, dtCore::RefPtr<NamedParameter> defaultValue);

      private:

         /**
          * Constructs the manager.
          */
         DefaultPropertyManager();

         /**
          * Free resources used by this manager.
          */
         virtual ~DefaultPropertyManager();

         ///Singleton instance of this class.
         static dtCore::RefPtr<DefaultPropertyManager> mInstance;

         typedef std::map<dtUtil::RefString, dtCore::RefPtr<NamedParameter> > ValMap;
         typedef std::map<dtUtil::RefString, ValMap> KeyValMap;
         KeyValMap mDefaultValues;
   };
}

#endif
