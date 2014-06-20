/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008-2012, Alion Science and Technology
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
 * @author David Guthrie
*/
#ifndef DELTA_CONFIGPROPERTIES
#define DELTA_CONFIGPROPERTIES

#include <string>
#include <vector>
#include <dtUtil/export.h>

namespace dtUtil
{
   /**
     * Pure virtual interface used to get and set name/value pairs of data,
     * typically used for configuration settings. Derive and provide an implementation
     * for the methods. Store the name/value pairs in a container for later lookup.
     */
   class DT_UTIL_EXPORT ConfigProperties
   {
      public:
         /**
           * Get the value corresponding to the supplied name. If the name isn't
           * found, the defaultValue should be returned.
           * @return a string value that is paired with the given name.
           * @param name The name of the configuration to find the value for
           * @param defaultValue If name isn't found, this value should be returned
           */
         virtual const std::string& GetConfigPropertyValue(const std::string& name, const std::string& defaultValue = "") const = 0;

         /**
          * Fills the output vector with key/value pairs where the key begins with the supplied prefix.
          * @param prefix the prefix to use to filter.
          * @param resultOut the result output parameter
          * @param removePrefix optional paramater defaulting to true.  Iff true, the supplied prefix will be stripped from the string.
          */
         virtual void GetConfigPropertiesWithPrefix(const std::string& prefix, std::vector<std::pair<std::string,std::string> >& resultOut, bool removePrefix = true) const = 0;

         /**
           * Sets the value of a given config property.
           * @param name The name of the configuration to set the value on
           * @param value The value to assign to the configuration
           */
         virtual void SetConfigPropertyValue(const std::string& name, const std::string& value) = 0;

         /**
           * Removes the configuration with the given name.
           * @param name The name of the configuration to use. If not found, nothing
           * should happen.
           */
         virtual void RemoveConfigPropertyValue(const std::string& name) = 0;

         /**
          * @return true iff the named property exists.
          * @param name the name of the property to query for.
          */
         virtual bool IsConfigPropertyDefined(const std::string& name) const = 0;

         /// Virtual destructor to prevent undefined behavior in derived classes
         virtual ~ConfigProperties();
   };
}

#endif /*DELTA_CONFIGPROPERTIES*/
